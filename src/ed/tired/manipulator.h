
#pragma once

#include <QObject>
#include <QPoint>

#include <vsg/all.h>
#include <vsg/app/Viewer.h>
#include <vsg/ui/Keyboard.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/ui/ScrollWheelEvent.h>
#include <vsg/ui/TouchEvent.h>

#include "inputhandler.h"

namespace tire {

struct Trackball;

// ======================================================================================
// ==================== Manipulator =======================================================
// ======================================================================================

struct Manipulator final : QObject {
    Q_OBJECT

    Q_PROPERTY( QVector3D eye READ eye NOTIFY eyeChanged FINAL )
    Q_PROPERTY( QVector3D center READ center NOTIFY centerChanged FINAL )
    Q_PROPERTY( QVector3D up READ up NOTIFY upChanged FINAL )

public:
    Manipulator( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel = {},
                 QObject* parent = nullptr );

    auto trackball() -> const vsg::ref_ptr<Trackball>;

    QVector3D eye() const;
    QVector3D center() const;
    QVector3D up() const;

    friend Trackball;
signals:
    void eyeChanged();
    void centerChanged();
    void upChanged();

private:
    vsg::ref_ptr<Trackball> _trackball{};
};

// ======================================================================================
// ==================== Trackball =======================================================
// ======================================================================================

struct Trackball final : public vsg::Trackball {
    Trackball( vsg::ref_ptr<vsg::Camera> camera, vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel = {} );

    void apply( vsg::MoveEvent& moveEvent ) override;

    auto eye() const -> vsg::dvec3;
    auto center() const -> vsg::dvec3;
    auto up() const -> vsg::dvec3;

private:
};

}  // namespace tire
