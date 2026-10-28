//
//  tracer.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "tracer.h"

#include <numbers>

Tracer::Tracer( Camera& camera, RayAccelerator& accelerator )
    : rnd( 0 )
    , camera( camera )
    , accelerator( accelerator ) {
}

Tracer::HitPoint Tracer::samplePrimary( float px, float py ) {
    const Triangle* tri;
    Ray r = camera.generateRay( px, py );

    if ( ( tri = accelerator.trace( r ) ) != 0 ) {
        return { true, dsbtt::add( r.origin, dsbtt::scale( r.dir, r.maxT ) ), tri->n, 0.0f };
    }
    return {};
}

float Tracer::sampleAO( const Tracer::HitPoint& h ) {
    const float radius = 32.0f;

    tire::algebra::vector<float, 3> up( 1.0f, 0.0f, 0.0f );
    if ( std::fabs( h.n.data()[0] ) > 0.1f ) {
        up = tire::algebra::vector<float, 3>( 0.0f, 1.0f, 0.0f );
    }

    tire::algebra::vector<float, 3> u = dsbtt::normalize( dsbtt::cross( up, h.n ) );
    tire::algebra::vector<float, 3> v = dsbtt::cross( h.n, u );

    float f = 0.0f;

    for ( int sy = 0; sy < aoSamplesRoot; ++sy ) {
        for ( int sx = 0; sx < aoSamplesRoot; ++sx ) {
            float r1 = 2.0f * std::numbers::pi_v<float> * ( sx + rnd.nextFloat() ) * ( 1.0f / aoSamplesRoot );
            float r2 = ( sy + rnd.nextFloat() ) * ( 1.0f / aoSamplesRoot );
            float r2s = std::sqrt( r2 );

            tire::algebra::vector<float, 3> d = dsbtt::add(
                dsbtt::add( dsbtt::scale( u, std::cos( r1 ) * r2s ), dsbtt::scale( v, std::sin( r1 ) * r2s ) ),
                dsbtt::scale( h.n, std::sqrt( 1.0f - r2 ) ) );
            d = dsbtt::normalize( d );
            const float* data = d.data();

            Ray r = { h.p, d, tire::algebra::vector<float, 3>( 1.0f / data[0], 1.0f / data[1], 1.0f / data[2] ),
                      1e-3f, radius };

            if ( accelerator.trace( r ) ) f += 1.0f;
        }
    }

    return 1.0f - f * ( 1.0f / ( aoSamplesRoot * aoSamplesRoot ) );
}
