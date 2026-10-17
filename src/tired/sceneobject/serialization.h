#pragma once

#include <QJsonObject>

#include "sceneobjectnode.h"
#include "box.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"
#include "cone.h"
#include "torus.h"

namespace tire {

// ======================================================================================
// ==================== SerializationVisitor ============================================
// ======================================================================================

struct SerializationVisitor {
    virtual ~SerializationVisitor() = default;

    auto serialize( BoxNode *object ) -> void;
    auto serialize( SphereNode *object ) -> void;
    auto serialize( CylinderNode *object ) -> void;
    auto serialize( CapsuleNode *object ) -> void;
    auto serialize( ConeNode *object ) -> void;
    auto serialize( TorusNode *object ) -> void;
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

    auto serialize( BoxNode *object ) -> void;
    auto serialize( SphereNode *object ) -> void;
    auto serialize( CylinderNode *object ) -> void;
    auto serialize( CapsuleNode *object ) -> void;
    auto serialize( ConeNode *object ) -> void;
    auto serialize( TorusNode *object ) -> void;

private:
    auto baseData( SceneObjectNode *object ) const -> QJsonObject;
};

}  // namespace tire
