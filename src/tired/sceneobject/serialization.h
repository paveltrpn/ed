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
};

// ======================================================================================
// ==================== Serializeable ===================================================
// ======================================================================================

struct Serializeable {
    virtual auto serialize( SerializationVisitor *visitor ) -> void = 0;
};

// ======================================================================================
// ==================== SerializationVisitor ============================================
// ======================================================================================

struct JsonSerializationVisitor final : public SerializationVisitor {
    ~JsonSerializationVisitor() override = default;

    auto serialize( BoxNode *object ) -> void override;
    auto serialize( SphereNode *object ) -> void override;
    auto serialize( CylinderNode *object ) -> void override;
    auto serialize( CapsuleNode *object ) -> void override;
    auto serialize( ConeNode *object ) -> void override;
    auto serialize( TorusNode *object ) -> void override;

    [[nodiscard]] auto json() const -> QJsonObject;

private:
    auto baseData( SceneObjectNode *object ) const -> QJsonObject;

private:
    QJsonObject _json{};
};

}  // namespace tire
