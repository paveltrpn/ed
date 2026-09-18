
set(VSG_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of VulkanSceneGraph.")
set(VSG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of VulkanSceneGraph.")

# Set the instrumentation level to compile into sources
set(VSG_MAX_INSTRUMENTATION_LEVEL 1 CACHE STRING "Set the instrumentation level to build into the VSG library, 0 for off, 1 coarse grained, 2 medium, 3 fine grained." )

find_package(Vulkan 1.1 QUIET)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(xcb REQUIRED IMPORTED_TARGET xcb)

add_subdirectory(src/vsg)
