
#pragma once

#include <vector>
#include <memory>

namespace tire {

// Dual-Indexing (Per-Face Normals/UVs).
// Compatible with .obj, FBX, glTF
struct ObjFileTriangleIndices {
    int vertexIndex[3];
    int normalIndex[3];
    int texCoordIndex[3];
};

struct Mesh {
    Mesh( const std::string& name );

    Mesh( const Mesh& other ) = delete;
    Mesh( Mesh&& other ) = delete;
    auto operator=( const Mesh& other ) -> Mesh& = delete;
    auto operator=( Mesh&& other ) -> Mesh& = delete;

    virtual ~Mesh() = default;

    [[nodiscard]] auto name() -> std::string;

protected:
    std::string _name{};
};

struct OBJMesh : Mesh {
    OBJMesh( std::string name, std::vector<float> vertices, std::vector<float> vnormals, std::vector<float> txcoords,
             std::vector<ObjFileTriangleIndices> indices );

    [[nodiscard]] auto vertices() const -> const std::vector<float>&;

    [[nodiscard]] auto vnormals() const -> const std::vector<float>&;

    [[nodiscard]] auto txcoords() const -> const std::vector<float>&;

    [[nodiscard]] auto indices() const -> const std::vector<ObjFileTriangleIndices>&;

private:
    std::vector<float> _vertices{};
    std::vector<float> _vnormals{};
    std::vector<float> _txcoords{};
    std::vector<ObjFileTriangleIndices> _indices{};
};

struct ArraysMesh : Mesh {
    [[nodiscard]] auto vertices() const -> const std::vector<float>&;

    [[nodiscard]] auto vnormals() const -> const std::vector<float>&;

    [[nodiscard]] auto txcoords() const -> const std::vector<float>&;

private:
    std::vector<float> _vertices{};
    std::vector<float> _vnormals{};
    std::vector<float> _txcoords{};
};

struct IndexedMesh : Mesh {
    [[nodiscard]] auto vertices() const -> const std::vector<float>&;

    [[nodiscard]] auto vnormals() const -> const std::vector<float>&;

    [[nodiscard]] auto txcoords() const -> const std::vector<float>&;

    [[nodiscard]] auto indices() const -> const std::vector<int>&;

private:
    std::vector<float> _vertices{};
    std::vector<float> _vnormals{};
    std::vector<float> _txcoords{};
    std::vector<int> _indices{};
};

struct InterleavedMesh : Mesh {
    [[nodiscard]] auto mesh() const -> const std::vector<float>&;

private:
    std::vector<float> _mesh{};
};

}  // namespace tire
