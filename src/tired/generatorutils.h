#pragma once

#include <vsg/all.h>

namespace tire {

// ======================================================================================
// ==================== VsgMeshData =====================================================
// ======================================================================================

struct VsgMeshData final {
    vsg::ref_ptr<vsg::vec3Array> vertices{};
    vsg::ref_ptr<vsg::vec3Array> normals{};
    vsg::ref_ptr<vsg::vec3Array> colors{};
    vsg::ref_ptr<vsg::vec2Array> texcrds{};
    vsg::ref_ptr<vsg::uintArray> indices{};
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
