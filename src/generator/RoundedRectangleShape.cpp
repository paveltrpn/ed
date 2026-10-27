// Copyright 2015 Markus Ilmola
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include "generator/RoundedRectangleShape.hpp"

using namespace generator;

RoundedRectangleShape::RoundedRectangleShape( double radius, const tire::algebra::vector<double, 2>& size, int slices,
                                              const tire::algebra::vector<int, 2>& segments )
    : mergeShape_{ { { size[0] + radius, -size[1] }, { size[0] + radius, size[1] }, segments[1] },
                   { { radius, slices, 0.0, tire::algebra::degToRad( 90.0 ) }, size },
                   { { size[0], size[1] + radius }, { -size[0], size[1] + radius }, segments[0] },
                   { { radius, slices, tire::algebra::degToRad( 90.0 ), tire::algebra::degToRad( 90.0 ) },
                     { -size[0], size[1] } },
                   { { -size[0] - radius, size[1] }, { -size[0] - radius, -size[1] }, segments[1] },
                   { { radius, slices, tire::algebra::degToRad( 180.0 ), tire::algebra::degToRad( 90.0 ) }, -size },
                   { { -size[0], -size[1] - radius }, { size[0], -size[1] - radius }, segments[0] },
                   { { radius, slices, tire::algebra::degToRad( 270.0 ), tire::algebra::degToRad( 90.0 ) },
                     { size[0], -size[1] } } } {
}
