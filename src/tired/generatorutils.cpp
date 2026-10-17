
#include "generator/BoxMesh.hpp"
#include "generator/SphereMesh.hpp"
#include "generator/CappedCylinderMesh.hpp"
#include "generator/CapsuleMesh.hpp"
#include "generator/CappedConeMesh.hpp"
#include "generator/TorusMesh.hpp"
#include "generator/IcoSphereMesh.hpp"
#include "generator/IcosahedronMesh.hpp"
#include "generator/DodecahedronMesh.hpp"
#include "generator/PlaneMesh.hpp"
#include "generator/DiskMesh.hpp"
#include "generator/TubeMesh.hpp"
#include "generator/CappedTubeMesh.hpp"
#include "generator/RoundedBoxMesh.hpp"
#include "generator/SphericalConeMesh.hpp"
#include "generator/TorusKnotMesh.hpp"
#include "generator/SpringMesh.hpp"
#include "generator/TeapotMesh.hpp"

#include "generatorutils.h"

namespace tire {

auto VsgMeshDataGenerator::box( const gml::dvec3& size, const gml::ivec3& segments ) -> VsgMeshData {
    auto mesh = generator::BoxMesh{ size, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::sphere( double radius, int slices, int segments, double sliceStart, double sliceSweep,
                                   double segmentStart, double segmentSweep ) -> VsgMeshData {
    auto mesh = generator::SphereMesh{ radius, slices, segments, sliceStart, sliceSweep, segmentStart, segmentSweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::cylinder( double radius, double size, int slices, int segments, int rings, double start,
                                     double sweep ) -> VsgMeshData {
    auto mesh = generator::CappedCylinderMesh{ radius, size, slices, segments, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::capsule( double radius, double size, int slices, int segments, int rings, double start,
                                    double sweep ) -> VsgMeshData {
    auto mesh = generator::CapsuleMesh{ radius, size, slices, segments, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::cone( double radius, double size, int slices, int segments, int rings, double start,
                                 double sweep ) -> VsgMeshData {
    auto mesh = generator::CappedConeMesh{ radius, size, slices, segments, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::torus( double minor, double major, int slices, int segments, double minorStart,
                                  double minorSweep, double majorStart, double majorSweep ) -> VsgMeshData {
    auto mesh = generator::TorusMesh{ minor, major, slices, segments, minorStart, minorSweep, majorStart, majorSweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::icosphere( double radius, int segments ) -> VsgMeshData {
    auto mesh = generator::IcoSphereMesh{ radius, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::icosahedron( double radius, int segments ) -> VsgMeshData {
    auto mesh = generator::IcosahedronMesh{ radius, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::dodecahedron( double radius, int segments, int rings ) -> VsgMeshData {
    auto mesh = generator::DodecahedronMesh{ radius, segments, rings };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::plane( const gml::dvec2& size, const gml::ivec2& segments ) -> VsgMeshData {
    auto mesh = generator::PlaneMesh{ size, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::disk( double radius, double innerRadius, int slices, int rings, double start, double sweep )
    -> VsgMeshData {
    auto mesh = generator::DiskMesh{ radius, innerRadius, slices, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::tube( double radius, double innerRadius, double size, int slices, int segments,
                                 double start, double sweep ) -> VsgMeshData {
    auto mesh = generator::TubeMesh{ radius, innerRadius, size, slices, segments, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::cappedTube( double radius, double innerRadius, double size, int slices, int segments,
                                       int rings, double start, double sweep ) -> VsgMeshData {
    auto mesh = generator::CappedTubeMesh{ radius, innerRadius, size, slices, segments, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::roundedBox( double radius, const gml::dvec3& size, int slices,
                                       const gml::ivec3& segments ) -> VsgMeshData {
    auto mesh = generator::RoundedBoxMesh{ radius, size, slices, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::sphericalCone( double radius, double size, int slices, int segments, int rings,
                                          double start, double sweep ) -> VsgMeshData {
    auto mesh = generator::SphericalConeMesh{ radius, size, slices, segments, rings, start, sweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::torusKnot( int p, int q, int slices, int segments ) -> VsgMeshData {
    auto mesh = generator::TorusKnotMesh{ p, q, slices, segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::spring( double minor, double major, double size, int slices, int segments,
                                   double minorStart, double minorSweep, double majorStart,
                                   double majorSweep ) -> VsgMeshData {
    auto mesh =
        generator::SpringMesh{ minor, major, size, slices, segments, minorStart, minorSweep, majorStart, majorSweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::teapot( int segments ) -> VsgMeshData {
    auto mesh = generator::TeapotMesh{ segments };
    return copy( mesh.vertices(), mesh.triangles() );
}

}  // namespace tire
