#include <QQmlEngine>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>

#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

#include "tired.h"
#include "extfunctions.h"
#include "log/log.h"

#include "sceneobject/sceneobjectdata.h"

namespace tire {

auto Tired::init() -> void {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() -> void {
        _instance.store( new Tired{} );
        _initSuccess = true;
    } );
}

auto Tired::instance() -> Tired& {
    auto* ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init( ... ) before calling instance()." );
    }

    return *ptr;
}

auto Tired::pointer() -> Tired* {
    auto* ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init( ... ) before calling instance()." );
    }

    // "Regardless of the method, you must manage ownership explicitly. The default behavior is that if QML receives a
    // QObject* from a Q_INVOKABLE method and that object has no parent, QML assumes ownership (JavaScriptOwnership) and
    // will delete it when the garbage collector runs."
    QQmlEngine::setObjectOwnership( ptr, QQmlEngine::CppOwnership );

    return ptr;
}

Tired::Tired( QObject* parent )
    : QObject{ parent }
    , _render{ std::make_shared<tire::VsgRender>() } {
    _render->viewer();

    // Setup scenegraph.
    {
        //
        _scenegraph = new Scenegraph{ vsg::observer_ptr<vsg::Viewer>{ _render->viewer() },
                                      vsg::observer_ptr<vsg::DynamicRenderGraph>{ _render->renderGraph() }, this };
    }

    _render->sceneTransform()->addChild( _scenegraph->root() );

    _render->setFrameUpdateCallback( [this]( const vsg::Camera& camera ) -> void {
        //
        _scenegraph->frameUpdate( camera );
    } );

    // Setup manipulator object.
    { _manipulator = new Manipulator{ _render->camera(), this }; }

    // Setup event handler object.
    {
        //
        _inputHandler =
            new InputHandler{ _scenegraph, _manipulator, vsg::observer_ptr<vsg::Camera>{ _render->camera() }, this };
    }

    connect( _manipulator, &Manipulator::lookChanged, _scenegraph, &Scenegraph::lookChanged );

    // Finalize viewer object setup.
    {
        _render->viewer()->addEventHandler( _manipulator->trackball() );
        _render->viewer()->addEventHandler( _inputHandler->handler() );
    }

    _render->viewer()->compile();

    // Add default scene nodes.
    const auto makeBaseMap = []( const QString& name, QVariantList position, QVariantList orientation,
                                 QVariantList scale, QVariantList color ) -> QVariantMap {
        return { { "name", name },
                 { "uid", QUuid::createUuid().toString() },
                 { "position", position },
                 { "orientation", orientation },
                 { "scale", scale },
                 { "color", color } };
    };

    const auto white = QVariantList{ 1.0, 1.0, 1.0, 1.0 };
    const auto zero3 = QVariantList{ 0.0, 0.0, 0.0 };
    const auto one3 = QVariantList{ 1.0, 1.0, 1.0 };

    {
        auto data = makeBaseMap( "OBJECT", { -2.0, 2.0, 0.0 }, { 22.0, 45.0, 12.0 }, { 1.0, 1.3, 1.30 }, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::BOX ) );
        data.insert( "derived",
                     QVariantMap{ { "sizeX", 0.5 }, { "sizeY", 0.5 }, { "sizeZ", 0.5 },
                                  { "segmentsX", 1 }, { "segmentsY", 1 }, { "segmentsZ", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -2.0, -2.0, 0.0 }, { 72.0, 25.0, -91.0 }, { 2.0, 1.3, 1.3 }, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::BOX ) );
        data.insert( "derived",
                     QVariantMap{ { "sizeX", 0.5 }, { "sizeY", 0.5 }, { "sizeZ", 0.5 },
                                  { "segmentsX", 1 }, { "segmentsY", 1 }, { "segmentsZ", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 2.0, 2.0, 0.0 }, { 32.0, 225.0, -191.0 }, { 2.0, 1.3, 1.3 }, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::BOX ) );
        data.insert( "derived",
                     QVariantMap{ { "sizeX", 0.5 }, { "sizeY", 0.5 }, { "sizeZ", 0.5 },
                                  { "segmentsX", 1 }, { "segmentsY", 1 }, { "segmentsZ", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 2.0, -2.0, 0.0 }, { 12.0, 65.0, 121.0 }, { 2.0, 1.3, 1.3 }, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::BOX ) );
        data.insert( "derived",
                     QVariantMap{ { "sizeX", 0.5 }, { "sizeY", 0.5 }, { "sizeZ", 0.5 },
                                  { "segmentsX", 1 }, { "segmentsY", 1 }, { "segmentsZ", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 0.0, 2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::SPHERE ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 },          { "slices", 16 },
                                  { "segments", 8 },          { "sliceStart", 0.0 },
                                  { "sliceSweep", gml::radians( 360.0 ) }, { "segmentStart", 0.0 },
                                  { "segmentSweep", gml::radians( 180.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 0.0, 4.0, 0.0 }, zero3, { 1.0, 1.0, 3.0 }, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::CYLINDER ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 }, { "size", 0.5 }, { "slices", 16 }, { "segments", 8 },
                                  { "rings", 4 },   { "start", 0.0 }, { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 0.0, -2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::CAPSULE ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 }, { "size", 0.5 }, { "slices", 16 }, { "segments", 8 },
                                  { "rings", 4 },   { "start", 0.0 }, { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 4.0, 2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::ICOSPHERE ) );
        data.insert( "derived", QVariantMap{ { "radius", 0.5 }, { "segments", 4 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 4.0, -2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::ICOSAHEDRON ) );
        data.insert( "derived", QVariantMap{ { "radius", 0.5 }, { "segments", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -4.0, 2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::DODECAHEDRON ) );
        data.insert( "derived", QVariantMap{ { "radius", 0.5 }, { "segments", 1 }, { "rings", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -4.0, -2.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::PLANE ) );
        data.insert( "derived",
                     QVariantMap{ { "sizeX", 0.5 }, { "sizeY", 0.5 }, { "segmentsX", 1 }, { "segmentsY", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 6.0, 0.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::DISK ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 }, { "innerRadius", 0.0 }, { "slices", 16 }, { "rings", 4 },
                                  { "start", 0.0 }, { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -6.0, 0.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::TUBE ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 }, { "innerRadius", 0.25 }, { "size", 0.5 }, { "slices", 16 },
                                  { "segments", 8 }, { "start", 0.0 },       { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 0.0, 6.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::CAPPED_TUBE ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 },   { "innerRadius", 0.25 }, { "size", 0.5 }, { "slices", 16 },
                                  { "segments", 8 },   { "rings", 1 },
                                  { "start", 0.0 },    { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 0.0, -6.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::ROUNDED_BOX ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.1 },   { "sizeX", 0.4 },     { "sizeY", 0.4 }, { "sizeZ", 0.4 },
                                  { "slices", 4 },     { "segmentsX", 1 },   { "segmentsY", 1 },
                                  { "segmentsZ", 1 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 2.0, 4.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::SPHERICAL_CONE ) );
        data.insert( "derived",
                     QVariantMap{ { "radius", 0.5 }, { "size", 0.5 }, { "slices", 16 }, { "segments", 8 },
                                  { "rings", 4 },   { "start", 0.0 }, { "sweep", gml::radians( 360.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -2.0, -4.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::TORUS_KNOT ) );
        data.insert( "derived",
                     QVariantMap{ { "p", 2 }, { "q", 3 }, { "slices", 8 }, { "segments", 96 } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { 4.0, 4.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::SPRING ) );
        data.insert( "derived",
                     QVariantMap{ { "minor", 0.25 },   { "major", 1.0 },
                                  { "size", 1.0 },     { "slices", 8 },
                                  { "segments", 32 },  { "minorStart", 0.0 },
                                  { "minorSweep", gml::radians( 360.0 ) }, { "majorStart", 0.0 },
                                  { "majorSweep", gml::radians( 720.0 ) } } );

        _scenegraph->scene()->add( data );
    }

    {
        auto data = makeBaseMap( "OBJECT", { -4.0, -4.0, 0.0 }, zero3, one3, white );
        data.insert( "type", static_cast<int>( SceneObjectTypeEnum::TEAPOT ) );
        data.insert( "derived", QVariantMap{ { "segments", 8 } } );

        _scenegraph->scene()->add( data );
    }
};

auto Tired::vsgRender() const -> std::shared_ptr<VsgRender> {
    return _render;
}

auto Tired::manipulator() const -> QObject* {
    return _manipulator;
}

auto Tired::inputHandler() const -> QObject* {
    return _inputHandler;
}

auto Tired::scenegraph() const -> QObject* {
    return _scenegraph;
}

auto Tired::setControlMode( int value ) -> void {
    _controlMode = static_cast<ControlModes>( value );
    emit controlModeChanged( value );
}

auto Tired::controlMode() const -> int {
    return static_cast<int>( _controlMode );
}

}  // namespace tire
