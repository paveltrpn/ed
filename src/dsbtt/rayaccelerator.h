//
//  rayaccelerator.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_rayaccelerator_h
#define tracer_rayaccelerator_h

#include "ray.h"
#include "mesh.h"

// ============================================================================
// ============================== RayAccelerator ==============================
// ============================================================================
class RayAccelerator {
public:
    virtual auto trace( Ray& ray ) const -> const Triangle* = 0;
    virtual ~RayAccelerator() {}
};

#endif
