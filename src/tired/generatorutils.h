#pragma once

#include <vsg/all.h>

#include "algebra/vector.h"
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
    static auto box( const algebra::vector<double, 3>& size, const algebra::vector<int, 3>& segments )
        -> VsgMeshData;

    static auto sphere( double radius, int slices, int segments, double sliceStart, double sliceSweep,
                        double segmentStart, double segmentSweep ) -> VsgMeshData;

    static auto cylinder( double radius, double size, int slices, int segments, int rings, double start, double sweep )
        -> VsgMeshData;

    static auto capsule( double radius, double size, int slices, int segments, int rings, double start, double )
        -> VsgMeshData;

    static auto cone( double radius, double size, int slices, int segments, int rings, double start, double sweep )
        -> VsgMeshData;

    static auto torus( double minor, double major, int slices, int segments, double minorStart, double minorSweep,
                       double majorStart, double majorSweep ) -> VsgMeshData;

    static auto icosphere( double radius, int segments ) -> VsgMeshData;

    static auto icosahedron( double radius, int segments ) -> VsgMeshData;

    static auto dodecahedron( double radius, int segments, int rings ) -> VsgMeshData;

    static auto plane( const algebra::vector<double, 2>& size, const algebra::vector<int, 2>& segments )
        -> VsgMeshData;

    static auto disk( double radius, double innerRadius, int slices, int rings, double start, double sweep )
        -> VsgMeshData;

    static auto tube( double radius, double innerRadius, double size, int slices, int segments, double start,
                      double sweep ) -> VsgMeshData;

    static auto cappedTube( double radius, double innerRadius, double size, int slices, int segments, int rings,
                            double start, double sweep ) -> VsgMeshData;

    static auto roundedBox( double radius, const algebra::vector<double, 3>& size, int slices,
                            const algebra::vector<int, 3>& segments ) -> VsgMeshData;

    static auto sphericalCone( double radius, double size, int slices, int segments, int rings, double start,
                               double sweep ) -> VsgMeshData;

    static auto torusKnot( int p, int q, int slices, int segments ) -> VsgMeshData;

    static auto spring( double minor, double major, double size, int slices, int segments, double minorStart,
                        double minorSweep, double majorStart, double majorSweep ) -> VsgMeshData;

    static auto teapot( int segments ) -> VsgMeshData;

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

        result._normals = vsg::vec3Array::create( normalsArray.size() );
        std::copy( normalsArray.begin(), normalsArray.end(), result._normals->begin() );

        return result;
    }
};

}  // namespace tire
