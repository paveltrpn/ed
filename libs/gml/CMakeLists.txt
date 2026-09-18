
file(GLOB_RECURSE MOD_GML_SRC "include/glm/*.h")

add_library(gml ${MOD_GML_SRC} )

target_compile_options(gml PRIVATE "${CXX_STANDART}")


