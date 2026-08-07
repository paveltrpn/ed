
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Box final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( tire::BoxObjectData data READ getData NOTIFY dataChanged FINAL )

public:
    Box( const BoxObjectData& data );

    auto data() const -> BoxObjectData const& override;
    auto data() -> BoxObjectData& override;

    tire::BoxObjectData getData();

private:
    BoxObjectData _data{};
};

}  // namespace tire::object