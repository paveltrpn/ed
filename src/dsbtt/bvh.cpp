//
//  bvh.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "bvh.h"
#include <algorithm>

// ============================================================================
// ================================ Predicate =================================
// ============================================================================
struct Predicate {
    unsigned _axis;

    auto operator()( const Triangle& i, const Triangle& j ) const -> bool {
        //
        return i.center()[_axis] < j.center()[_axis];
    }
};

Bvh::Bvh( Mesh& mesh )
    : _mesh{ mesh } {
    _depth = 0;

    Node root{ 0, 0xffffffff };
    _nodes.push_back( root );

    auto& triangles{ _mesh.triangles() };

    Aabb bounds{};
    bounds.reset();

    for ( size_t i{ 0 }; i < triangles.size(); ++i ) {
        bounds.include( triangles[i] );
    }

    build( bounds, 0, 0, static_cast<unsigned>( triangles.size() ), &triangles[0], 0 );
}

auto Bvh::depth() const -> unsigned {
    return _depth;
}

auto Bvh::nodes() const -> const Bvh::Node* {
    return &_nodes[0];
}

auto Bvh::triangles() const -> const Triangle* {
    return &_mesh.triangles()[0];
}

auto Bvh::makeImplicit() -> void {
    std::vector<Bvh::Node, AlignedAllocator<Bvh::Node, 64>> implicitNodes{};
    implicitNodes.resize( ( 1 << ( _depth + 1 ) ) - 1 );

    buildImplicit( 0, &implicitNodes[0], 0, 0 );
    std::swap( _nodes, implicitNodes );
}

auto Bvh::buildImplicit( int node, Bvh::Node* implicitNodes, int depth, int levelIndex ) -> void {
    auto n{ _nodes[node] };

    n._parent = ( 1 << ( depth - 1 ) ) - 1 + ( levelIndex >> 1 );

    if ( n._kind ) {
        n._first = ( 1 << ( depth + 1 ) ) - 1 + ( levelIndex << 1 );
        n._last = ( 1 << ( depth + 1 ) ) + ( levelIndex << 1 );
    }

    implicitNodes[( 1 << depth ) - 1 + levelIndex] = n;

    if ( n._kind ) {
        buildImplicit( _nodes[node]._first, implicitNodes, depth + 1, levelIndex << 1 );
        buildImplicit( _nodes[node]._last, implicitNodes, depth + 1, ( levelIndex << 1 ) + 1 );
    }
}

auto Bvh::build( const Aabb& bounds, unsigned node, unsigned first, unsigned last, Triangle* triangles, unsigned depth )
    -> void {
    if ( _depth < depth ) {
        _depth = depth;
    }

    if ( last - first < 8 ) {
        _nodes[node]._first = first;
        _nodes[node]._last = last;
        return;
    }

    unsigned pivot{};

    if ( depth > 8 ) {
        pivot = medianPartition( bounds, first, last, triangles, depth, _nodes[node]._kind );
    } else {
        pivot = sahPartition( bounds, first, last, triangles, depth, _nodes[node]._kind );
    }

    if ( pivot == 0xffffffff ) {
        _nodes[node]._first = first;
        _nodes[node]._last = last;
        return;
    }

    const auto left{ static_cast<unsigned>( _nodes.size() ) };
    const auto right{ static_cast<unsigned>( _nodes.size() + 1 ) };

    Aabb leftBounds{}, rightBounds{};
    leftBounds.reset();
    rightBounds.reset();

    for ( size_t i{ first }; i < pivot; ++i ) {
        leftBounds.include( triangles[i] );
    }

    for ( size_t i{ pivot }; i < last; ++i ) {
        rightBounds.include( triangles[i] );
    }

    Node child{ 0, node };
    _nodes.push_back( child );
    _nodes.push_back( child );

    _nodes[node]._first = left;
    _nodes[node]._last = right;
    _nodes[node]._left = leftBounds;
    _nodes[node]._right = rightBounds;

    build( leftBounds, left, first, pivot, triangles, depth + 1 );
    build( rightBounds, right, pivot, last, triangles, depth + 1 );
}

auto Bvh::medianPartition( const Aabb& bounds, unsigned first, unsigned last, Triangle* triangles, unsigned depth,
                           unsigned& kind ) -> unsigned {
    const auto pivot{ ( first + last ) / 2 };

    auto bestDim{ bounds.largestAxis() };
    auto bestSeparation{ -1e+10f };

    for ( size_t dim{ 0 }; dim < 3; ++dim ) {
        Predicate predicate{ static_cast<unsigned>( dim ) };
        std::sort( triangles + first, triangles + last, predicate );

        Aabb left{}, right{};
        left.reset();
        right.reset();

        for ( size_t i{ first }; i < pivot; ++i ) {
            left.include( triangles[i] );
        }

        for ( size_t i{ pivot }; i < last; ++i ) {
            right.include( triangles[i] );
        }

        const auto separation{ right.center()[dim] - left.center()[dim] };

        if ( separation > bestSeparation ) {
            bestDim = static_cast<unsigned>( dim );
            bestSeparation = separation;
        }
    }

    kind = bestDim + 1;
    Predicate predicate{ bestDim };
    std::sort( triangles + first, triangles + last, predicate );
    return pivot;
}

auto Bvh::sahPartition( const Aabb& parentBounds, unsigned first, unsigned last, Triangle* triangles, unsigned depth,
                        unsigned& kind ) -> unsigned {
    const auto traversalCost{ 2.0f };
    const auto intersectionCost{ 1.0f };

    static std::vector<float> accumulatedArea{};

    const auto divPsa{ 1.0f / parentBounds.surfaceArea() };

    auto bestSah{ 1e+10f };
    auto bestDim{ 0xffffffff };
    auto bestSplit{ 0xffffffff };

    for ( size_t dim{ 0 }; dim < 3; ++dim ) {
        Predicate predicate{ static_cast<unsigned>( dim ) };
        std::sort( triangles + first, triangles + last, predicate );

        Aabb bounds{};
        bounds.reset();
        accumulatedArea.resize( 0 );

        for ( size_t i{ first }; i < last - 1; ++i ) {
            bounds.include( triangles[i] );
            accumulatedArea.push_back( bounds.surfaceArea() );
        }

        bounds.reset();

        for ( size_t i{ last - 1 }; i > first; --i ) {
            bounds.include( triangles[i] );

            const auto lsa{ accumulatedArea[i - first - 1] };
            const auto rsa{ bounds.surfaceArea() };

            const auto sah{ traversalCost + intersectionCost * divPsa *  // Constant cost.
                                ( lsa * static_cast<float>( i - first ) +  // Left child.
                                  rsa * static_cast<float>( last - i ) ) };  // Right child.

            if ( sah < bestSah ) {
                bestSah = sah;
                bestSplit = static_cast<unsigned>( i );
                bestDim = static_cast<unsigned>( dim );
            }
        }
    }

    if ( bestSah > static_cast<float>( last - first ) * intersectionCost ) {
        return 0xffffffff;
    }

    kind = bestDim + 1;
    Predicate predicate{ bestDim };
    std::sort( triangles + first, triangles + last, predicate );
    return bestSplit;
}
