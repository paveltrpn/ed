
#pragma once

#include <vsg/all.h>

#include <QQuickWidget>
#include <QMainWindow>
#include <QSettings>
#include <QSplitter>

#include "vsgwidget.h"
#include "appearance.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

struct TiredUI final : QMainWindow {
    Q_OBJECT
public:
    TiredUI( QObject *parent = nullptr );

    auto writeSettings() -> void;
    auto readSettings() -> void;

    Q_INVOKABLE void moveWindow();
    Q_INVOKABLE void resizeWindow( int edge );

    Q_INVOKABLE QVector2D mainWindowCenter() const;

    Q_INVOKABLE void quitApplication();

    Q_INVOKABLE void enlargeRightPanel( float factor );
    Q_INVOKABLE void enlargeLeftPanel( float factor );

    Q_INVOKABLE void resetPanelsSize();

    void closeEvent( QCloseEvent *event ) override;

private:
    auto registerTypes() -> void;

private:
    QSettings *_settings{};

    QQmlEngine *_engine{};
    QQmlContext *_context{};

    QSplitter *_columnSplitter{};
    QSplitter *_rowSplitter{};

    VsgWidget *_vsgWidget{};
    QQuickWidget *_topPanel{};
    QQuickWidget *_leftPanel{};
    QQuickWidget *_bottomPanel{};
    QQuickWidget *_rightPanel{};

    vsg::ref_ptr<KeyboardMap> keyboardMap;
};

}  // namespace tire