//
//  sparseaccelerator.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-07.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "sparseaccelerator.h"

inline auto sibling( int node ) -> int {
    return node + ( ( node & 1 ) << 1 ) - 1;
}

SparseAccelerator::SparseAccelerator( Bvh& bvh )
    : _bvh{ bvh } {
}

auto SparseAccelerator::trace( Ray& ray ) const -> const Triangle* {
    const Triangle* nearest{};

    const auto* triangles{ _bvh.triangles() };
    const auto* nodes{ _bvh.nodes() };

    unsigned levelIndex{ 0 };
    unsigned node{ 0 };

    do {
        const auto& n{ nodes[node] };

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

            if ( t0 != maxT || t1 != maxT ) {
                levelIndex += levelIndex;
                if ( t0 < t1 ) {
                    node = n._first;
                    if ( t1 == maxT ) {
                        ++levelIndex;
                    }
                } else {
                    node = n._last;
                    if ( t0 == maxT ) {
                        ++levelIndex;
                    }
                }
                continue;
            }
        }

        ++levelIndex;

        while ( ( levelIndex & 1 ) == 0 ) {
            node = nodes[node]._parent;
            levelIndex >>= 1;
        }

        node = sibling( node );
    } while ( node != 0xffffffff );

    return nearest;
}
