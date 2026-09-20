#include "generator/BoxMesh.hpp"
#include "generator/SphereMesh.hpp"
#include "generator/CappedCylinderMesh.hpp"
#include "generator/CapsuleMesh.hpp"
#include "generator/TorusMesh.hpp"
#include "generator/ConeMesh.hpp"

#include "generatorutils.h"

namespace tire {

auto VsgMeshDataGenerator::box( const gml::dvec3& size, const gml::ivec3& segments ) -> VsgMeshData {
    auto mesh = generator::BoxMesh{ { 0.5, 0.5, 0.5 }, { 1, 1, 1 } };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::sphere( double radius, int slices, int segments, double sliceStart, double sliceSweep,
                                   double segmentStart, double segmentSweep ) -> VsgMeshData {
    auto mesh = generator::SphereMesh{ radius, slices, segments, sliceStart, sliceSweep, segmentStart, segmentSweep };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::cylinder() -> VsgMeshData {
    return {};
}

auto VsgMeshDataGenerator::capsule() -> VsgMeshData {
    return {};
}

auto VsgMeshDataGenerator::torus() -> VsgMeshData {
    return {};
}

auto VsgMeshDataGenerator::cone() -> VsgMeshData {
    return {};
}

}  // namespace tire
