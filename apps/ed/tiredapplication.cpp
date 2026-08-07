
#include <QMouseEvent>

#include <vsg/all.h>

#include "tiredapplication.h"

TiredApplication::TiredApplication( int &argc, char **argv )
    : QApplication( argc, argv ) {
    _tiredUI = new tire::TiredUI{ this };

    connect( this, &TiredApplication::gobalMousePostionChanged, _tiredUI, &tire::TiredUI::onGlobalMouseMove );

    setAttribute( Qt::AA_UseStyleSheetPropagationInWidgetStyles );
}

bool TiredApplication::notify( QObject *receiver, QEvent *event ) {
    if ( event->type() == QEvent::MouseMove ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        gobalMousePostionChanged( mouseEvent->globalPosition() );
    }

    return QApplication::notify( receiver, event );
}
