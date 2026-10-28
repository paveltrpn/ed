//
//  aabb.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_aabb_h
#define tracer_aabb_h

#include <algorithm>

#include "triangle.h"
#include "ray.h"

// ============================================================================
// =================================== Aabb ===================================
// ============================================================================
struct Aabb {
    tire::algebra::vector<float, 3> _min, _max;

    auto reset() -> void {
        _min = tire::algebra::vector<float, 3>{ 1e8f, 1e8f, 1e8f };
        _max = tire::algebra::vector<float, 3>{ -1e8f, -1e8f, -1e8f };
    }

    auto center() const -> tire::algebra::vector<float, 3> {
        //
        return ( _min + _max ) * 0.5f;
    }

    auto include( const tire::algebra::vector<float, 3>& v ) -> void {
        _min[0] = std::min( _min[0], v[0] );
        _min[1] = std::min( _min[1], v[1] );
        _min[2] = std::min( _min[2], v[2] );

        _max[0] = std::max( _max[0], v[0] );
        _max[1] = std::max( _max[1], v[1] );
        _max[2] = std::max( _max[2], v[2] );
    }

    auto include( const Triangle& t ) -> void {
        include( t._p0 );
        include( t._p1 );
        include( t._p2 );
    }

    auto include( const Aabb& aabb ) -> void {
        _min[0] = std::min( _min[0], aabb._min[0] );
        _min[1] = std::min( _min[1], aabb._min[1] );
        _min[2] = std::min( _min[2], aabb._min[2] );

        _max[0] = std::max( _max[0], aabb._max[0] );
        _max[1] = std::max( _max[1], aabb._max[1] );
        _max[2] = std::max( _max[2], aabb._max[2] );
    }

    auto largestAxis() const -> unsigned {
        const auto d{ _max - _min };
        if ( d[0] > d[1] && d[0] > d[2] ) {
            return 0;
        }
        if ( d[1] > d[2] ) {
            return 1;
        }
        return 2;
    }

    auto surfaceArea() const -> float {
        const auto d{ _max - _min };
        return 2.0f * ( d[0] * d[1] + d[0] * d[2] + d[1] * d[2] );
    }

    inline auto intersect( const Ray& ray ) const -> float {
        auto t0{ ray._minT };
        auto t1{ ray._maxT };
        const auto* minData{ _min.data() };
        const auto* maxData{ _max.data() };
        const auto* originData{ ray._origin.data() };
        const auto* invDirData{ ray._invDir.data() };

        for ( size_t i{ 0 }; i < 3; ++i ) {
            auto near{ ( minData[i] - originData[i] ) * invDirData[i] };
            auto far{ ( maxData[i] - originData[i] ) * invDirData[i] };

            if ( near > far ) {
                std::swap( near, far );
            }

            if ( near > t0 ) {
                t0 = near;
            }
            if ( far < t1 ) {
                t1 = far;
            }

            if ( t0 > t1 ) {
                return ray._maxT;
            }
        }

        return t0;
    }
};

#endif
