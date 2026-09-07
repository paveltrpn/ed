#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "definitions.h"
#include "programsource.h"

namespace tire {

struct BytecodeProgramSource final : ProgramSource {
public:
    BytecodeProgramSource( std::string programName );

    [[nodiscard]]
    auto sources() const -> const std::vector<std::pair<ShaderStageType, std::vector<uint32_t>>>&;

private:
    std::vector<std::pair<ShaderStageType, std::vector<uint32_t>>> _sources{};
};

}  // namespace tire