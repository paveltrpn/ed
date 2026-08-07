
#include <vsg/all.h>

#include "sphere.h"

namespace tire::object {

namespace {

auto sphereData() -> std::tuple<vsg::ref_ptr<vsg::vec3Array>, vsg::ref_ptr<vsg::vec3Array>,
                                vsg::ref_ptr<vsg::vec2Array>, vsg::ref_ptr<vsg::uintArray>> {
    auto vertices = vsg::vec3Array::create();

    auto colors = vsg::vec3Array::create();

    auto texcoords = vsg::vec2Array::create();

    auto indices = vsg::uintArray::create();

    return { vertices, colors, texcoords, indices };
}

}  // namespace

Sphere::Sphere( const SphereObjectData& data )
    : _data{ data } {
}

auto Sphere::data() const -> tire::SphereObjectData const& {
    return _data;
}

auto Sphere::data() -> tire::SphereObjectData& {
    return _data;
}

tire::SphereObjectData Sphere::getData() {
    return _data;
}

}  // namespace tire::object
