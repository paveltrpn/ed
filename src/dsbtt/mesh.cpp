//
//  mesh.cpp
//  tracer
//
//  Created by Rasmus Barringer on 2012-10-06.
//  Copyright (c) 2012 Lund University. All rights reserved.
//

#include "mesh.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

Mesh::Mesh( const char* filename, bool flipFaces ) {
    ifstream file{ filename };

    if ( !file ) {
        throw runtime_error{ ( string{ "cannot find " } + filename ).c_str() };
    }

    string line{};

    vector<tire::algebra::vector<int, 3>> fileTriangles{};
    vector<tire::algebra::vector<float, 3>> filePositions{};

    while ( getline( file, line ) ) {
        float x{}, y{}, z{};
        int a[3]{}, b[3]{}, c[3]{}, d[3]{};

        if ( sscanf( line.c_str(), "v %f %f %f", &x, &y, &z ) == 3 ) {
            filePositions.push_back( tire::algebra::vector<float, 3>{ x, y, z } );
        } else if ( sscanf( line.c_str(), "f %d %d %d", &a[0], &b[0], &c[0] ) == 3 ) {
            fileTriangles.push_back( tire::algebra::vector<int, 3>{ c[0] - 1, b[0] - 1, a[0] - 1 } );
        } else if ( sscanf( line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a[0], &a[1], &a[2], &b[0], &b[1],
                            &b[2], &c[0], &c[1], &c[2], &d[0], &d[1], &d[2] ) == 12 ||
                    sscanf( line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &a[0], &a[2], &b[0], &b[2], &c[0], &c[2],
                            &d[0], &d[2] ) == 8 ) {
            fileTriangles.push_back( tire::algebra::vector<int, 3>{ a[0] - 1, b[0] - 1, c[0] - 1 } );
            fileTriangles.push_back( tire::algebra::vector<int, 3>{ a[0] - 1, c[0] - 1, d[0] - 1 } );
        } else if ( sscanf( line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &a[0], &a[1], &a[2], &b[0], &b[1], &b[2],
                            &c[0], &c[1], &c[2] ) == 9 ||
                    sscanf( line.c_str(), "f %d//%d %d//%d %d//%d", &a[0], &a[2], &b[0], &b[2], &c[0], &c[2] ) == 6 ) {
            fileTriangles.push_back( tire::algebra::vector<int, 3>{ a[0] - 1, b[0] - 1, c[0] - 1 } );
        }
    }

    for ( size_t i{ 0 }; i < fileTriangles.size(); ++i ) {
        const auto p{ fileTriangles[i] };

        auto p0{ filePositions[p[0]] };
        auto p1{ filePositions[p[1]] };
        auto p2{ filePositions[p[2]] };

        if ( flipFaces ) {
            std::swap( p0, p1 );
        }

        const auto a{ p1 - p0 };
        const auto b{ p2 - p0 };

        const auto n{ tire::algebra::normalize( tire::algebra::cross( a, b ) ) };

        Triangle t{ p0, p1, p2, n };

        _triangles.push_back( t );
    }
}

auto Mesh::triangles() -> std::vector<Triangle, AlignedAllocator<Triangle, 64>>& {
    return _triangles;
}
