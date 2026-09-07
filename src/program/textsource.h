#pragma once

#include <print>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>



#include "definitions.h"
#include "programsource.h"

namespace tire {

struct TextProgramSource final : ProgramSource {
public:
    TextProgramSource( std::string programName );

    [[nodiscard]]
    auto sources() const -> const std::vector<std::pair<ShaderStageType, std::string>>& ;

private:
    std::vector<std::pair<ShaderStageType, std::string>> _sources{};
};

}  // namespace tire