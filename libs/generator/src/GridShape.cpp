#include "generator/GridShape.hpp"

using namespace generator;

GridShape::GridShape(
	const tire::algebra::vector<double, 2>& size,
	const tire::algebra::vector<int, 2>& segments, const tire::algebra::vector<int, 2>& subSegments
) noexcept :
	mImpl{
		// Horizontal lines
		RepeatShape<LineShape>{
			LineShape{
				tire::algebra::vector<double, 2>{-size[0], -size[1]}, tire::algebra::vector<double, 2>{size[0], -size[1]},
				segments[0] * subSegments[0]
			},
			segments[1] < 1 ? 0 : segments[1] + 1,
			tire::algebra::vector<double, 2>{0.0, 2.0 * size[1] / std::max(segments[1], 1)}
		},
		// Vertical lines
		RepeatShape<LineShape>{
			LineShape{
				tire::algebra::vector<double, 2>{-size[0], -size[1]}, tire::algebra::vector<double, 2>{-size[0], size[1]},
				segments[1] * subSegments[1]
			},
			segments[0] < 1 ? 0 : segments[0] + 1,
			tire::algebra::vector<double, 2>{2.0 * size[0] / std::max(segments[0], 1), 0.0}
		},
	}
{
	//
}


GridShape::Edges GridShape::edges() const noexcept
{
	return mImpl.edges();
}


GridShape::Vertices GridShape::vertices() const noexcept
{
	return mImpl.vertices();
}

