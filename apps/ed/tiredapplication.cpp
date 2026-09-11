
#include <QMouseEvent>

#include <vsg/all.h>

#include "tiredapplication.h"

TiredApplication::TiredApplication( int &argc, char **argv )
    : QApplication( argc, argv )
    , _tiredUI{ new tire::TiredUI{ this } } {
    setAttribute( Qt::AA_UseStyleSheetPropagationInWidgetStyles );
}

bool TiredApplication::notify( QObject *receiver, QEvent *event ) {
    if ( event->type() == QEvent::MouseMove ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
    }

    return QApplication::notify( receiver, event );
}

bool TiredApplication::eventFilter( QObject *watched, QEvent *event ) {
    //
    return QApplication::eventFilter( watched, event );
}
