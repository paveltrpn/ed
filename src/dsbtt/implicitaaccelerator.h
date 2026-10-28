//
//  implicitaaccelerator.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-07.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_implicitaaccelerator_h
#define tracer_implicitaaccelerator_h

#include "bvh.h"
#include "rayaccelerator.h"

// ============================================================================
// =========================== ImplicitAAccelerator ===========================
// ============================================================================

class ImplicitAAccelerator : public RayAccelerator {
private:
    Bvh& _bvh;

public:
    ImplicitAAccelerator( Bvh& bvh );

    virtual auto trace( Ray& ray ) const -> const Triangle*;
};

#endif
