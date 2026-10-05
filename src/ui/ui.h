
#pragma once

#include <vsg/all.h>

#include <QQuickView>
#include <QSettings>
#include <QTimer>

#include "renderitem.h"

namespace tire {

// ====================================================================
// ========== AppStateSettings ========================================
// ====================================================================

struct AppStateSettings final : public QObject {
    Q_OBJECT

    Q_PROPERTY( float topPanelHeight READ topPanelHeight WRITE setTopPanelHeight NOTIFY topPanelHeightChanged FINAL )
    Q_PROPERTY( float bottomPanelHeight READ bottomPanelHeight WRITE setBottomPanelHeight NOTIFY
                    bottomPanelHeightChanged FINAL )

    Q_PROPERTY( float leftPanelWidth READ leftPanelWidth WRITE setLeftPanelWidth NOTIFY leftPanelWidthChanged FINAL )
    Q_PROPERTY(
        float rightPanelWidth READ rightPanelWidth WRITE setRightPanelWidth NOTIFY rightPanelWidthChanged FINAL )

public:
    AppStateSettings( QObject* parent = nullptr );

    auto write() -> void;
    auto restore() -> void;

    Q_INVOKABLE void enlargeRightPanel( float factor );
    Q_INVOKABLE void enlargeLeftPanel( float factor );

    Q_INVOKABLE void resetPanelsSize();

    auto topPanelHeight() const -> float;
    auto bottomPanelHeight() const -> float;
    auto leftPanelWidth() const -> float;
    auto rightPanelWidth() const -> float;

    auto setTopPanelHeight( float value ) -> void;
    auto setBottomPanelHeight( float value ) -> void;
    auto setLeftPanelWidth( float value ) -> void;
    auto setRightPanelWidth( float value ) -> void;

signals:
    void topPanelHeightChanged();
    void bottomPanelHeightChanged();
    void leftPanelWidthChanged();
    void rightPanelWidthChanged();

private:
    QSettings* _settings{};

    float _topPanelHeight{};
    float _bottomPanelHeight{};
    float _leftPanelWidth{};
    float _rightPanelWidth{};

private:
    static constexpr float _columnLayoutFactor{ 0.07f };
    static constexpr float _rowLayoutFactor{ 0.11f };
};

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

struct TiredUI final : public QQuickView {
    Q_OBJECT
public:
    TiredUI( std::shared_ptr<VsgRender> render, QObject* parent = nullptr );

    Q_INVOKABLE void moveWindow();
    Q_INVOKABLE void resizeWindow( int edge );

    Q_INVOKABLE QVector2D mainWindowCenter() const;

    Q_INVOKABLE void quitApplication();

    void closeEvent( QCloseEvent* ev ) override;
    void keyPressEvent( QKeyEvent* ev ) override;
    void keyReleaseEvent( QKeyEvent* ev ) override;
    void mouseMoveEvent( QMouseEvent* ev ) override;
    void mousePressEvent( QMouseEvent* ev ) override;
    void mouseReleaseEvent( QMouseEvent* ev ) override;
    void resizeEvent( QResizeEvent* ev ) override;

private:
    AppStateSettings* _settings{};

    QQmlEngine* _engine{};
    QQmlContext* _context{};

    std::shared_ptr<VsgRender> _render{};

    RenderItem* _renderItemHandle{};

    QTimer _update{};
};

}  // namespace tire