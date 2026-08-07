#pragma once

#include <string>
#include <memory>

#include "mesh.h"

namespace tire {

namespace {

[[nodiscard]] auto readWavefrontObjFile( const std::string &filePath ) -> std::shared_ptr<OBJMesh>;

}

}  // namespace tire
