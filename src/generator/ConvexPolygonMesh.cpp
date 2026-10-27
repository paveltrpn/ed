// Copyright 2016 Markus Ilmola
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include "generator/ConvexPolygonMesh.hpp"

#include "generator/CircleShape.hpp"
#include "generator/Iterator.hpp"

using namespace generator;



ConvexPolygonMesh::Triangles::Triangles(const ConvexPolygonMesh& mesh) noexcept :
	mMesh{&mesh},
	mOdd{false},
	mSegmentIndex{0},
	mSideIndex{0},
	mRingIndex{0}
{

}


bool ConvexPolygonMesh::Triangles::done() const noexcept {
	return
		mMesh->mSegments == 0 || mMesh->mVertices.size() < 3 ||
		mRingIndex == mMesh->mRings;
}


Triangle ConvexPolygonMesh::Triangles::generate() const {
	if (done()) throw std::runtime_error("Done!");

	Triangle triangle{};

	const int verticesPerRing = mMesh->mSegments * mMesh->mVertices.size();
	const int delta = mRingIndex * verticesPerRing + 1;

	const int n1 = mSideIndex * mMesh->mSegments + mSegmentIndex;
	const int n2 = (n1 + 1) % verticesPerRing;

	if (mRingIndex == mMesh->mRings - 1) {
		triangle.vertices[0] = 0;
		triangle.vertices[1] = n1 + delta;
		triangle.vertices[2] = n2 + delta;
	}
	else {
		if (!mOdd) {
			triangle.vertices[0] = n1 + delta;
			triangle.vertices[1] = n2 + delta;
			triangle.vertices[2] = n1 + verticesPerRing + delta;
		}
		else {
			triangle.vertices[0] = n2 + delta;
			triangle.vertices[1] = n2 + verticesPerRing + delta;
			triangle.vertices[2] = n1 + verticesPerRing + delta;
		}
	}

	return triangle;
}




void ConvexPolygonMesh::Triangles::next() {
	if (done()) throw std::runtime_error("Done!");

	if (mRingIndex == mMesh->mRings - 1) {
		++mSegmentIndex;

		if (mSegmentIndex == mMesh->mSegments) {
			mSegmentIndex = 0;

			++mSideIndex;

			if (mSideIndex == static_cast<int>(mMesh->mVertices.size())) {
				++mRingIndex;
			}
		}
	}
	else {

		mOdd = !mOdd;

		if (!mOdd) {

			++mSegmentIndex;

			if (mSegmentIndex == mMesh->mSegments) {
				mSegmentIndex = 0;

				++mSideIndex;

				if (mSideIndex == static_cast<int>(mMesh->mVertices.size())) {
					mSideIndex = 0;
					mOdd = false;

					++mRingIndex;
				}
			}

		}
	}
}



ConvexPolygonMesh::Vertices::Vertices(const ConvexPolygonMesh& mesh) noexcept :
	mMesh{&mesh},
	mCenterDone{false},
	mSegmentIndex{0},
	mSideIndex{0},
	mRingIndex{0}
{
}


bool ConvexPolygonMesh::Vertices::done() const noexcept {
	return
		mMesh->mSegments == 0 || mMesh->mRings == 0 ||
		mMesh->mVertices.size() < 3 || mRingIndex == mMesh->mRings;
}




MeshVertex ConvexPolygonMesh::Vertices::generate() const {
	if (done()) throw std::runtime_error("Done!");

	MeshVertex vertex{};

	if (mCenterDone) {

		const double ringDelta = static_cast<double>(mRingIndex) / mMesh->mRings;
		const double segmentDelta = static_cast<double>(mSegmentIndex) / mMesh->mSegments;

		const int nextSide = (mSideIndex + 1) % mMesh->mVertices.size();
		const tire::algebra::vector<double, 3> a = tire::algebra::mix(mMesh->mVertices.at(mSideIndex), mMesh->mCenter, ringDelta);
		const tire::algebra::vector<double, 3> b = tire::algebra::mix(mMesh->mVertices.at(nextSide), mMesh->mCenter, ringDelta);

		vertex.position = tire::algebra::mix(a, b, segmentDelta);
	}
	else {
		vertex.position = mMesh->mCenter;
	}

	vertex.normal = mMesh->mNormal;

	const tire::algebra::vector<double, 3> delta = vertex.position - mMesh->mCenter;

	vertex.texCoord[0] = tire::algebra::dot(mMesh->mTangent, delta);
	vertex.texCoord[1] = tire::algebra::dot(mMesh->mBitangent, delta);

	vertex.texCoord -= mMesh->mTexDelta;

	return vertex;
}




void ConvexPolygonMesh::Vertices::next() {
	if (done()) throw std::runtime_error("Done!");

	if (!mCenterDone) {
		mCenterDone = true;
	}
	else {

		++mSegmentIndex;

		if (mSegmentIndex == mMesh->mSegments) {
			mSegmentIndex = 0;

			++mSideIndex;

			if (mSideIndex == static_cast<int>(mMesh->mVertices.size())) {

				mSideIndex = 0;

				++mRingIndex;
			}
		}
	}
}


namespace {


std::vector<tire::algebra::vector<double, 3>> makeVertices(double radius, int sides) noexcept {
	std::vector<tire::algebra::vector<double, 3>> result{};

	CircleShape circle{radius, sides};
	for (const auto& v : circle.vertices()) {
		result.push_back(tire::algebra::vector<double, 3>{v.position[0], v.position[1], 0.0});
	}
	result.pop_back(); // The last one is a dublicate with the first one

	return result;
}



std::vector<tire::algebra::vector<double, 3>> convertVertices(const std::vector<tire::algebra::vector<double, 2>>& vertices) noexcept {
	std::vector<tire::algebra::vector<double, 3>> result(vertices.size());

	for (std::size_t i = 0; i < vertices.size(); ++i) {
		result.at(i) = tire::algebra::vector<double, 3>{vertices.at(i)[0], vertices.at(i)[1], 0.0};
	}

	return result;
}


tire::algebra::vector<double, 3> calcCenter(const std::vector<tire::algebra::vector<double, 3>>& vertices) noexcept {
	tire::algebra::vector<double, 3> result{};
	for (const auto& v : vertices) {
		result += v;
	}
	return result / static_cast<double>(vertices.size());
}


tire::algebra::vector<double, 3> calcNormal(const tire::algebra::vector<double, 3>& center, const std::vector<tire::algebra::vector<double, 3>>& vertices) {
	tire::algebra::vector<double, 3> normal{};
	for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
		normal += tire::algebra::normal(center, vertices[i], vertices[(i + 1) % vertices.size()]);
	}
	return tire::algebra::normalize(normal);
}


}


ConvexPolygonMesh::ConvexPolygonMesh(
	double radius, int sides, int segments, int rings
) noexcept :
	ConvexPolygonMesh{makeVertices(radius, sides), segments, rings}
{ }


ConvexPolygonMesh::ConvexPolygonMesh(
	const std::vector<tire::algebra::vector<double, 2>>& vertices, int segments, int rings
) noexcept :
	ConvexPolygonMesh{convertVertices(vertices), segments, rings}
{ }


ConvexPolygonMesh::ConvexPolygonMesh(
	std::vector<tire::algebra::vector<double, 3>> vertices, int segments, int rings
) noexcept :
	mVertices{std::move(vertices)},
	mSegments{segments},
	mRings{rings},
	mCenter{calcCenter(mVertices)},
	mNormal{calcNormal(mCenter, mVertices)},
	mTangent{},
	mBitangent{},
	mTexDelta{}
{

	if (mVertices.size() > 0) {
		mTangent = tire::algebra::normalize(mVertices.at(0) - mCenter);
		mBitangent = tire::algebra::cross(mNormal, mTangent);

		tire::algebra::vector<double, 2> texMax{};

		for (const tire::algebra::vector<double, 3>& vertex : mVertices) {
			tire::algebra::vector<double, 3> delta = vertex - mCenter;

			tire::algebra::vector<double, 2> uv{tire::algebra::dot(mTangent, delta), tire::algebra::dot(mBitangent, delta)};

			mTexDelta = tire::algebra::min(mTexDelta, uv);
			texMax = tire::algebra::max(texMax, uv);
		}

		tire::algebra::vector<double, 2> size = texMax - mTexDelta;

		mTangent /= size[0];
		mBitangent /= size[1];

		mTexDelta /= size;

	}
}


ConvexPolygonMesh::Triangles ConvexPolygonMesh::triangles() const noexcept {
	return Triangles{*this};
}


ConvexPolygonMesh::Vertices ConvexPolygonMesh::vertices() const noexcept {
	return Vertices{*this};
}


