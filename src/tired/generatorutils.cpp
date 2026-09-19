#include "generator/BoxMesh.hpp"

#include "generatorutils.h"

namespace tire {

auto VsgMeshDataGenerator::box() -> VsgMeshData {
    auto mesh = generator::BoxMesh{ { 0.5, 0.5, 0.5 }, { 1, 1, 1 } };
    return copy( mesh.vertices(), mesh.triangles() );
}

auto VsgMeshDataGenerator::sphere() -> VsgMeshData {
    return {};
}

auto VsgMeshDataGenerator::cylinder() -> VsgMeshData {
    return {};
}

auto VsgMeshDataGenerator::capsule() -> VsgMeshData {
    return {};
}

}  // namespace tire
