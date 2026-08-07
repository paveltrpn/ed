
#include <array>

#include "log/log.h"
#include "image/image.h"
#include "algebra/vector2.h"
#include "algebra/vector3.h"
#include "algebra/vector4.h"

#include "billboard.h"

namespace tire {

using namespace algebra;

auto Billboard::setColor( const Colorf &value ) -> void {
    //
    color_ = value;
}

auto Billboard::setPos( float px, float py ) -> void {
    //
    posX_ = px;
    posY_ = py;
}

auto Billboard::setSize( float width, float height ) -> void {
    //
    width_ = width;
    height_ = height;
}

auto Billboard::setZ( float z ) -> void {
    //
    z_ = z;
}

auto Billboard::draw() -> void {
    const vector3_type topLeftVt = { posX_, posY_, z_ };
    const vector3_type topRightVt = { posX_ + width_, posY_, z_ };
    const vector3_type bottomRightVt = { posX_ + width_, posY_ - height_, z_ };
    const vector3_type bottomLeftVt = { posX_, posY_ - height_, z_ };

    quadVerticies_[0] = topLeftVt;
    quadVerticies_[1] = topRightVt;
    quadVerticies_[2] = bottomRightVt;
    quadVerticies_[3] = topLeftVt;
    quadVerticies_[4] = bottomRightVt;
    quadVerticies_[5] = bottomLeftVt;

    const vector2_type topLeftTc{ 0.0f, 0.0f };
    const vector2_type topRightTc{ 1.0f, 0.0f };
    const vector2_type bottomRightTc{ 1.0f, 1.0f };
    const vector2_type bottomLeftTc{ 0.0f, 1.0f };

    quadTexcrds_[0] = topLeftTc;
    quadTexcrds_[1] = topRightTc;
    quadTexcrds_[2] = bottomRightTc;
    quadTexcrds_[3] = bottomRightTc;
    quadTexcrds_[4] = bottomLeftTc;
    quadTexcrds_[5] = topLeftTc;

    const vector4_type color = color_.asVector4f();
    quadsColors_[0] = color;
    quadsColors_[1] = color;
    quadsColors_[2] = color;
    quadsColors_[3] = color;
    quadsColors_[4] = color;
    quadsColors_[5] = color;
}

auto Billboard::lettersCount() const -> size_t {
    //
    return JUST_SINGLE_QUAD;
}

auto Billboard::bufferVerticesSize() const -> size_t {
    //
    return JUST_SINGLE_QUAD * VERTICIES_PER_QUAD * sizeof( vector3_type );
}

auto Billboard::bufferTexcrdsSize() const -> size_t {
    //
    return JUST_SINGLE_QUAD * VERTICIES_PER_QUAD * sizeof( vector2_type );
}

auto Billboard::bufferVertclrsSize() const -> size_t {
    //
    return JUST_SINGLE_QUAD * VERTICIES_PER_QUAD * sizeof( vector4_type );
}

auto Billboard::verteciesData() const -> const vector3<value_type> * {
    //
    return quadVerticies_.data();
}

auto Billboard::texcrdsData() const -> const vector2<value_type> * {
    return quadTexcrds_.data();
}

auto Billboard::clrsData() const -> const vector4<value_type> * {
    return quadsColors_.data();
}

}  // namespace tire