module;

#include <optional>
#include <print>
#include <variant>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <format>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include <shaderc/shaderc.hpp>

#include "context/context.h"
#include "log/log.h"

export module program : program;

import : definitions;
import : programsource;
import : bytecodesource;
import : textsource;

namespace tire {

// Each program must contain at least two shader stages - VERTEX and FRAGMENT (despite
// of the vulkan specification demands at least one shader stage - VERTEX for graphics
// pipeline or it can be COMPUTE shader for compute pipeline).
export struct Program final {
    Program() = delete;

    Program( const Program& other ) = delete;
    auto operator=( const Program& other ) -> Program& = delete;

    Program( Program&& other ) = default;
    auto operator=( Program&& other ) -> Program& = default;

    ~Program() {
        for ( const auto& module : _modules ) {
            auto [_, m] = module;
            vkDestroyShaderModule( Context::instance().device(), m, nullptr );
        }
    }

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
    auto get( ShaderStageType stage ) const -> VkShaderModule const {
        try {
            return _modules.at( stage );
        } catch ( std::out_of_range& e ) {
            return VK_NULL_HANDLE;
        }
    }

    template <ShaderStageType stage>
    requires ShaderStage<stage> [[nodiscard]] auto get() const -> VkShaderModule {
        try {
            return _modules.at( stage );
        } catch ( std::out_of_range& e ) {
            return VK_NULL_HANDLE;
        }
    }

    auto destroy( ShaderStageType stage ) -> void {
        try {
            auto module = _modules.at( stage );
            vkDestroyShaderModule( Context::instance().device(), module, nullptr );
            _modules.erase( stage );
        } catch ( std::out_of_range& e ) {
            log::warning()( "Unable to destroy! Module \"{}\" not exist!", StageTypeToSuffixMap.at( stage ) );
            return;
        }
    }

private:
    // Create vulkan shader module.
    auto push( ShaderStageType stage, const std::vector<uint32_t> bytecode ) -> void {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size() * sizeof( uint32_t );
        createInfo.pCode = bytecode.data();

        VkShaderModule module{};
        if ( const auto err = vkCreateShaderModule( Context::instance().device(), &createInfo, nullptr, &module );
             err != VK_SUCCESS ) {
            throw std::runtime_error( std::format( "Failed to create shader module \"{}\" with code {}!",
                                                   StageTypeToSuffixMap.at( stage ), string_VkResult( err ) ) );
        } else {
            log::debug()( "Shader module \"{}\" created!", StageTypeToSuffixMap.at( stage ) );
        }

        _modules[stage] = module;
    }

    auto compile( glslang_stage_t stage, const std::string& text ) -> std::optional<std::vector<uint32_t>> {
        std::vector<uint32_t> result{};

        const auto input = glslang_input_t{ .language = GLSLANG_SOURCE_GLSL,
                                            .stage = stage,
                                            .client = GLSLANG_CLIENT_VULKAN,
                                            .client_version = GLSLANG_TARGET_VULKAN_1_3,
                                            .target_language = GLSLANG_TARGET_SPV,
                                            .target_language_version = GLSLANG_TARGET_SPV_1_6,
                                            .code = text.c_str(),
                                            .default_version = 450,
                                            .default_profile = GLSLANG_CORE_PROFILE,
                                            .force_default_version_and_profile = false,
                                            .forward_compatible = false,
                                            .messages = GLSLANG_MSG_DEFAULT_BIT,
                                            .resource = glslang_default_resource() };

        auto* shader = glslang_shader_create( &input );

        const auto preprocessResult = glslang_shader_preprocess( shader, &input );

        if ( !preprocessResult ) {
            std::println( "preprocess failed!" );

            const auto infoLog = glslang_shader_get_info_log( shader );
            const auto infoDebugLog = glslang_shader_get_info_debug_log( shader );

            std::println( "info log:\n{}", infoLog );
            std::println( "info debug log:\n{}", infoDebugLog );

            glslang_shader_delete( shader );

            return std::nullopt;
        }

        const auto parseResult = glslang_shader_parse( shader, &input );

        if ( !parseResult ) {
            std::println( "parse failed!" );

            const auto infoLog = glslang_shader_get_info_log( shader );
            const auto infoDebugLog = glslang_shader_get_info_debug_log( shader );

            std::println( "info log:\n{}", infoLog );
            std::println( "info debug log:\n{}", infoDebugLog );

            glslang_shader_delete( shader );

            return std::nullopt;
        }

        auto* program = glslang_program_create();
        glslang_program_add_shader( program, shader );

        // Use standard Spv and Vulkan structural validation rules during linking.
        auto msg_mask = GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT | GLSLANG_MSG_ENHANCED;
        const auto linkResult = glslang_program_link( program, msg_mask );

        if ( !linkResult ) {
            std::println( "link failed!" );

            const auto infoLog = glslang_program_get_info_log( program );
            const auto infoDebugLog = glslang_program_get_info_debug_log( program );

            glslang_program_delete( program );
            glslang_shader_delete( shader );

            return std::nullopt;
        }

        // Transform the parsed program AST into SPIR-V intermediate binary.
        glslang_program_SPIRV_generate( program, input.stage );

        auto size = glslang_program_SPIRV_get_size( program );
        if ( size > 0 ) {
            result.resize( size );
            glslang_program_SPIRV_get( program, result.data() );
        }

        if ( glslang_program_SPIRV_get_messages( program ) ) {
            const auto msg = glslang_program_SPIRV_get_messages( program );
            std::println( "SPIR-V Messages: {}", msg );
        }

        // Cleanup resources in reverse order.
        glslang_program_delete( program );
        glslang_shader_delete( shader );

        return result;
    }

    auto beginCompile() -> void {
        //
        glslang_initialize_process();
    }

    auto endCompile() -> void {
        //
        glslang_finalize_process();
    }

private:
    std::variant<BytecodeProgramSource, TextProgramSource> _sources;
    std::unordered_map<ShaderStageType, VkShaderModule> _modules{};
};

}  // namespace tire