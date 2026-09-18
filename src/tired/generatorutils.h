#pragma once

#include <vsg/all.h>

#include "generator/Triangle.hpp"
#include "generator/MeshVertex.hpp"

namespace tire {

// ======================================================================================
// ==================== VsgMeshData =====================================================
// ======================================================================================

struct VsgMeshData final {
    vsg::ref_ptr<vsg::vec3Array> _vertices{};
    vsg::ref_ptr<vsg::vec3Array> _normals{};
    vsg::ref_ptr<vsg::vec3Array> _colors{};
    vsg::ref_ptr<vsg::vec2Array> _texcrds{};
    vsg::ref_ptr<vsg::uintArray> _indices{};
};

// ======================================================================================
// ==================== VsgMeshDataGenerator ============================================
// ======================================================================================

struct VsgMeshDataGenerator final {
public:
    static auto box() -> VsgMeshData;
    static auto sphere() -> VsgMeshData;
    static auto cylinder() -> VsgMeshData;
    static auto capsule() -> VsgMeshData;

private:
    template <typename T, typename V>
    static auto copy(T vg, V tg) -> VsgMeshData {
        auto positionsArray = std::vector<vsg::vec3>{};
        auto normalsArray = std::vector<vsg::vec3>{};
        auto texcoordsArray = std::vector<vsg::vec2>{};
        auto colorsArray = std::vector<vsg::vec3>{};

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

        auto indicesArray = std::vector<unsigned int>{};

        while ( !tg.done() ) {
            generator::Triangle triangle = tg.generate();

            auto indices = triangle.vertices;

            indicesArray.push_back( indices[0] );
            indicesArray.push_back( indices[1] );
            indicesArray.push_back( indices[2] );

            tg.next();
        }

        auto result = VsgMeshData {};

        // Retrive geometry.
        result._vertices = vsg::vec3Array::create( positionsArray.size() );
        std::copy( positionsArray.begin(), positionsArray.end(), result._vertices->begin() );

        result._texcrds = vsg::vec2Array::create( texcoordsArray.size() );
        std::copy( texcoordsArray.begin(), texcoordsArray.end(), result._texcrds->begin() );

        result._indices = vsg::uintArray::create( indicesArray.size() );
        std::copy( indicesArray.begin(), indicesArray.end(), result._indices->begin() );

        auto colors = vsg::vec3Array::create( colorsArray.size() );
        std::copy( colorsArray.begin(), colorsArray.end(), colors->begin() );
    }
};

}
