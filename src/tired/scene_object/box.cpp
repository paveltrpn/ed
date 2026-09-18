
#include <print>

#include <vsg/all.h>

#include "generator/BoxMesh.hpp"

#include "box.h"

namespace tire::object {

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
    auto positionsArray = std::vector<vsg::vec3>{};
    auto normalsArray = std::vector<vsg::vec3>{};
    auto texcoordsArray = std::vector<vsg::vec2>{};
    auto indicesArray = std::vector<unsigned int>{};

    auto boxMesh = generator::BoxMesh{ { 0.5, 0.5, 0.5 }, { 1, 1, 1 } };

    auto vg = boxMesh.vertices();
    while ( !vg.done() ) {
        generator::MeshVertex vertex = vg.generate();

        auto position = vertex.position.data();
        auto normal = vertex.normal.data();
        auto texcoord = vertex.texCoord.data();

        positionsArray.emplace_back( position[0], position[1], position[2] );
        normalsArray.emplace_back( normal[0], normal[1], normal[2] );
        texcoordsArray.emplace_back( texcoord[0], texcoord[1] );

        vg.next();
    }

    auto tg = boxMesh.triangles();
    while ( !tg.done() ) {
        generator::Triangle triangle = tg.generate();

        auto indices = triangle.vertices;

        indicesArray.push_back( indices[0] );
        indicesArray.push_back( indices[1] );
        indicesArray.push_back( indices[2] );

        tg.next();
    }

    // Retrive geometry.
    auto vertices = vsg::vec3Array::create( positionsArray.size() );
    for ( size_t i = 0; i < positionsArray.size(); ++i ) {
        vertices->at( i ) = positionsArray[i];
    }

    auto texcoords = vsg::vec2Array::create( texcoordsArray.size() );
    for ( size_t i = 0; i < texcoordsArray.size(); ++i ) {
        texcoords->at( i ) = texcoordsArray[i];
    }

    auto colors = vsg::vec3Array::create( {
        { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
    } );

    auto indices = vsg::uintArray::create( indicesArray.size() );
    for ( size_t i = 0; i < indicesArray.size(); ++i ) {
        indices->at( i ) = indicesArray[i];
    }

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ vertices, colors, texcoords } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( 36, 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object