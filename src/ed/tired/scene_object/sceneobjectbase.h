
#pragma once

#include <QObject>
#include <QVector3D>

#include <vsg/all.h>

#include "sceneobjectgraph.h"
#include "sceneobjectdata.h"

namespace tire {

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
    SceneObjectBase( QObject* parent = nullptr );

    SceneObjectBase( const SceneObjectData& data );
    SceneObjectBase( SceneObjectTypeEnum type, const QString& name, const QUuid& uid, vsg::dvec3 position,
                     vsg::dvec3 orientation, vsg::dvec3 scale, vsg::dvec4 color );

    SceneObjectBase( const SceneObjectBase& other ) = delete;
    SceneObjectBase( SceneObjectBase&& other ) = delete;

    SceneObjectBase& operator=( const SceneObjectBase& other ) = delete;
    SceneObjectBase& operator=( SceneObjectBase&& other ) = delete;

    SceneObjectTypeEnum type() const {
        //
        return _type;
    }

    QString name() const {
        //
        return _name;
    }

    QString uid() const {
        //
        return _uid.toString();
    }

    QVector3D position() const {
        //
        return QVector3D{ static_cast<float>( _position.x ), static_cast<float>( _position.y ),
                          static_cast<float>( _position.z ) };
    }

    QVector3D orientation() const {
        //
        return QVector3D{ static_cast<float>( _orientation.x ), static_cast<float>( _orientation.y ),
                          static_cast<float>( _orientation.z ) };
    }

    QVector3D scale() const {
        //
        return QVector3D{ static_cast<float>( _scale.x ), static_cast<float>( _scale.y ),
                          static_cast<float>( _scale.z ) };
    }

    QVector4D color() const {
        //
        return QVector4D{ static_cast<float>( _color.r ), static_cast<float>( _color.g ),
                          static_cast<float>( _color.b ), static_cast<float>( _color.a ) };
    }

    void setName( const QString& value ) {
        //
        _name = value;
    }

    void setPosition( QVector3D value ) {
        //
        _position = vsg::dvec3{ value.x(), value.y(), value.z() };

        const auto rtX = vsg::rotate( vsg::radians( _orientation.x ), vsg::dvec3{ 1.0, 0.0, 0.0 } );
        const auto rtY = vsg::rotate( vsg::radians( _orientation.y ), vsg::dvec3{ 0.0, 1.0, 0.0 } );
        const auto rtZ = vsg::rotate( vsg::radians( _orientation.z ), vsg::dvec3{ 0.0, 0.0, 1.0 } );

        _node->matrix = vsg::translate( _position ) * ( rtX * rtY * rtZ ) * vsg::scale( _scale );
    }

    void setOrientation( QVector3D value ) {
        //
        _orientation = vsg::dvec3{ value.x(), value.y(), value.z() };
    }

    void setScale( QVector3D value ) {
        //
        _scale = vsg::dvec3{ value.x(), value.y(), value.z() };
    }

    void setColor( QVector4D value ) {
        //
        _color = vsg::dvec4{ value.x(), value.y(), value.z(), value.w() };
    }

    auto node() const -> vsg::ref_ptr<SceneObjectGraph>;

signals:
    void typeChanged();
    void uidChanged();
    void nameChanged();
    void positionChanged();
    void orientationChanged();
    void scaleChanged();
    void colorChanged();

protected:
    vsg::ref_ptr<SceneObjectGraph> _node{};

protected:
    SceneObjectTypeEnum _type{};
    QString _name{};
    QUuid _uid{};
    vsg::dvec3 _position{};
    vsg::dvec3 _orientation{};
    vsg::dvec3 _scale{};
    vsg::vec4 _color{};
};

}  // namespace tire