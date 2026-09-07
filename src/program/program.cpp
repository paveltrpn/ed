
#include <optional>
#include <print>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <format>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include <shaderc/shaderc.hpp>

#include "program.h"

namespace tire {

auto Program::push( ShaderStageType stage, const std::vector<uint32_t> bytecode ) -> void {
    _modules[stage] = bytecode;
}

auto Program::compile( glslang_stage_t stage, const std::string& text ) -> std::optional<std::vector<uint32_t>> {
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

auto Program::spirv( ShaderStageType stage ) const -> std::vector<uint32_t> const {
    try {
        return _modules.at( stage );
    } catch ( std::out_of_range& e ) {
        return {};
    }
}

auto Program::beginCompile() -> void {
    //
    glslang_initialize_process();
}

auto Program::endCompile() -> void {
    //
    glslang_finalize_process();
}

}  // namespace tire