
#include <print>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "textsource.h"
#include "config/config.h"
#include "log/log.h"

namespace tire {

TextProgramSource::TextProgramSource( std::string programName )
    : ProgramSource{ programName } {
    const auto basePath = Config::instance().basePath();
    const auto spirvPath = basePath / "shaders";

    const auto glslShadersList = listDirectory( spirvPath, programName, ".glsl" );

    if ( glslShadersList.size() < 2 ) {
        const auto msg = std::format( "Not enough source files for program: {}", programName );
        throw std::runtime_error( msg );
    }

    for ( auto&& item : glslShadersList ) {
        const auto stage = stageType( item );

        if ( stage == ShaderStageType::UNKNOWN ) {
            const auto msg = std::format( "Unknown shader stage for file: {}", item );
            throw std::runtime_error( msg );
        }

        auto file = std::ifstream{ item };

        if ( !file.is_open() ) {
            const auto msg = std::format( "Failed to open file: {}", item );
            throw std::runtime_error( msg );
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        _sources.emplace_back( stage, buffer.str() );
    }
};

[[nodiscard]] auto TextProgramSource::sources() const -> const std::vector<std::pair<ShaderStageType, std::string>>& {
    //
    return _sources;
}

}  // namespace tire