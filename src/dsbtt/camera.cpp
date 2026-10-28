//
//  camera.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "camera.h"

auto Camera::lookAt( const tire::algebra::vector<float, 3>& origin, const tire::algebra::vector<float, 3>& target,
                     const tire::algebra::vector<float, 3>& up, float fov, float near, float far, int width, int height )
    -> void {
    const auto forward{ tire::algebra::normalize( target - origin ) };
    const auto right{ tire::algebra::normalize( tire::algebra::cross( forward, up ) ) };
    const auto cameraUp{ tire::algebra::cross( right, forward ) };

    const auto aspect{ static_cast<float>( height ) / static_cast<float>( width ) };
    const auto imageExtentX{ std::tan( tire::algebra::degToRad( 0.5f * fov ) ) };
    const auto imageExtentY{ std::tan( tire::algebra::degToRad( 0.5f * fov * aspect ) ) };

    _origin = origin;
    _right = right * ( 2.0f / static_cast<float>( width ) * imageExtentX );
    _up = cameraUp * ( -2.0f / static_cast<float>( height ) * imageExtentY );
    _view = forward - right * imageExtentX + cameraUp * imageExtentY;
}

auto Camera::generateRay( float px, float py ) const -> Ray {
    const auto d{
        dsbtt::normalize( dsbtt::add( dsbtt::add( _view, dsbtt::scale( _right, px ) ), dsbtt::scale( _up, py ) ) )
    };
    const auto* data{ d.data() };

    return { _origin, d, tire::algebra::vector<float, 3>{ 1.0f / data[0], 1.0f / data[1], 1.0f / data[2] }, 1e-3f,
             1e+6 };
}
