cmake_minimum_required(VERSION 4.2)

project(tire_ed)

include_directories("src")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CXX_STANDART -std=c++26)

# Build Vulkan Scene Graph alongside with project.
add_subdirectory(vsg)

# Add shape generator subdirectory.
add_subdirectory(libs/generator)

find_package(Vulkan 1.1 QUIET)
find_package(glslang 15.0 REQUIRED)

include_directories(src)
include_directories(libs/generator/include)
include_directories(libs/gml/include)

set(QT_VERSION 6.10)
list(APPEND CMAKE_PREFIX_PATH $ENV{LOCAL_LATEST_QT})
find_package(Qt6 ${QT_VERSION} COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickWidgets
    QuickControls2
    QuickLayouts
)

# ================================
# Module config
# ================================
file(GLOB MOD_MESH_SRC "src/mesh/*.cpp" "src/mesh/*.h")
add_library(mesh ${MOD_MESH_SRC})
target_link_libraries(mesh log)
target_compile_options(mesh PRIVATE "${CXX_STANDART}")
target_link_libraries(mesh
   algebra
)

# ================================
# Module config
# ================================
file(GLOB MOD_CONFIG_SRC "src/config/*.cpp" "src/config/*.h")
add_library(config ${MOD_CONFIG_SRC})
target_link_libraries(config log)
target_compile_options(config PRIVATE "${CXX_STANDART}")

# ================================
# Module algebra
# ================================
file(GLOB_RECURSE MOD_ALGEBRA_SRC "src/algebra/*.h")
add_library(algebra ${MOD_ALGEBRA_SRC})
set_target_properties(algebra PROPERTIES AUTOMOC TRUE)
target_compile_options(algebra PRIVATE "${CXX_STANDART}")

# ================================
# Module log
# ================================
file(GLOB_RECURSE MOD_LOG_SRC "src/log/*.cpp" "src/log/*.h")
add_library(log ${MOD_LOG_SRC})
set_target_properties(log PROPERTIES AUTOMOC TRUE)
target_compile_options(log PRIVATE "${CXX_STANDART}")
target_link_libraries(log
   algebra
)

# ================================
# Module image
# ================================
file(GLOB_RECURSE MOD_IMAGE_SRC "src/image/*.cpp" "src/image/*.h")
add_library(image ${MOD_IMAGE_SRC})
set_target_properties(image PROPERTIES AUTOMOC TRUE)
target_compile_options(image PRIVATE "${CXX_STANDART}")
target_link_libraries(image
    log
)

# ================================
# Module event
# ================================
file(GLOB_RECURSE MOD_EVENT_SRC "src/r/event/*.cpp" "src/r/event/*.h")
add_library(event ${MOD_EVENT_SRC})
set_target_properties(event PROPERTIES AUTOMOC TRUE)
target_compile_options(event PRIVATE "${CXX_STANDART}")
target_link_libraries(event
    log
)

# ================================
# Module program
# ================================
file(GLOB MOD_PROGRAM_SRC  src/program/*.cpp )
add_library(program ${MOD_PROGRAM_SRC})
target_compile_features(program PUBLIC cxx_std_26)
target_link_libraries(program
    config
    log
    vulkan
    glslang
    glslang-default-resource-limits
    SPIRV-Tools-opt
    SPIRV-Tools
    SPIRV-Tools-diff
    SPIRV-Tools-link
    SPIRV-Tools-lint
    SPIRV-Tools-reduce
    SPIRV
)

# ================================
# Module tired
# ================================
file(GLOB_RECURSE MOD_TIRED_SRC "src/tired/*.cpp" "src/tired/*.h")
add_library(tired ${MOD_TIRED_SRC})
set_target_properties(tired PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(tired PROPERTIES AUTOMOC TRUE)
target_compile_options(tired PRIVATE "${CXX_STANDART}")
target_link_libraries(tired
    vsg
    xcb
    vulkan
    GenericCodeGen
    glslang
    glslang-default-resource-limits
    MachineIndependent
    OSDependent
    SPIRV-Tools-opt
    SPIRV-Tools
    SPIRV-Tools-diff
    SPIRV-Tools-link
    SPIRV-Tools-lint
    SPIRV-Tools-reduce
    SPIRV
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Qml
    Qt::QuickWidgets
    Qt::QuickControls2
    Qt::QuickLayouts
    log
    image
    event
    vsg
    mesh
    program
    generator
)

# ================================
# Module ed_ui
# ================================
file(GLOB_RECURSE MOD_TIRED_ED_UI_SRC "src/ui/*.cpp" "src/ui/*.h")
add_library(ed_ui ${MOD_TIRED_ED_UI_SRC})
set_target_properties(ed_ui PROPERTIES AUTOMOC TRUE)
target_compile_options(ed_ui PRIVATE "${CXX_STANDART}")
target_link_libraries(ed_ui
    vsg
    xcb
    image
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Qml
    Qt::QuickWidgets
    Qt::QuickControls2
    Qt::QuickLayouts
    vsg
)

# ================================
# tire editor executable
# ================================
set(TIRE_EDITOR_EXE ed)
add_executable(${TIRE_EDITOR_EXE})
file(GLOB_RECURSE EXE_ED_SRC "apps/ed/*.cpp" "apps/ed/*.h")
target_sources(${TIRE_EDITOR_EXE} PRIVATE ${EXE_ED_SRC})
set_target_properties(${TIRE_EDITOR_EXE} PROPERTIES AUTOMOC TRUE)
target_compile_options(${TIRE_EDITOR_EXE} PRIVATE "${CXX_STANDART}")
target_link_libraries(${TIRE_EDITOR_EXE}
    vsg
    xcb
    vulkan
    GenericCodeGen
    glslang
    glslang-default-resource-limits
    MachineIndependent
    OSDependent
    SPIRV-Tools-opt
    SPIRV-Tools
    SPIRV-Tools-diff
    SPIRV-Tools-link
    SPIRV-Tools-lint
    SPIRV-Tools-reduce
    SPIRV
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Qml
    Qt::QuickWidgets
    Qt::QuickControls2
    Qt::QuickLayouts
    ed_ui
    tired
    ed_ui
    vsg
    mesh
)
    
