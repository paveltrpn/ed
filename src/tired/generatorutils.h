#pragma once

#include <vsg/all.h>

namespace tire {

// ======================================================================================
// ==================== VsgMeshData =====================================================
// ======================================================================================

struct VsgMeshData final {
    vsg::ref_ptr<vsg::vec3Array> _vertices{};
    vsg::ref_ptr<vsg::vec3Array> _normals{};
    vsg::ref_ptr<vsg::vec3Array> _colors{};
    vsg::ref_ptr<vsg::vec2Array> _texcrds{};
    vsg::ref_ptr<vsg::uintArray> _indices{};
};

// ======================================================================================
// ==================== VsgMeshDataGenerator ============================================
// ======================================================================================

struct VsgMeshDataGenerator final {
public:
    static auto box() -> VsgMeshData;
    static auto sphere() -> VsgMeshData;
    static auto cylinder() -> VsgMeshData;
    static auto capsule() -> VsgMeshData;

private:

};

}
