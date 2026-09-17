
#include <vsg/all.h>

#include "cylinder.h"

namespace tire::object {

namespace {

auto cylinderData() -> std::tuple<vsg::ref_ptr<vsg::vec3Array>, vsg::ref_ptr<vsg::vec3Array>,
                                  vsg::ref_ptr<vsg::vec2Array>, vsg::ref_ptr<vsg::uintArray>> {
    auto vertices = vsg::vec3Array::create();

    auto colors = vsg::vec3Array::create();

    auto texcoords = vsg::vec2Array::create();

    auto indices = vsg::uintArray::create();

    return { vertices, colors, texcoords, indices };
}

}  // namespace

Cylinder::Cylinder( QObject* parent ) {
}

}  // namespace tire::object