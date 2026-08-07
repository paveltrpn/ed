
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Sphere final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( tire::SphereObjectData data READ getData NOTIFY dataChanged FINAL )

public:
    Sphere( const SphereObjectData& data );

    auto data() const -> SphereObjectData const& override;
    auto data() -> SphereObjectData& override;

    tire::SphereObjectData getData();

private:
    SphereObjectData _data{};
};

}  // namespace tire::object