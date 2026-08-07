cmake_minimum_required(VERSION 4.2)

project(tire_ed)

include_directories("src")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CXX_STANDART -std=c++23)

if (DEFINED ENV{VULKAN_SDK} AND NOT "$ENV{VULKAN_SDK}" STREQUAL "")
    message( "VULKAN_SDK is set to $ENV{VULKAN_SDK}")
    include_directories($ENV{VULKAN_SDK}/include)
else()
    message(SEND_ERROR "VULKAN_SDK is not set")
endif()

if (DEFINED ENV{VMA_DIR} AND NOT "$ENV{VMA_DIR}" STREQUAL "")
    message( "VMA_DIR is set to $ENV{VMA_DIR}")
    include_directories($ENV{VMA_DIR}/include)
else()
    message(SEND_ERROR "VMA_DIR is not set")
endif()

if (DEFINED ENV{vsg_DIR} AND NOT "$ENV{vsg_DIR}" STREQUAL "")
    message( "vsg_DIR is set to $ENV{vsg_DIR}")
    include_directories($ENV{vsg_DIR}/include)
    link_directories($ENV{vsg_DIR}/lib)
else()
    message(SEND_ERROR "vsg_DIR is not set")
endif()

find_package(glslang 15.0 REQUIRED)

list(APPEND CMAKE_PREFIX_PATH $ENV{LOCAL_GLFW})
find_package(glfw3 3.3 REQUIRED)

find_package(nlohmann_json 3.11 REQUIRED)

include_directories("src")

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
file(GLOB MOD_CONFIG_SRC "src/config/*.cpp" "src/config/*.h")
add_library(config ${MOD_CONFIG_SRC})
target_link_libraries(config log)
target_compile_options(config PRIVATE "${CXX_STANDART}")

# ================================
# Module context
# ================================
file(GLOB_RECURSE MOD_CONTEXT_SRC "src/r/context/*.cpp" "src/r/context/*.h")
add_library(context ${MOD_CONTEXT_SRC})
target_link_libraries(context vulkan config image)
target_compile_options(context PRIVATE "${CXX_STANDART}")

# ================================
# Module r_ui
# ================================
file(GLOB MOD_R_UI_SRC "src/r/ui/*.cpp" "src/r/ui/*.h")
add_library(r_ui ${MOD_R_UI_SRC})
target_link_libraries(r_ui log algebra image)
target_compile_options(r_ui PRIVATE "${CXX_STANDART}")

# ================================
# Module render
# ================================
file(GLOB_RECURSE MOD_RENDER_SRC "src/r/render/*.cpp" "src/r/render/*.h")
add_library(render ${MOD_RENDER_SRC})
target_link_libraries(render vulkan context r_ui config algebra)
target_compile_options(render PRIVATE "${CXX_STANDART}")

# ================================
# Module lib/fast_obj
# ================================
add_subdirectory(lib/fast_obj)

# ================================
# Module lib/libtga
# ================================
add_subdirectory(lib/libtga)

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
# Module vk
# ================================
file(GLOB_RECURSE MOD_VK_SRC "src/ed/vk/*.cpp" "src/ed/vk/*.h")
add_library(vk ${MOD_VK_SRC})
set_target_properties(vk PROPERTIES AUTOMOC TRUE)
target_compile_options(vk PRIVATE "${CXX_STANDART}")
target_link_libraries(vk
   vulkan
   log
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
    libtga
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
# Module tired
# ================================
file(GLOB_RECURSE MOD_TIRED_SRC "src/ed/tired/*.cpp" "src/ed/tired/*.h")
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
    vk
    log
    image
    fast_obj
    event
)

# ================================
# Module ed_ui
# ================================
file(GLOB_RECURSE MOD_TIRED_ED_UI_SRC "src/ed/ui/*.cpp" "src/ed/ui/*.h")
add_library(ed_ui ${MOD_TIRED_ED_UI_SRC})
set_target_properties(ed_ui PROPERTIES AUTOMOC TRUE)
target_compile_options(ed_ui PRIVATE "${CXX_STANDART}")
target_link_libraries(ed_ui
    vsg
    xcb
    vk
    image
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Qml
    Qt::QuickWidgets
    Qt::QuickControls2
    Qt::QuickLayouts
)

# ================================
# Module qt_window
# ================================
file(GLOB MOD_QT_WINDOW_SRC "src/ed/window/*.cpp" "src/ed/window/*.h")
add_library(qt_window ${MOD_QT_WINDOW_SRC})
set_target_properties(qt_window PROPERTIES AUTOMOC TRUE)
target_link_libraries(qt_window
    vsg
    xcb
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Qml
    Qt::QuickWidgets
    Qt::QuickControls2
    Qt::QuickLayouts
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
    tired
    ed_ui
    qt_window
)

# ================================
# tire executable
# ================================
set(TIRE_EXE r)
add_executable(${TIRE_EXE})
file(GLOB_RECURSE EXE_R_SRC "apps/r/*.cpp" "apps/r/*.h")
target_sources(${TIRE_EXE} PRIVATE ${EXE_R_SRC})
set_target_properties(${TIRE_EXE} PROPERTIES AUTOMOC TRUE)
target_compile_options(${TIRE_EXE} PRIVATE "${CXX_STANDART}")
target_link_libraries(${TIRE_EXE}
    event
    context
    log
    algebra
    config
    render
    glfw
    Vulkan::Vulkan
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
)
    
