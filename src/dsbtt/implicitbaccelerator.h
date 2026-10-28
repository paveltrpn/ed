//
//  implicitbaccelerator.h
//  tracer
//
//  Created by Rasmus Barringer on 2013-02-14.
//  Copyright (c) 2013 Lund University. All rights reserved.
//

#ifndef tracer_implicitbaccelerator_h
#define tracer_implicitbaccelerator_h

#include "bvh.h"
#include "rayaccelerator.h"

// ============================================================================
// =========================== ImplicitBAccelerator ===========================
// ============================================================================
class ImplicitBAccelerator : public RayAccelerator {
private:
    Bvh& _bvh;

public:
    ImplicitBAccelerator( Bvh& bvh );

    virtual auto trace( Ray& ray ) const -> const Triangle*;
};

#endif
