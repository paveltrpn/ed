#pragma once

#include <array>
#include <memory>

#include "log/log.h"
#include "image/image.h"
#include "algebra/vector2.h"
#include "algebra/vector3.h"
#include "algebra/vector4.h"

namespace tire {

using namespace algebra;

struct Label final {
    using value_type = float;
    using vector2_type = vector2<value_type>;
    using vector3_type = vector3<value_type>;
    using vector4_type = vector4<value_type>;

    auto setGlyphWidth( float value ) -> void;

    auto setGlyphHeight( float value ) -> void;

    auto setGlyphScale( float value ) -> void;

    auto setGlyphGap( float value ) -> void;

    auto setTextPosition( float x, float y ) -> void;

    auto setColor( const Colorf &value ) -> void;

    auto resetStringParameters() -> void;

    auto setPos( float x, float y ) -> void;

    auto draw( const std::string &string ) -> void;

    [[nodiscard]] auto lettersCount() const -> size_t;

    [[nodiscard]] auto bufferVerticesSize() const -> size_t;

    [[nodiscard]] auto bufferTexcrdsSize() const -> size_t;

    [[nodiscard]] auto bufferVertclrsSize() const -> size_t;

    [[nodiscard]] auto verteciesData() const -> const vector3<value_type> *;

    [[nodiscard]] auto texcrdsData() const -> const vector2<value_type> *;

    [[nodiscard]] auto clrsData() const -> const vector4<value_type> *;

private:
// Size of "Letters buffer" - this is the number of all characters
// from all draw() calls that can one instance of this class operates
#define VERTICIES_PER_QUAD 6
#define MAX_LETTERS_COUNT 64
    std::array<vector3_type, MAX_LETTERS_COUNT * VERTICIES_PER_QUAD> letterQuadsVertecies_{};
    std::array<vector2_type, MAX_LETTERS_COUNT * VERTICIES_PER_QUAD> letterQuadsTexcrds_{};
    std::array<vector4_type, MAX_LETTERS_COUNT * VERTICIES_PER_QUAD> letterQuadsColors_{};

    float glyphScale_{ 2.0f };
#define GLYPH_WIDTH 0.5f
#define GLYPH_HEIGHT 1.6f
    float glyphQuadWdt_{ GLYPH_WIDTH * glyphScale_ };
    float glyphQuadHgt_{ GLYPH_HEIGHT * glyphScale_ };
#define GLYPH_GAP 0.0f
    float glyphGap_{ GLYPH_GAP };

    float posX_{ 0.0f };
    float posY_{ 0.0f };

    // Формат шрифта - изображение TGA с началом сверху слева,
    // 32 столбца на 8 строк символов, первый символ - 32 ("пробел").
    // Размер ячейки с символом получается делением горизонтального и вертикального
    // размера изображения на количество столбцов и строк соответственно.
    int32_t fontColumnCount_{ 32 };  // Количество столбцов символов в шрифте
    int32_t fontRowCount_{ 8 };      // Количество строк символов в шрифте

    size_t lettersCount_{};

    Colorf color_{};
};

}  // namespace tire