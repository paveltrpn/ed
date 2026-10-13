#include <QQmlEngine>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>

#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

#include "tired.h"
#include "extfunctions.h"
#include "log/log.h"

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

    _render->setFrameUpdateCallback( [this]( const vsg::Camera& camera ) { _scenegraph->frameUpdate( camera ); } );

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
    _scenegraph->scene()->addBox( "OBJECT", QUuid::createUuid().toString().toStdString(), { -2.0, 2.0, 0.0 },
                                  { 22.0, 45.0, 12.0 }, { 1.0, 1.3, 1.30 }, { 1.0f, 1.0f, 1.0f, 1.0f },
                                  { 0.5, 0.5, 0.5 }, { 1, 1, 1 } );

    _scenegraph->scene()->addBox( "OBJECT", QUuid::createUuid().toString().toStdString(), { -2.0, -2.0, 0.0 },
                                  { 72.0, 25.0, -91.0 }, { 2.0, 1.3, 1.3 }, { 1.0f, 1.0f, 1.0f, 1.0f },
                                  { 0.5, 0.5, 0.5 }, { 1, 1, 1 } );

    _scenegraph->scene()->addBox( "OBJECT", QUuid::createUuid().toString().toStdString(), { 2.0, 2.0, 0.0 },
                                  { 32.0, 225.0, -191.0 }, { 2.0, 1.3, 1.3 }, { 1.0f, 1.0f, 1.0f, 1.0f },
                                  { 0.5, 0.5, 0.5 }, { 1, 1, 1 } );

    _scenegraph->scene()->addBox( "OBJECT", QUuid::createUuid().toString().toStdString(), { 2.0, -2.0, 0.0 },
                                  { 12.0, 65.0, 121.0 }, { 2.0, 1.3, 1.3 }, { 1.0f, 1.0f, 1.0f, 1.0f },
                                  { 0.5, 0.5, 0.5 }, { 1, 1, 1 } );

    _scenegraph->scene()->addSphere( "OBJECT", QUuid::createUuid().toString().toStdString(), { 0.0, 2.0, 0.0 },
                                     { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.5, 16,
                                     8, 0.0, gml::radians( 360.0 ), 0.0, gml::radians( 180.0 ) );

    _scenegraph->scene()->addCylinder( "OBJECT", QUuid::createUuid().toString().toStdString(), { 0.0, 4.0, 0.0 },
                                       { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 3.0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.5,
                                       0.5, 16, 8, 4, 0.0, gml::radians( 360.0 ) );

    _scenegraph->scene()->addCapsule( "OBJECT", QUuid::createUuid().toString().toStdString(), { 0.0, -2.0, 0.0 },
                                      { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 1.0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.5,
                                      0.5, 16, 8, 4, 0.0, gml::radians( 360.0 ) );
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
