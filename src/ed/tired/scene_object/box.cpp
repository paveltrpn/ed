
#include <vsg/all.h>

#include "box.h"

namespace tire::object {

namespace {

auto boxData() -> std::tuple<vsg::ref_ptr<vsg::vec3Array>, vsg::ref_ptr<vsg::vec3Array>, vsg::ref_ptr<vsg::vec2Array>,
                             vsg::ref_ptr<vsg::uintArray>> {
    auto vertices = vsg::vec3Array::create( { { -0.5f, -0.5f, 0.5f },
                                              { 0.5f, -0.5f, 0.5f },
                                              { 0.5f, 0.5f, 0.5f },
                                              { -0.5f, 0.5f, 0.5f },
                                              { -0.5f, -0.5f, -0.5f },
                                              { 0.5f, -0.5f, -0.5f },
                                              { 0.5f, 0.5f, -0.5f },
                                              { -0.5f, 0.5f, -0.5f } } );

    // VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE
    auto colors = vsg::vec3Array::create( {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
    } );

    // VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE
    auto texcoords = vsg::vec2Array::create( { { 0.0f, 0.0f },
                                               { 1.0f, 0.0f },
                                               { 1.0f, 1.0f },
                                               { 0.0f, 1.0f },
                                               { 0.0f, 0.0f },
                                               { 1.0f, 0.0f },
                                               { 1.0f, 1.0f },
                                               { 0.0f, 1.0f } } );

    // VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE
    //
    // TOP BOTTOM NEAR FAR LEFT RIGHT
    auto indices = vsg::uintArray::create( { 0, 1, 2, 2, 3, 0, 4, 7, 6, 6, 5, 4, 7, 3, 2, 2, 6, 7,
                                             4, 5, 1, 1, 0, 4, 3, 7, 4, 4, 0, 3, 6, 2, 1, 1, 5, 6 } );

    return { vertices, colors, texcoords, indices };
}

}  // namespace

Box::Box( const BoxObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _width{ data._width }
    , _height{ data._height }
    , _depth{ data._depth } {
    init();
}

Box::Box( SceneObjectTypeEnum type, const QString& name, const QUuid& uid, vsg::dvec3 position, vsg::dvec3 orientation,
          vsg::dvec3 scale, vsg::dvec4 color, double width, double height, double depth )
    : SceneObjectBase{ type, name, uid, position, orientation, scale, color }
    , _width{ width }
    , _height{ height }
    , _depth{ depth } {
}

Box::Box( QObject* parent )
    : SceneObjectBase{} {
    init();
}

auto Box::init() -> void {
    // Retrive geometry.
    auto [vertices, colors, texcoords, indices] = boxData();

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ vertices, colors, texcoords } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( 36, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object