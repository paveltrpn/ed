#include "tired/renderimageitem.h"

#include <QPainter>
#include <QRectF>
#include <QSizeF>

namespace tire {

RenderImageItem::RenderImageItem( QQuickItem *parent )
    : QQuickPaintedItem{ parent } {
}

auto RenderImageItem::image() const -> QImage {
    return _image;
}

auto RenderImageItem::setImage( const QImage &image ) -> void {
    if ( _image == image ) {
        return;
    }

    _image = image;
    emit imageChanged();
    update();
}

auto RenderImageItem::paint( QPainter *painter ) -> void {
    const auto bounds = boundingRect();
    if ( _image.isNull() || bounds.isEmpty() ) {
        return;
    }

    auto scaledSize = QSizeF{ _image.size() };
    scaledSize.scale( bounds.size(), Qt::KeepAspectRatio );

    auto target = QRectF{ QPointF{ 0.0, 0.0 }, scaledSize };
    target.moveCenter( bounds.center() );

    painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
    painter->drawImage( target, _image, QRectF{ _image.rect() } );
}

}  // namespace tire
