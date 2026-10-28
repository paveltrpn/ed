//
//  random.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_random_h
#define tracer_random_h

// ============================================================================
// ================================== Random ==================================
// ============================================================================
class Random {
private:
    unsigned int _w;
    unsigned int _z;

public:
    Random( int seed ) {
        _w = seed + 1;
        _z = seed * seed + seed + 2;
    }

    auto nextInt() -> int {
        _z = 36969 * ( _z & 65535 ) + ( _z >> 16 );
        _w = 18000 * ( _w & 65535 ) + ( _w >> 16 );

        return ( _z << 16 ) + _w;
    }

    auto nextFloat() -> float {
        //
        return ( nextInt() & 0xffff ) * ( 1.0f / 0xffff );
    }
};

#endif
