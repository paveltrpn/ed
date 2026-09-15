#include <QQmlEngine>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>

#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

#include "tired.h"
#include "log/log.h"

namespace tire {

void Tired::init( vsg::ref_ptr<vsg::Window> windowAdapter, vsg::ref_ptr<Viewer> viewer, uint32_t width,
                  uint32_t height ) {
    if ( _initSuccess ) {
        log::error()( "Warning: Singleton already initialized. Ignoring new arguments." );
    }

    std::call_once( _initFlag, [&]() {
        _instance.store( new Tired( windowAdapter, viewer, width, height ) );
        _initSuccess = true;
    } );
}

Tired& Tired::instance() {
    auto* ptr = _instance.load();

    if ( !ptr ) {
        throw std::logic_error( "Singleton must be initialized via init( ... ) before calling instance()." );
    }

    return *ptr;
}

Tired* Tired::pointer() {
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

Tired::Tired( vsg::ref_ptr<vsg::Window> windowAdapter, vsg::ref_ptr<Viewer> viewer, uint32_t width, uint32_t height,
              QObject* parent )
    : QObject{ parent }
    , _viewer{ viewer } {
    // Setup the camera.
    {
        auto lookAt = vsg::LookAt::create( vsg::dvec3( 0.0, -16.0, 8.0 ), vsg::dvec3{ 0.0, 0.0, 0.0 },
                                           vsg::dvec3( 0.0, 0.0, 1.0 ) );

        vsg::ref_ptr<vsg::ProjectionMatrix> perspective =
            vsg::Perspective::create( 30.0, static_cast<double>( width ) / static_cast<double>( height ), 0.01, 500.0 );

        _camera = vsg::Camera::create( perspective, lookAt, vsg::ViewportState::create( VkExtent2D{ width, height } ) );
    }

    // Setup scenegraph.
    {
        //
        _scenegraph = new Scenegraph{ _viewer, this };
    }

    // Setup manipulator object.
    {
        _manipulator = new Manipulator{ _camera, this };
        _manipulator->trackball()->addWindow( windowAdapter );
    }

    // Setup event handler object.
    {
        //
        _inputHandler = new InputHandler{ _camera, _viewer, _scenegraph, this };
    }

    connect( _manipulator, &Manipulator::lookChanged, _scenegraph, &Scenegraph::lookChanged );

    // Finalize viewer object setup.
    {
        _viewer->addEventHandler( _manipulator->trackball() );
        _viewer->addEventHandler( _inputHandler->handler() );
        auto commandGraph = vsg::createCommandGraphForView( windowAdapter, _camera, _scenegraph->root() );
        _viewer->addRecordAndSubmitTaskAndPresentation( { commandGraph } );

        constexpr auto UPDATE_INTERVAL{ 8 };
        _viewer->setInterval( UPDATE_INTERVAL );

        constexpr auto CONTINOUS_UPDATE{ true };
        _viewer->continuousUpdate = CONTINOUS_UPDATE;
    }

    _viewer->compile();

    // Add default cube.
    _scenegraph->addExBox( -2.0, 1.5, 0.0, 22.0, 45.0, 12.0, 1.0, 1.3, 1.3 );
    _scenegraph->addExBox( 4.4, -1.8, 0.0, 62.0, 121.0, -32.0, 1.1, 1.8, 3.3 );
    _scenegraph->addExBox( -1.0, 2.5, 0.0, 72.0, 25.0, -91.0, 2.0, 1.3, 1.3 );
    _scenegraph->addExBox( -1.0, -2.5, 0.0, 32.0, 225.0, -191.0, 2.0, 1.3, 1.3 );
    _scenegraph->addExBox( 1.0, 2.5, 0.0, 12.0, 65.0, 121.0, 2.0, 1.3, 1.3 );
};

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

QObject* Tired::scenegraph() const {
    return _scenegraph;
}

void Tired::setControlMode( int value ) {
    _controlMode = static_cast<ControlModes>( value );
    emit controlModeChanged( value );
}

int Tired::controlMode() {
    return static_cast<int>( _controlMode );
}

}  // namespace tire
