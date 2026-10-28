//
//  camera.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "camera.h"

void Camera::lookAt( const tire::algebra::vector<float, 3>& origin, const tire::algebra::vector<float, 3>& target,
                     const tire::algebra::vector<float, 3>& up, float fov, float near, float far, int width, int height ) {
    tire::algebra::vector<float, 3> forward = tire::algebra::normalize( target - origin );
    tire::algebra::vector<float, 3> right = tire::algebra::normalize( tire::algebra::cross( forward, up ) );
    tire::algebra::vector<float, 3> cameraUp = tire::algebra::cross( right, forward );

    float aspect = (float)height / (float)width;
    float imageExtentX = std::tan( tire::algebra::degToRad( 0.5f * fov ) );
    float imageExtentY = std::tan( tire::algebra::degToRad( 0.5f * fov * aspect ) );

    this->origin = origin;
    this->right = right * ( 2.0f / (float)width * imageExtentX );
    this->up = cameraUp * ( -2.0f / (float)height * imageExtentY );
    this->view = forward - right * imageExtentX + cameraUp * imageExtentY;
}

Ray Camera::generateRay( float px, float py ) const {
    tire::algebra::vector<float, 3> d =
        dsbtt::normalize( dsbtt::add( dsbtt::add( view, dsbtt::scale( right, px ) ), dsbtt::scale( up, py ) ) );
    const float* data = d.data();

    return { origin, d, tire::algebra::vector<float, 3>( 1.0f / data[0], 1.0f / data[1], 1.0f / data[2] ), 1e-3f,
             1e+6 };
}
