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

// ============================================================================
// ================================= Triangle =================================
// ============================================================================
struct Triangle {
    tire::algebra::vector<float, 3> _p0, _p1, _p2;
    tire::algebra::vector<float, 3> _n;
    float _padding[4];

    auto center() const -> tire::algebra::vector<float, 3> {
        //
        return ( _p0 + _p1 + _p2 ) * ( 1.0f / 3.0f );
    }

    inline auto intersect( Ray& ray ) const -> bool {
        const auto epsilon{ 1e-6f };

        const auto d{ ray._dir };
        const auto p{ ray._origin };

        const auto e1{ dsbtt::subtract( _p1, _p0 ) };
        const auto e2{ dsbtt::subtract( _p2, _p0 ) };

        const auto n{ dsbtt::cross( e1, e2 ) };
        auto s{ -dsbtt::dot( d, n ) };

        if ( std::fabs( s ) < epsilon ) {
            return false;
        }

        s = 1.0f / s;

        const auto r{ dsbtt::subtract( p, _p0 ) };
        const auto t{ dsbtt::dot( r, n ) * s };

        if ( t <= ray._minT || t >= ray._maxT ) {
            return false;
        }

        const auto q{ dsbtt::negate( dsbtt::cross( d, r ) ) };
        const auto v{ dsbtt::dot( q, e2 ) * s };

        if ( v < 0.0f ) {
            return false;
        }

        const auto w{ -dsbtt::dot( e1, q ) * s };

        if ( w < 0.0f || v + w > 1.0f ) {
            return false;
        }

        ray._maxT = t;
        return true;
    }
};

#endif
