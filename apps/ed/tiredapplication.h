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

protected:
    bool eventFilter( QObject *watched, QEvent *event ) override;

private:
    tire::TiredUI *_tiredUI{};
};
