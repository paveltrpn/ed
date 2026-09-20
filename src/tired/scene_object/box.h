
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
        return _segmentsX;
    }

    auto segmentsY() const -> int {
        //
        return _segmentsY;
    }

    auto segmentsZ() const -> int {
        //
        return _segmentsZ;
    }

    auto setSegmentsX( int value ) -> void {
        //
        _segmentsX = value;
    }

    auto setSegmentsY( int value ) -> void {
        //
        _segmentsY = value;
    }

    auto setSegmentsZ( int value ) -> void {
        //
        _segmentsZ = value;
    }

signals:
    void segmentsXChanged();
    void segmentsYChanged();
    void segmentsZChanged();

private:
    auto init() -> void;

private:
    int _segmentsX{ 1 };
    int _segmentsY{ 1 };
    int _segmentsZ{ 1 };
};

}  // namespace tire::object