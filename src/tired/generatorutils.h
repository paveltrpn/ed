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
    int _indicesCount{};
};

// ======================================================================================
// ==================== VsgMeshDataGenerator ============================================
// ======================================================================================

struct VsgMeshDataGenerator final {
public:
    static auto box( const gml::dvec3& size = { 1.0, 1.0, 1.0 }, const gml::ivec3& segments = { 8, 8, 8 } )
        -> VsgMeshData;

    static auto sphere( double radius = 1.0, int slices = 32, int segments = 16, double sliceStart = 0.0,
                        double sliceSweep = gml::radians( 360.0 ), double segmentStart = 0.0,
                        double segmentSweep = gml::radians( 180.0 ) ) -> VsgMeshData;

    static auto cylinder( double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8, int rings = 4,
                          double start = 0.0, double sweep = gml::radians( 360.0 ) ) -> VsgMeshData;

    static auto capsule( double radius = 1.0, double size = 0.5, int slices = 32, int segments = 4, int rings = 8,
                         double start = 0.0, double sweep = gml::radians( 360.0 ) ) -> VsgMeshData;

    static auto cone( double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8, int rings = 4,
                      double start = 0.0, double sweep = gml::radians( 360.0 ) ) -> VsgMeshData;

    static auto torus( double minor = 0.25, double major = 1.0, int slices = 16, int segments = 32,
                       double minorStart = 0.0, double minorSweep = gml::radians( 360.0 ), double majorStart = 0.0,
                       double majorSweep = gml::radians( 360.0 ) ) -> VsgMeshData;

private:
    template <typename T, typename V>
    requires requires( T x, V y ) {
        x.generate();
        y.generate();
    }
    static auto copy( T vg, V tg ) -> VsgMeshData {
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

        auto result = VsgMeshData{};

        result._indicesCount = indicesArray.size();

        // Retrive geometry.
        result._vertices = vsg::vec3Array::create( positionsArray.size() );
        std::copy( positionsArray.begin(), positionsArray.end(), result._vertices->begin() );

        result._texcrds = vsg::vec2Array::create( texcoordsArray.size() );
        std::copy( texcoordsArray.begin(), texcoordsArray.end(), result._texcrds->begin() );

        result._indices = vsg::uintArray::create( indicesArray.size() );
        std::copy( indicesArray.begin(), indicesArray.end(), result._indices->begin() );

        result._colors = vsg::vec3Array::create( colorsArray.size() );
        std::copy( colorsArray.begin(), colorsArray.end(), result._colors->begin() );

        return result;
    }
};

}  // namespace tire
