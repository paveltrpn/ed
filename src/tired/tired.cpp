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
    _viewer = _render->viewer();
    _camera = _render->camera();

    // Setup scenegraph.
    {
        //
        _scenegraph = new Scenegraph{ vsg::observer_ptr<vsg::Viewer>{ _viewer }, this };
    }

    _render->sceneTransform()->addChild( _scenegraph->root() );

    // Setup manipulator object.
    { _manipulator = new Manipulator{ _camera, this }; }

    // Setup event handler object.
    {
        //
        _inputHandler = new InputHandler{ _scenegraph, _manipulator, vsg::observer_ptr<vsg::Camera>{ _camera }, this };
    }

    connect( _manipulator, &Manipulator::lookChanged, _scenegraph, &Scenegraph::lookChanged );

    // Finalize viewer object setup.
    {
        _viewer->addEventHandler( _manipulator->trackball() );
        _viewer->addEventHandler( _inputHandler->handler() );
    }

    _viewer->compile();

    // Add default scene nodes.
    {
        auto data = BoxObjectData{};
        data._position = { -2.0, 2.0, 0.0 };
        data._orientation = {
            22.0,
            45.0,
            12.0,
        };
        data._scale = { 1.0, 1.3, 1.30 };
        _scenegraph->scene()->addBox( data );
    }

    {
        auto data = BoxObjectData{};
        data._position = { -2.0, -2.0, 0.0 };
        data._orientation = { 72.0, 25.0, -91.0 };
        data._scale = { 2.0, 1.3, 1.3 };
        _scenegraph->scene()->addBox( data );
    }

    {
        auto data = BoxObjectData{};
        data._position = { 2.0, 2.0, 0.0 };
        data._orientation = { 32.0, 225.0, -191.0 };
        data._scale = { 2.0, 1.3, 1.3 };
        _scenegraph->scene()->addBox( data );
    }

    {
        auto data = BoxObjectData{};
        data._position = { 2.0, -2.0, 0.0 };
        data._orientation = { 12.0, 65.0, 121.0 };
        data._scale = { 2.0, 1.3, 1.3 };
        _scenegraph->scene()->addBox( data );
    }

    {
        auto data = SphereObjectData{};
        data._position = { 0.0, 2.0, 0.0 };
        data._orientation = { 0.0, 0.0, 0.0 };
        data._scale = { 1.0, 1.0, 1.0 };
        _scenegraph->scene()->addSphere( data );
    }

    {
        auto data = CylinderObjectData{};
        data._position = { 0.0, 4.0, 0.0 };
        data._orientation = { 0.0, 0.0, 0.0 };
        data._scale = { 1.0, 1.0, 3.0 };
        _scenegraph->scene()->addCylinder( data );
    }

    {
        auto data = CapsuleObjectData{};
        data._position = { 0.0, -2.0, 0.0 };
        data._orientation = { 0.0, 0.0, 0.0 };
        data._scale = { 1.0, 1.0, 1.0 };
        _scenegraph->scene()->addCapsule( data );
    }
};

auto Tired::vsgRender() const -> std::shared_ptr<VsgRender> {
    return _render;
}

auto Tired::viewer() -> vsg::ref_ptr<Viewer> {
    return _viewer;
}

auto Tired::manipulator() const -> QObject* {
    return _manipulator;
}

auto Tired::inputHandler() const -> QObject* {
    return _inputHandler;
}

auto Tired::camera() -> vsg::ref_ptr<vsg::Camera> {
    return _camera;
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
