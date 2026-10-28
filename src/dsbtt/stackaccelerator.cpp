//
//  stackaccelerator.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "stackaccelerator.h"

StackAccelerator::StackAccelerator( Bvh& bvh )
    : _bvh{ bvh } {
}

auto StackAccelerator::trace( Ray& ray ) const -> const Triangle* {
    const Triangle* nearest{};

    const auto* triangles{ _bvh.triangles() };
    const auto* nodes{ _bvh.nodes() };

    unsigned stack[32]{};
    auto* stackPtr{ stack };

    unsigned node{ 0 };

    for ( ;; ) {
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
                if ( t0 < t1 ) {
                    node = n._first;
                    if ( t1 != maxT ) {
                        *( stackPtr++ ) = n._last;
                    }
                } else {
                    node = n._last;
                    if ( t0 != maxT ) {
                        *( stackPtr++ ) = n._first;
                    }
                }
                continue;
            }
        }

        if ( stack == stackPtr ) {
            break;
        }

        node = *( --stackPtr );
    }

    return nearest;
}
