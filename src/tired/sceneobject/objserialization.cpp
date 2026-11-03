
#include "objserialization.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <locale>
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

[[nodiscard]] auto ObjSerializationVisitor::compile() const -> std::string {
    //
    return _output;
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

    auto stream = std::ostringstream{};
    stream.imbue( std::locale::classic() );
    stream.precision( std::numeric_limits<double>::max_digits10 );

    for ( size_t index = 0; index < vertexCount; ++index ) {
        const auto &source = ( *positions )[index];
        const auto transformed = matrix * vsg::dvec4{ source.x, source.y, source.z, 1.0 };

        if ( !std::isfinite( transformed.x ) || !std::isfinite( transformed.y ) || !std::isfinite( transformed.z ) ) {
            log::warning()( "OBJ serialization: node {} skipped: non-finite transformed position.", className );
            return;
        }

        stream << "v " << transformed.x << " " << transformed.y << " " << transformed.z << "\n";
    }

    for ( size_t index = 0; index < vertexCount; ++index ) {
        const auto &source = ( *texcoords )[index];

        if ( !std::isfinite( source.x ) || !std::isfinite( source.y ) ) {
            log::warning()( "OBJ serialization: node {} skipped: non-finite texture coordinate.", className );
            return;
        }

        stream << "vt " << source.x << " " << source.y << "\n";
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

        stream << "vn " << unit.x << " " << unit.y << " " << unit.z << "\n";
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

        const auto firstPosition = _positionOffset + corners[0] + 1;
        const auto secondPosition = _positionOffset + corners[1] + 1;
        const auto thirdPosition = _positionOffset + corners[2] + 1;

        const auto firstTexcoord = _uvOffset + corners[0] + 1;
        const auto secondTexcoord = _uvOffset + corners[1] + 1;
        const auto thirdTexcoord = _uvOffset + corners[2] + 1;

        const auto firstNormal = _normalOffset + corners[0] + 1;
        const auto secondNormal = _normalOffset + corners[1] + 1;
        const auto thirdNormal = _normalOffset + corners[2] + 1;

        stream << "f " << firstPosition << "/" << firstTexcoord << "/" << firstNormal << " " << secondPosition << "/"
               << secondTexcoord << "/" << secondNormal << " " << thirdPosition << "/" << thirdTexcoord << "/"
               << thirdNormal << "\n";
    }

    _output += stream.str();

    _positionOffset += vertexCount;
    _uvOffset += vertexCount;
    _normalOffset += vertexCount;
}

}  // namespace tire
