set ( headers
Geometry/Config.hh
Geometry/EigenVectorT.hh
Geometry/LoopSchemeMaskT.hh
Geometry/MathDefs.hh
Geometry/NormalConeT.hh
Geometry/NormalConeT_impl.hh
Geometry/Plane3d.hh
Geometry/QuadricT.hh
Geometry/Vector11T.hh
Geometry/VectorT.hh
Geometry/VectorT_inc.hh
IO/BinaryHelper.hh
IO/IOInstances.hh
IO/IOManager.hh
IO/MeshIO.hh
IO/OFFFormat.hh
IO/OMFormat.hh
IO/OMFormatT_impl.hh
IO/Options.hh
IO/SR_binary.hh
IO/SR_binary_spec.hh
IO/SR_binary_vector_of_bool.hh 
IO/SR_rbo.hh
IO/SR_store.hh
IO/SR_types.hh
IO/StoreRestore.hh
IO/exporter/BaseExporter.hh
IO/exporter/ExporterT.hh
IO/importer/BaseImporter.hh
IO/importer/ImporterT.hh
IO/reader/BaseReader.hh
IO/reader/OBJReader.hh
IO/reader/OFFReader.hh
IO/reader/OMReader.hh
IO/reader/PLYReader.hh
IO/reader/STLReader.hh
IO/writer/BaseWriter.hh
IO/writer/OBJWriter.hh
IO/writer/OFFWriter.hh
IO/writer/OMWriter.hh
IO/writer/PLYWriter.hh
IO/writer/STLWriter.hh
IO/writer/VTKWriter.hh
Mesh/ArrayItems.hh
Mesh/ArrayKernel.hh
Mesh/ArrayKernelT_impl.hh
Mesh/AttribKernelT.hh
Mesh/Attributes.hh
Mesh/BaseKernel.hh
Mesh/BaseMesh.hh
Mesh/Casts.hh
Mesh/CirculatorsT.hh
Mesh/DefaultPolyMesh.hh
Mesh/DefaultTriMesh.hh
Mesh/FinalMeshItemsT.hh
Mesh/Handles.hh
Mesh/IteratorsT.hh
Mesh/PolyConnectivity.hh
Mesh/PolyConnectivity_inline_impl.hh
Mesh/PolyMeshT.hh
Mesh/PolyMeshT_impl.hh
Mesh/PolyMesh_ArrayKernelT.hh
Mesh/SmartHandles.hh
Mesh/SmartRange.hh
Mesh/Status.hh
Mesh/Tags.hh
Mesh/Traits.hh
Mesh/TriConnectivity.hh
Mesh/TriMeshT.hh
Mesh/TriMeshT_impl.hh
Mesh/TriMesh_ArrayKernelT.hh
Mesh/gen/circulators_header.hh
Mesh/gen/circulators_template.hh
Mesh/gen/footer.hh
Mesh/gen/iterators_header.hh
Mesh/gen/iterators_template.hh
System/OpenMeshDLLMacros.hh
System/compiler.hh
System/config.hh
System/mostream.hh
System/omstream.hh
Utils/AutoPropertyHandleT.hh
Utils/BaseProperty.hh
Utils/Endian.hh
Utils/GenProg.hh
Utils/HandleToPropHandle.hh
Utils/Noncopyable.hh
Utils/Predicates.hh
Utils/Property.hh
Utils/PropertyContainer.hh
Utils/PropertyCreator.hh
Utils/PropertyManager.hh
Utils/RandomNumberGenerator.hh
Utils/SingletonT.hh
Utils/SingletonT_impl.hh
Utils/color_cast.hh
Utils/typename.hh
Utils/vector_cast.hh
Utils/vector_traits.hh
)

set ( sources
IO/BinaryHelper.cc
IO/IOManager.cc
IO/OMFormat.cc
IO/reader/BaseReader.cc
IO/reader/OBJReader.cc
IO/reader/OFFReader.cc
IO/reader/OMReader.cc
IO/reader/PLYReader.cc
IO/reader/STLReader.cc
IO/writer/BaseWriter.cc
IO/writer/OBJWriter.cc
IO/writer/OFFWriter.cc
IO/writer/OMWriter.cc
IO/writer/PLYWriter.cc
IO/writer/STLWriter.cc
IO/writer/VTKWriter.cc
Mesh/ArrayKernel.cc
Mesh/BaseKernel.cc
Mesh/PolyConnectivity.cc
Mesh/TriConnectivity.cc
System/omstream.cc
Utils/BaseProperty.cc
Utils/Endian.cc
Utils/PropertyCreator.cc
Utils/RandomNumberGenerator.cc
)


add_library (OpenMeshCore ${sources} ${headers})

target_include_directories(OpenMeshCore PUBLIC
                                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
                                          $<INSTALL_INTERFACE:include>)

# target_include_directories(OpenMeshCoreStatic PUBLIC
#                                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
#                                           $<INSTALL_INTERFACE:include>)

set_target_properties (OpenMeshCore PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
                                               SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

