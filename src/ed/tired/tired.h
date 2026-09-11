
#pragma once

#include <memory>

#include <QObject>

#include <vsg/all.h>

#include "ed/vk/context.h"
#include "ed/ui/vsgwidget.h"
#include "manipulator.h"
#include "inputhandler.h"
#include "scenegraph.h"

namespace tire {

enum class ControlModes {
    //
    SCENE,
    SYSTEM
};

struct Tired final : QObject {
    Q_OBJECT

    Q_PROPERTY( QObject* manipulator READ manipulator NOTIFY manipulatorChanged FINAL )
    Q_PROPERTY( QObject* inputHandler READ inputHandler NOTIFY inputHandlerChanged FINAL )
    Q_PROPERTY( QObject* scenegraph READ scenegraph NOTIFY scenegraphChanged FINAL )

    Q_PROPERTY( float globalMousePosX READ globalMousePosX WRITE setGlobalMousePosX NOTIFY globalMousePosChanged FINAL )
    Q_PROPERTY( float globalMousePosY READ globalMousePosY WRITE setGlobalMousePosY NOTIFY globalMousePosChanged FINAL )

    Q_PROPERTY( int controlMode READ controlMode WRITE setControlMode NOTIFY controlModeChanged FINAL )

public:
    Tired( QObject* parent = nullptr );

    auto init( VsgWidget* window, uint32_t width, uint32_t height ) -> void;

    auto viewer() -> vsg::ref_ptr<Viewer>;
    auto rootNode() -> vsg::ref_ptr<vsg::Node>;
    auto camera() -> vsg::ref_ptr<vsg::Camera>;

    auto manipulator() const -> QObject*;
    auto inputHandler() const -> QObject*;
    auto scenegraph() const -> QObject*;

    auto registerTypes() -> void;

    auto setGlobalMousePosX( float value ) -> void;
    auto setGlobalMousePosY( float value ) -> void;

    auto globalMousePosX() -> float;
    auto globalMousePosY() -> float;

    void setControlMode( int value );
    int controlMode();

signals:
    void manipulatorChanged();
    void inputHandlerChanged();
    void scenegraphChanged();

    void globalMousePosChanged( float x, float y );

    void controlModeChanged( int );

private:
    std::shared_ptr<vk::Context> _context{};

    vsg::ref_ptr<Viewer> _viewer{};
    vsg::ref_ptr<vsg::Camera> _camera{};

    Manipulator* _manipulator{};
    InputHandler* _inputHandler{};
    Scenegraph* _scenegraph{};

    float _globalMousePosX{};
    float _globalMousePosY{};

    ControlModes _controlMode{ ControlModes::SCENE };
};

}  // namespace tire
