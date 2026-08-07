#pragma once

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <span>
#include <vulkan/vulkan.h>
#include <fstream>
#include <format>
#include <algorithm>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "context.h"
#include "log/log.h"

namespace tire {

enum class ShaderStageType {
    VERTEX,
    FRAGMENT,
    TESSELATION_EVAL,
    TESSELATION_CTRL,
    COMPUTE,
    GEOMETRY,
    RAYGEN,
    ANYHIT,
    CLOSESTHIT,
    MISS,
    INTERSECTION,
    CALLABLE,
    TASK,
    MESH,
    SUBPASSSHADING,
    CLUSTERCULLING,
};

constexpr std::string vertex_stage_suffix{ "VERTEX" };
constexpr std::string tessctrl_stage_suffix{ "TESSCTRL" };
constexpr std::string tesseval_stage_suffix{ "TESSEVAL" };
constexpr std::string geometry_stage_suffix{ "GEOMETRY" };
constexpr std::string fragment_stage_suffix{ "FRAGMENT" };
constexpr std::string compute_stage_suffix{ "COMPUTE" };
constexpr std::string raygen_stage_suffix{ "RAYGEN" };
constexpr std::string anyhit_stage_suffix{ "ANYHIT" };
constexpr std::string closeshit_stage_suffix{ "CLOSESTHIT" };
constexpr std::string miss_stage_suffix{ "MISS" };
constexpr std::string intersection_stage_suffix{ "INTERSECTION" };
constexpr std::string callable_stage_suffix{ "CALLABLE" };
constexpr std::string task_stage_suffix{ "TASK" };
constexpr std::string mesh_stage_suffix{ "MESH" };
constexpr std::string subpassshading_stage_suffix{ "SUBPASSSHADING" };
constexpr std::string clusterculling_stage_suffix{ "CLUSTERCULLING" };

template <ShaderStageType Stage>
concept ShaderStage = ( Stage == ShaderStageType::VERTEX ) || ( Stage == ShaderStageType::FRAGMENT ) ||
                      ( Stage == ShaderStageType::TESSELATION_EVAL ) ||
                      ( Stage == ShaderStageType::TESSELATION_CTRL ) || ( Stage == ShaderStageType::GEOMETRY ) ||
                      ( Stage == ShaderStageType::COMPUTE ) || ( Stage == ShaderStageType::RAYGEN ) ||
                      ( Stage == ShaderStageType::ANYHIT ) || ( Stage == ShaderStageType::CLOSESTHIT ) ||
                      ( Stage == ShaderStageType::MISS ) || ( Stage == ShaderStageType::INTERSECTION ) ||
                      ( Stage == ShaderStageType::CALLABLE ) || ( Stage == ShaderStageType::TASK ) ||
                      ( Stage == ShaderStageType::MESH ) || ( Stage == ShaderStageType::SUBPASSSHADING ) ||
                      ( Stage == ShaderStageType::CLUSTERCULLING );

const std::unordered_map<ShaderStageType, std::string> StagesSuffixMap = {
    { ShaderStageType::VERTEX, vertex_stage_suffix },
    { ShaderStageType::FRAGMENT, fragment_stage_suffix },
    { ShaderStageType::TESSELATION_EVAL, tesseval_stage_suffix },
    { ShaderStageType::TESSELATION_CTRL, tessctrl_stage_suffix },
    { ShaderStageType::GEOMETRY, geometry_stage_suffix },
    { ShaderStageType::COMPUTE, compute_stage_suffix },
    { ShaderStageType::RAYGEN, raygen_stage_suffix },
    { ShaderStageType::ANYHIT, anyhit_stage_suffix },
    { ShaderStageType::CLOSESTHIT, closeshit_stage_suffix },
    { ShaderStageType::MISS, miss_stage_suffix },
    { ShaderStageType::INTERSECTION, intersection_stage_suffix },
    { ShaderStageType::CALLABLE, callable_stage_suffix },
    { ShaderStageType::TASK, task_stage_suffix },
    { ShaderStageType::MESH, mesh_stage_suffix },
    { ShaderStageType::SUBPASSSHADING, subpassshading_stage_suffix },
    { ShaderStageType::CLUSTERCULLING, clusterculling_stage_suffix },
};

// Program is a set of vulkan shader modules loaded from spirv bytecode.
// Each program must contain at least two shader stages - VERTEX and FRAGMENT (despite
// of the vulkan specification demands at least one shader stage - VERTEX for graphics
// pipeline or it can be COMPUTE shader for compute pipeline).
struct Program final {
    Program();

    Program( const Program &other ) = delete;
    Program( Program &&other ) = delete;
    Program &operator=( const Program &other ) = delete;
    Program &operator=( Program &&other ) = delete;

    ~Program();

    // Add single shader stage from precompiled spirv file.
    // Preconditions:
    // 1) File must be compiled SPIRV bytecode with ".spv" extension.
    // 2) Filename must contains "_STAGESUFFIX" suffix ("_VERTEX", "_FRAGMENT" etc.)
    // 3) Shader mudules map contains one shader module per stage, i.e.
    // there is only one mudule for each stage and modules count
    // equal to vulkan shader stages types.
    void add( const std::filesystem::path &path );

    // Add shader stage as bytecode array
    void add( std::span<uint8_t> bytecode, const std::string &name );

    void fill( const std::vector<std::filesystem::path> &files );

    void fill( const std::vector<std::pair<std::span<uint8_t>, std::string>> &sources );

    [[nodiscard]] VkShaderModule get( const std::string &name );

    // Return shader vulkan module
    template <ShaderStageType Stage>
    requires ShaderStage<Stage> [[nodiscard]] auto get() const -> VkShaderModule {
        // We sure that "std::map<>::at() return proper value because
        // of concept keep invariant
        const std::string suffix = StagesSuffixMap.at( Stage );

        for ( const auto &item : modules_ ) {
            const auto [name, module] = item;
            if ( name.ends_with( suffix ) ) {
                return module;
            }
        }

        log::warning()( "shader module with suffix \"{}\" not found!", suffix );

        return VK_NULL_HANDLE;
    }

    void destroy( const std::string &name );

    void list();

private:
    void push( std::span<uint8_t> bytecode, const std::string &name );
    bool checkStageExist( const std::string &stageSuffix );
    bool isValidName( const std::string &name );

private:
    // Contains shader stages vulkan shader mudules. One module per stage.
    std::unordered_map<std::string, VkShaderModule> modules_{};
    std::unordered_map<ShaderStageType, bool> check_{};
};

}  // namespace tire
