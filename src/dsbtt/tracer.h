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

class Tracer {
public:
    static const int aoSamplesRoot = 4;

    struct HitPoint {
        bool hit;
        tire::algebra::vector<float, 3> p;
        tire::algebra::vector<float, 3> n;
        float padding;
    };

private:
    Random rnd;
    Camera& camera;
    RayAccelerator& accelerator;

public:
    Tracer( Camera& camera, RayAccelerator& accelerator );

    HitPoint samplePrimary( float px, float py );

    float sampleAO( const HitPoint& h );
};

#endif
