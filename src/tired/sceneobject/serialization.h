#pragma once

#include <QJsonObject>

namespace tire {

struct SceneObjectNode;
struct BoxNode;
struct SphereNode;
struct CylinderNode;
struct CapsuleNode;
struct ConeNode;
struct TorusNode;
struct IcosphereNode;
struct IcosahedronNode;
struct DodecahedronNode;
struct PlaneNode;
struct DiskNode;
struct TubeNode;
struct CappedTubeNode;
struct RoundedBoxNode;
struct SphericalConeNode;
struct TorusKnotNode;
struct SpringNode;
struct TeapotNode;

// ======================================================================================
// ==================== SerializationVisitor ============================================
// ======================================================================================

struct SerializationVisitor {
    virtual ~SerializationVisitor() = default;

    virtual auto serialize( BoxNode *object ) -> void = 0;
    virtual auto serialize( SphereNode *object ) -> void = 0;
    virtual auto serialize( CylinderNode *object ) -> void = 0;
    virtual auto serialize( CapsuleNode *object ) -> void = 0;
    virtual auto serialize( ConeNode *object ) -> void = 0;
    virtual auto serialize( TorusNode *object ) -> void = 0;
    virtual auto serialize( IcosphereNode *object ) -> void = 0;
    virtual auto serialize( IcosahedronNode *object ) -> void = 0;
    virtual auto serialize( DodecahedronNode *object ) -> void = 0;
    virtual auto serialize( PlaneNode *object ) -> void = 0;
    virtual auto serialize( DiskNode *object ) -> void = 0;
    virtual auto serialize( TubeNode *object ) -> void = 0;
    virtual auto serialize( CappedTubeNode *object ) -> void = 0;
    virtual auto serialize( RoundedBoxNode *object ) -> void = 0;
    virtual auto serialize( SphericalConeNode *object ) -> void = 0;
    virtual auto serialize( TorusKnotNode *object ) -> void = 0;
    virtual auto serialize( SpringNode *object ) -> void = 0;
    virtual auto serialize( TeapotNode *object ) -> void = 0;
};

// ======================================================================================
// ==================== Serializeable ===================================================
// ======================================================================================

struct Serializeable {
    virtual auto serialize( SerializationVisitor *visitor ) -> void = 0;
};

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
