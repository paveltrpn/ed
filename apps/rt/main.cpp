//
//  main.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include <iostream>
#include <cassert>
#include <charconv>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <cstring>
#include <string_view>
#include <utility>

#include <uv.h>

#include "tcpbridge/tcpbridge.h"

#include "dsbtt/bvh.h"
#include "dsbtt/tracer.h"
#include "dsbtt/stackaccelerator.h"
#include "dsbtt/laineaccelerator.h"
#include "dsbtt/sparseaccelerator.h"
#include "dsbtt/hapalaaccelerator.h"
#include "dsbtt/implicitaaccelerator.h"
#include "dsbtt/implicitbaccelerator.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

static double seconds() {
#ifdef _WIN32
    return GetTickCount64() * ( 1.0 / 1000.0 );
#else
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * ( 1.0 / 1000000.0 );
#endif
}

static RayAccelerator* createAccelerator( Bvh& bvh, const char* name ) {
    std::cout << "Accelerator: " << name << "." << std::endl;

    if ( strcmp( name, "stack" ) == 0 )
        return new StackAccelerator( bvh );
    else if ( strcmp( name, "sparse" ) == 0 )
        return new SparseAccelerator( bvh );
    else if ( strcmp( name, "implicita" ) == 0 )
        return new ImplicitAAccelerator( bvh );
    else if ( strcmp( name, "implicitb" ) == 0 )
        return new ImplicitBAccelerator( bvh );
    else if ( strcmp( name, "laine0" ) == 0 )
        return new LaineAccelerator<0>( bvh );
    else if ( strcmp( name, "laine1" ) == 0 )
        return new LaineAccelerator<1>( bvh );
    else if ( strcmp( name, "laine2" ) == 0 )
        return new LaineAccelerator<2>( bvh );
    else if ( strcmp( name, "laine4" ) == 0 )
        return new LaineAccelerator<4>( bvh );
    else if ( strcmp( name, "laine8" ) == 0 )
        return new LaineAccelerator<8>( bvh );
    else if ( strcmp( name, "hapala" ) == 0 )
        return new HapalaAccelerator( bvh );

    std::cout << "ERROR: Invalid accelerator." << std::endl;
    std::cout << "Pick one of: stack, implicita, implicitb, sparse, hapala, laine0,1,2,4,8." << std::endl;

    exit( 1 );

    return 0;
}

struct ServeOptions {
    bool _serve{ false };
    bool _valid{ true };
    uint16_t _port{ 45454 };
};

static auto parseServeOptions( int argc, const char* argv[] ) -> ServeOptions {
    auto options = ServeOptions{};
    for ( int index = 1; index < argc; ++index ) {
        if ( std::string_view{ argv[index] } == "--serve" ) {
            options._serve = true;
            break;
        }
    }
    if ( !options._serve ) {
        return options;
    }

    for ( int index = 1; index < argc; ++index ) {
        const auto argument = std::string_view{ argv[index] };
        if ( argument == "--serve" ) {
            continue;
        }
        if ( argument != "--port" || index + 1 >= argc ) {
            options._valid = false;
            return options;
        }

        const auto value = std::string_view{ argv[++index] };
        auto port = 0;
        const auto conversion = std::from_chars( value.data(), value.data() + value.size(), port );
        if ( conversion.ec != std::errc{} || conversion.ptr != value.data() + value.size() || port < 1 ||
             port > 65535 ) {
            options._valid = false;
            return options;
        }
        options._port = static_cast<uint16_t>( port );
    }
    return options;
}

static auto runServe( uint16_t port ) -> int {
    auto bridge = tire::TcpBridge{};
    auto mutex = std::mutex{};
    auto condition = std::condition_variable{};
    auto finished = false;
    auto exitCode = 1;

    auto callbacks = tire::TcpBridgeCallbacks{};

    callbacks._message = [&bridge]( std::vector<std::byte> message ) -> void {
        //
        bridge.send( std::move( message ) );
    };

    callbacks._disconnected = [&mutex, &condition, &finished, &exitCode]() -> void {
        {
            //
            std::scoped_lock lock{ mutex };
            if ( !finished ) {
                exitCode = 0;
            }
            finished = true;
        }
        condition.notify_one();
    };

    callbacks._error = [&mutex, &condition, &finished, &exitCode]( tire::TcpBridgeError error ) -> void {
        //
        std::cerr << error._context;
        if ( error._status != 0 ) {
            std::cerr << ": " << uv_strerror( error._status );
        }
        std::cerr << std::endl;
        {
            std::scoped_lock lock{ mutex };
            finished = true;
            exitCode = 1;
        }
        condition.notify_one();
    };

    bridge.setCallbacks( std::move( callbacks ) );
    bridge.listen( "127.0.0.1", port );

    std::unique_lock lock{ mutex };
    condition.wait( lock, [&finished]() -> bool { return finished; } );
    lock.unlock();
    bridge.close();

    return exitCode;
}

auto main( int argc, const char* argv[] ) -> int {
    const auto serveOptions = parseServeOptions( argc, argv );
    if ( serveOptions._serve ) {
        if ( !serveOptions._valid ) {
            std::cerr << "Usage: rt --serve [--port N], where N is between 1 and 65535." << std::endl;
            return 2;
        }
        return runServe( serveOptions._port );
    }

    assert( sizeof( Triangle ) == 64 );
    assert( sizeof( Bvh::Node ) == 64 );
    assert( sizeof( Tracer::HitPoint ) == 32 );

    std::cout << "Loading obj..." << std::flush;
    Mesh mesh( "battlefield.obj", false );
    std::cout << "done." << std::endl;

    std::cout << "Building bvh..." << std::flush;
    Bvh bvh( mesh );
    std::cout << "done." << std::endl;

    const int width = 800;
    const int height = 300;
    const int samplesRoot = 4;

    Camera camera;
    RayAccelerator* accelerator = createAccelerator( bvh, argc > 1 ? argv[1] : "" );
    Tracer tracer( camera, *accelerator );

    camera.lookAt( tire::algebra::vector<float, 3>( 300.0f, 100.0f, 300.0f ),
                   tire::algebra::vector<float, 3>( 0.0f, 10.0f, 0.0f ),
                   tire::algebra::vector<float, 3>( 0.0f, 1.0f, 0.0f ), 50.0f, 1.0f, 1000.0f, width, height );

    std::vector<Tracer::HitPoint, AlignedAllocator<Tracer::HitPoint, 32>> hitPoints( width * height * samplesRoot *
                                                                                     samplesRoot );
    std::vector<float> image( width * height );

    Random rnd( 31251 );

    std::cout << "Primary rays: " << std::flush;

    double primStart = seconds();
    int currentHitPoint = 0;

    for ( int i = 0; i < height; ++i ) {
        for ( int j = 0; j < width; ++j ) {
            for ( int sy = 0; sy < samplesRoot; ++sy ) {
                for ( int sx = 0; sx < samplesRoot; ++sx ) {
                    float px = j + ( sx + rnd.nextFloat() ) * ( 1.0f / samplesRoot );
                    float py = i + ( sy + rnd.nextFloat() ) * ( 1.0f / samplesRoot );

                    hitPoints[currentHitPoint++] = tracer.samplePrimary( px, py );
                }
            }
        }
    }

    double primEnd = seconds();
    std::cout << ( ( width * height * samplesRoot * samplesRoot ) / ( primEnd - primStart ) ) << " rays/sec."
              << std::endl;

    std::cout << "AO rays: " << std::flush;
    double aoStart = seconds();

    int aoRays = 0;
    currentHitPoint = 0;

    for ( int i = 0; i < height; ++i ) {
        for ( int j = 0; j < width; ++j ) {
            float c = 0.0f;

            for ( int sy = 0; sy < samplesRoot; ++sy ) {
                for ( int sx = 0; sx < samplesRoot; ++sx ) {
                    Tracer::HitPoint h = hitPoints[currentHitPoint++];

                    if ( !h._hit ) {
                        c += 1.0f;
                        continue;
                    }

                    aoRays += Tracer::_aoSamplesRoot * Tracer::_aoSamplesRoot;
                    c += tracer.sampleAO( h );
                }
            }

            image[i * width + j] = c * ( 1.0f / ( samplesRoot * samplesRoot ) );
        }
    }

    double aoEnd = seconds();
    std::cout << ( aoRays / ( aoEnd - aoStart ) ) << " rays/sec." << std::endl;

    std::cout << "Total time: " << ( ( primEnd - primStart ) + ( aoEnd - aoStart ) ) << " sec." << std::endl;

    std::cout << "Saving image.ppm..." << std::flush;

    FILE* f = fopen( "image.ppm", "w" );
    fprintf( f, "P3\n%d %d\n%d\n", width, height, 255 );
    for ( int i = 0; i < width * height; ++i ) {
        int c = (int)( image[i] * 255.0f + 0.5f );
        fprintf( f, "%d %d %d ", c, c, c );
    }
    fclose( f );

    std::cout << "done." << std::endl;
    delete accelerator;
    return 0;
}
