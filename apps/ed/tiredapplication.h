#pragma once

#include <vsg/all.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPushButton>

#include "ed/ui/ui.h"

class TiredApplication final : public QApplication {
    Q_OBJECT

public:
    TiredApplication( int &argc, char **argv );

    ~TiredApplication() override = default;

    bool notify( QObject *receiver, QEvent *event ) override;

    int exec() {
        installEventFilter( this );
        return QApplication::exec();
    }

signals:
    void gobalMousePostionChanged( const QPointF &pos );

protected:
    bool eventFilter( QObject *watched, QEvent *event ) override {
        //
        return QApplication::eventFilter( watched, event );
    }

private:
    tire::TiredUI *_tiredUI{};
};
