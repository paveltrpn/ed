
#include <string>
#include <vector>
#include <map>

#include "obj.h"

namespace tire {

enum class ObjTokens {
    GEOMETRIC_VERTICES,
    TEXTURE_VERTICES,
    VERTEX_NORMALS,
    PARAMETER_SPACE_VERTICES,
    CURVE_OR_SURFACE,
    DEGREE,
    BASIS_MATRIX,
    STEP_SIZE,
    POINT,
    LINE,
    FACE,
    CURVE,
    CURVE_2D,
    SURFACE,
    PARAMETER_VALUES,
    OUTER_TRIMMING_LOOP,
    INNER_TRIMMING_LOOP,
    SPECIAL_CURVE,
    SPECIAL_POINT,
    END_STATEMENT,
    CONNECT,
    GROUP_NAME,
    SMOOTHING_GROUP,
    MERGING_GROUP,
    OBJECT_NAME,
    BEVEL_INTERPOLATION,
    COLOR_INTERPOLATION,
    DISSOLVE_INTERPOLATION,
    LEVEL_OF_DETAIL,
    MATERIAL_NAME,
    MATERIAL_LIBRARY,
    SHADOW_CASTING,
    RAY_TRACING,
    CURVE_APPROXIMATION_TECHNIQUE,
    SURFACE_APPROXIMATION_TECHNIQUE,
    COMMENT,
};

std::map<ObjTokens, std::string> tokens_ = {
    { ObjTokens::GEOMETRIC_VERTICES, "v " },
    { ObjTokens::TEXTURE_VERTICES, "vt " },
    { ObjTokens::VERTEX_NORMALS, "vn " },
    { ObjTokens::PARAMETER_SPACE_VERTICES, "vp " },
    { ObjTokens::CURVE_OR_SURFACE, "cstype " },
    { ObjTokens::DEGREE, "deg " },
    { ObjTokens::BASIS_MATRIX, "bmat " },
    { ObjTokens::STEP_SIZE, "step " },
    { ObjTokens::POINT, "p " },
    { ObjTokens::LINE, "l " },
    { ObjTokens::FACE, "f " },
    { ObjTokens::CURVE, "curv " },
    { ObjTokens::CURVE_2D, "curv2 " },
    { ObjTokens::SURFACE, "surf " },
    { ObjTokens::PARAMETER_VALUES, "parm " },
    { ObjTokens::OUTER_TRIMMING_LOOP, "trim " },
    { ObjTokens::INNER_TRIMMING_LOOP, "hole " },
    { ObjTokens::SPECIAL_CURVE, "scrv " },
    { ObjTokens::SPECIAL_POINT, "sp " },
    { ObjTokens::END_STATEMENT, "end " },
    { ObjTokens::CONNECT, "con " },
    { ObjTokens::GROUP_NAME, "g " },
    { ObjTokens::SMOOTHING_GROUP, "s " },
    { ObjTokens::MERGING_GROUP, "mg " },
    { ObjTokens::OBJECT_NAME, "o " },
    { ObjTokens::BEVEL_INTERPOLATION, "bevel " },
    { ObjTokens::COLOR_INTERPOLATION, "c_interp " },
    { ObjTokens::DISSOLVE_INTERPOLATION, "d_interp " },
    { ObjTokens::LEVEL_OF_DETAIL, "lod " },
    { ObjTokens::MATERIAL_NAME, "usemtl " },
    { ObjTokens::MATERIAL_LIBRARY, "mtllib " },
    { ObjTokens::SHADOW_CASTING, "shadow_obj " },
    { ObjTokens::RAY_TRACING, "trace_obj " },
    { ObjTokens::CURVE_APPROXIMATION_TECHNIQUE, "ctech " },
    { ObjTokens::SURFACE_APPROXIMATION_TECHNIQUE, "stech " },
    { ObjTokens::COMMENT, "# " },
};

namespace {

auto split( std::string_view str, char delim ) -> std::vector<std::string> {
    std::vector<std::string> result{};
    auto left = str.begin();
    for ( auto it = left; it != str.end(); ++it ) {
        if ( *it == delim ) {
            result.emplace_back( &*left, it - left );
            left = it + 1;
        }
    }
    if ( left != str.end() ) result.emplace_back( &*left, str.end() - left );
    return result;
}

[[nodiscard]] auto parseVertexString( std::string_view str ) -> std::tuple<float, float, float> {
    const auto vertexValuesString = split( str, ' ' );
    const auto [x, y, z] = std::make_tuple( std::stof( vertexValuesString[0] ), std::stof( vertexValuesString[1] ),
                                            std::stof( vertexValuesString[2] ) );
    return { x, y, z };
}

[[nodiscard]] auto parseNormalString( std::string_view str ) -> std::tuple<float, float, float> {
    const auto normalValuesString = split( str, ' ' );
    const auto [x, y, z] = std::make_tuple( std::stof( normalValuesString[0] ), std::stof( normalValuesString[1] ),
                                            std::stof( normalValuesString[2] ) );
    return { x, y, z };
}

[[nodiscard]] auto parseTexCoordString( std::string_view str ) -> std::tuple<float, float> {
    const auto texCoordValuesString = split( str, ' ' );
    const auto [u, v] = std::make_tuple( std::stof( texCoordValuesString[0] ), std::stof( texCoordValuesString[1] ) );
    return { u, v };
}

[[nodiscard]] auto parseTriangleString( std::string_view str ) -> ObjFileTriangleIndices {
    const auto indicesString = split( str, ' ' );

    ObjFileTriangleIndices triangle{};

    for ( int i = 0; auto &&indexString : std::move( indicesString ) ) {
        const auto indicies = split( indexString, '/' );
        const auto [v, t, n] =
            std::make_tuple( std::stoi( indicies[0] ), std::stoi( indicies[1] ), std::stoi( indicies[2] ) );

        // In obj file indicies start from 1, but in our code we start from 0
        triangle.vertexIndex[i] = v - 1;
        triangle.normalIndex[i] = n - 1;
        triangle.texCoordIndex[i] = t - 1;

        ++i;
    }

    return triangle;
}
}  // namespace

// [[nodiscard]]
// auto readWavefrontObjFile( const std::string &filePath ) -> std::shared_ptr<SeparatedBuffersMesh> {
//     // Assert file exist.
//     if ( !std::filesystem::exists( filePath ) ) {
//         throw std::runtime_error( std::format( "File \"{}\" not exist!", filePath ) );
//     }

//     // Open file.
//     std::ifstream fileHandle;
//     fileHandle.open( filePath );
//     if ( !fileHandle ) {
//         throw std::runtime_error( std::format( "Somehow file \"{}\" not opened!", filePath ) );
//     }

//     // Buffer to read in.
//     auto objMesh = ObjMesh{};

//     // Read file content
//     std::string str;
//     while ( std::getline( fileHandle, str ) ) {
//         if ( str.starts_with( tokens_[ObjTokens::COMMENT] ) ) {
//             continue;
//         } else if ( str.starts_with( tokens_[ObjTokens::OBJECT_NAME] ) ) {
//             objMesh.setName( str.erase( 0, tokens_[ObjTokens::OBJECT_NAME].length() ) );
//             continue;
//         } else if ( str.starts_with( tokens_[ObjTokens::GEOMETRIC_VERTICES] ) ) {
//             const auto vertexString = str.erase( 0, tokens_[ObjTokens::GEOMETRIC_VERTICES].length() );
//             const auto [x, y, z] = parseVertexString( vertexString );
//             objMesh.vertices_.push_back( vector3<MeshValueType>{ x, y, z } );
//             continue;
//         } else if ( str.starts_with( tokens_[ObjTokens::VERTEX_NORMALS] ) ) {
//             const auto normalString = str.erase( 0, tokens_[ObjTokens::VERTEX_NORMALS].length() );
//             const auto [nx, ny, nz] = parseNormalString( normalString );
//             objMesh.normals_.push_back( vector3<MeshValueType>{ nx, ny, nz } );
//             continue;
//         } else if ( str.starts_with( tokens_[ObjTokens::TEXTURE_VERTICES] ) ) {
//             const auto texcrdString = str.erase( 0, tokens_[ObjTokens::TEXTURE_VERTICES].length() );
//             const auto [u, v] = parseTexCoordString( texcrdString );
//             objMesh.texcrds_.push_back( vector2<float>{ u, v } );
//             continue;
//         } else if ( str.starts_with( tokens_[ObjTokens::FACE] ) ) {
//             const auto triangleString = str.erase( 0, tokens_[ObjTokens::FACE].length() );
//             const auto triangle = parseTriangleString( triangleString );
//             objMesh.triangles_.push_back( triangle );
//             continue;
//         }
//     }

//     auto mesh = new SeparatedBuffersMesh{ objMesh };

//     return std::shared_ptr<SeparatedBuffersMesh>( mesh );
// }

}  // namespace tire