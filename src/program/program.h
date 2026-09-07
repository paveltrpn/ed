#pragma once

#include <optional>
#include <variant>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include <shaderc/shaderc.hpp>

#include "log/log.h"

#include "definitions.h"
#include "textsource.h"
#include "bytecodesource.h"

namespace tire {

struct Program final {
    Program() = delete;

    Program( const Program& other ) = delete;
    auto operator=( const Program& other ) -> Program& = delete;

    Program( Program&& other ) = default;
    auto operator=( Program&& other ) -> Program& = default;

    ~Program() = default;

    template <typename ProgramSourceType>
    requires std::derived_from<ProgramSourceType, ProgramSource> explicit Program( ProgramSourceType sources )
        : _sources{ std::move( sources ) } {
        // If ProgramSourceType is already bytecode simply initialize shader modules from
        // that bytecode...
        if constexpr ( std::is_same_v<ProgramSourceType, BytecodeProgramSource> ) {
            const auto src = std::get<BytecodeProgramSource>( _sources );
            for ( auto&& shader : src.sources() ) {
                auto [stage, bytecode] = shader;
                push( stage, bytecode );
            }
            // ... and if ProgramSourceType is plain text glsl code we need compile it
            // at first.
        } else if constexpr ( std::is_same_v<ProgramSourceType, TextProgramSource> ) {
            const auto src = std::get<TextProgramSource>( _sources );

            beginCompile();

            for ( auto&& shader : src.sources() ) {
                auto [stage, text] = shader;

                // TODO: Make more robust!
                const auto s = GLSLANGStageToStageTypeMap.at( stage );

                const auto bytecode = compile( s, text );

                if ( !bytecode.has_value() ) {
                    log::fatal()( "Compilation failed!" );
                }

                push( stage, bytecode.value() );
            }

            endCompile();
        } else {
            log::fatal()( "Unknown program source type!" );
        }
    };

    [[nodiscard]]
    auto spirv( ShaderStageType stage ) const -> std::vector<uint32_t> const ;

private:
    // Create vulkan shader module.
    auto push( ShaderStageType stage, const std::vector<uint32_t> bytecode ) -> void ;
    auto compile( glslang_stage_t stage, const std::string& text ) -> std::optional<std::vector<uint32_t>> ;

    auto beginCompile() -> void ;
    auto endCompile() -> void ;

private:
    std::variant<BytecodeProgramSource, TextProgramSource> _sources;
    std::unordered_map<ShaderStageType, std::vector<uint32_t>> _modules{};
};

}  // namespace tire