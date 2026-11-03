
#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

#include "serialization.h"

namespace tire {

// ======================================================================================
// ==================== ObjChunk ========================================================
// ======================================================================================

struct ObjChunk {
    struct TriangleIndices {
        std::array<size_t, 3> _vertexIndices;
        std::array<size_t, 3> _normalIndices;
        std::array<size_t, 3> _texCoordIndices;
    };

    std::vector<double> _vertices;
    std::vector<double> _vnormals;
    std::vector<double> _txcoords;
    std::vector<TriangleIndices> _triangles;
};

// ======================================================================================
// ==================== ObjSerializationVisitor =========================================
// ======================================================================================

struct ObjSerializationVisitor final : public SerializationVisitor {
    ~ObjSerializationVisitor() override = default;

    auto serialize( BoxNode *object ) -> void override;
    auto serialize( SphereNode *object ) -> void override;
    auto serialize( CylinderNode *object ) -> void override;
    auto serialize( CapsuleNode *object ) -> void override;
    auto serialize( ConeNode *object ) -> void override;
    auto serialize( TorusNode *object ) -> void override;
    auto serialize( IcosphereNode *object ) -> void override;
    auto serialize( IcosahedronNode *object ) -> void override;
    auto serialize( DodecahedronNode *object ) -> void override;
    auto serialize( PlaneNode *object ) -> void override;
    auto serialize( DiskNode *object ) -> void override;
    auto serialize( TubeNode *object ) -> void override;
    auto serialize( CappedTubeNode *object ) -> void override;
    auto serialize( RoundedBoxNode *object ) -> void override;
    auto serialize( SphericalConeNode *object ) -> void override;
    auto serialize( TorusKnotNode *object ) -> void override;
    auto serialize( SpringNode *object ) -> void override;
    auto serialize( TeapotNode *object ) -> void override;

    [[nodiscard]] auto compile() const -> std::string;

private:
    auto serializeMesh( SceneObjectNode *object ) -> void;

private:
    std::vector<ObjChunk> _chunks;
};

}  // namespace tire
