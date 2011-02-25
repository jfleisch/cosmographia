# Qt project file for Cosmographia

TEMPLATE = app
TARGET = Cosmographia
DESTDIR = build

QT += opengl
QT += network

#### App sources ####

MAIN_PATH = src/Main

APP_SOURCES = \
    $$MAIN_PATH/main.cpp \
    $$MAIN_PATH/Cosmographia.cpp \
    $$MAIN_PATH/UniverseView.cpp \
    $$MAIN_PATH/NetworkTextureLoader.cpp \
    $$MAIN_PATH/LocalImageLoader.cpp \
    $$MAIN_PATH/ChebyshevPolyTrajectory.cpp \
    $$MAIN_PATH/InterpolatedRotation.cpp \
    $$MAIN_PATH/InterpolatedStateTrajectory.cpp \
    $$MAIN_PATH/JPLEphemeris.cpp \
    $$MAIN_PATH/KeplerianSwarm.cpp \
    $$MAIN_PATH/LinearCombinationTrajectory.cpp \
    $$MAIN_PATH/TleTrajectory.cpp \
    $$MAIN_PATH/UniverseCatalog.cpp \
    $$MAIN_PATH/UniverseLoader.cpp \
    $$MAIN_PATH/TwoVectorFrame.cpp \
    $$MAIN_PATH/WMSRequester.cpp \
    $$MAIN_PATH/WMSTiledMap.cpp \
    $$MAIN_PATH/MultiWMSTiledMap.cpp \
    $$MAIN_PATH/astro/IAULunarRotationModel.cpp \
    $$MAIN_PATH/astro/Nutation.cpp \
    $$MAIN_PATH/astro/Precession.cpp \
    $$MAIN_PATH/compatibility/CatalogParser.cpp \
    $$MAIN_PATH/compatibility/Scanner.cpp \
    $$MAIN_PATH/compatibility/TransformCatalog.cpp

APP_HEADERS = \
    $$MAIN_PATH/Cosmographia.h \
    $$MAIN_PATH/UniverseView.h \
    $$MAIN_PATH/NetworkTextureLoader.h \
    $$MAIN_PATH/LocalImageLoader.h \
    $$MAIN_PATH/ChebyshevPolyTrajectory.h \
    $$MAIN_PATH/InterpolatedRotation.h \
    $$MAIN_PATH/InterpolatedStateTrajectory.h \
    $$MAIN_PATH/JPLEphemeris.h \
    $$MAIN_PATH/KeplerianSwarm.h \
    $$MAIN_PATH/LinearCombinationTrajectory.h \
    $$MAIN_PATH/TleTrajectory.h \
    $$MAIN_PATH/TwoVectorFrame.h \
    $$MAIN_PATH/UniverseCatalog.h \
    $$MAIN_PATH/UniverseLoader.h \
    $$MAIN_PATH/WMSRequester.h \
    $$MAIN_PATH/WMSTiledMap.h \
    $$MAIN_PATH/MultiWMSTiledMap.h \
    $$MAIN_PATH/astro/IAULunarRotationModel.h \
    $$MAIN_PATH/astro/Nutation.h \
    $$MAIN_PATH/astro/Precession.h \
    $$MAIN_PATH/astro/Rotation.h \
    $$MAIN_PATH/compatibility/CatalogParser.h \
    $$MAIN_PATH/compatibility/Scanner.h \
    $$MAIN_PATH/compatibility/TransformCatalog.h


VESTA_PATH = thirdparty/vesta
LIB3DS_PATH = thirdparty/lib3ds
GLEW_PATH = thirdparty/glew
NORADTLE_PATH = thirdparty/noradtle
QJSON_PATH = thirdparty/qjson

VESTA_SOURCES = \
    $$VESTA_PATH/Arc.cpp \
    $$VESTA_PATH/ArrowGeometry.cpp \
    $$VESTA_PATH/ArrowVisualizer.cpp \
    $$VESTA_PATH/Atmosphere.cpp \
    $$VESTA_PATH/AxesVisualizer.cpp \
    $$VESTA_PATH/BillboardGeometry.cpp \
    $$VESTA_PATH/Body.cpp \
    $$VESTA_PATH/BodyDirectionVisualizer.cpp \
    $$VESTA_PATH/BodyFixedFrame.cpp \
    $$VESTA_PATH/CelestialCoordinateGrid.cpp \
    $$VESTA_PATH/Chronology.cpp \
    $$VESTA_PATH/ConeGeometry.cpp \
    $$VESTA_PATH/ConstellationsLayer.cpp \
    $$VESTA_PATH/CubeMapFramebuffer.cpp \
    $$VESTA_PATH/DataChunk.cpp \
    $$VESTA_PATH/DDSLoader.cpp \
    $$VESTA_PATH/Debug.cpp \
    $$VESTA_PATH/Entity.cpp \
    $$VESTA_PATH/FixedPointTrajectory.cpp \
    $$VESTA_PATH/FixedRotationModel.cpp \
    $$VESTA_PATH/Frame.cpp \
    $$VESTA_PATH/Framebuffer.cpp \
    $$VESTA_PATH/Geometry.cpp \
    $$VESTA_PATH/GregorianDate.cpp \
    $$VESTA_PATH/HierarchicalTiledMap.cpp \
    $$VESTA_PATH/InertialFrame.cpp \
    $$VESTA_PATH/KeplerianTrajectory.cpp \
    $$VESTA_PATH/LabelGeometry.cpp \
    $$VESTA_PATH/LightSource.cpp \
    $$VESTA_PATH/MapLayer.cpp \
    $$VESTA_PATH/MeshGeometry.cpp \
    $$VESTA_PATH/NadirVisualizer.cpp \
    $$VESTA_PATH/Observer.cpp \
    $$VESTA_PATH/OrbitalElements.cpp \
    $$VESTA_PATH/ParticleSystemGeometry.cpp \
    $$VESTA_PATH/PlanarProjection.cpp \
    $$VESTA_PATH/PlaneGeometry.cpp \
    $$VESTA_PATH/PlanetaryRings.cpp \
    $$VESTA_PATH/PlanetGridLayer.cpp \
    $$VESTA_PATH/PlaneVisualizer.cpp \
    $$VESTA_PATH/PrimitiveBatch.cpp \
    $$VESTA_PATH/QuadtreeTile.cpp \
    $$VESTA_PATH/RenderContext.cpp \
    $$VESTA_PATH/LightingEnvironment.cpp \
    $$VESTA_PATH/SensorFrustumGeometry.cpp \
    $$VESTA_PATH/SensorVisualizer.cpp \
    $$VESTA_PATH/ShaderBuilder.cpp \
    $$VESTA_PATH/SkyImageLayer.cpp \
    $$VESTA_PATH/Spectrum.cpp \
    $$VESTA_PATH/StarCatalog.cpp \
    $$VESTA_PATH/StarsLayer.cpp \
    $$VESTA_PATH/Submesh.cpp \
    $$VESTA_PATH/TextureFont.cpp \
    $$VESTA_PATH/TextureMap.cpp \
    $$VESTA_PATH/TextureMapLoader.cpp \
    $$VESTA_PATH/TrajectoryGeometry.cpp \
    $$VESTA_PATH/TwoBodyRotatingFrame.cpp \
    $$VESTA_PATH/UniformRotationModel.cpp \
    $$VESTA_PATH/Universe.cpp \
    $$VESTA_PATH/UniverseRenderer.cpp \
    $$VESTA_PATH/VelocityVisualizer.cpp \
    $$VESTA_PATH/VertexArray.cpp \
    $$VESTA_PATH/VertexBuffer.cpp \
    $$VESTA_PATH/VertexPool.cpp \
    $$VESTA_PATH/VertexSpec.cpp \
    $$VESTA_PATH/Visualizer.cpp \
    $$VESTA_PATH/WorldGeometry.cpp \
    $$VESTA_PATH/interaction/ObserverController.cpp \
    $$VESTA_PATH/internal/InputDataStream.cpp \
    $$VESTA_PATH/internal/OutputDataStream.cpp \
    $$VESTA_PATH/internal/ObjLoader.cpp

VESTA_HEADERS = \
    $$VESTA_PATH/Arc.h \
    $$VESTA_PATH/ArrowGeometry.h \
    $$VESTA_PATH/ArrowVisualizer.h \
    $$VESTA_PATH/Atmosphere.h \
    $$VESTA_PATH/AxesVisualizer.h \
    $$VESTA_PATH/BillboardGeometry.h \
    $$VESTA_PATH/Body.h \
    $$VESTA_PATH/BodyDirectionVisualizer.h \
    $$VESTA_PATH/BodyFixedFrame.h \
    $$VESTA_PATH/BoundingBox.h \
    $$VESTA_PATH/BoundingSphere.h \
    $$VESTA_PATH/CelestialCoordinateGrid.h \
    $$VESTA_PATH/Chronology.h \
    $$VESTA_PATH/ConeGeometry.h \
    $$VESTA_PATH/ConstellationsLayer.h \
    $$VESTA_PATH/CubeMapFramebuffer.h \
    $$VESTA_PATH/DataChunk.h \
    $$VESTA_PATH/Debug.h \
    $$VESTA_PATH/DDSLoader.h \
    $$VESTA_PATH/Entity.h \
    $$VESTA_PATH/FadeRange.h \
    $$VESTA_PATH/Frame.h \
    $$VESTA_PATH/Framebuffer.h \
    $$VESTA_PATH/Frustum.h \
    $$VESTA_PATH/FixedPointTrajectory.h \
    $$VESTA_PATH/FixedRotationModel.h \
    $$VESTA_PATH/Geometry.h \
    $$VESTA_PATH/GregorianDate.h \
    $$VESTA_PATH/HierarchicalTiledMap.h \
    $$VESTA_PATH/InertialFrame.h \
    $$VESTA_PATH/IntegerTypes.h \
    $$VESTA_PATH/Intersect.h \
    $$VESTA_PATH/JavaCallbackTrajectory.h \
    $$VESTA_PATH/KeplerianTrajectory.h \
    $$VESTA_PATH/LabelGeometry.h \
    $$VESTA_PATH/LightSource.h \
    $$VESTA_PATH/MapLayer.h \
    $$VESTA_PATH/Material.h \
    $$VESTA_PATH/MeshGeometry.h \
    $$VESTA_PATH/NadirVisualizer.h \
    $$VESTA_PATH/Object.h \
    $$VESTA_PATH/Observer.h \
    $$VESTA_PATH/OGLHeaders.h \
    $$VESTA_PATH/OrbitalElements.h \
    $$VESTA_PATH/ParticleSystemGeometry.h \
    $$VESTA_PATH/PickResult.h \
    $$VESTA_PATH/PlanarProjection.h \
    $$VESTA_PATH/PlaneGeometry.h \
    $$VESTA_PATH/PlanetaryRings.h \
    $$VESTA_PATH/PlanetGridLayer.h \
    $$VESTA_PATH/PlaneVisualizer.h \
    $$VESTA_PATH/PrimitiveBatch.h \
    $$VESTA_PATH/QuadtreeTile.h \
    $$VESTA_PATH/RenderContext.h \
    $$VESTA_PATH/LightingEnvironment.h \
    $$VESTA_PATH/RotationModel.h \
    $$VESTA_PATH/SensorFrustumGeometry.h \
    $$VESTA_PATH/SensorVisualizer.h \
    $$VESTA_PATH/ShaderBuilder.h \
    $$VESTA_PATH/ShaderInfo.h \
    $$VESTA_PATH/SingleTextureTiledMap.h \
    $$VESTA_PATH/SkyImageLayer.h \
    $$VESTA_PATH/SkyLayer.h \
    $$VESTA_PATH/Spectrum.h \
    $$VESTA_PATH/StarCatalog.h \
    $$VESTA_PATH/StarsLayer.h \
    $$VESTA_PATH/StateVector.h \
    $$VESTA_PATH/Submesh.h \
    $$VESTA_PATH/TextureFont.h \
    $$VESTA_PATH/TextureMap.h \
    $$VESTA_PATH/TextureMapLoader.h \
    $$VESTA_PATH/TiledMap.h \
    $$VESTA_PATH/Trajectory.h \
    $$VESTA_PATH/TrajectoryGeometry.h \
    $$VESTA_PATH/TwoBodyRotatingFrame.h \
    $$VESTA_PATH/UniformRotationModel.h \
    $$VESTA_PATH/Units.h \
    $$VESTA_PATH/Universe.h \
    $$VESTA_PATH/UniverseRenderer.h \
    $$VESTA_PATH/VelocityVisualizer.h \
    $$VESTA_PATH/VertexArray.h \
    $$VESTA_PATH/VertexAttribute.h \
    $$VESTA_PATH/VertexBuffer.h \
    $$VESTA_PATH/VertexPool.h \
    $$VESTA_PATH/VertexSpec.h \
    $$VESTA_PATH/Viewport.h \
    $$VESTA_PATH/Visualizer.h \
    $$VESTA_PATH/WorldGeometry.h \
    $$VESTA_PATH/interaction/ObserverController.h \
    $$VESTA_PATH/internal/AtomicInt.h \
    $$VESTA_PATH/internal/InputDataStream.h \
    $$VESTA_PATH/internal/OutputDataStream.h \
    $$VESTA_PATH/internal/ObjLoader.h


### particle system module ###

VESTA_SOURCES += \
    $$VESTA_PATH/particlesys/ParticleEmitter.cpp

VESTA_HEADERS += \
    $$VESTA_PATH/particlesys/ParticleEmitter.h \
    $$VESTA_PATH/particlesys/ParticleRenderer.h \
    $$VESTA_PATH/particlesys/PseudorandomGenerator.h \
    $$VESTA_PATH/particlesys/InitialStateGenerator.h \
    $$VESTA_PATH/particlesys/BoxGenerator.h \
    $$VESTA_PATH/particlesys/DiscGenerator.h \
    $$VESTA_PATH/particlesys/PointGenerator.h


### glhelp module ###

VESTA_SOURCES += \
    $$VESTA_PATH/glhelp/GLFramebuffer.cpp \
    $$VESTA_PATH/glhelp/GLShader.cpp \
    $$VESTA_PATH/glhelp/GLShaderProgram.cpp \
    $$VESTA_PATH/glhelp/GLBufferObject.cpp \
    $$VESTA_PATH/glhelp/GLElementBuffer.cpp \
    $$VESTA_PATH/glhelp/GLVertexBuffer.cpp

VESTA_HEADERS += \
    $$VESTA_PATH/glhelp/GLFramebuffer.h \
    $$VESTA_PATH/glhelp/GLShader.h \
    $$VESTA_PATH/glhelp/GLShaderProgram.h \
    $$VESTA_PATH/glhelp/GLBufferObject.h \
    $$VESTA_PATH/glhelp/GLElementBuffer.h \
    $$VESTA_PATH/glhelp/GLVertexBuffer.h


### TLE support

NORADTLE_HEADERS += \
    $$NORADTLE_PATH/norad.h

NORADTLE_SOURCES += \
    $$NORADTLE_PATH/basics.cpp \
    $$NORADTLE_PATH/common.cpp \
    $$NORADTLE_PATH/deep.cpp \
    $$NORADTLE_PATH/get_el.cpp \
    $$NORADTLE_PATH/sdp4.cpp \
    $$NORADTLE_PATH/sdp8.cpp \
    $$NORADTLE_PATH/sgp.cpp \
    $$NORADTLE_PATH/sgp4.cpp \
    $$NORADTLE_PATH/sgp8.cpp


### lib3ds ###

LIB3DS_SOURCES = \
    $$LIB3DS_PATH/lib3ds_atmosphere.c \
    $$LIB3DS_PATH/lib3ds_background.c \
    $$LIB3DS_PATH/lib3ds_camera.c \
    $$LIB3DS_PATH/lib3ds_chunk.c \
    $$LIB3DS_PATH/lib3ds_chunktable.c \
    $$LIB3DS_PATH/lib3ds_file.c \
    $$LIB3DS_PATH/lib3ds_io.c \
    $$LIB3DS_PATH/lib3ds_light.c \
    $$LIB3DS_PATH/lib3ds_material.c \
    $$LIB3DS_PATH/lib3ds_math.c \
    $$LIB3DS_PATH/lib3ds_matrix.c \
    $$LIB3DS_PATH/lib3ds_mesh.c \
    $$LIB3DS_PATH/lib3ds_node.c \
    $$LIB3DS_PATH/lib3ds_quat.c \
    $$LIB3DS_PATH/lib3ds_shadow.c \
    $$LIB3DS_PATH/lib3ds_track.c \
    $$LIB3DS_PATH/lib3ds_util.c \
    $$LIB3DS_PATH/lib3ds_vector.c \
    $$LIB3DS_PATH/lib3ds_viewport.c

LIB3DS_HEADERS = \
    $$LIB3DS_PATH/lib3ds.h \
    $$LIB3DS_PATH/lib3ds_impl.h


### GL extension wrangler ###

GLEW_SOURCES = \
    $$GLEW_PATH/glew.c

GLEW_HEADERS = \
    $$GLEW_PATH/GL/glew.h \
    $$GLEW_PATH/GL/glxew.h \
    $$GLEW_PATH/GL/wglew.h

DEFINES += GLEW_STATIC


### CurvePlot sources ###

CURVEPLOT_SOURCES = \
    thirdparty/curveplot/curveplot.cpp

CURVEPLOT_HEADERS = \
    thirdparty/curveplot/curveplot.h


### QJSON sources ###

QJSON_SOURCES = \
    $$QJSON_PATH/json_parser.cc \
    $$QJSON_PATH/json_scanner.cpp \
    $$QJSON_PATH/parser.cpp \
    $$QJSON_PATH/parserrunnable.cpp \
    $$QJSON_PATH/qobjecthelper.cpp \
    $$QJSON_PATH/serializer.cpp \
    $$QJSON_PATH/serializerrunnable.cpp

QJSON_HEADERS = \
    $$QJSON_PATH/parser.h \
    $$QJSON_PATH/parserrunnable.h \
    $$QJSON_PATH/qobjecthelper.h \
    $$QJSON_PATH/serializer.h \
    $$QJSON_PATH/serializerrunnable.h \
    $$QJSON_PATH/qjson_export.h



SOURCES = \
    $$VESTA_SOURCES \
    $$NORADTLE_SOURCES \
    $$LIB3DS_SOURCES \
    $$GLEW_SOURCES \
    $$CURVEPLOT_SOURCES \
    $$QJSON_SOURCES \
    $$APP_SOURCES

HEADERS = \
    $$VESTA_HEADERS \
    $$NORADTLE_HEADERS \
    $$LIB3DS_HEADERS \
    $$GLEW_HEADERS \
    $$CURVEPLOT_HEADERS \
    $$QJSON_HEADERS \
    $$APP_HEADERS

RESOURCES = resources/icons.qrc

INCLUDEPATH += thirdparty/glew thirdparty/curveplot thirdparty


# ##############################################################################
# ##############################################################################
# FFMPEG: START OF CONFIGURATION BELOW ->
# Copy these lines into your own project
# Make sure to set the path variables for:
# 1) QTFFmpegWrapper sources (i.e. where the QVideoEncoder.cpp and QVideoDecoder.cpp lie),
# 2) FFMPEG include path (i.e. where the directories libavcodec, libavutil, etc. lie),
# 3) the binary FFMPEG libraries (that must be compiled separately).
# Under Linux path 2 and 3 may not need to be set as these are usually in the standard include and lib path.
# Under Windows, path 2 and 3 must be set to the location where you placed the FFMPEG includes and compiled binaries
# Note that the FFMPEG dynamic librairies (i.e. the .dll files) must be in the PATH
# ##############################################################################
# ##############################################################################
# ##############################################################################
# Modify here: set FFMPEG_LIBRARY_PATH and FFMPEG_INCLUDE_PATH
# ##############################################################################
# Set QTFFMPEGWRAPPER_SOURCE_PATH to point to the directory containing the QTFFmpegWrapper sources
#QTFFMPEGWRAPPER_SOURCE_PATH = thirdparty/QTFFmpegWrapper/QTFFmpegWrapper

# Set FFMPEG_LIBRARY_PATH to point to the directory containing the FFmpeg import libraries (if needed - typically for Windows), i.e. the dll.a files
#FFMPEG_LIBRARY_PATH = ../trunk/lib/macosx/ffmpeg

# Set FFMPEG_INCLUDE_PATH to point to the directory containing the FFMPEG includes (if needed - typically for Windows)
#FFMPEG_INCLUDE_PATH = thirdparty/QTFFmpegWrapper/QTFFmpegWrapper

# ##############################################################################
# Do not modify: FFMPEG default settings
# ##############################################################################
# Sources for QT wrapper
#SOURCES += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
#    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp
#HEADERS += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
#    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h

# Set list of required FFmpeg libraries
#LIBS += \
#    -lavutil \
#    -lavcodec \
#    -lavformat \
#    -lswscale \
#    -lz \
#    -lbz2

# Add the path
#LIBS += -L$$FFMPEG_LIBRARY_PATH
#INCLUDEPATH += QVideoEncoder
#INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################


DEFINES += EIGEN_USE_NEW_STDVECTOR
DEFINES += QJSON_EXPORT=

win32-g++ {
    # Work around alignment problems with MinGW. Fixed-size Eigen matrices
    # are sometimes allocated on the stack at unaligned addresses even though
    # __alignof e.g. Vector4d is 16.
    DEFINES += EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
}

win32 {
    DEFINES += NOMINMAX
}

macx {
    # Media files for the Mac bundle
    TEXTURES.path = Contents/Resources/data/textures
    TEXTURES.files = \
        data/textures/milkyway.jpg \
        data/textures/moon.dds \
        data/textures/moon-normal.dds \
        data/textures/earth-normal.dds \
        data/textures/mars4k.dds \
        data/textures/mercury.jpg \
        data/textures/venus.jpg \
        data/textures/mars.jpg \
        data/textures/jupiter.jpg \
        data/textures/saturn.jpg \
        data/textures/uranus.jpg \
        data/textures/neptune.jpg \
        data/textures/sun.jpg \
        data/textures/saturn-rings.png \
        data/textures/uranus-rings.png


    QMAKE_BUNDLE_DATA += TEXTURES

    MODELS.path = Contents/Resources/data/models
    MODELS.files = \
        data/models/jason.obj \
        data/models/jason.mtl \
        data/models/jas_solr.png \
        data/models/jas_brsh.png

    QMAKE_BUNDLE_DATA += MODELS

    DATA.path = Contents/Resources/data
    DATA.files = \
        data/tycho2.stars \
        data/solarsys.json \
        data/sans-12.txf \
        data/sans-24.txf \
        data/sans-light-24.txf \
        data/de406_1800-2100.dat \
        data/earth.atmscat

    QMAKE_BUNDLE_DATA += DATA

    CONFIG += x86
    # QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk

    QMAKE_LFLAGS += -framework CoreFoundation
}