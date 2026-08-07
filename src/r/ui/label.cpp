#include <array>
#include <memory>

#include "log/log.h"
#include "image/image.h"
#include "algebra/vector2.h"
#include "algebra/vector3.h"
#include "algebra/vector4.h"

#include "label.h"

namespace tire {

using namespace algebra;


    auto Label::setGlyphWidth( float value ) -> void {
        //
    }

    auto Label::setGlyphHeight( float value ) -> void {
        //
    }

    auto Label::setGlyphScale( float value ) -> void {
        //
    }

    auto Label::setGlyphGap( float value ) -> void {
        //
        glyphGap_ = value;
    }

    auto Label::setTextPosition( float x, float y ) -> void {
        //
    }

    auto Label::setColor( const Colorf &value ) -> void {
        //
        color_ = value;
    }

    auto Label::resetStringParameters() -> void {
        //
    }

    auto Label::setPos( float x, float y ) -> void {
        posX_ = x;
        posY_ = y;
    }


    auto Label::draw( const std::string &string ) -> void {
        // Размер ячейки с символом в долях текстурных координат по горизонтали
        float tcGapX = 1.0f / static_cast<float>( fontColumnCount_ );
        // Размер ячейки с символом в долях текстурных координат по вертикали
        float tcGapY = 1.0f / static_cast<float>( fontRowCount_ );

        for ( size_t i{ 0 }; i < string.length(); ++i ) {
            // Смещение квада с i-ым символом, зависит от ширины квадов и зазора между ними
            const auto offset = ( glyphQuadWdt_ + glyphGap_ ) * static_cast<float>( i );

            // Столбец, в котором находится символ
            const auto glyphX = static_cast<float>( string[i] % fontColumnCount_ );

            // Строка, в котором находится символ
            const auto glyphY = static_cast<float>( ( string[i] / fontColumnCount_ ) - 1 );

            // Build character quad vertecies data.
            const vector3_type topLeftVt = { ( offset + 0.0f ) + posX_, 0.0f + posY_, 0.0f };
            const vector3_type topRightVt = { ( offset + glyphQuadWdt_ ) + posX_, 0.0f + posY_, 0.0f };
            const vector3_type bottomRightVt = { ( offset + glyphQuadWdt_ ) + posX_, -glyphQuadHgt_ + posY_, 0.0f };
            const vector3_type bottomLeftVt = { ( offset + 0.0f ) + posX_, -glyphQuadHgt_ + posY_, 0.0f };

            // Build character quad texture coordinates data.
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 0] = topLeftVt;
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 1] = topRightVt;
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 2] = bottomRightVt;
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 3] = bottomRightVt;
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 4] = bottomLeftVt;
            letterQuadsVertecies_[( i * VERTICIES_PER_QUAD ) + 5] = topLeftVt;

            const vector2_type topLeftTc{ ( tcGapX * glyphX ) + 0.0f, ( tcGapY * glyphY ) + 0.0f };
            const vector2_type topRightTc{ ( tcGapX * glyphX ) + tcGapX, ( tcGapY * glyphY ) + 0.0f };
            const vector2_type bottomRightTc{ ( tcGapX * glyphX ) + tcGapX, ( tcGapY * glyphY ) + tcGapY };
            const vector2_type bottomLeftTc{ ( tcGapX * glyphX ) + 0.0f, ( tcGapY * glyphY ) + tcGapY };

            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 0] = topLeftTc;
            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 1] = topRightTc;
            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 2] = bottomRightTc;
            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 3] = bottomRightTc;
            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 4] = bottomLeftTc;
            letterQuadsTexcrds_[( i * VERTICIES_PER_QUAD ) + 5] = topLeftTc;

            // Build character quad color data.
            const vector4_type color = color_.asVector4f();
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 0] = color;
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 1] = color;
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 2] = color;
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 3] = color;
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 4] = color;
            letterQuadsColors_[( i * VERTICIES_PER_QUAD ) + 5] = color;
            lettersCount_++;
        }
    }

   auto Label::lettersCount() const -> size_t {
        //
        return lettersCount_;
    }

    auto Label::bufferVerticesSize() const -> size_t {
        //
        return lettersCount_ * VERTICIES_PER_QUAD * sizeof( vector3_type );
    }

    auto Label::bufferTexcrdsSize() const -> size_t {
        //
        return lettersCount_ * VERTICIES_PER_QUAD * sizeof( vector2_type );
    }

    auto Label::bufferVertclrsSize() const -> size_t {
        //
        return lettersCount_ * VERTICIES_PER_QUAD * sizeof( vector4_type );
    }

     auto Label::verteciesData() const -> const vector3<value_type> * { return letterQuadsVertecies_.data(); }

     auto Label::texcrdsData() const -> const vector2<value_type> * { return letterQuadsTexcrds_.data(); }

 auto Label::clrsData() const -> const vector4<value_type> * { return letterQuadsColors_.data(); }

}  // namespace tire