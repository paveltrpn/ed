
#include <vsg/all.h>

#include "generator/SphereMesh.hpp"

#include "sphere.h"

namespace tire::object {

Sphere::Sphere( QObject* parent )
    : SceneObjectBase{} {
    init();
};

Sphere::Sphere( const SphereObjectData& data )
    : SceneObjectBase{ data._type, data._name, data._uid, data._position, data._orientation, data._scale, data._color }
    , _radius{ data._radius } {
    init();
}

auto Sphere::init() -> void {
    auto positionsArray = std::vector<vsg::vec3>{};
    auto normalsArray = std::vector<vsg::vec3>{};
    auto texcoordsArray = std::vector<vsg::vec2>{};
    auto indicesArray = std::vector<unsigned int>{};
    auto colorsArray = std::vector<vsg::vec3>{};

    auto boxMesh = generator::SphereMesh{ /* radius */ 0.5,
                                          /* slices */ 8,
                                          /* segments */ 8,
                                          /* sliceStart */ 0.0,
                                          /* sliceSweep */ gml::radians( 360.0 ),
                                          /* segmentStart */ 0.0,
                                          /* segmentSweep */ gml::radians( 180.0 ) };

    auto vg = boxMesh.vertices();
    while ( !vg.done() ) {
        generator::MeshVertex vertex = vg.generate();

        auto position = vertex.position.data();
        auto normal = vertex.normal.data();
        auto texcoord = vertex.texCoord.data();

        positionsArray.emplace_back( position[0], position[1], position[2] );
        normalsArray.emplace_back( normal[0], normal[1], normal[2] );
        texcoordsArray.emplace_back( texcoord[0], texcoord[1] );
        colorsArray.emplace_back( 1.0f, 1.0f, 1.0f );

        vg.next();
    }

    auto i = 0;
    auto tg = boxMesh.triangles();
    while ( !tg.done() ) {
        generator::Triangle triangle = tg.generate();

        auto indices = triangle.vertices;

        indicesArray.push_back( indices[0] );
        indicesArray.push_back( indices[1] );
        indicesArray.push_back( indices[2] );

        tg.next();
        ++i;
    }

    // Retrive geometry.
    auto vertices = vsg::vec3Array::create( positionsArray.size() );
    std::copy( positionsArray.begin(), positionsArray.end(), vertices->begin() );

    auto texcoords = vsg::vec2Array::create( texcoordsArray.size() );
    std::copy( texcoordsArray.begin(), texcoordsArray.end(), texcoords->begin() );

    auto indices = vsg::uintArray::create( indicesArray.size() );
    std::copy( indicesArray.begin(), indicesArray.end(), indices->begin() );

    auto colors = vsg::vec3Array::create( colorsArray.size() );
    std::copy( colorsArray.begin(), colorsArray.end(), colors->begin() );

    // Setup geometry.
    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild( vsg::BindVertexBuffers::create( 0, vsg::DataList{ vertices, colors, texcoords } ) );
    drawCommands->addChild( vsg::BindIndexBuffer::create( indices ) );
    drawCommands->addChild( vsg::DrawIndexed::create( indicesArray.size(), 1, 0, 0, 0 ) );

    _node->addChild( drawCommands );
}

}  // namespace tire::object
