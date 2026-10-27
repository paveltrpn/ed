// Copyright 2015 Markus Ilmola
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.


#include "generator/KnotPath.hpp"


using namespace generator;


namespace {

tire::algebra::vector<double, 3> knot(int p, int q, double t) {
	t *= tire::algebra::degToRad(360.0);

	const double pt = p * t;
	const double qt = q * t;

	const double sinpt = std::sin(pt);
	const double cospt = std::cos(pt);
	const double sinqt = std::sin(qt);
	const double cosqt = std::cos(qt);

	const double r = 0.5 * (2.0 + sinqt);

	return tire::algebra::vector<double, 3>{r * cospt, r * sinpt, r * cosqt};
}

}


KnotPath::KnotPath(
	int p,
	int q,
	int segments
) :
	parametricPath_{
		[p, q] (double t) {
			PathVertex vertex;

			vertex.position = knot(p, q, t);

			const tire::algebra::vector<double, 3> prev = knot(p, q, t - 0.01);
			const tire::algebra::vector<double, 3> next = knot(p, q, t + 0.01);

			vertex.tangent = normalize(next - prev);

			vertex.normal = normalize(cross(next - prev, next + prev));

			vertex.texCoord = t;

			return vertex;
		},
		segments
	}
{ }


