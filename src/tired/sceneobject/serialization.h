
#pragma once

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

struct JsonSerializationVisitor;

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

}  // namespace tire
