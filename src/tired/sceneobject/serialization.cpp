
#include "serialization.h"

namespace tire {

// ======================================================================================
// ==================== SerializationVisitor ============================================
// ======================================================================================

auto SerializationVisitor::serialize( BoxNode *object ) -> void {
}

auto SerializationVisitor::serialize( SphereNode *object ) -> void {
}

auto SerializationVisitor::serialize( CylinderNode *object ) -> void {
}

auto SerializationVisitor::serialize( CapsuleNode *object ) -> void {
}

auto SerializationVisitor::serialize( ConeNode *object ) -> void {
}

auto SerializationVisitor::serialize( TorusNode *object ) -> void {
}

// ======================================================================================
// ==================== Serializeable ===================================================
// ======================================================================================

// ======================================================================================
// ==================== SerializationVisitor ============================================
// ======================================================================================

auto JsonSerializationVisitor::baseData( SceneObjectNode *object ) const -> QJsonObject {
}

auto JsonSerializationVisitor::serialize( BoxNode *object ) -> void {
}

auto JsonSerializationVisitor::serialize( SphereNode *object ) -> void {
}

auto JsonSerializationVisitor::serialize( CylinderNode *object ) -> void {
}

auto JsonSerializationVisitor::serialize( CapsuleNode *object ) -> void {
}

auto JsonSerializationVisitor::serialize( ConeNode *object ) -> void {
}

auto JsonSerializationVisitor::serialize( TorusNode *object ) -> void {
}

}  // namespace tire
