

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPushButton>

#include "tiredapplication.h"
#include "config/config.h"

auto main( int argc, char* argv[] ) -> int {
    tire::Config::init( "assets/config.json" );
    TiredApplication application( argc, argv );
    application.exec();
}
