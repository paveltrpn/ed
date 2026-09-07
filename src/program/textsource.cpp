
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "textsource.h"
#include "config/config.h"
#include "log/log.h"

namespace tire {

TextProgramSource::TextProgramSource( std::string programName )
    : ProgramSource{ programName } {
    const auto basePath = Config::instance().basePath();
    const auto glslPath = basePath / "shaders";

    const auto glslShadersList = listDirectory( glslPath, programName, ".glsl" );

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

        const auto file = std::ifstream{ item };

        if ( !file.is_open() ) {
            const auto msg = std::format( "Failed to open file: {}", item );
            throw std::runtime_error( msg );
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        _sources.emplace_back( stage, buffer.str() );
    }
};

auto TextProgramSource::sources() const -> const std::vector<std::pair<ShaderStageType, std::string>>& {
    //
    return _sources;
}

auto TextProgramSource::stageSource( ShaderStageType stage ) -> std::string {
    auto it = std::find_if( _sources.begin(), _sources.end(), [stage]( const auto item ) {
        //
        const auto [s, _] = item;
        return stage == s;
    } );

    if ( it != _sources.end() ) {
        auto [_, t] = ( *it );
        return t;
    }

    return {};
}

}  // namespace tire