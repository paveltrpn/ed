#include <vsg/all.h>
#include <vsg/app/Viewer.h>

#ifdef vsgXchange_FOUND
#include <vsgXchange/all.h>
#endif

#include "tired.h"

namespace tire {

Tired::Tired( QObject* parent )
    : QObject{ parent }
    , _viewer{ Viewer::create() } {};

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

QObject* Tired::grid() const {
    return _scenegraph->grid();
}

auto Tired::init( Window* window, uint32_t width, uint32_t height ) -> void {
    // Setup the camera.
    {
        auto lookAt = vsg::LookAt::create( vsg::dvec3( 3.0, -8.0, 5.0 ), vsg::dvec3{ 0.0, 0.0, 0.0 },
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

    // Setup manipulator and event handler objects.
    {
        _manipulator = new Manipulator{ _camera, nullptr, this };
        _manipulator->trackball()->addWindow( *window );
        _inputHandler = new InputHandler{ _camera, _viewer, _scenegraph, this };
    }

    // Setup viewer object.
    {
        _viewer->addEventHandler( _manipulator->trackball() );
        _viewer->addEventHandler( _inputHandler->handler() );
        auto commandGraph = vsg::createCommandGraphForView( *window, _camera, _scenegraph->root() );
        _viewer->addRecordAndSubmitTaskAndPresentation( { commandGraph } );

        constexpr auto UPDATE_INTERVAL{ 8 };
        _viewer->setInterval( UPDATE_INTERVAL );

        constexpr auto CONTINOUS_UPDATE{ true };
        _viewer->continuousUpdate = CONTINOUS_UPDATE;
    }

    // Borrow vulkan data from VSG.
    {
        const auto wa = window->windowAdapter;
        const auto instance = wa->getInstance()->vk();
        const auto physicalDevice = wa->getPhysicalDevice()->vk();
        const auto device = wa->getDevice()->vk();
        const auto surface = wa->getSurface()->vk();
        const auto rp = wa->getRenderPass()->vk();

        _context = std::make_shared<vk::Context>( instance, physicalDevice, device, surface, rp, 0, 0 );
    }

    // Viewer compile.
    {
        //
        _viewer->compile();
    }

    // Add default cube.
    _scenegraph->addExBox( -2.0, 1.5, 0.0, 22.0, 45.0, 12.0, 1.0, 1.3, 0.3 );
    _scenegraph->addExBox( 4.4, -1.8, 0.0, 62.0, 121.0, -32.0, 0.1, 1.8, 3.3 );
    _scenegraph->addExBox( -1.0, 2.5, 0.0, 72.0, 25.0, -91.0, 2.0, 0.3, 1.3 );
}

auto Tired::registerTypes() -> void {
    qRegisterMetaType<tire::SceneObjectTypeEnum>( "SceneObjectTypeEnum" );

    qRegisterMetaType<tire::SceneObjectData>( "SceneObjectData" );
    qRegisterMetaType<tire::BoxObjectData>( "BoxObject" );
    qRegisterMetaType<tire::SphereObjectData>( "SphereObjectData" );
    qRegisterMetaType<tire::MeshData>( "MeshData" );
}

auto Tired::setGlobalMousePosX( float value ) -> void {
    _globalMousePosX = value;
    emit globalMousePosChanged( _globalMousePosX, _globalMousePosY );
}

auto Tired::setGlobalMousePosY( float value ) -> void {
    _globalMousePosY = value;
    emit globalMousePosChanged( _globalMousePosX, _globalMousePosY );
}

auto Tired::globalMousePosX() -> float {
    return _globalMousePosX;
}

auto Tired::globalMousePosY() -> float {
    return _globalMousePosY;
}

}  // namespace tire
