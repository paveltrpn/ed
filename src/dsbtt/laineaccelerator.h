//
//  laineaccelerator.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-10.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_laineaccelerator_h
#define tracer_laineaccelerator_h

#include "bvh.h"
#include "rayaccelerator.h"

// ============================================================================
// ============================= LaineAccelerator =============================
// ============================================================================
template <unsigned ShortStackSize>
class LaineAccelerator : public RayAccelerator {
private:
    Bvh& _bvh;

public:
    LaineAccelerator( Bvh& bvh )
        : _bvh{ bvh } {}

    virtual auto trace( Ray& ray ) const -> const Triangle* {
        constexpr auto stackCapacity{ ShortStackSize + ( ShortStackSize == 0 ) };
        unsigned shortStack[stackCapacity]{};
        unsigned stackHead{ 0 };

        const Triangle* nearest{};

        const auto* triangles{ _bvh.triangles() };
        const auto* nodes{ _bvh.nodes() };

        unsigned level{ 0x80000000 };
        unsigned popLevel{ 0 };
        unsigned trail{ 0 };

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

                if ( t0 != maxT && t1 != maxT ) {
                    auto near{ n._first };
                    auto far{ n._last };

                    if ( t0 > t1 ) {
                        std::swap( near, far );
                    }

                    level >>= 1;

                    if ( trail & level ) {
                        node = far;
                    } else {
                        node = near;

                        if ( ShortStackSize > 0 ) {
                            // Push far to short stack.
                            if ( stackHead < ShortStackSize ) {
                                shortStack[stackHead++] = far;
                            } else {
                                for ( size_t i{ 0 }; i < ShortStackSize - 1; ++i ) {
                                    shortStack[i] = shortStack[i + 1];
                                }
                                shortStack[ShortStackSize - 1] = far;
                            }
                        }
                    }
                    continue;
                } else if ( t0 != maxT || t1 != maxT ) {
                    level >>= 1;

                    if ( level != popLevel ) {
                        trail |= level;
                        if ( t0 != maxT ) {
                            node = n._first;
                        } else {
                            node = n._last;
                        }
                        continue;
                    }
                }
            }

            trail &= static_cast<unsigned>( -static_cast<int>( level ) );
            trail += level;

            const auto temp{ trail >> 1 };
            level = ( ( ( temp - 1 ) ^ temp ) + 1 );

            if ( trail & 0x80000000 ) {
                break;
            }

            popLevel = level;

            if ( ShortStackSize > 0 && stackHead > 0 ) {
                // Pop short stack.
                node = shortStack[--stackHead];
            } else {
                node = 0;
                level = 0x80000000;
            }
        }

        return nearest;
    }
};

#endif
