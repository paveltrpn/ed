//
//  tracer.h
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#ifndef tracer_tracer_h
#define tracer_tracer_h

#include "camera.h"
#include "random.h"
#include "rayaccelerator.h"

// ============================================================================
// ================================== Tracer ==================================
// ============================================================================
class Tracer {
public:
    static const int _aoSamplesRoot{ 4 };

// ============================================================================
// ================================= HitPoint =================================
// ============================================================================
    struct HitPoint {
        bool _hit;
        tire::algebra::vector<float, 3> _p;
        tire::algebra::vector<float, 3> _n;
        float _padding;
    };

private:
    Random _rnd;
    Camera& _camera;
    RayAccelerator& _accelerator;

public:
    Tracer( Camera& camera, RayAccelerator& accelerator );

    auto samplePrimary( float px, float py ) -> HitPoint;

    auto sampleAO( const HitPoint& h ) -> float;
};

#endif
