#pragma once

#include <array>

#include "log/log.h"
#include "image/image.h"
#include "algebra/vector2.h"
#include "algebra/vector3.h"
#include "algebra/vector4.h"

namespace tire {

#define VERTICIES_PER_QUAD 6
#define JUST_SINGLE_QUAD 1

using namespace algebra;

struct Billboard final {
    using value_type = float;
    using vector2_type = vector2<value_type>;
    using vector3_type = vector3<value_type>;
    using vector4_type = vector4<value_type>;

    auto setColor( const Colorf &value ) -> void;

    auto setPos( float px, float py ) -> void;

    auto setSize( float width, float height ) -> void;

    auto setZ( float z ) -> void;

    auto draw() -> void;

    [[nodiscard]] auto lettersCount() const -> size_t;

    [[nodiscard]] auto bufferVerticesSize() const -> size_t;

    [[nodiscard]] auto bufferTexcrdsSize() const -> size_t;

    [[nodiscard]] auto bufferVertclrsSize() const -> size_t;

    [[nodiscard]] auto verteciesData() const -> const vector3<value_type> *;

    [[nodiscard]] auto texcrdsData() const -> const vector2<value_type> *;

    [[nodiscard]] auto clrsData() const -> const vector4<value_type> *;

private:
    std::array<vector3_type, JUST_SINGLE_QUAD * VERTICIES_PER_QUAD> quadVerticies_{};
    std::array<vector2_type, JUST_SINGLE_QUAD * VERTICIES_PER_QUAD> quadTexcrds_{};
    std::array<vector4_type, JUST_SINGLE_QUAD * VERTICIES_PER_QUAD> quadsColors_{};

    Colorf color_{};

    float posX_{ 0.0f };
    float posY_{ 0.0f };
    float z_{ 0.0f };

    float width_{ 1.0f };
    float height_{ 1.0f };
};

}  // namespace tire