//
//  bvh.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_bvh_h
#define tracer_bvh_h

#include "mesh.h"
#include "aabb.h"
#include "alignedallocator.h"

// ============================================================================
// =================================== Bvh ====================================
// ============================================================================

class Bvh {
public:
    struct Node {
        unsigned _kind, _parent;
        unsigned _first, _last;
        Aabb _left, _right;
    };

private:
    unsigned _depth;
    Mesh& _mesh;
    std::vector<Node, AlignedAllocator<Node, 64>> _nodes;

public:
    Bvh( Mesh& mesh );

    auto depth() const -> unsigned;

    auto nodes() const -> const Node*;

    auto triangles() const -> const Triangle*;

    auto makeImplicit() -> void;

private:
    auto buildImplicit( int node, Bvh::Node* implicitNodes, int depth, int levelIndex ) -> void;

    auto build( const Aabb& bounds, unsigned node, unsigned first, unsigned last, Triangle* triangles, unsigned depth )
        -> void;

    auto medianPartition( const Aabb& bounds, unsigned first, unsigned last, Triangle* triangles, unsigned depth,
                          unsigned& kind ) -> unsigned;

    auto sahPartition( const Aabb& parentBounds, unsigned first, unsigned last, Triangle* triangles, unsigned depth,
                       unsigned& kind ) -> unsigned;
};

#endif
