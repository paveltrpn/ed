module;

#include <string>
#include <unordered_map>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

export module program : definitions;

namespace tire {

export enum class ShaderStageType {
    UNKNOWN,
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

export constexpr std::string vertex_stage_suffix{ "VERTEX" };
export constexpr std::string tessctrl_stage_suffix{ "TESSCTRL" };
export constexpr std::string tesseval_stage_suffix{ "TESSEVAL" };
export constexpr std::string geometry_stage_suffix{ "GEOMETRY" };
export constexpr std::string fragment_stage_suffix{ "FRAGMENT" };
export constexpr std::string compute_stage_suffix{ "COMPUTE" };
export constexpr std::string raygen_stage_suffix{ "RAYGEN" };
export constexpr std::string anyhit_stage_suffix{ "ANYHIT" };
export constexpr std::string closeshit_stage_suffix{ "CLOSESTHIT" };
export constexpr std::string miss_stage_suffix{ "MISS" };
export constexpr std::string intersection_stage_suffix{ "INTERSECTION" };
export constexpr std::string callable_stage_suffix{ "CALLABLE" };
export constexpr std::string task_stage_suffix{ "TASK" };
export constexpr std::string mesh_stage_suffix{ "MESH" };
export constexpr std::string subpassshading_stage_suffix{ "SUBPASSSHADING" };
export constexpr std::string clusterculling_stage_suffix{ "CLUSTERCULLING" };

export template <ShaderStageType Stage>
concept ShaderStage =
    ( Stage == ShaderStageType::VERTEX ) || ( Stage == ShaderStageType::FRAGMENT ) ||
    ( Stage == ShaderStageType::TESSELATION_EVAL ) || ( Stage == ShaderStageType::TESSELATION_CTRL ) ||
    ( Stage == ShaderStageType::GEOMETRY ) || ( Stage == ShaderStageType::COMPUTE ) ||
    ( Stage == ShaderStageType::RAYGEN ) || ( Stage == ShaderStageType::ANYHIT ) ||
    ( Stage == ShaderStageType::CLOSESTHIT ) || ( Stage == ShaderStageType::MISS ) ||
    ( Stage == ShaderStageType::INTERSECTION ) || ( Stage == ShaderStageType::CALLABLE ) ||
    ( Stage == ShaderStageType::TASK ) || ( Stage == ShaderStageType::MESH ) ||
    ( Stage == ShaderStageType::SUBPASSSHADING ) || ( Stage == ShaderStageType::CLUSTERCULLING );

const std::unordered_map<ShaderStageType, std::string> StageTypeToSuffixMap = {
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

const std::unordered_map<ShaderStageType, glslang_stage_t> GLSLANGStageToStageTypeMap = {
    { ShaderStageType::VERTEX, GLSLANG_STAGE_VERTEX },
    { ShaderStageType::FRAGMENT, GLSLANG_STAGE_FRAGMENT },
    { ShaderStageType::TESSELATION_EVAL, GLSLANG_STAGE_TESSEVALUATION },
    { ShaderStageType::TESSELATION_CTRL, GLSLANG_STAGE_TESSCONTROL },
    { ShaderStageType::GEOMETRY, GLSLANG_STAGE_GEOMETRY },
    { ShaderStageType::COMPUTE, GLSLANG_STAGE_COMPUTE },
    { ShaderStageType::RAYGEN, GLSLANG_STAGE_RAYGEN },
    { ShaderStageType::ANYHIT, GLSLANG_STAGE_ANYHIT },
    { ShaderStageType::CLOSESTHIT, GLSLANG_STAGE_CLOSESTHIT },
    { ShaderStageType::MISS, GLSLANG_STAGE_MISS },
    { ShaderStageType::INTERSECTION, GLSLANG_STAGE_INTERSECT },
    { ShaderStageType::CALLABLE, GLSLANG_STAGE_CALLABLE },
    { ShaderStageType::TASK, GLSLANG_STAGE_TASK },
    { ShaderStageType::MESH, GLSLANG_STAGE_MESH },
};

const std::unordered_map<std::string, ShaderStageType> SuffixToStageTypeMap = {
    { vertex_stage_suffix, ShaderStageType::VERTEX },
    { fragment_stage_suffix, ShaderStageType::FRAGMENT },
    { tesseval_stage_suffix, ShaderStageType::TESSELATION_EVAL },
    { tessctrl_stage_suffix, ShaderStageType::TESSELATION_CTRL },
    { geometry_stage_suffix, ShaderStageType::GEOMETRY },
    { compute_stage_suffix, ShaderStageType::COMPUTE },
    { raygen_stage_suffix, ShaderStageType::RAYGEN },
    { anyhit_stage_suffix, ShaderStageType::ANYHIT },
    { closeshit_stage_suffix, ShaderStageType::CLOSESTHIT },
    { miss_stage_suffix, ShaderStageType::MISS },
    { intersection_stage_suffix, ShaderStageType::INTERSECTION },
    { callable_stage_suffix, ShaderStageType::CALLABLE },
    { task_stage_suffix, ShaderStageType::TASK },
    { mesh_stage_suffix, ShaderStageType::MESH },
    { subpassshading_stage_suffix, ShaderStageType::SUBPASSSHADING },
    { clusterculling_stage_suffix, ShaderStageType::CLUSTERCULLING },
};

}  // namespace tire