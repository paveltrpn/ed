
#pragma once

#include <QObject>
#include <QVector3D>

#include <vsg/all.h>

#include "sceneobjectnode.h"
#include "sceneobjectdata.h"

namespace tire {

// ======================================================================================
// ==================== SceneObjectBase =================================================
// ======================================================================================

struct SceneObjectBase : public QObject {
    Q_OBJECT

    Q_PROPERTY( SceneObjectTypeEnum type READ type NOTIFY typeChanged FINAL )
    Q_PROPERTY( QString uid READ uid NOTIFY uidChanged FINAL )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged FINAL )
    Q_PROPERTY( QVector3D position READ position WRITE setPosition NOTIFY positionChanged FINAL )
    Q_PROPERTY( QVector3D orientation READ orientation WRITE setOrientation NOTIFY orientationChanged FINAL )
    Q_PROPERTY( QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged FINAL )
    Q_PROPERTY( QVector4D color READ color WRITE setColor NOTIFY colorChanged FINAL )

public:
    SceneObjectBase( /* QObject* parent = nullptr*/ ) = default;

    SceneObjectBase( const SceneObjectBase& other ) = delete;
    SceneObjectBase( SceneObjectBase&& other ) = delete;

    SceneObjectBase& operator=( const SceneObjectBase& other ) = delete;
    SceneObjectBase& operator=( SceneObjectBase&& other ) = delete;

    [[nodiscard]] SceneObjectTypeEnum type() const {
        //
        return node()->_type;
    }

    [[nodiscard]] QString name() const {
        //
        return QString::fromStdString( node()->_name );
    }

    [[nodiscard]] QString uid() const {
        //
        return QString::fromStdString( node()->_uid );
    }

    [[nodiscard]] QVector3D position() const {
        //
        return QVector3D{ static_cast<float>( node()->_position.x ), static_cast<float>( node()->_position.y ),
                          static_cast<float>( node()->_position.z ) };
    }

    [[nodiscard]] QVector3D orientation() const {
        //
        return QVector3D{ static_cast<float>( node()->_orientation.x ), static_cast<float>( node()->_orientation.y ),
                          static_cast<float>( node()->_orientation.z ) };
    }

    [[nodiscard]] QVector3D scale() const {
        //
        return QVector3D{ static_cast<float>( node()->_scale.x ), static_cast<float>( node()->_scale.y ),
                          static_cast<float>( node()->_scale.z ) };
    }

    [[nodiscard]] QVector4D color() const {
        //
        return QVector4D{ static_cast<float>( node()->_color.r ), static_cast<float>( node()->_color.g ),
                          static_cast<float>( node()->_color.b ), static_cast<float>( node()->_color.a ) };
    }

    void setName( const QString& value ) {
        //
        node()->_name = value.toStdString();
    }

    void setPosition( QVector3D value ) {
        const auto position = vsg::dvec3{ value.x(), value.y(), value.z() };
        if ( node()->_position == position ) {
            return;
        }
        node()->_position = position;
        node()->setOrigin( position );
        emit positionChanged();
    }

    void setOrientation( QVector3D value ) {
        //
        const auto orientation = vsg::dvec3{ value.x(), value.y(), value.z() };
        if ( node()->_orientation == orientation ) {
            return;
        }
        node()->_orientation = orientation;
        node()->setRotation( orientation );
        emit orientationChanged();
    }

    void setScale( QVector3D value ) {
        //
        const auto scale = vsg::dvec3{ value.x(), value.y(), value.z() };
        if ( node()->_scale == scale ) {
            return;
        }
        node()->_scale = scale;
        node()->setScale( scale );
        emit scaleChanged();
    }

    void setColor( QVector4D value ) {
        //
        node()->_color = vsg::dvec4{ value.x(), value.y(), value.z(), value.w() };
    }

    [[nodiscard]] virtual auto node() const -> vsg::ref_ptr<SceneObjectNode> = 0;

private:
    auto init() -> void;

signals:
    void typeChanged();
    void uidChanged();
    void nameChanged();
    void positionChanged();
    void orientationChanged();
    void scaleChanged();
    void colorChanged();
};

}  // namespace tire