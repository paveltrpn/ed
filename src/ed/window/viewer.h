#pragma once

// Portions of this code are derived from vsgQt, licensed under the MIT License.
// See: https:/https://github.com/vsg-dev/vsgQt/LICENSE

#include <vsg/app/Viewer.h>

#include <QTimer>

namespace tire {

// forward declare
class Window;

class Viewer : public vsg::Inherit<vsg::Viewer, Viewer> {
public:
    /// Create Viewer.
    /// If msecTimerInterval is > 0 then call setInterval(msecTimerInterval) to set up QTime to call Viewer::render() at this interval,
    /// otherwise the redraw is left application to call Viewer::render() when required, call Viewer::setInverval() to set up the timer,
    /// or to call the underlying vsg::Viewer methods for rendering a framne within the application when the viewer needs to redraw a frame.
    Viewer( int msecTimerInterval = 0 );

    QTimer timer;
    std::atomic_uint requests;
    bool continuousUpdate = true;

    /// override pollEvents to prevent the window->pollEvents() from being called by vsg::Viewer
    bool pollEvents( bool discardPreviousEvents = true ) override;

    /// increment the requests count to signal that a new frame should be rendered on the next timer call.
    virtual void request();

    /// called by the QTimer and will do the viewer frame calls to render all windows associated with the viewer.
    /// if continuousUpdate is false then the viewer frame calls are only done if the requests count is > 0
    virtual void render( double simulationTime = vsg::Viewer::UseTimeSinceStartPoint );

    /// set the QTimer interval in milliseconds, this controls how often the Viewer::render() is called
    void setInterval( int msecTimerInterval );
};

}  // namespace tired

EVSG_type_name( tire::Viewer );
