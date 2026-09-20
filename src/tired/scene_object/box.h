
#pragma once

#include "sceneobjectbase.h"

namespace tire::object {

struct Box final : public SceneObjectBase {
    Q_OBJECT

    Q_PROPERTY( int segmentsX READ segmentsX WRITE setSegmentsX NOTIFY segmentsXChanged FINAL )
    Q_PROPERTY( int segmentsY READ segmentsY WRITE setSegmentsY NOTIFY segmentsYChanged FINAL )
    Q_PROPERTY( int segmentsZ READ segmentsZ WRITE setSegmentsZ NOTIFY segmentsZChanged FINAL )

public:
    Box( const BoxObjectData& data );

    Box( const Box& other ) = delete;
    Box( Box&& other ) = delete;

    Box& operator=( const Box& other ) = delete;
    Box& operator=( Box&& other ) = delete;

    auto segmentsX() const -> int {
        //
        return _data._segmentsX;
    }

    auto segmentsY() const -> int {
        //
        return _data._segmentsY;
    }

    auto segmentsZ() const -> int {
        //
        return _data._segmentsZ;
    }

    auto setSegmentsX( int value ) -> void {
        //
        _data._segmentsX = value;
    }

    auto setSegmentsY( int value ) -> void {
        //
        _data._segmentsY = value;
    }

    auto setSegmentsZ( int value ) -> void {
        //
        _data._segmentsZ = value;
    }

signals:
    void segmentsXChanged();
    void segmentsYChanged();
    void segmentsZChanged();

private:
    auto init() -> void;

private:
    BoxObjectData _data{};
};

}  // namespace tire::object