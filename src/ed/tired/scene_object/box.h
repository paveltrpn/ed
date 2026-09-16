
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Box final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( double width READ width WRITE setWidth NOTIFY widthChanged FINAL )
    Q_PROPERTY( double height READ height WRITE setHeight NOTIFY heightChanged FINAL )
    Q_PROPERTY( double depth READ depth WRITE setDepth NOTIFY depthChanged FINAL )

public:
    Box( QObject* parent = nullptr );

    Box( const BoxObjectData& data );
    Box( SceneObjectTypeEnum type, const QString& name, const QUuid& uid, vsg::dvec3 position, vsg::dvec3 orientation,
         vsg::dvec3 scale, vsg::dvec4 color, double width, double height, double depth );

    Box( const Box& other ) = delete;
    Box( Box&& other ) = delete;

    Box& operator=( const Box& other ) = delete;
    Box& operator=( Box&& other ) = delete;

    auto width() const -> double {
        //
        return _width;
    }

    auto height() const -> double {
        //
        return _height;
    }

    auto depth() const -> double {
        //
        return _depth;
    }

    auto setWidth( double value ) -> void {
        //
        _width = value;
    }

    auto setHeight( double value ) -> void {
        //
        _height = value;
    }

    auto setDepth( double value ) -> void {
        //
        _depth = value;
    }

signals:
    void widthChanged();
    void heightChanged();
    void depthChanged();

private:
    auto init() -> void;

private:
    double _width{};
    double _height{};
    double _depth{};
};

}  // namespace tire::object