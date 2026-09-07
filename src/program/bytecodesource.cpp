module;

#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <fstream>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "log/log.h"

import config;

export module program : bytecodesource;

import : definitions;
import : programsource;

namespace tire {

export struct BytecodeProgramSource final : ProgramSource {
public:
    BytecodeProgramSource( std::string programName )
        : ProgramSource{ programName } {
        const auto basePath = Config::instance().basePath();
        const auto spirvPath = basePath / "shaders" / "spirv";

        // Obtain shader sources file names for given shader program with name "programName".
        const auto spvShadersList = listDirectory( spirvPath, programName, ".spv" );

        if ( spvShadersList.size() < 2 ) {
            const auto msg = std::format( "Not enough source files for program: {}", programName );
            throw std::runtime_error( msg );
        }

        for ( auto&& item : spvShadersList ) {
            const auto stage = stageType( item );

            if ( stage == ShaderStageType::UNKNOWN ) {
                const auto msg = std::format( "Unknown shader stage for file: {}", item );
                throw std::runtime_error( msg );
            }

            auto file = std::ifstream{ item, std::ios::binary | std::ios::ate };

            if ( !file.is_open() ) {
                const auto msg = std::format( "Failed to open file: {}", item );
                throw std::runtime_error( msg );
            }

            // Get file size.
            const auto size = file.tellg();
            if ( size <= 0 ) {
                const auto msg = std::format( "File is empty or invalid: {}", item );
                throw std::runtime_error( msg );
            }

            // Seek back to beginning.
            file.seekg( 0, std::ios::beg );

            // Allocate vector and read.
            std::vector<uint32_t> bytecode( static_cast<size_t>( size / 4 ) );
            if ( !file.read( reinterpret_cast<char*>( bytecode.data() ), size ) ) {
                throw std::runtime_error( "Failed to read file: " + item );
            }

            _sources.emplace_back( stage, bytecode );
        }
    };

    [[nodiscard]]
    auto sources() const -> const std::vector<std::pair<ShaderStageType, std::vector<uint32_t>>>& {
        //
        return _sources;
    }

private:
    std::vector<std::pair<ShaderStageType, std::vector<uint32_t>>> _sources{};
};

}  // namespace tire