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
    : _rnd{ 0 }
    , _camera{ camera }
    , _accelerator{ accelerator } {
}

auto Tracer::samplePrimary( float px, float py ) -> Tracer::HitPoint {
    const Triangle* tri{};
    auto r{ _camera.generateRay( px, py ) };

    if ( ( tri = _accelerator.trace( r ) ) != 0 ) {
        return { true, dsbtt::add( r._origin, dsbtt::scale( r._dir, r._maxT ) ), tri->_n, 0.0f };
    }

    return {};
}

auto Tracer::sampleAO( const Tracer::HitPoint& h ) -> float {
    const auto radius{ 32.0f };

    tire::algebra::vector<float, 3> up{ 1.0f, 0.0f, 0.0f };
    if ( std::fabs( h._n.data()[0] ) > 0.1f ) {
        up = tire::algebra::vector<float, 3>{ 0.0f, 1.0f, 0.0f };
    }

    const auto u{ dsbtt::normalize( dsbtt::cross( up, h._n ) ) };
    const auto v{ dsbtt::cross( h._n, u ) };

    auto f{ 0.0f };

    for ( size_t sy{ 0 }; sy < static_cast<size_t>( _aoSamplesRoot ); ++sy ) {
        for ( size_t sx{ 0 }; sx < static_cast<size_t>( _aoSamplesRoot ); ++sx ) {
            const auto r1{ 2.0f * std::numbers::pi_v<float> * ( sx + _rnd.nextFloat() ) * ( 1.0f / _aoSamplesRoot ) };
            const auto r2{ ( sy + _rnd.nextFloat() ) * ( 1.0f / _aoSamplesRoot ) };
            const auto r2s{ std::sqrt( r2 ) };

            auto d{ dsbtt::add(
                dsbtt::add( dsbtt::scale( u, std::cos( r1 ) * r2s ), dsbtt::scale( v, std::sin( r1 ) * r2s ) ),
                dsbtt::scale( h._n, std::sqrt( 1.0f - r2 ) ) ) };
            d = dsbtt::normalize( d );

            const auto* data{ d.data() };

            Ray r{ h._p, d, tire::algebra::vector<float, 3>{ 1.0f / data[0], 1.0f / data[1], 1.0f / data[2] }, 1e-3f,
                   radius };

            if ( _accelerator.trace( r ) ) {
                f += 1.0f;
            }
        }
    }

    return 1.0f - f * ( 1.0f / ( _aoSamplesRoot * _aoSamplesRoot ) );
}
