
#include "trackball.h"

namespace tire {

Trackball::Trackball( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel )
    : vsg::Trackball{ camera, ellipsoidModel } {
}

void Trackball::apply( vsg::MoveEvent& moveEvent ) {
    vsg::Trackball::apply( moveEvent );
}

}  // namespace tire
