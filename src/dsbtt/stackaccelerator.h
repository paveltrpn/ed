//
//  stackaccelerator.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_stackaccelerator_h
#define tracer_stackaccelerator_h

#include "bvh.h"
#include "rayaccelerator.h"

// ============================================================================
// ============================= StackAccelerator =============================
// ============================================================================
class StackAccelerator : public RayAccelerator {
private:
    Bvh& _bvh;

public:
    StackAccelerator( Bvh& bvh );

    virtual auto trace( Ray& ray ) const -> const Triangle*;
};

#endif
