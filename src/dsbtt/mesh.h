//
//  mesh.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_mesh_h
#define tracer_mesh_h

#include "triangle.h"
#include "alignedallocator.h"
#include <iosfwd>
#include <vector>

// ============================================================================
// =================================== Mesh ===================================
// ============================================================================
class Mesh {
public:
    Mesh( const char* filename, bool flipFaces );
    Mesh( std::istream& input, bool flipFaces );

    auto triangles() -> std::vector<Triangle, AlignedAllocator<Triangle, 64>>&;

private:
    auto load( std::istream& input, bool flipFaces ) -> void;

private:
    std::vector<Triangle, AlignedAllocator<Triangle, 64>> _triangles;
};

#endif
