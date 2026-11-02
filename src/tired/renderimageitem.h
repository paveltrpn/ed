#pragma once

#include <QImage>
#include <QQuickPaintedItem>

namespace tire {

// ======================================================================================
// ==================== RenderImageItem =================================================
// ======================================================================================

struct RenderImageItem : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY( QImage image READ image WRITE setImage NOTIFY imageChanged FINAL )

public:
    explicit RenderImageItem( QQuickItem *parent = nullptr );

    auto image() const -> QImage;
    auto setImage( const QImage &image ) -> void;

    auto paint( QPainter *painter ) -> void override;

signals:
    auto imageChanged() -> void;

private:
    QImage _image{};
};

}  // namespace tire
