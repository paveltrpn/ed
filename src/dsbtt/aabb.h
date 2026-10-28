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

struct Aabb {
    tire::algebra::vector<float, 3> min, max;

    void reset() {
        min = tire::algebra::vector<float, 3>( 1e8f, 1e8f, 1e8f );
        max = tire::algebra::vector<float, 3>( -1e8f, -1e8f, -1e8f );
    }

    tire::algebra::vector<float, 3> center() const { return ( min + max ) * 0.5f; }

    void include( const tire::algebra::vector<float, 3>& v ) {
        min[0] = std::min( min[0], v[0] );
        min[1] = std::min( min[1], v[1] );
        min[2] = std::min( min[2], v[2] );

        max[0] = std::max( max[0], v[0] );
        max[1] = std::max( max[1], v[1] );
        max[2] = std::max( max[2], v[2] );
    }

    void include( const Triangle& t ) {
        include( t.p0 );
        include( t.p1 );
        include( t.p2 );
    }

    void include( const Aabb& aabb ) {
        min[0] = std::min( min[0], aabb.min[0] );
        min[1] = std::min( min[1], aabb.min[1] );
        min[2] = std::min( min[2], aabb.min[2] );

        max[0] = std::max( max[0], aabb.max[0] );
        max[1] = std::max( max[1], aabb.max[1] );
        max[2] = std::max( max[2], aabb.max[2] );
    }

    unsigned largestAxis() const {
        tire::algebra::vector<float, 3> d = max - min;
        if ( d[0] > d[1] && d[0] > d[2] ) {
            return 0;
        }
        if ( d[1] > d[2] ) {
            return 1;
        }
        return 2;
    }

    float surfaceArea() const {
        tire::algebra::vector<float, 3> d = max - min;
        return 2.0f * ( d[0] * d[1] + d[0] * d[2] + d[1] * d[2] );
    }

    inline float intersect( const Ray& ray ) const {
        float t0 = ray.minT;
        float t1 = ray.maxT;
        const float* minData = min.data();
        const float* maxData = max.data();
        const float* originData = ray.origin.data();
        const float* invDirData = ray.invDir.data();

        for ( int i = 0; i < 3; ++i ) {
            float near = ( minData[i] - originData[i] ) * invDirData[i];
            float far = ( maxData[i] - originData[i] ) * invDirData[i];

            if ( near > far ) std::swap( near, far );

            if ( near > t0 ) t0 = near;
            if ( far < t1 ) t1 = far;

            if ( t0 > t1 ) return ray.maxT;
        }

        return t0;
    }
};

#endif
