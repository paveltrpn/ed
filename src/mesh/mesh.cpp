
#include "mesh.h"

namespace tire {

// ============================================================================
// ======================= Mesh ===============================================
// ============================================================================

Mesh::Mesh( const std::string& name )
    : _name{ name } {
}

auto Mesh::name() -> std::string {
    return _name;
}

// ============================================================================
// ======================= OBJMesh ============================================
// ============================================================================

OBJMesh::OBJMesh( std::string name, std::vector<float> vertices, std::vector<float> vnormals,
                  std::vector<float> txcoords, std::vector<ObjFileTriangleIndices> indices )
    : Mesh( name )
    , _vertices{ std::move( vertices ) }
    , _vnormals{ std::move( vnormals ) }
    , _txcoords{ std::move( txcoords ) }
    , _indices{ std::move( indices ) } {
}

auto OBJMesh::vertices() const -> const std::vector<float>& {
    return _vertices;
}

auto OBJMesh::vnormals() const -> const std::vector<float>& {
    return _vnormals;
}

auto OBJMesh::txcoords() const -> const std::vector<float>& {
    return _txcoords;
}

auto OBJMesh::indices() const -> const std::vector<ObjFileTriangleIndices>& {
    return _indices;
}

// ============================================================================
// ======================= ArraysMesh =========================================
// ============================================================================

auto ArraysMesh::vertices() const -> const std::vector<float>& {
    return _vertices;
}

auto ArraysMesh::vnormals() const -> const std::vector<float>& {
    return _vnormals;
}

auto ArraysMesh::txcoords() const -> const std::vector<float>& {
    return _txcoords;
}

// ============================================================================
// ======================= IndexedMesh ========================================
// ============================================================================

auto IndexedMesh::vertices() const -> const std::vector<float>& {
    return _vertices;
}

auto IndexedMesh::vnormals() const -> const std::vector<float>& {
    return _vnormals;
}

auto IndexedMesh::txcoords() const -> const std::vector<float>& {
    return _txcoords;
}

auto IndexedMesh::indices() const -> const std::vector<int>& {
    return _indices;
}

// ============================================================================
// ======================= InterleavedMesh ====================================
// ============================================================================

auto InterleavedMesh::mesh() const -> const std::vector<float>& {
    return _mesh;
}

}  // namespace tire
