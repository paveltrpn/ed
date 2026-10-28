//
//  implicitbaccelerator.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2013-02-14.
//  Copyright (c) 2013 Lund University. All rights reserved.
//

#include "implicitbaccelerator.h"

inline auto sibling( unsigned levelIndex ) -> unsigned {
    return levelIndex + 1 - ( ( levelIndex & 1 ) << 1 );
}

ImplicitBAccelerator::ImplicitBAccelerator( Bvh& bvh )
    : _bvh{ bvh } {
    _bvh.makeImplicit();
}

auto ImplicitBAccelerator::trace( Ray& ray ) const -> const Triangle* {
    const Triangle* nearest{};

    const auto* triangles{ _bvh.triangles() };
    const auto* nodes{ _bvh.nodes() };

    unsigned node{ 1 };
    unsigned traversal{ 0 };

    for ( ;; ) {
        const auto& n{ nodes[node - 1] };

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
                node += node + ( t0 >= t1 );
                traversal += traversal + single;
                continue;
            }
        }

        ++traversal;

#ifdef _WIN32
        int up{};
        if ( !_BitScanForward( reinterpret_cast<unsigned long*>( &up ), traversal ) ) {
            up = 0;
        }
#else
        const auto up{ __builtin_ctz( traversal ) };
#endif

        traversal >>= up;
        node >>= up;

        const auto level{ node };

        node = sibling( node );

        if ( level <= 1 ) {
            break;
        }
    }

    return nearest;
}
