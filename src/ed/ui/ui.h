
#pragma once

#include <vsg/all.h>

#include <QQuickWidget>
#include <QMainWindow>

#include "ed/window/window.h"

#include "appearance.h"
#include "ed/tired/tired.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

struct TiredUI final : QMainWindow {
    Q_OBJECT
public:
    TiredUI( vsg::ref_ptr<vsg::WindowTraits> traits, QObject *parent = nullptr );

    void onGlobalMouseMove( const QPointF &pos );

    Q_INVOKABLE void moveWindow();
    Q_INVOKABLE void resizeWindow( int edge );
    Q_INVOKABLE void quitApplication();

    void closeEvent( QCloseEvent *event ) override;

private:
    auto reigsterTypes() -> void;

private:
    std::unique_ptr<tire::Tired> _tired{};

    QQmlEngine *_engine;
    QQmlContext *_context;

    Window *_vsgWindow{};
    QWidget *_vsgWidget{};

    QQuickWidget *_topPanel{};
    QQuickWidget *_leftPanel{};
    QQuickWidget *_bottomPanel{};
    QQuickWidget *_rightPanel{};

    Appearance *_theme;
};

}  // namespace tire