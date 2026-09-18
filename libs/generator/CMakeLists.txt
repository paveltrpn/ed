
include_directories(${CMAKE_SOURCE_DIR}/libs/generator/include)

set(TARGET_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}generator${SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

add_subdirectory(src)
