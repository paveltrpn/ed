set ( headers  
Decimater/BaseDecimaterT.hh
Decimater/BaseDecimaterT_impl.hh
Decimater/CollapseInfoT.hh
Decimater/DecimaterT.hh
Decimater/DecimaterT_impl.hh
Decimater/McDecimaterT.hh
Decimater/McDecimaterT_impl.hh
Decimater/MixedDecimaterT.hh
Decimater/MixedDecimaterT_impl.hh
Decimater/ModAspectRatioT.hh
Decimater/ModAspectRatioT_impl.hh
Decimater/ModBaseT.hh
Decimater/ModEdgeLengthT.hh
Decimater/ModEdgeLengthT_impl.hh
Decimater/ModHausdorffT.hh
Decimater/ModHausdorffT_impl.hh
Decimater/ModIndependentSetsT.hh
Decimater/ModNormalDeviationT.hh
Decimater/ModNormalFlippingT.hh
Decimater/ModProgMeshT.hh
Decimater/ModProgMeshT_impl.hh
Decimater/ModQuadricT.hh
Decimater/ModQuadricT_impl.hh
Decimater/ModRoundnessT.hh
Decimater/Observer.hh
Dualizer/meshDualT.hh
HoleFiller/HoleFillerT.hh
HoleFiller/HoleFillerT_impl.hh
SmartTagger/SmartTaggerT.hh
SmartTagger/SmartTaggerT_impl.hh
Smoother/JacobiLaplaceSmootherT.hh
Smoother/JacobiLaplaceSmootherT_impl.hh
Smoother/LaplaceSmootherT.hh
Smoother/LaplaceSmootherT_impl.hh
Smoother/SmootherT.hh
Smoother/SmootherT_impl.hh
Smoother/smooth_mesh.hh
Subdivider/Adaptive/Composite/CompositeT.hh
Subdivider/Adaptive/Composite/CompositeT_impl.hh
Subdivider/Adaptive/Composite/CompositeTraits.hh
Subdivider/Adaptive/Composite/RuleInterfaceT.hh
Subdivider/Adaptive/Composite/RulesT.hh
Subdivider/Adaptive/Composite/RulesT_impl.hh
Subdivider/Adaptive/Composite/Traits.hh
Subdivider/Uniform/Composite/CompositeT.hh
Subdivider/Uniform/Composite/CompositeT_impl.hh
Subdivider/Uniform/Composite/CompositeTraits.hh
Subdivider/Uniform/CatmullClarkT.hh
Subdivider/Uniform/CatmullClarkT_impl.hh
Subdivider/Uniform/CompositeLoopT.hh
Subdivider/Uniform/CompositeSqrt3T.hh
Subdivider/Uniform/LongestEdgeT.hh
Subdivider/Uniform/LoopT.hh
Subdivider/Uniform/MidpointT.hh
Subdivider/Uniform/ModifiedButterFlyT.hh
Subdivider/Uniform/Sqrt3InterpolatingSubdividerLabsikGreinerT.hh
Subdivider/Uniform/Sqrt3T.hh
Subdivider/Uniform/SubdividerT.hh
Utils/Config.hh
Utils/GLConstAsString.hh
Utils/Gnuplot.hh
Utils/HeapT.hh
Utils/MeshCheckerT.hh
Utils/MeshCheckerT_impl.hh
Utils/NumLimitsT.hh
Utils/StripifierT.hh
Utils/StripifierT_impl.hh
Utils/TestingFramework.hh
Utils/Timer.hh
Utils/conio.hh
VDPM/MeshTraits.hh
VDPM/StreamingDef.hh
VDPM/VFront.hh
VDPM/VHierarchy.hh
VDPM/VHierarchyNode.hh
VDPM/VHierarchyNodeIndex.hh
VDPM/VHierarchyWindow.hh
VDPM/ViewingParameters.hh
)


set ( sources
Decimater/Observer.cc
Utils/Gnuplot.cc
Utils/Timer.cc
Utils/conio.cc
VDPM/VFront.cc
VDPM/VHierarchy.cc
VDPM/VHierarchyNodeIndex.cc
VDPM/VHierarchyWindow.cc
VDPM/ViewingParameters.cc
)



add_library (OpenMeshTools ${sources} ${headers})

target_include_directories(OpenMeshTools PUBLIC
                                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
                                          $<INSTALL_INTERFACE:include>)

# target_include_directories(OpenMeshToolsStatic PUBLIC
#                                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
#                                           $<INSTALL_INTERFACE:include>)

set_target_properties (OpenMeshTools PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
                                                SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

target_link_libraries (OpenMeshTools OpenMeshCore)



