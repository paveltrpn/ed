
#pragma once

#include <cstddef>
#include <string>

#include "serialization.h"

namespace tire {

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
    std::string _output{ "o scene\n" };
    size_t _positionOffset{};
    size_t _uvOffset{};
    size_t _normalOffset{};
};

}  // namespace tire
