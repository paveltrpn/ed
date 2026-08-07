#include <string>
#include <variant>
#include <forward_list>

#include "log/log.h"

#include "ui.h"
#include "billboard.h"
#include "label.h"

namespace tire {

using UiComponent = std::variant<tire::Label, tire::Billboard>;

Ui::Ui(){
    // Preallocate.
    // componentsList_.resize( 32 );
};

auto Ui::label( float px, float py, const std::string &msg ) -> void {
    auto l = tire::Label{};

    l.setColor( { "white" } );
    l.setGlyphGap( 0.1f );
    l.setPos( px, py );
    l.draw( msg );

    componentsList_.emplace_front( std::move( l ) );
}

auto Ui::billboard( float px, float py, float sx, float sy, float z ) -> void {
    //
    auto b = tire::Billboard{};

    b.setPos( px, py );
    b.setSize( sx, sy );
    b.setZ( z );
    b.setColor( { "#b852ac33" } );
    b.draw();

    componentsList_.emplace_front( std::move( b ) );
}

}  // namespace tire