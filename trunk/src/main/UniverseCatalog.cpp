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

#include "UniverseCatalog.h"

using namespace vesta;


UniverseCatalog::UniverseCatalog()
{
}


UniverseCatalog::~UniverseCatalog()
{
}


bool UniverseCatalog::contains(const QString& name) const
{
    return m_bodies.contains(name);
}


/** Lookup the VESTA body with the specified name.
  */
Entity* UniverseCatalog::find(const QString& name) const
{
    return m_bodies.value(name).ptr();
}


/** Look up the extra (i.e. non-VESTA) information for
  * named body. This method returns null if the named
  * body isn't found or if it doesn't have any extra
  * information.
  */
BodyInfo* UniverseCatalog::findInfo(const QString& name) const
{
    return m_info.value(name).ptr();
}


void UniverseCatalog::removeBody(const QString& name)
{
    m_bodies.remove(name);
    m_info.remove(name);
}


void UniverseCatalog::addBody(const QString& name, vesta::Entity* body, BodyInfo* info)
{
    m_bodies[name] = counted_ptr<Entity>(body);
    m_info[name] = info;
}


/** Set the addition information record for a body. This has
  * no effect if the named object doesn't exist in the catalog.
  */
void UniverseCatalog::setBodyInfo(const QString& name, BodyInfo* info)
{
    if (m_bodies.contains(name))
    {
        m_info[name] = info;
    }
}
