
#pragma once

#include <vsg/all.h>

#include <QQuickWidget>
#include <QMainWindow>
#include <QSettings>

#include "vsgwidget.h"
#include "appearance.h"
#include "ed/tired/tired.h"

namespace tire {

// ====================================================================
// ========== TiredUi =================================================
// ====================================================================

struct TiredUI final : QMainWindow {
    Q_OBJECT
public:
    TiredUI( QObject *parent = nullptr );

    auto writeSettings() -> void;
    [[maybe_unused]] auto readSettings() -> std::pair<int, int>;

    Q_INVOKABLE void moveWindow();
    Q_INVOKABLE void resizeWindow( int edge );
    Q_INVOKABLE QVector2D mainWindowCenter() const;
    Q_INVOKABLE void quitApplication();

    void closeEvent( QCloseEvent *event ) override;

private:
    auto reigsterTypes() -> void;

private:
    tire::Tired *_tired{};

    QSettings *_settings{};

    QQmlEngine *_engine{};
    QQmlContext *_context{};

    VsgWidget *_vsgWidget{};
    QQuickWidget *_topPanel{};
    QQuickWidget *_leftPanel{};
    QQuickWidget *_bottomPanel{};
    QQuickWidget *_rightPanel{};

    Appearance *_theme;

    vsg::ref_ptr<KeyboardMap> keyboardMap;
};

}  // namespace tire