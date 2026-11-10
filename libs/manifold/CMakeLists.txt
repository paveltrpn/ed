set(MANIFOLD_SRCS
    src/boolean2.cpp
    src/boolean2_diagnostics.cpp
    src/boolean2_offset.cpp
    src/boolean2_predicates.cpp
    src/boolean2_sweep.cpp
    src/boolean3.cpp
    src/boolean_result.cpp
    src/constructors.cpp
    src/cross_section.cpp
    src/csg_tree.cpp
    src/edge_op.cpp
    src/execution_impl.cpp
    src/face_op.cpp
    src/impl.cpp
    src/manifold.cpp
    src/minkowski.cpp
    src/polygon.cpp
    src/properties.cpp
    src/quickhull.cpp
    src/sdf.cpp
    src/smoothing.cpp
    src/sort.cpp
    src/subdivision.cpp
    src/tree2d.cpp
)

add_library(manifold ${MANIFOLD_SRCS})
target_compile_features(manifold PUBLIC cxx_std_17)
target_compile_definitions(manifold PUBLIC MANIFOLD_PAR=-1)
target_include_directories(manifold
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
