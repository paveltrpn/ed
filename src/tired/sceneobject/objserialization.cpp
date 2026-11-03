
#include "objserialization.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>
#include <sstream>
#include <string_view>
#include <utility>

#include <vsg/all.h>

#include "log/log.h"

#include "sceneobjectnode.h"
#include "box.h"
#include "sphere.h"
#include "cylinder.h"
#include "capsule.h"
#include "cone.h"
#include "torus.h"
#include "icosphere.h"
#include "icosahedron.h"
#include "dodecahedron.h"
#include "plane.h"
#include "disk.h"
#include "tube.h"
#include "cappedtube.h"
#include "roundedbox.h"
#include "sphericalcone.h"
#include "torusknot.h"
#include "spring.h"
#include "teapot.h"

namespace tire {

// ======================================================================================
// ==================== ObjSerializationVisitor =========================================
// ======================================================================================

auto ObjSerializationVisitor::serialize( BoxNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( SphereNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( CylinderNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( CapsuleNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( ConeNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( TorusNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( IcosphereNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( IcosahedronNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( DodecahedronNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( PlaneNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( DiskNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( TubeNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( CappedTubeNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( RoundedBoxNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( SphericalConeNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( TorusKnotNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( SpringNode *object ) -> void {
    //
    serializeMesh( object );
}

auto ObjSerializationVisitor::serialize( TeapotNode *object ) -> void {
    //
    serializeMesh( object );
}

[[nodiscard]] auto ObjSerializationVisitor::compile() const -> std::tuple<std::string, ObjSceneInfo> {
    //
    auto stream = std::ostringstream{};
    stream << std::format( "o scene\n" );

    auto sceneInfo = ObjSceneInfo{};
    sceneInfo._meshCount = _chunks.size();

    size_t positionOffset{};
    size_t uvOffset{};
    size_t normalOffset{};

    for ( const auto &chunk : _chunks ) {
        const auto vertexCount = chunk._vertices.size() / 3;
        sceneInfo._vertexCount += vertexCount;
        sceneInfo._triangleCount += chunk._triangles.size();

        for ( size_t index = 0; index < vertexCount; ++index ) {
            stream << std::format( "v {:.17g} {:.17g} {:.17g}\n", chunk._vertices[index * 3],
                                   chunk._vertices[( index * 3 ) + 1], chunk._vertices[( index * 3 ) + 2] );
        }

        for ( size_t index = 0; index < vertexCount; ++index ) {
            stream << std::format( "vt {:.17g} {:.17g}\n", chunk._txcoords[index * 2],
                                   chunk._txcoords[( index * 2 ) + 1] );
        }

        for ( size_t index = 0; index < vertexCount; ++index ) {
            stream << std::format( "vn {:.17g} {:.17g} {:.17g}\n", chunk._vnormals[index * 3],
                                   chunk._vnormals[( index * 3 ) + 1], chunk._vnormals[( index * 3 ) + 2] );
        }

        for ( const auto &triangle : chunk._triangles ) {
            const auto firstPosition = positionOffset + triangle._vertexIndices[0] + 1;
            const auto secondPosition = positionOffset + triangle._vertexIndices[1] + 1;
            const auto thirdPosition = positionOffset + triangle._vertexIndices[2] + 1;

            const auto firstTexcoord = uvOffset + triangle._texCoordIndices[0] + 1;
            const auto secondTexcoord = uvOffset + triangle._texCoordIndices[1] + 1;
            const auto thirdTexcoord = uvOffset + triangle._texCoordIndices[2] + 1;

            const auto firstNormal = normalOffset + triangle._normalIndices[0] + 1;
            const auto secondNormal = normalOffset + triangle._normalIndices[1] + 1;
            const auto thirdNormal = normalOffset + triangle._normalIndices[2] + 1;

            stream << std::format( "f {}/{}/{} {}/{}/{} {}/{}/{}\n", firstPosition, firstTexcoord, firstNormal,
                                   secondPosition, secondTexcoord, secondNormal, thirdPosition, thirdTexcoord,
                                   thirdNormal );
        }

        positionOffset += vertexCount;
        uvOffset += vertexCount;
        normalOffset += vertexCount;
    }

    return { stream.str(), sceneInfo };
}

auto ObjSerializationVisitor::serializeMesh( SceneObjectNode *object ) -> void {
    //
    if ( !object ) {
        log::warning()( "OBJ serialization: skipped a null scene object node." );
        return;
    }

    const auto className = std::string_view{ object->className() };

    if ( object->children.size() != 1 ) {
        log::warning()( "OBJ serialization: node {} skipped: expected a single draw command child.", className );
        return;
    }

    const auto drawCommands = object->children[0].cast<vsg::Commands>();

    if ( !drawCommands || drawCommands->children.size() != 3 ) {
        log::warning()( "OBJ serialization: node {} skipped: missing draw commands container.", className );
        return;
    }

    const auto bindVertexBuffers = drawCommands->children[0].cast<vsg::BindVertexBuffers>();
    const auto bindIndexBuffer = drawCommands->children[1].cast<vsg::BindIndexBuffer>();
    const auto drawIndexed = drawCommands->children[2].cast<vsg::DrawIndexed>();

    if ( !bindVertexBuffers || !bindIndexBuffer || !drawIndexed ) {
        log::warning()( "OBJ serialization: node {} skipped: unsupported command layout.", className );
        return;
    }

    if ( ( bindVertexBuffers->firstBinding != 0 ) || ( bindVertexBuffers->arrays.size() != 4 ) ) {
        log::warning()( "OBJ serialization: node {} skipped: unexpected vertex buffer bindings.", className );
        return;
    }

    const auto &buffers = bindVertexBuffers->arrays;

    if ( !buffers[0] || !buffers[2] || !buffers[3] || !bindIndexBuffer->indices || !bindIndexBuffer->indices->data ) {
        log::warning()( "OBJ serialization: node {} skipped: missing vertex or index buffers.", className );
        return;
    }

    const auto positions = buffers[0]->data.cast<vsg::vec3Array>();
    const auto texcoords = buffers[2]->data.cast<vsg::vec2Array>();
    const auto normals = buffers[3]->data.cast<vsg::vec3Array>();
    const auto indices = bindIndexBuffer->indices->data.cast<vsg::uintArray>();

    if ( !positions || !texcoords || !normals || !indices ) {
        log::warning()( "OBJ serialization: node {} skipped: unexpected buffer data types.", className );
        return;
    }

    const auto vertexCount = positions->size();

    if ( ( vertexCount == 0 ) || ( texcoords->size() != vertexCount ) || ( normals->size() != vertexCount ) ) {
        log::warning()( "OBJ serialization: node {} skipped: vertex channel counts do not match.", className );
        return;
    }

    const auto drawCount = static_cast<size_t>( drawIndexed->indexCount );
    const auto firstIndex = static_cast<size_t>( drawIndexed->firstIndex );
    const auto vertexOffset = static_cast<int64_t>( drawIndexed->vertexOffset );

    if ( ( drawCount == 0 ) || ( ( drawCount % 3 ) != 0 ) || ( ( firstIndex + drawCount ) > indices->size() ) ) {
        log::warning()( "OBJ serialization: node {} skipped: invalid indexed triangle range.", className );
        return;
    }

    const auto matrix = object->matrix;
    const auto determinant = vsg::determinant( matrix );

    if ( !( std::abs( determinant ) > std::numeric_limits<double>::min() ) ) {
        log::warning()( "OBJ serialization: node {} skipped: singular node matrix.", className );
        return;
    }

    const auto normalMatrix = vsg::transpose( vsg::inverse( matrix ) );
    const auto reverseWinding = determinant < 0.0;

    auto chunk = ObjChunk{};

    for ( size_t index = 0; index < vertexCount; ++index ) {
        const auto &source = ( *positions )[index];
        const auto transformed = matrix * vsg::dvec4{ source.x, source.y, source.z, 1.0 };

        if ( !std::isfinite( transformed.x ) || !std::isfinite( transformed.y ) || !std::isfinite( transformed.z ) ) {
            log::warning()( "OBJ serialization: node {} skipped: non-finite transformed position.", className );
            return;
        }

        chunk._vertices.push_back( transformed.x );
        chunk._vertices.push_back( transformed.y );
        chunk._vertices.push_back( transformed.z );
    }

    for ( size_t index = 0; index < vertexCount; ++index ) {
        const auto &source = ( *texcoords )[index];

        if ( !std::isfinite( source.x ) || !std::isfinite( source.y ) ) {
            log::warning()( "OBJ serialization: node {} skipped: non-finite texture coordinate.", className );
            return;
        }

        chunk._txcoords.push_back( source.x );
        chunk._txcoords.push_back( source.y );
    }

    for ( size_t index = 0; index < vertexCount; ++index ) {
        const auto &source = ( *normals )[index];
        const auto transformed = normalMatrix * vsg::dvec4{ source.x, source.y, source.z, 0.0 };
        const auto direction = vsg::dvec3{ transformed.x, transformed.y, transformed.z };
        const auto length = vsg::length( direction );

        if ( !( std::isfinite( direction.x ) && std::isfinite( direction.y ) && std::isfinite( direction.z ) &&
                ( length > 0.0 ) ) ) {
            log::warning()( "OBJ serialization: node {} skipped: invalid transformed normal.", className );
            return;
        }

        const auto unit = direction / length;

        chunk._vnormals.push_back( unit.x );
        chunk._vnormals.push_back( unit.y );
        chunk._vnormals.push_back( unit.z );
    }

    for ( size_t triangle = 0; triangle < ( drawCount / 3 ); ++triangle ) {
        auto corners = std::array<size_t, 3>{};

        for ( size_t corner = 0; corner < 3; ++corner ) {
            const auto rawIndex = ( *indices )[firstIndex + ( triangle * 3 ) + corner];
            const auto resolved = static_cast<int64_t>( rawIndex ) + vertexOffset;

            if ( ( resolved < 0 ) || ( static_cast<size_t>( resolved ) >= vertexCount ) ) {
                log::warning()( "OBJ serialization: node {} skipped: vertex index out of bounds.", className );
                return;
            }

            corners[corner] = static_cast<size_t>( resolved );
        }

        if ( reverseWinding ) {
            std::swap( corners[1], corners[2] );
        }

        chunk._triangles.push_back( ObjChunk::TriangleIndices{ corners, corners, corners } );
    }

    _chunks.push_back( std::move( chunk ) );
}

}  // namespace tire
