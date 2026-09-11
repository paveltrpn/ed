

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPushButton>

#include "tiredapplication.h"

auto main( int argc, char* argv[] ) -> int {
    TiredApplication application( argc, argv );
    application.exec();
}
