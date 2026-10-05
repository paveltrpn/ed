
#pragma once

#include <QObject>

#include <vsg/all.h>

#include "viewer.h"
#include "manipulator.h"
#include "inputhandler.h"
#include "scenegraph.h"
#include "vsgrender.h"

namespace tire {

enum class ControlModes {
    //
    SCENE,
    SYSTEM
};

// ======================================================================================
// ==================== Tired ===========================================================
// ======================================================================================

struct Tired final : QObject {
    Q_OBJECT

    Q_PROPERTY( QObject* manipulator READ manipulator NOTIFY manipulatorChanged FINAL )
    Q_PROPERTY( QObject* inputHandler READ inputHandler NOTIFY inputHandlerChanged FINAL )
    Q_PROPERTY( QObject* scenegraph READ scenegraph NOTIFY scenegraphChanged FINAL )

    Q_PROPERTY( int controlMode READ controlMode WRITE setControlMode NOTIFY controlModeChanged FINAL )

public:
    static auto init() -> void;
    [[nodiscard]] static auto instance() -> Tired&;
    [[nodiscard]] static auto pointer() -> Tired*;

    auto vsgRender() const -> std::shared_ptr<VsgRender>;

    auto viewer() -> vsg::ref_ptr<Viewer>;
    auto rootNode() -> vsg::ref_ptr<vsg::Node>;
    auto camera() -> vsg::ref_ptr<vsg::Camera>;

    [[nodiscard]] auto manipulator() const -> QObject*;
    [[nodiscard]] auto inputHandler() const -> QObject*;
    [[nodiscard]] auto scenegraph() const -> QObject*;

    auto setControlMode( int value ) -> void;
    [[nodiscard]] auto controlMode() const -> int;

signals:
    void manipulatorChanged();
    void inputHandlerChanged();
    void scenegraphChanged();
    void controlModeChanged( int );

    void restorePanelsSize();

private:
    Tired( QObject* parent = nullptr );

    ~Tired() override = default;

    inline static std::atomic<Tired*> _instance{ nullptr };
    inline static std::once_flag _initFlag;
    inline static bool _initSuccess{ false };

private:
    std::shared_ptr<VsgRender> _render{};

    Manipulator* _manipulator{};
    InputHandler* _inputHandler{};
    Scenegraph* _scenegraph{};

    ControlModes _controlMode{ ControlModes::SCENE };
};

}  // namespace tire
