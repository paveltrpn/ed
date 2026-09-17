
#pragma once

#include <QObject>

#include <vsg/all.h>

#include "sceneobjectgraph.h"
#include "sceneobjectdata.h"

namespace tire {

struct SceneObjectBase : public QObject {
    Q_OBJECT

    Q_PROPERTY( SceneObjectTypeEnum type READ type NOTIFY typeChanged FINAL )
    Q_PROPERTY( QString uid READ uid NOTIFY uidChanged FINAL )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged FINAL )
    Q_PROPERTY( vsg::dvec3 position READ position WRITE setPosition NOTIFY positionChanged FINAL )
    Q_PROPERTY( vsg::dvec3 orientation READ orientation WRITE setOrientation NOTIFY orientationChanged FINAL )
    Q_PROPERTY( vsg::dvec3 scale READ scale WRITE setScale NOTIFY scaleChanged FINAL )
    Q_PROPERTY( vsg::dvec4 color READ color WRITE setColor NOTIFY colorChanged FINAL )

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

    vsg::dvec3 position() const {
        //
        return _position;
    }

    vsg::dvec3 orientation() const {
        //
        return _orientation;
    }

    vsg::dvec3 scale() const {
        //
        return _scale;
    }

    vsg::dvec4 color() const {
        //
        return _color;
    }

    void setName( const QString& value ) {
        //
        _name = value;
    }

    void setPosition( vsg::dvec3 value ) {
        //
        _position = value;
    }

    void setOrientation( vsg::dvec3 value ) {
        //
        _orientation = value;
    }

    void setScale( vsg::dvec3 value ) {
        //
        _scale = value;
    }

    void setColor( vsg::dvec4 value ) {
        //
        _color = value;
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
    vsg::dvec4 _color{};
};

}  // namespace tire