//
//  triangle.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_triangle_h
#define tracer_triangle_h

#include "ray.h"

struct Triangle {
    tire::algebra::vector<float, 3> p0, p1, p2;
    tire::algebra::vector<float, 3> n;
    float padding[4];

    tire::algebra::vector<float, 3> center() const { return ( p0 + p1 + p2 ) * ( 1.0f / 3.0f ); }

    inline bool intersect( Ray& ray ) const {
        const float epsilon = 1e-6f;

        tire::algebra::vector<float, 3> d = ray.dir;
        tire::algebra::vector<float, 3> p = ray.origin;

        tire::algebra::vector<float, 3> e1 = dsbtt::subtract( p1, p0 );
        tire::algebra::vector<float, 3> e2 = dsbtt::subtract( p2, p0 );

        tire::algebra::vector<float, 3> n = dsbtt::cross( e1, e2 );
        float s = -dsbtt::dot( d, n );

        if ( std::fabs( s ) < epsilon ) return false;

        s = 1.0f / s;

        tire::algebra::vector<float, 3> r = dsbtt::subtract( p, p0 );
        float t = dsbtt::dot( r, n ) * s;

        if ( t <= ray.minT || t >= ray.maxT ) return false;

        tire::algebra::vector<float, 3> q = dsbtt::negate( dsbtt::cross( d, r ) );
        float v = dsbtt::dot( q, e2 ) * s;

        if ( v < 0.0f ) return false;

        float w = -dsbtt::dot( e1, q ) * s;

        if ( w < 0.0f || v + w > 1.0f ) return false;

        ray.maxT = t;
        return true;
    }
};

#endif
