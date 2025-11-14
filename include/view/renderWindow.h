
#pragma once

#include <QWindow>
#include <QPoint>
#include <QTimer>

namespace acre
{
class Swapchain;
class DeviceMgr;
class Renderer;
class Scene;
struct Profiler;
namespace config
{
struct Raster;
struct RayTracing;
struct PathTracing;
} // namespace config
} // namespace acre

class SceneMgr;
class CameraController;
class AnimationController;
class Exporter;
class RenderWindow : public QWindow
{
    using TimePoint = std::chrono::steady_clock::time_point;

    std::unique_ptr<acre::DeviceMgr>           m_device_mgr   = nullptr;
    std::unique_ptr<acre::Swapchain>           m_swapchain    = nullptr;
    std::unique_ptr<acre::Scene>               m_renderScene  = nullptr;
    std::unique_ptr<acre::Renderer>            m_renderer     = nullptr;
    std::unique_ptr<acre::config::Raster>      m_rasterConfig = nullptr;
    std::unique_ptr<acre::config::RayTracing>  m_rayConfig    = nullptr;
    std::unique_ptr<acre::config::PathTracing> m_pathConfig   = nullptr;

    acre::Profiler* m_profiler      = nullptr;
    uint32_t        m_profilerCount = 0;

    QPointF m_mousePosition;
    bool    m_enableRotate = false;

    QTimer*              m_timer = nullptr;
    TimePoint            m_lastFrameTime;
    bool                 m_enableAnimate  = false;
    AnimationController* m_animController = nullptr;

    SceneMgr*         m_scene            = nullptr;
    CameraController* m_cameraController = nullptr;
    Exporter*         m_exporter         = nullptr;

protected:
    virtual void exposeEvent(QExposeEvent*) override;
    virtual void resizeEvent(QResizeEvent*) override;
    virtual void paintEvent(QPaintEvent*) override;
    virtual void keyPressEvent(QKeyEvent*) override;
    virtual void keyReleaseEvent(QKeyEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

public:
    RenderWindow();

    ~RenderWindow();

    auto getScene() { return m_scene; }

    void renderFrame();

    void animateFrame();

    std::string profiler_info();

    std::string pickPixel(uint32_t x, uint32_t y);

    void saveFrame(const std::string& fileName);

    void resetView();

private:
    void _createRenderer();

    void _initScene();
};
