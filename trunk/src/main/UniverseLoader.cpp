// This file is part of Cosmographia.
//
// Copyright (C) 2011 Chris Laurel <claurel@gmail.com>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Cosmographia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Cosmographia. If not, see <http://www.gnu.org/licenses/>.

#include "UniverseLoader.h"
#include "TleTrajectory.h"
#include "InterpolatedStateTrajectory.h"
#include "InterpolatedRotation.h"
#include "TwoVectorFrame.h"
#include "WMSTiledMap.h"
#include "MultiWMSTiledMap.h"
#include "compatibility/Scanner.h"
#include <vesta/Units.h>
#include <vesta/Body.h>
#include <vesta/Arc.h>
#include <vesta/Trajectory.h>
#include <vesta/Frame.h>
#include <vesta/InertialFrame.h>
#include <vesta/BodyFixedFrame.h>
#include <vesta/RotationModel.h>
#include <vesta/UniformRotationModel.h>
#include <vesta/FixedRotationModel.h>
#include <vesta/KeplerianTrajectory.h>
#include <vesta/FixedPointTrajectory.h>
#include <vesta/WorldGeometry.h>
#include <vesta/Atmosphere.h>
#include <vesta/DataChunk.h>
#include <vesta/ArrowGeometry.h>
#include <vesta/MeshGeometry.h>
#include <vesta/SensorFrustumGeometry.h>
#include <vesta/AxesVisualizer.h>
#include <vesta/BodyDirectionVisualizer.h>
#include <vesta/Units.h>
#include <vesta/GregorianDate.h>
#include <QDateTime>
#include <QFile>
#include <QColor>
#include <QDebug>

using namespace vesta;
using namespace Eigen;


class SimpleRotationModel : public RotationModel
{
public:
    SimpleRotationModel(double inclination,
                        double ascendingNode,
                        double rotationRate,
                        double meridianAngleAtEpoch,
                        double epoch) :
        m_rotationRate(rotationRate),
        m_meridianAngleAtEpoch(meridianAngleAtEpoch),
        m_epoch(epoch),
        m_rotation(AngleAxisd(ascendingNode, Vector3d::UnitZ()) * AngleAxisd(inclination, Vector3d::UnitX()))
    {
    }


    Quaterniond
    orientation(double t) const
    {
        double meridianAngle = m_meridianAngleAtEpoch + (t - m_epoch) * m_rotationRate;
        return m_rotation * Quaterniond(AngleAxis<double>(meridianAngle, Vector3d::UnitZ()));
    }


    Vector3d
    angularVelocity(double /* t */) const
    {
        return m_rotation * (Vector3d::UnitZ() * m_rotationRate);
    }

private:
    double m_rotationRate;
    double m_meridianAngleAtEpoch;
    double m_epoch;
    Quaterniond m_rotation;
};


static bool readNextDouble(Scanner* scanner, double* value)
{
    if (scanner->readNext() == Scanner::Double || scanner->currentToken() == Scanner::Integer)
    {
        *value = scanner->doubleValue();
        return true;
    }
    else
    {
        return false;
    }
}


static bool readNextVector3(Scanner* scanner, Vector3d* value)
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    if (readNextDouble(scanner, &x) && readNextDouble(scanner, &y) && readNextDouble(scanner, &z))
    {
        *value = Vector3d(x, y, z);
        return true;
    }
    else
    {
        return false;
    }
}


static bool readNextQuaternion(Scanner* scanner, Quaterniond* value)
{
    double w = 0.0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    if (readNextDouble(scanner, &w) &&
        readNextDouble(scanner, &x) &&
        readNextDouble(scanner, &y) &&
        readNextDouble(scanner, &z))
    {
        *value = Quaterniond(w, x, y, z);
        return true;
    }
    else
    {
        return false;
    }
}


/** Load a list of time/state vector records from a file. The values
  * are stored in ASCII format with newline terminated hash comments
  * allowed. Dates are given as TDB Julian dates, positions are
  * in units of kilometers, and velocities are km/sec.
  */
InterpolatedStateTrajectory*
LoadXYZVTrajectory(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open trajectory file " << fileName;
        return NULL;
    }

    InterpolatedStateTrajectory::TimeStateList states;

    Scanner scanner(&file);
    bool ok = true;
    bool done = false;
    while (!done)
    {
        double jd = 0.0;
        Vector3d position = Vector3d::Zero();
        Vector3d velocity = Vector3d::Zero();
        if (!readNextDouble(&scanner, &jd))
        {
            done = true;
            if (!scanner.atEnd())
            {
                ok = false;
            }
        }
        else
        {
            if (!readNextVector3(&scanner, &position) || !readNextVector3(&scanner, &velocity))
            {
                ok = false;
                done = true;
            }
        }

        if (!done)
        {
            double tdbSec = daysToSeconds(jd - vesta::J2000);
            InterpolatedStateTrajectory::TimeState state;
            state.tsec = tdbSec;
            state.state = StateVector(position, velocity);
            states.push_back(state);
        }
    }

    if (!ok)
    {
        qDebug() << "Error in xyzv trajectory file, record " << states.size();
        return NULL;
    }
    else
    {
        return new InterpolatedStateTrajectory(states);
    }
}


/** Load a list of time/position records from a file. The values
  * are stored in ASCII format with newline terminated hash comments
  * allowed. Dates are given as TDB Julian dates and positions are
  * in units of kilometers.
  */
InterpolatedStateTrajectory*
LoadXYZTrajectory(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open trajectory file " << fileName;
        return NULL;
    }

    InterpolatedStateTrajectory::TimePositionList positions;

    Scanner scanner(&file);
    bool ok = true;
    bool done = false;
    while (!done)
    {
        double jd = 0.0;
        Vector3d position = Vector3d::Zero();
        if (!readNextDouble(&scanner, &jd))
        {
            done = true;
            if (!scanner.atEnd())
            {
                ok = false;
            }
        }
        else
        {
            if (!readNextVector3(&scanner, &position))
            {
                ok = false;
                done = true;
            }
        }

        if (!done)
        {
            double tdbSec = daysToSeconds(jd - vesta::J2000);
            InterpolatedStateTrajectory::TimePosition record;
            record.tsec = tdbSec;
            record.position = position;
            positions.push_back(record);
        }
    }

    if (!ok)
    {
        qDebug() << "Error in xyz trajectory file, record " << positions.size();
        return NULL;
    }
    else
    {
        return new InterpolatedStateTrajectory(positions);
    }
}


/** Load a list of time/quaternion records from a file. The values
  * are stored in ASCII format with newline terminated hash comments
  * allowed. Dates are given as TDB Julian dates and orientations are
  * given as quaternions with components ordered w, x, y, z (i.e. the
  * real part of the quaternion is before the imaginary parts.)
  */
InterpolatedRotation*
LoadInterpolatedRotation(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open trajectory file " << fileName;
        return NULL;
    }

    InterpolatedRotation::TimeOrientationList orientations;

    Scanner scanner(&file);
    bool ok = true;
    bool done = false;
    while (!done)
    {
        double jd = 0.0;
        Quaterniond q = Quaterniond::Identity();
        if (!readNextDouble(&scanner, &jd))
        {
            done = true;
            if (!scanner.atEnd())
            {
                ok = false;
            }
        }
        else
        {
            if (!readNextQuaternion(&scanner, &q))
            {
                ok = false;
                done = true;
            }
        }

        if (!done)
        {
            double tdbSec = daysToSeconds(jd - vesta::J2000);
            InterpolatedRotation::TimeOrientation record;
            record.tsec = tdbSec;
            record.orientation = q;
            orientations.push_back(record);
        }
    }

    if (!ok)
    {
        qDebug() << "Error in .q orientation file, record " << orientations.size();
        return NULL;
    }
    else
    {
        return new InterpolatedRotation(orientations);
    }
}


UniverseLoader::UniverseLoader() :
    m_dataSearchPath(".")
{
}


UniverseLoader::~UniverseLoader()
{
}


static double doubleValue(QVariant v, double defaultValue = 0.0)
{
    bool ok = false;
    double value = v.toDouble(&ok);
    if (ok)
    {
        return value;
    }
    else
    {
        return defaultValue;
    }
}


// Return distance in kilometers.
static double distanceValue(QVariant v, double defaultValue = 0.0)
{
    // TODO: need to parse units
    return doubleValue(v, defaultValue);
}


static Vector3d vec3Value(QVariant v, bool* ok)
{
    Vector3d result = Vector3d::Zero();
    bool loadOk = false;

    if (v.type() == QVariant::List)
    {
        QVariantList list = v.toList();
        if (list.length() == 3)
        {
            if (list.at(0).canConvert(QVariant::Double) &&
                list.at(1).canConvert(QVariant::Double) &&
                list.at(2).canConvert(QVariant::Double))
            {
                result = Vector3d(list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble());
                loadOk = true;
            }
        }
    }

    if (ok)
    {
        *ok = loadOk;
    }

    return result;
}


static Spectrum colorValue(QVariant v, const Spectrum& defaultValue)
{
    Spectrum result = defaultValue;

    if (v.type() == QVariant::List)
    {
        bool ok = false;
        Vector3d vec = vec3Value(v, &ok);
        if (ok)
        {
            result = Spectrum(float(vec.x()), float(vec.y()), float(vec.z()));
        }
    }
    else if (v.type() == QVariant::String)
    {
        QColor c = v.value<QColor>();
        result = Spectrum(c.redF(), c.greenF(), c.blueF());
    }

    return result;
}


static Quaterniond quaternionValue(QVariant v, bool* ok)
{
    Quaterniond result = Quaterniond::Identity();
    bool loadOk = false;

    if (v.type() == QVariant::List)
    {
        QVariantList list = v.toList();
        if (list.length() == 4)
        {
            if (list.at(0).canConvert(QVariant::Double) &&
                list.at(1).canConvert(QVariant::Double) &&
                list.at(2).canConvert(QVariant::Double) &&
                list.at(3).canConvert(QVariant::Double))
            {
                result = Quaterniond(list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble(), list.at(3).toDouble());
                loadOk = true;
            }
        }
    }

    if (ok)
    {
        *ok = loadOk;
    }

    return result;
}



static double angleValue(QVariant v, double defaultValue = 0.0)
{
    bool ok = false;
    double value = v.toDouble(&ok);
    if (ok)
    {
        return toRadians(value);
    }
    else
    {
        return defaultValue;
    }
}


static double durationValue(QVariant v, double defaultValue = 0.0)
{
    bool ok = false;
    double value = v.toDouble(&ok);
    if (ok)
    {
        return value;
    }
    else
    {
        return defaultValue;
    }
}


// Parse a date value. This can be either a double precision Julian date
// or an ISO 8601 date string with an optional time system suffix.
static double dateValue(QVariant v, bool* ok)
{
    double tsec = 0.0;

    if (v.type() == QVariant::String)
    {
        QString dateString = v.toString();
        QDateTime d = QDateTime::fromString(dateString);
        if (d.isValid())
        {
            *ok = true;
            GregorianDate date(d.date().year(), d.date().month(), d.date().day(),
                               d.time().hour(), d.time().minute(), d.time().second(), d.time().msec() * 1000,
                               TimeScale_TDB);
            tsec = date.toTDBSec();
        }
        else
        {
            *ok = false;
        }
    }
    else if (v.type() == QVariant::Double || v.type() == QVariant::Int)
    {
        *ok = true;
        double jd = v.toDouble();
        tsec = daysToSeconds(jd - vesta::J2000);
    }
    else
    {
        *ok = false;
    }

    return tsec;
}


vesta::Trajectory* loadFixedTrajectory(const QVariantMap& info)
{
    bool ok = false;
    Vector3d position = vec3Value(info.value("position"), &ok);
    if (!ok)
    {
        qDebug() << "Invalid or missing position given for FixedPoint trajectory.";
        return NULL;
    }

    return new FixedPointTrajectory(position);
}


vesta::Trajectory*
loadKeplerianTrajectory(const QVariantMap& info)
{
    double sma = doubleValue(info.value("semiMajorAxis"), 0.0);
    if (sma <= 0.0)
    {
        qDebug() << "Invalid semimajor axis given for Keplerian orbit.";
        return NULL;
    }

    double period = doubleValue(info.value("period"), 0.0);
    if (period <= 0.0)
    {
        qDebug() << "Invalid period given for Keplerian orbit.";
        return NULL;
    }

    OrbitalElements elements;
    elements.eccentricity = doubleValue(info.value("eccentricity"));
    elements.inclination = toRadians(doubleValue(info.value("inclination")));
    elements.meanMotion = toRadians(360.0) / daysToSeconds(period);
    elements.longitudeOfAscendingNode = toRadians(doubleValue(info.value("ascendingNode")));
    elements.argumentOfPeriapsis = toRadians(doubleValue(info.value("argumentOfPeriapsis")));
    elements.meanAnomalyAtEpoch = toRadians(doubleValue(info.value("meanAnomaly")));
    elements.periapsisDistance = (1.0 - elements.eccentricity) * sma;

    KeplerianTrajectory* trajectory = new KeplerianTrajectory(elements);

    return trajectory;
}


vesta::Trajectory*
UniverseLoader::loadBuiltinTrajectory(const QVariantMap& info)
{
    if (info.contains("name"))
    {
        QString name = info.value("name").toString();
        return m_builtinOrbits[name].ptr();
    }
    else
    {
        qDebug() << "Builtin trajectory is missing name.";
        return NULL;
    }
}


vesta::Trajectory*
UniverseLoader::loadInterpolatedStatesTrajectory(const QVariantMap& info)
{
    if (info.contains("source"))
    {
        QString name = info.value("source").toString();

        QString fileName = dataFileName(name);
        if (name.toLower().endsWith(".xyzv"))
        {
            return LoadXYZVTrajectory(fileName);
        }
        else if (name.toLower().endsWith(".xyz"))
        {
            return LoadXYZTrajectory(fileName);
        }
        else
        {
            qDebug() << "Unknown sampled trajectory format.";
            return NULL;
        }
    }
    else
    {
        qDebug() << "No source file specified for sampled trajectory.";
        return NULL;
    }
}


vesta::Trajectory*
UniverseLoader::loadTleTrajectory(const QVariantMap& info)
{
    QVariant nameVar = info.value("name");
    QVariant line1Var = info.value("line1");
    QVariant line2Var = info.value("line2");
    QVariant sourceVar = info.value("source");

    if (nameVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing name for TLE trajectory";
        return NULL;
    }

    if (line1Var.type() != QVariant::String)
    {
        qDebug() << "Bad or missing first line (line1) for TLE trajectory";
        return NULL;
    }

    if (line2Var.type() != QVariant::String)
    {
        qDebug() << "Bad or missing second line (line2) for TLE trajectory";
        return NULL;
    }

    TleTrajectory* tleTrajectory = TleTrajectory::Create(line1Var.toString().toAscii().data(),
                                                         line2Var.toString().toAscii().data());
    if (!tleTrajectory)
    {
        qDebug() << "Invalid TLE data for " << nameVar.toString();
        return NULL;
    }

    return tleTrajectory;
}


vesta::Trajectory*
UniverseLoader::loadTrajectory(const QVariantMap& map)
{
    QVariant typeData = map.value("type");
    if (typeData.type() != QVariant::String)
    {
        qDebug() << "Trajectory definition is missing type.";
    }

    QString type = typeData.toString();
    if (type == "FixedPoint")
    {
        return loadFixedTrajectory(map);
    }
    else if (type == "Keplerian")
    {
        return loadKeplerianTrajectory(map);
    }
    else if (type == "Builtin")
    {
        return loadBuiltinTrajectory(map);
    }
    else if (type == "InterpolatedStates")
    {
        return loadInterpolatedStatesTrajectory(map);
    }
    else if (type == "TLE")
    {
        return loadTleTrajectory(map);
    }
    else
    {
        qDebug() << "Unknown trajectory type " << type;
    }

    return NULL;
}


vesta::RotationModel*
loadFixedRotationModel(const QVariantMap& map)
{
    QVariant quatVar = map.value("quaternion");
    if (quatVar.isValid())
    {
        bool ok = false;
        Quaterniond q = quaternionValue(quatVar, &ok);
        if (!ok)
        {
            qDebug() << "Invalid quaternion given for FixedRotation";
            return NULL;
        }
        else
        {
            return new FixedRotationModel(q);
        }
    }

    return NULL;
}


vesta::RotationModel*
loadUniformRotationModel(const QVariantMap& map)
{
    double inclination   = angleValue(map.value("inclination"));
    double ascendingNode = angleValue(map.value("ascendingNode"));
    double meridianAngle = angleValue(map.value("meridianAngle"));
    double period        = durationValue(map.value("period"), 0.0);

    Vector3d axis = (AngleAxisd(ascendingNode, Vector3d::UnitZ()) * AngleAxisd(inclination, Vector3d::UnitX())) * Vector3d::UnitZ();
    //Vector3d axis = (AngleAxisd(inclination, Vector3d::UnitX()) * AngleAxisd(ascendingNode, Vector3d::UnitZ())) * Vector3d::UnitZ();
    double rotationRate = 2 * PI / daysToSeconds(period);

    //return new UniformRotationModel(axis, rotationRate, meridianAngle);
    return new SimpleRotationModel(inclination, ascendingNode, rotationRate, meridianAngle, 0.0);
}


vesta::RotationModel*
UniverseLoader::loadBuiltinRotationModel(const QVariantMap& info)
{
    if (info.contains("name"))
    {
        QString name = info.value("name").toString();
        return m_builtinRotations[name].ptr();
    }
    else
    {
        qDebug() << "Builtin rotation model is missing name.";
        return NULL;
    }
}


vesta::RotationModel*
UniverseLoader::loadInterpolatedRotationModel(const QVariantMap& info)
{
    if (info.contains("source"))
    {
        QString name = info.value("source").toString();

        QString fileName = dataFileName(name);
        if (name.toLower().endsWith(".q"))
        {
            return LoadInterpolatedRotation(fileName);
        }
        else
        {
            qDebug() << "Unknown interpolated rotation format.";
            return NULL;
        }
    }
    else
    {
        qDebug() << "No source file specified for interpolated rotation.";
        return NULL;
    }
}


vesta::RotationModel*
UniverseLoader::loadRotationModel(const QVariantMap& map)
{
    QVariant typeVar = map.value("type");
    if (typeVar.type() != QVariant::String)
    {
        qDebug() << "RotationModel definition is missing type.";
    }

    QString type = typeVar.toString();
    if (type == "Fixed")
    {
        return loadFixedRotationModel(map);
    }
    else if (type == "Uniform")
    {
        return loadUniformRotationModel(map);
    }
    else if (type == "Builtin")
    {
        return loadBuiltinRotationModel(map);
    }
    else if (type == "Interpolated")
    {
        return loadInterpolatedRotationModel(map);
    }
    else
    {
        qDebug() << "Unknown rotation model type " << type;
    }

    return NULL;
}


vesta::InertialFrame* loadInertialFrame(const QString& name)
{
    if (name == "EclipticJ2000")
    {
        return InertialFrame::eclipticJ2000();
    }
    else if (name == "EquatorJ2000")
    {
        return InertialFrame::equatorJ2000();
    }
    else if (name == "EquatorB1950")
    {
        return InertialFrame::equatorB1950();
    }
    else if (name == "ICRF")
    {
        return InertialFrame::icrf();
    }
    qDebug() << "Inertial Frame: " << name;

    return NULL;
}


vesta::Frame* loadBodyFixedFrame(const QVariantMap& map,
                                 const UniverseCatalog* catalog)
{
    QVariant bodyVar = map.value("body");
    if (bodyVar.type() != QVariant::String)
    {
        qDebug() << "BodyFixed frame is missing body name.";
        return NULL;
    }

    QString bodyName = bodyVar.toString();
    Entity* body = catalog->find(bodyName);
    if (body)
    {
        BodyFixedFrame* frame = new BodyFixedFrame(body);
        return frame;
    }
    else
    {
        qDebug() << "BodyFixed frame refers to unknown body " << bodyName;
        return NULL;
    }
}


static bool parseAxisLabel(const QString& label, TwoVectorFrame::Axis* axis)
{
    bool validLabel = true;

    QString lcLabel = label.toLower();
    if (lcLabel == "x" || lcLabel == "+x")
    {
        *axis = TwoVectorFrame::PositiveX;
    }
    else if (lcLabel == "y" || lcLabel == "+y")
    {
        *axis = TwoVectorFrame::PositiveY;
    }
    else if (lcLabel == "z" || lcLabel == "+z")
    {
        *axis = TwoVectorFrame::PositiveZ;
    }
    else if (lcLabel == "-x")
    {
        *axis = TwoVectorFrame::NegativeX;
    }
    else if (lcLabel == "-y")
    {
        *axis = TwoVectorFrame::NegativeY;
    }
    else if (lcLabel == "-z")
    {
        *axis = TwoVectorFrame::NegativeZ;
    }
    else
    {
        validLabel = false;
    }

    return validLabel;
}


TwoVectorFrameDirection*
loadRelativePosition(const QVariantMap& map,
                     const UniverseCatalog* catalog)
{
    QVariant observerVar = map.value("observer");
    QVariant targetVar = map.value("target");

    if (observerVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing observer for RelativePosition direction";
        return NULL;
    }

    if (targetVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing target for RelativePosition direction";
        return NULL;
    }

    Entity* observer = catalog->find(observerVar.toString());
    if (!observer)
    {
        qDebug() << "Observer body " << observerVar.toString() << " for RelativePosition direction not found";
        return NULL;
    }

    Entity* target = catalog->find(targetVar.toString());
    if (!target)
    {
        qDebug() << "Target body " << targetVar.toString() << " for RelativePosition direction not found";
        return NULL;
    }

    return new RelativePositionVector(observer, target);
}


TwoVectorFrameDirection*
loadRelativeVelocity(const QVariantMap& map,
                     const UniverseCatalog* catalog)
{
    QVariant observerVar = map.value("observer");
    QVariant targetVar = map.value("target");

    if (observerVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing observer for RelativeVelocity direction";
        return NULL;
    }

    if (targetVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing target for RelativeVelocity direction";
        return NULL;
    }

    Entity* observer = catalog->find(observerVar.toString());
    if (!observer)
    {
        qDebug() << "Observer body " << observerVar.toString() << " for RelativeVelocity direction not found";
        return NULL;
    }

    Entity* target = catalog->find(targetVar.toString());
    if (!target)
    {
        qDebug() << "Target body " << targetVar.toString() << " for RelativeVelocity direction not found";
        return NULL;
    }

    return new RelativeVelocityVector(observer, target);
}


TwoVectorFrameDirection*
loadFrameVector(const QVariantMap& map,
                const UniverseCatalog* catalog)
{
    QVariant typeVar = map.value("type");
    if (typeVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing type for TwoVector frame direction.";
        return NULL;
    }

    QVariant type = typeVar.toString();
    if (type == "RelativePosition")
    {
        return loadRelativePosition(map, catalog);
    }
    else if (type == "RelativeVelocity")
    {
        return loadRelativeVelocity(map, catalog);
    }
    else
    {
        qDebug() << "Unknoown TwoVector frame direction type " << type;
        return NULL;
    }
}


vesta::Frame* loadTwoVectorFrame(const QVariantMap& map,
                                 const UniverseCatalog* catalog)
{
    QVariant primaryVar = map.value("primary");
    QVariant primaryAxisVar = map.value("primaryAxis");
    QVariant secondaryVar = map.value("secondary");
    QVariant secondaryAxisVar = map.value("secondaryAxis");

    if (primaryVar.type() != QVariant::Map)
    {
        qDebug() << "Invalid or missing primary direction in TwoVector frame";
        return NULL;
    }

    if (secondaryVar.type() != QVariant::Map)
    {
        qDebug() << "Invalid or missing secondary direction in TwoVector frame";
        return NULL;
    }

    if (primaryAxisVar.type() != QVariant::String)
    {
        qDebug() << "Invalid or missing primary axis in TwoVector frame";
        return NULL;
    }

    if (secondaryAxisVar.type() != QVariant::String)
    {
        qDebug() << "Invalid or missing secondary axis in TwoVector frame";
        return NULL;
    }

    TwoVectorFrame::Axis primaryAxis = TwoVectorFrame::PositiveX;
    TwoVectorFrame::Axis secondaryAxis = TwoVectorFrame::PositiveX;
    if (!parseAxisLabel(primaryAxisVar.toString(), &primaryAxis))
    {
        qDebug() << "Invalid label " << primaryAxisVar.toString() << " for primary axis in TwoVector frame";
        return NULL;
    }

    if (!parseAxisLabel(secondaryAxisVar.toString(), &secondaryAxis))
    {
        qDebug() << "Invalid label " << secondaryAxisVar.toString() << " for secondary axis in TwoVector frame";
        return NULL;
    }

    if (!TwoVectorFrame::orthogonalAxes(primaryAxis, secondaryAxis))
    {
        qDebug() << "Bad two vector frame. Primary and secondary axes must be orthogonal";
        return NULL;
    }

    TwoVectorFrameDirection* primaryDir = loadFrameVector(primaryVar.toMap(), catalog);
    TwoVectorFrameDirection* secondaryDir = loadFrameVector(secondaryVar.toMap(), catalog);

    if (primaryDir && secondaryDir)
    {
        return new TwoVectorFrame(primaryDir, primaryAxis, secondaryDir, secondaryAxis);
    }
    else
    {
        return NULL;
    }
}


vesta::Frame*
UniverseLoader::loadFrame(const QVariantMap& map,
                          const UniverseCatalog* catalog)
{
    QVariant typeVar = map.value("type");
    if (typeVar.type() != QVariant::String)
    {
        qDebug() << "Frame definition is missing type.";
    }

    QString type = typeVar.toString();
    if (type == "BodyFixed")
    {
        return loadBodyFixedFrame(map, catalog);
    }
    else if (type == "TwoVector")
    {
        return loadTwoVectorFrame(map, catalog);
    }
    else
    {
        qDebug() << "Unknown frame type " << type;
    }

    return NULL;
}


vesta::Arc*
UniverseLoader::loadArc(const QVariantMap& map,
                        const UniverseCatalog* catalog)
{
    vesta::Arc* arc = new vesta::Arc();

    QVariant centerData = map.value("center");
    QVariant trajectoryData = map.value("trajectory");
    QVariant rotationModelData = map.value("rotationModel");
    QVariant trajectoryFrameData = map.value("trajectoryFrame");
    QVariant bodyFrameData = map.value("bodyFrame");

    arc->setDuration(daysToSeconds(100 * 365.25));

    if (centerData.type() == QVariant::String)
    {
        QString centerName = centerData.toString();
        arc->setCenter(catalog->find(centerName));
    }
    else
    {
        qDebug() << "Missing center for object.";
        delete arc;
        return NULL;
    }

    if (trajectoryData.type() == QVariant::Map)
    {
        Trajectory* trajectory = loadTrajectory(trajectoryData.toMap());
        if (trajectory)
        {
            arc->setTrajectory(trajectory);
        }
    }

    if (rotationModelData.type() == QVariant::Map)
    {
        RotationModel* rotationModel = loadRotationModel(rotationModelData.toMap());
        if (rotationModel)
        {
            arc->setRotationModel(rotationModel);
        }
    }

    if (trajectoryFrameData.type() == QVariant::String)
    {
        // Inertial frame name
        InertialFrame* frame = loadInertialFrame(trajectoryFrameData.toString());
        if (frame)
        {
            arc->setTrajectoryFrame(frame);
        }
    }
    else if (trajectoryFrameData.type() == QVariant::Map)
    {
        Frame* frame = loadFrame(trajectoryFrameData.toMap(), catalog);
        if (frame)
        {
            arc->setTrajectoryFrame(frame);
        }
    }

    if (bodyFrameData.type() == QVariant::String)
    {
        // Inertial frame name
        InertialFrame* frame = loadInertialFrame(bodyFrameData.toString());
        if (frame)
        {
            arc->setTrajectoryFrame(frame);
        }
    }
    else if (bodyFrameData.type() == QVariant::Map)
    {
        Frame* frame = loadFrame(bodyFrameData.toMap(), catalog);
        if (frame)
        {
            arc->setBodyFrame(frame);
        }
    }

    return arc;
}


static TiledMap*
loadTiledMap(const QVariantMap& map, TextureMapLoader* textureLoader)
{
    QString type = map.value("type").toString();
    if (type == "WMS")
    {
        QVariant layerVar = map.value("layer");
        QVariant levelCountVar = map.value("levelCount");
        QVariant tileSizeVar = map.value("tileSize");

        if (layerVar.type() != QVariant::String)
        {
            qDebug() << "Bad or missing layer name for WMS tiled texture";
            return NULL;
        }

        if (!levelCountVar.canConvert(QVariant::Int))
        {
            qDebug() << "Bad or missing level count for WMS tiled texture";
            return NULL;
        }

        if (!tileSizeVar.canConvert(QVariant::Int))
        {
            qDebug() << "Bad or missing tileSize for WMS tiled texture";
            return NULL;
        }

        QString layer = layerVar.toString();
        int levelCount = levelCountVar.toInt();
        int tileSize = tileSizeVar.toInt();

        // Enforce some limits on tile size and level count
        levelCount = std::max(1, std::min(16, levelCount));
        tileSize = std::max(128, std::min(8192, tileSize));

        return new WMSTiledMap(textureLoader, layer, tileSize, levelCount);
    }
    else if (type == "MultiWMS")
    {
        QVariant baseLayerVar = map.value("baseLayer");
        QVariant baseLevelCountVar = map.value("baseLevelCount");
        QVariant detailLayerVar = map.value("detailLayer");
        QVariant detailLevelCountVar = map.value("detailLevelCount");
        QVariant tileSizeVar = map.value("tileSize");

        if (baseLayerVar.type() != QVariant::String)
        {
            qDebug() << "Bad or missing base layer name for MultiWMS tiled texture";
            return NULL;
        }

        if (!baseLevelCountVar.canConvert(QVariant::Int))
        {
            qDebug() << "Bad or missing base level count for MultiWMS tiled texture";
            return NULL;
        }

        if (detailLayerVar.type() != QVariant::String)
        {
            qDebug() << "Bad or missing detail layer name for MultiWMS tiled texture";
            return NULL;
        }

        if (!detailLevelCountVar.canConvert(QVariant::Int))
        {
            qDebug() << "Bad or missing detail level count for MultiWMS tiled texture";
            return NULL;
        }

        if (!tileSizeVar.canConvert(QVariant::Int))
        {
            qDebug() << "Bad or missing tileSize for MultiWMS tiled texture";
            return NULL;
        }

        QString baseLayer = baseLayerVar.toString();
        QString detailLayer = detailLayerVar.toString();
        int baseLevelCount = baseLevelCountVar.toInt();
        int detailLevelCount = detailLevelCountVar.toInt();
        int tileSize = tileSizeVar.toInt();

        // Enforce some limits on tile size and level count
        baseLevelCount = std::max(1, std::min(16, baseLevelCount));
        detailLevelCount = std::max(baseLevelCount + 1, std::min(16, detailLevelCount));
        tileSize = std::max(128, std::min(8192, tileSize));

        return new MultiWMSTiledMap(textureLoader, baseLayer, baseLevelCount, detailLayer, detailLevelCount, tileSize);
    }
    else
    {
        qDebug() << "Unknown tiled map type.";
        return NULL;
    }
}


static MeshGeometry*
loadMeshFile(const QString& fileName, TextureMapLoader* textureLoader)
{
    MeshGeometry* meshGeometry = MeshGeometry::loadFromFile(fileName.toUtf8().data(), textureLoader);
    if (!meshGeometry)
    {
        //QMessageBox::warning(NULL, "Missing mesh file", QString("Error opening mesh file %1.").arg(fileName));
    }
    else
    {
        // Optimize the mesh. The optimizations can be expensive for large meshes, but they can dramatically
        // improve rendering performance. The best solution is to use mesh files that are already optimized, but
        // the average model loaded off the web benefits from some preprocessing at load time.
        meshGeometry->mergeSubmeshes();
        meshGeometry->uniquifyVertices();
    }

    return meshGeometry;
}


Geometry*
UniverseLoader::loadGlobeGeometry(const QVariantMap& map)
{
    Vector3d radii = Vector3d::Zero();

    if (map.value("radius").canConvert(QVariant::Double))
    {
        double r = map.value("radius").toDouble();
        radii = Vector3d::Constant(r);
    }
    else if (map.contains("radii"))
    {
        bool ok = false;
        radii = vec3Value(map.value("radii"), &ok);
        if (!ok)
        {
            qDebug() << "Invalid radii given for globe geometry.";
            return NULL;
        }
    }

    WorldGeometry* world = new WorldGeometry();
    world->setEllipsoid(radii.cast<float>() * 2.0f);

    TextureProperties props;
    props.addressS = TextureProperties::Wrap;
    props.addressT = TextureProperties::Clamp;

    QVariant baseMapVar = map.value("baseMap");
    if (baseMapVar.type() == QVariant::String)
    {
        QString baseMapName = baseMapVar.toString();
        if (m_textureLoader.isValid())
        {
            TextureMap* tex = m_textureLoader->loadTexture(textureFileName(baseMapName).toUtf8().data(), props);
            world->setBaseMap(tex);
        }
    }
    else if (baseMapVar.type() == QVariant::Map)
    {
        TiledMap* tiledMap = loadTiledMap(baseMapVar.toMap(), m_textureLoader.ptr());
        if (tiledMap)
        {
            world->setBaseMap(tiledMap);
        }
    }

    if (map.contains("normalMap"))
    {
        TextureProperties normalMapProps;
        normalMapProps.addressS = TextureProperties::Wrap;
        normalMapProps.addressT = TextureProperties::Clamp;
        normalMapProps.usage = TextureProperties::CompressedNormalMap;

        QString normalMapBase = map.value("normalMap").toString();
        if (m_textureLoader.isValid())
        {
            TextureMap* normalTex = m_textureLoader->loadTexture(textureFileName(normalMapBase).toUtf8().data(), normalMapProps);
            world->setNormalMap(normalTex);
        }
    }

    QVariant emissiveVar = map.value("emissive");
    if (emissiveVar.type() == QVariant::Bool)
    {
        world->setEmissive(emissiveVar.toBool());
    }

    QVariant cloudMapVar = map.value("cloudMap");
    if (cloudMapVar.type() == QVariant::String)
    {
        TextureProperties cloudMapProps;
        cloudMapProps.addressS = TextureProperties::Wrap;
        cloudMapProps.addressT = TextureProperties::Clamp;

        QString cloudMapBase = cloudMapVar.toString();
        if (m_textureLoader.isValid())
        {
            TextureMap* cloudTex = m_textureLoader->loadTexture(textureFileName(cloudMapBase).toUtf8().data(), cloudMapProps);
            world->setCloudMap(cloudTex);
            world->setCloudAltitude(6.0f);
        }
    }

    QVariant atmosphereVar = map.value("atmosphere");
    if (atmosphereVar.type() == QVariant::String)
    {
        QString fileName = textureFileName(atmosphereVar.toString());
        QFile atmFile(fileName);
        if (atmFile.open(QIODevice::ReadOnly))
        {
            QByteArray data = atmFile.readAll();
            DataChunk chunk(data.data(), data.size());
            Atmosphere* atm = Atmosphere::LoadAtmScat(&chunk);
            if (atm)
            {
                atm->generateTextures();
                atm->addRef();
                world->setAtmosphere(atm);
            }
        }
    }

    return world;
}


Geometry*
UniverseLoader::loadMeshGeometry(const QVariantMap& map)
{
    MeshGeometry* mesh = NULL;

    // We permit two methods of scaling the mesh:
    //    1. Specifying the size will scale the mesh to fit in a sphere of that size
    //    2. Specifying scale will apply a scaling factor
    //
    // scale overrides size when it's present. If neither size nor scale is given, a default
    // size of 1.0 is used.
    double radius = doubleValue(map.value("size"), 1.0);
    double scale = doubleValue(map.value("scale"), 0.0);

    if (map.contains("source"))
    {
        QString sourceName = map.value("source").toString();
        mesh = loadMeshFile(modelFileName(sourceName), m_textureLoader.ptr());
        if (mesh)
        {
            if (scale != 0.0)
            {
                mesh->setMeshScale(float(scale));
            }
            else
            {
                mesh->setMeshScale(radius / mesh->boundingSphereRadius());
            }
        }
    }

    return mesh;
}


Geometry*
UniverseLoader::loadSensorGeometry(const QVariantMap& map, const UniverseCatalog* catalog)
{
    QVariant targetVar = map.value("target");
    QVariant rangeVar = map.value("range");
    QVariant shapeVar = map.value("shape");
    QVariant horizontalFovVar = map.value("horizontalFov");
    QVariant verticalFovVar = map.value("verticalFov");
    QVariant frustumColorVar = map.value("frustumColor");
    QVariant frustumBaseColorVar = map.value("frustumBaseColor");
    QVariant frustumOpacityVar = map.value("frustumOpacity");
    QVariant gridOpacityVar = map.value("gridOpacity");

    if (targetVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing target for sensor geometry";
        return NULL;
    }

    if (!rangeVar.canConvert(QVariant::Double))
    {
        qDebug() << "Bad or missing range for sensor geometry";
        return NULL;
    }

    double range = distanceValue(rangeVar, 1.0);
    QString shape = shapeVar.toString();
    double horizontalFov = angleValue(horizontalFovVar, 5.0);
    double verticalFov = angleValue(verticalFovVar, 5.0);
    Spectrum frustumColor = colorValue(frustumColorVar, Spectrum(1.0f, 1.0f, 1.0f));
    Spectrum frustumBaseColor = colorValue(frustumColorVar, Spectrum(1.0f, 1.0f, 1.0f));
    double frustumOpacity = doubleValue(frustumOpacityVar, 0.3);
    double gridOpacity = doubleValue(gridOpacityVar, 0.15);

    Entity* target = catalog->find(targetVar.toString());
    if (!target)
    {
        qDebug() << "Target for sensor geometry not found";
        return NULL;
    }

    SensorFrustumGeometry* sensorFrustum = new SensorFrustumGeometry();
    sensorFrustum->setTarget(target);
    sensorFrustum->setColor(frustumColor);
    sensorFrustum->setOpacity(frustumOpacity);
    sensorFrustum->setRange(range);
    sensorFrustum->setFrustumAngles(horizontalFov, verticalFov);
    if (shape == "elliptical")
    {
        sensorFrustum->setFrustumShape(SensorFrustumGeometry::Elliptical);
    }
    else if (shape == "rectangular")
    {
        sensorFrustum->setFrustumShape(SensorFrustumGeometry::Rectangular);
    }

    sensorFrustum->setSource(catalog->find(m_currentBodyName));

    return sensorFrustum;
}


static vesta::ArrowGeometry*
loadAxesGeometry(const QVariantMap& map)
{
    ArrowGeometry* axes = new ArrowGeometry(1.0f, 0.005f, 0.05f, 0.01f);
    axes->setVisibleArrows(ArrowGeometry::AllAxes);
    axes->setScale(float(doubleValue(map.value("scale"), 1.0)));

    return axes;
}


vesta::Geometry*
UniverseLoader::loadGeometry(const QVariantMap& map, const UniverseCatalog* catalog)
{
    Geometry* geometry = NULL;

    QVariant typeValue = map.value("type");
    if (typeValue.type() != QVariant::String)
    {
        qDebug() << "Bad or missing type for geometry.";
        return NULL;
    }

    QString type = typeValue.toString();

    if (type == "Globe")
    {
        geometry = loadGlobeGeometry(map);
    }
    else if (type == "Mesh")
    {
        geometry = loadMeshGeometry(map);
    }
    else if (type == "Axes")
    {
        geometry = loadAxesGeometry(map);
    }
    else if (type == "Sensor")
    {
        geometry = loadSensorGeometry(map, catalog);
    }
    else
    {
        qDebug() << "Unknown type " << type << " for geometry.";
    }

    return geometry;
}


Visualizer*
loadBodyAxesVisualizer(const QVariantMap& map)
{
    bool ok = false;
    double size = map.value("size", 1.0).toDouble(&ok);
    if (!ok)
    {
        qDebug() << "Bad size given for BodyAxes visualizer";
        return NULL;
    }
    else
    {
        return new AxesVisualizer(AxesVisualizer::BodyAxes, size);
    }
}


Visualizer*
loadFrameAxesVisualizer(const QVariantMap& map)
{
    bool ok = false;
    double size = map.value("size", 1.0).toDouble(&ok);
    if (!ok)
    {
        qDebug() << "Bad size given for FrameAxes visualizer";
        return NULL;
    }
    else
    {
        AxesVisualizer* axes = new AxesVisualizer(AxesVisualizer::FrameAxes, size);
        axes->arrows()->setOpacity(0.3f);
        return axes;
    }
}


Visualizer*
loadBodyDirectionVisualizer(const QVariantMap& map,
                            const UniverseCatalog* catalog)
{
    bool ok = false;
    double size = map.value("size", 1.0).toDouble(&ok);
    if (!ok)
    {
        qDebug() << "Bad size given for FrameAxes visualizer";
        return NULL;
    }

    QVariant targetVar = map.value("target");
    QVariant colorVar = map.value("color");
    Spectrum color = colorValue(colorVar, Spectrum::White());

    if (targetVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing target for BodyDirection visualizer";
        return NULL;
    }

    Entity* target = catalog->find(targetVar.toString());
    if (!target)
    {
        qDebug() << "Target body " << targetVar.toString() << " for BodyDirection visualizer not found";
        return NULL;
    }

    BodyDirectionVisualizer* direction = new BodyDirectionVisualizer(size, target);
    direction->setColor(color);

    return direction;
}


Visualizer*
UniverseLoader::loadVisualizer(const QVariantMap& map,
                               const UniverseCatalog* catalog)
{
    QVariant styleVar = map.value("style");
    if (styleVar.type() != QVariant::Map)
    {
        qDebug() << "Missing visualizer style.";
        return NULL;
    }

    QVariantMap style = styleVar.toMap();
    QVariant typeVar = style.value("type");
    if (typeVar.type() != QVariant::String)
    {
        qDebug() << "Bad or missing type for visualizer style.";
        return NULL;
    }

    QString type = typeVar.toString();
    if (type == "BodyAxes")
    {
        return loadBodyAxesVisualizer(style);
    }
    else if (type == "FrameAxes")
    {
        return loadFrameAxesVisualizer(style);
    }
    else if (type == "BodyDirection")
    {
        return loadBodyDirectionVisualizer(style, catalog);
    }
    else
    {
        qDebug() << "Unknown visualizer type " << type;
        return NULL;
    }
}


QStringList
UniverseLoader::loadSolarSystem(const QVariantMap& contents,
                                UniverseCatalog* catalog)
{
    qDebug() << contents["name"];

    QStringList bodyNames;

    if (!contents.contains("items"))
    {
        qDebug() << "No items defined.";
        return bodyNames;
    }

    if (contents["items"].type() != QVariant::List)
    {
        qDebug() << "items is not a list.";
        return bodyNames;
    }
    QVariantList items = contents["items"].toList();

    foreach (QVariant itemVar, items)
    {
        if (itemVar.type() != QVariant::Map)
        {
            qDebug() << "Invalid item in bodies list.";
        }
        else
        {
            QVariantMap item = itemVar.toMap();

            QString type = item.value("type").toString();
            if (type == "body" || type.isEmpty())
            {
                QString bodyName = item.value("name").toString();
                m_currentBodyName = bodyName;

                vesta::Body* body = dynamic_cast<Body*>(catalog->find(bodyName));
                if (body == NULL)
                {
                    // No body with this name exists, so create it
                    body = new vesta::Body();
                    body->setName(bodyName.toUtf8().data());

                    // Add the body to the catalog now so that it may be referenced by
                    // frames.
                    catalog->addBody(bodyName, body);
                }
                else
                {
                    // Body with the given name already exists; clear all information about
                    // it and reload.
                    body->setLightSource(NULL);
                    body->setGeometry(NULL);
                    body->setVisible(true);
                    body->chronology()->clearArcs();
                }

                if (item.contains("geometry"))
                {
                    QVariant geometryValue = item.value("geometry");
                    if (geometryValue.type() == QVariant::Map)
                    {
                        vesta::Geometry* geometry = loadGeometry(geometryValue.toMap(), catalog);
                        if (geometry)
                        {
                            body->setGeometry(geometry);
                        }
                    }
                    else
                    {
                        qDebug() << "Invalid geometry for body.";
                    }
                }

                vesta::Arc* arc = loadArc(item, catalog);

                if (!arc)
                {
                    delete body;
                }
                else
                {
                    body->chronology()->addArc(arc);
                }

                // Visible property
                body->setVisible(item.value("visible", true).toBool());

                bodyNames << bodyName;
            }
            else if (type == "Visualizer")
            {
                QVariant tagVar = item.value("tag");
                QVariant bodyVar = item.value("body");

                if (tagVar.type() != QVariant::String)
                {
                    qDebug() << "Bad or missing tag for visualizer";
                }
                else if (bodyVar.type() != QVariant::String)
                {
                    qDebug() << "Bad or missing body name for visualizer";
                }
                else
                {
                    QString tag = tagVar.toString();
                    QString bodyName = bodyVar.toString();

                    Entity* body = catalog->find(bodyName);
                    if (body == NULL)
                    {
                        qDebug() << "Can't find body " << bodyName << " for visualizer.";
                    }
                    else
                    {
                        Visualizer* visualizer = loadVisualizer(item, catalog);
                        if (visualizer)
                        {
                            body->setVisualizer(tag.toUtf8().data(), visualizer);
                        }
                    }
                }
            }
        }
    }

    return bodyNames;
}


void
UniverseLoader::addBuiltinOrbit(const QString& name, vesta::Trajectory* trajectory)
{
    m_builtinOrbits[name] = trajectory;
}


void
UniverseLoader::removeBuiltinOrbit(const QString& name)
{
    m_builtinOrbits.remove(name);
}


void
UniverseLoader::addBuiltinRotationModel(const QString& name, vesta::RotationModel* rotationModel)
{
    m_builtinRotations[name] = rotationModel;
}


void
UniverseLoader::removeBuiltinRotationModel(const QString& name)
{
    m_builtinRotations.remove(name);
}


void
UniverseLoader::setTextureLoader(vesta::TextureMapLoader *textureLoader)
{
    m_textureLoader = textureLoader;
}


void
UniverseLoader::setDataSearchPath(const QString& path)
{
    m_dataSearchPath = path;
}


void
UniverseLoader::setTextureSearchPath(const QString& path)
{
    m_textureSearchPath = path;
}


void
UniverseLoader::setModelSearchPath(const QString& path)
{
    m_modelSearchPath = path;
}


QString
UniverseLoader::dataFileName(const QString& fileName)
{
    return m_dataSearchPath + "/" + fileName;
}


QString
UniverseLoader::textureFileName(const QString& fileName)
{
    return m_textureSearchPath + "/" + fileName;
}


QString
UniverseLoader::modelFileName(const QString& fileName)
{
    return m_modelSearchPath + "/" + fileName;
}