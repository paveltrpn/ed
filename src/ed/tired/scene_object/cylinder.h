
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Cylinder final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( tire::CylinderObjectData data READ getData NOTIFY dataChanged FINAL )

public:
    Cylinder( const CylinderObjectData& data );

    auto data() const -> CylinderObjectData const& override;
    auto data() -> CylinderObjectData& override;

    tire::CylinderObjectData getData();

private:
    CylinderObjectData _data{};
};

}  // namespace tire::object