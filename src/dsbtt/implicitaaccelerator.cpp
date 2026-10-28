//
//  implicitaaccelerator.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-07.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "implicitaaccelerator.h"

ImplicitAAccelerator::ImplicitAAccelerator( Bvh& bvh )
    : _bvh{ bvh } {
    _bvh.makeImplicit();
}

auto ImplicitAAccelerator::trace( Ray& ray ) const -> const Triangle* {
    const Triangle* nearest{};

    const auto* triangles{ _bvh.triangles() };
    const auto* nodes{ _bvh.nodes() };

    unsigned node{ 1 };
    unsigned swapMask{ 0 };

    do {
        const auto idx{ node - 1 + swapMask - ( ( node & swapMask ) << 1 ) };
        const auto& n{ nodes[idx] };

        if ( !n._kind ) {
            for ( size_t i{ n._first }; i < n._last; ++i ) {
                if ( triangles[i].intersect( ray ) ) {
                    nearest = &triangles[i];
                }
            }
        } else {
            const auto maxT{ ray._maxT };

            const auto t0{ n._left.intersect( ray ) };
            const auto t1{ n._right.intersect( ray ) };

            const auto firsthit{ static_cast<int>( t0 != maxT ) };
            const auto lasthit{ static_cast<int>( t1 != maxT ) };

            if ( firsthit | lasthit ) {
                const auto single{ firsthit ^ lasthit };
                swapMask += swapMask + ( ( t0 >= t1 ) ^ single );
                node += node + single;
                continue;
            }
        }

        ++node;

#ifdef _WIN32
        int up{};
        if ( !_BitScanForward( reinterpret_cast<unsigned long*>( &up ), node ) ) {
            up = 0;
        }
#else
        const auto up{ __builtin_ctz( node ) };
#endif

        node >>= up;
        swapMask >>= up;
    } while ( node > 1 );

    return nearest;
}
