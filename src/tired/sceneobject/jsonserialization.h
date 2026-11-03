
#pragma once

#include <QJsonObject>

#include "serialization.h"

namespace tire {

// ======================================================================================
// ==================== JsonSerializationVisitor ========================================
// ======================================================================================

struct JsonSerializationVisitor final : public SerializationVisitor {
    ~JsonSerializationVisitor() override = default;

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

    [[nodiscard]] auto json() const -> QJsonObject;

private:
    auto baseData( SceneObjectNode *object ) const -> QJsonObject;

private:
    QJsonObject _json{};
};

}  // namespace tire
