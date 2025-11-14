
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

    std::unique_ptr<acre::DeviceMgr>           m_device_mgr    = nullptr;
    std::unique_ptr<acre::Swapchain>           m_swapchain     = nullptr;
    std::unique_ptr<acre::Scene>               m_render_scene  = nullptr;
    std::unique_ptr<acre::Renderer>            m_renderer      = nullptr;
    std::unique_ptr<acre::config::Raster>      m_raster_config = nullptr;
    std::unique_ptr<acre::config::RayTracing>  m_ray_config    = nullptr;
    std::unique_ptr<acre::config::PathTracing> m_path_config   = nullptr;

    acre::Profiler* m_profiler       = nullptr;
    uint32_t        m_profiler_count = 0;

    QPointF m_mouse_pos;
    bool    m_enable_rotate = false;

    QTimer*              m_timer = nullptr;
    TimePoint            m_last_frame_time;
    bool                 m_enable_animate = false;
    AnimationController* m_anim_ctrlr     = nullptr;

    SceneMgr*         m_scene      = nullptr;
    CameraController* m_camr_ctrlr = nullptr;
    Exporter*         m_exporter   = nullptr;

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

    auto get_scene() { return m_scene; }

    void render_frame();

    void animate_frame();

    std::string profiler_info();

    std::string pick_pixel(uint32_t x, uint32_t y);

    void save_frame(const std::string& fileName);

    void reset_view();

private:
    void _create_renderer();

    void _init_scene();
};
