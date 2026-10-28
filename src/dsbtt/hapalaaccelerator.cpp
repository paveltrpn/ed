//
//  hapalaaccelerator.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-07.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "hapalaaccelerator.h"

inline auto nearChild( const Bvh::Node& node, const Ray& ray ) -> unsigned {
    if ( ray._dir.data()[node._kind - 1] > 0.0f ) {
        return node._first;
    } else {
        return node._last;
    }
}

HapalaAccelerator::HapalaAccelerator( Bvh& bvh )
    : _bvh{ bvh } {
}

auto HapalaAccelerator::trace( Ray& ray ) const -> const Triangle* {
    const Triangle* nearest{};

    const auto* triangles{ _bvh.triangles() };
    const auto* nodes{ _bvh.nodes() };

    unsigned last{ 0xffffffff };
    unsigned current{ 0 };

    do {
        const auto& n{ nodes[current] };

        if ( !n._kind ) {
            for ( size_t i{ n._first }; i < n._last; ++i ) {
                if ( triangles[i].intersect( ray ) ) {
                    nearest = &triangles[i];
                }
            }

            // Go up.
            last = current;
            current = n._parent;
            continue;
        }

        const auto near{ nearChild( n, ray ) };
        auto far{ n._last };
        if ( near != n._first ) {
            far = n._first;
        }

        if ( last == far ) {
            // Already returned from far child − traverse up.
            last = current;
            current = n._parent;
            continue;
        }

        // If coming from parent, try near child, else far child.
        auto tryChild{ far };
        if ( last == n._parent ) {
            tryChild = near;
        }
        const auto* bb{ &n._right };
        if ( tryChild == n._first ) {
            bb = &n._left;
        }

        const auto maxT{ ray._maxT };

        if ( bb->intersect( ray ) != maxT ) {
            // If box was hit, descend.
            last = current;
            current = tryChild;
            continue;
        }

        if ( tryChild == near ) {
            // Next is far.
            last = near;
        } else {
            // Go up instead.
            last = current;
            current = n._parent;
        }
    } while ( current != 0xffffffff );

    return nearest;
}
