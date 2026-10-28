//
//  ray.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_ray_h
#define tracer_ray_h

#include <cmath>

#include "algebra/algebra.h"

namespace dsbtt {

inline auto add( const tire::algebra::vector<float, 3>& a, const tire::algebra::vector<float, 3>& b )
    -> tire::algebra::vector<float, 3> {
    const float* aData = a.data();
    const float* bData = b.data();
    return tire::algebra::vector<float, 3>( aData[0] + bData[0], aData[1] + bData[1], aData[2] + bData[2] );
}

inline auto subtract( const tire::algebra::vector<float, 3>& a, const tire::algebra::vector<float, 3>& b )
    -> tire::algebra::vector<float, 3> {
    const float* aData = a.data();
    const float* bData = b.data();
    return tire::algebra::vector<float, 3>( aData[0] - bData[0], aData[1] - bData[1], aData[2] - bData[2] );
}

inline auto negate( const tire::algebra::vector<float, 3>& v ) -> tire::algebra::vector<float, 3> {
    const float* data = v.data();
    return tire::algebra::vector<float, 3>( -data[0], -data[1], -data[2] );
}

inline auto scale( const tire::algebra::vector<float, 3>& v, float factor ) -> tire::algebra::vector<float, 3> {
    const float* data = v.data();
    return tire::algebra::vector<float, 3>( data[0] * factor, data[1] * factor, data[2] * factor );
}

inline auto normalize( const tire::algebra::vector<float, 3>& v ) -> tire::algebra::vector<float, 3> {
    const float* data = v.data();
    const float invLength = 1.0f / std::sqrt( data[0] * data[0] + data[1] * data[1] + data[2] * data[2] );
    return tire::algebra::vector<float, 3>( data[0] * invLength, data[1] * invLength, data[2] * invLength );
}

inline auto dot( const tire::algebra::vector<float, 3>& a, const tire::algebra::vector<float, 3>& b ) -> float {
    const float* aData = a.data();
    const float* bData = b.data();
    return aData[0] * bData[0] + aData[1] * bData[1] + aData[2] * bData[2];
}

inline auto cross( const tire::algebra::vector<float, 3>& a, const tire::algebra::vector<float, 3>& b )
    -> tire::algebra::vector<float, 3> {
    const float* aData = a.data();
    const float* bData = b.data();
    return tire::algebra::vector<float, 3>( aData[1] * bData[2] - aData[2] * bData[1],
                                           aData[2] * bData[0] - aData[0] * bData[2],
                                           aData[0] * bData[1] - aData[1] * bData[0] );
}

}

struct Ray {
    tire::algebra::vector<float, 3> origin, dir, invDir;
    float minT, maxT;
};

#endif
