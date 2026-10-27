// Copyright 2015 Markus Ilmola
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#ifndef GENERATOR_MESHVERTEX_HPP
#define GENERATOR_MESHVERTEX_HPP

#include "math.hpp"

namespace generator {

class MeshVertex {
public:
    tire::algebra::vector<double, 3> position;

    /// Unit vector perpendicular to the surface.
    tire::algebra::vector<double, 3> normal;

    /// UV texture coordinates
    tire::algebra::vector<double, 2> texCoord;

    MeshVertex() noexcept
        : position{}
        , normal{}
        , texCoord{} {}
};

}  // namespace generator

#endif
