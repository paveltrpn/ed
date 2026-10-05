
#pragma once

#include <vsg/all.h>

#include <QQuickView>
#include <QSettings>
#include <QTimer>

#include "renderitem.h"
#include "appearance.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

struct VsgRender;

struct TiredUI final : public QQuickView {
    Q_OBJECT
public:
    TiredUI( std::shared_ptr<VsgRender> render, QObject* parent = nullptr );

    auto writeSettings() -> void;
    auto readSettings() -> void;

    Q_INVOKABLE void moveWindow();
    Q_INVOKABLE void resizeWindow( int edge );

    Q_INVOKABLE QVector2D mainWindowCenter() const;

    Q_INVOKABLE void quitApplication();

    Q_INVOKABLE void enlargeRightPanel( float factor );
    Q_INVOKABLE void enlargeLeftPanel( float factor );

    Q_INVOKABLE void resetPanelsSize();

    void closeEvent( QCloseEvent* ev ) override;
    void keyPressEvent( QKeyEvent* ev ) override;
    void keyReleaseEvent( QKeyEvent* ev ) override;
    void mouseMoveEvent( QMouseEvent* ev ) override;
    void mousePressEvent( QMouseEvent* ev ) override;
    void mouseReleaseEvent( QMouseEvent* ev ) override;
    void resizeEvent( QResizeEvent* ev ) override;

private:
    auto registerTypes() -> void;

private:
    QSettings* _settings{};

    QQmlEngine* _engine{};
    QQmlContext* _context{};

    std::shared_ptr<VsgRender> _render{};

    RenderItem* _renderItemHandle{};

    QTimer _update{};

private:
    static constexpr float _columnLayoutFactor{ 0.07f };
    static constexpr float _rowLayoutFactor{ 0.11f };
};

}  // namespace tire