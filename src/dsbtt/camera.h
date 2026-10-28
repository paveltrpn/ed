//
//  camera.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_camera_h
#define tracer_camera_h

#include "ray.h"

class Camera {
private:
    tire::algebra::vector<float, 3> origin;
    tire::algebra::vector<float, 3> view;
    tire::algebra::vector<float, 3> right;
    tire::algebra::vector<float, 3> up;

public:
    void lookAt( const tire::algebra::vector<float, 3>& origin, const tire::algebra::vector<float, 3>& target,
                 const tire::algebra::vector<float, 3>& up, float fov, float near, float far, int width, int height );

    Ray generateRay( float px, float py ) const;
};

#endif
