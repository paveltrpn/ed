#pragma once

#include <string>
#include <variant>
#include <forward_list>

#include "log/log.h"

#include "label.h"
#include "billboard.h"

namespace tire {

using UiComponent = std::variant<tire::Label, tire::Billboard>;

struct Ui {
    Ui();

    Ui( const Ui &other ) = delete;
    Ui( Ui &&other ) = delete;

    auto operator=( const Ui &other ) -> Ui & = delete;
    auto operator=( Ui &&other ) -> Ui & = delete;

    virtual ~Ui() = default;

    auto label( float px, float py, const std::string &msg ) -> void ;

    auto billboard( float px, float py, float sx, float sy, float z ) -> void ;

    virtual auto flush() -> void = 0;

protected:
    std::forward_list<UiComponent> componentsList_{};
};

}  // namespace tire