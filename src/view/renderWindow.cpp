#include <config.h>

#include <view/renderWindow.h>
#include <controller/cameraController.h>
#include <controller/animationController.h>
#include <controller/recorderController.h>
#include <controller/exporter.h>
#include <model/sceneMgr.h>

#include <acre/render/deviceMgr.h>
#include <acre/render/swapchain.h>
#include <acre/render/scene.h>
#include <acre/render/renderer.h>
#include <acre/render/config.h>

#include <QKeyEvent>
#include <QWheelEvent>
#include <QDir>

#include <iostream>
#include <chrono>

static constexpr float  g_pixelRatio  = 1.5f;
static constexpr float  g_degreeRatio = 180.f;
static acre::RenderPath g_renderPath  = acre::RenderPath::rRasterGLTF;

template <uint32_t N>
std::string toNchar(const std::string& str)
{
    std::string ret = str;
    ret.resize(N, ' ');
    return ret;
}

RenderWindow::RenderWindow() :
    QWindow()
{
#if USE_VULKAN
    m_device_mgr = std::make_unique<acre::DeviceMgr>(acre::DeviceMgr::Type::rVulkan);
#else
    m_device_mgr = std::make_unique<acre::DeviceMgr>(acre::DeviceMgr::Type::rDX12);
#endif

#ifndef _DEBUG
    m_render_scene = std::make_unique<acre::Scene>(m_device_mgr.get(), QDir::currentPath().toStdString().c_str());
#else
    auto src_dir   = SRC_DIR;
    auto dst_dir   = DST_DIR;
    m_render_scene = std::make_unique<acre::Scene>(m_device_mgr.get(), src_dir, dst_dir);
#endif

    m_raster_config = std::make_unique<acre::config::Raster>();
    m_ray_config    = std::make_unique<acre::config::RayTracing>();
    m_path_config   = std::make_unique<acre::config::PathTracing>();

    _init_scene();

    // Recommended: use QTimer to drive animation and rendering
    m_last_frame_time = std::chrono::steady_clock::now();
    m_timer           = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        animate_frame();
        render_frame();
    });
    m_timer->start(16); // About 60FPS
}

RenderWindow::~RenderWindow() = default;

void RenderWindow::exposeEvent(QExposeEvent* event)
{
    // TODO: there is a bug
    // if (!m_swapchain)
    // {
    //     _create_renderer();
    // }

    // render_frame();
}

void RenderWindow::resizeEvent(QResizeEvent* event)
{
    if (!m_swapchain) _create_renderer();

    if (!m_swapchain) return;

    if (width() == 0 || height() == 0) return;

    if (!isVisible()) return;

    m_swapchain->resize(g_pixelRatio * width(), g_pixelRatio * height());

    m_camr_ctrlr->resize(width(), height());

    render_frame();
}

void RenderWindow::paintEvent(QPaintEvent* event)
{
    if (!m_swapchain) _create_renderer();

    if (!m_swapchain) return;

    render_frame();
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_swapchain) _create_renderer();

    if (m_enable_rotate)
    {
        auto currentPosition = event->position();
        auto delta           = currentPosition - m_mouse_pos;
        m_mouse_pos          = currentPosition;
        if (delta.x() != 0 || delta.y() != 0)
        {
            m_camr_ctrlr->rotateY(delta.x() * g_degreeRatio / float(width()));
            m_camr_ctrlr->rotateX(delta.y() * g_degreeRatio / float(height()));
        }
    }

    render_frame();
}

void RenderWindow::mousePressEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enable_rotate = true;
    m_mouse_pos     = event->position();

    pick_pixel(m_mouse_pos.x() * g_pixelRatio, m_mouse_pos.y() * g_pixelRatio);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enable_rotate = false;

    auto currentPosition = event->position();
    auto delta           = currentPosition - m_mouse_pos;
    if (delta.x() != 0 || delta.y() != 0)
    {
        m_camr_ctrlr->rotateY(delta.x() * g_degreeRatio / float(width()));
        m_camr_ctrlr->rotateX(delta.y() * g_degreeRatio / float(height()));
    }

    render_frame();
}

void RenderWindow::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta();

    if (numDegrees.y() < 0)
    {
        m_camr_ctrlr->moveForward();
    }
    else if (numDegrees.y() > 0)
    {
        m_camr_ctrlr->moveBack();
    }

    render_frame();
}

void RenderWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_A: m_camr_ctrlr->move(1.0f, 0, 0); break;
        case Qt::Key_D: m_camr_ctrlr->move(-1.0f, 0, 0); break;
        case Qt::Key_W: m_camr_ctrlr->move(0, 0, 1.0f); break;
        case Qt::Key_S: m_camr_ctrlr->move(0, 0, -1.0f); break;
        case Qt::Key_Q: m_camr_ctrlr->move(0, 1.0f, 0); break;
        case Qt::Key_E: m_camr_ctrlr->move(0, -1.0f, 0); break;
        case Qt::Key_Left: m_camr_ctrlr->rotateY(1.0f); break;
        case Qt::Key_Right: m_camr_ctrlr->rotateY(-1.0f); break;
        case Qt::Key_Up: m_camr_ctrlr->rotateX(1.0f); break;
        case Qt::Key_Down: m_camr_ctrlr->rotateX(-1.0f); break;
        case Qt::Key_2: m_camr_ctrlr->frontView(); break;
        case Qt::Key_8: m_camr_ctrlr->backView(); break;
        case Qt::Key_4: m_camr_ctrlr->leftView(); break;
        case Qt::Key_6: m_camr_ctrlr->rightView(); break;
        case Qt::Key_5: m_camr_ctrlr->topView(); break;
        case Qt::Key_0: m_camr_ctrlr->bottomView(); break;
        case Qt::Key_R: m_renderer->mark_shader_dirty(); break;
        case Qt::Key_P: std::cout << profiler_info(); break;
        case Qt::Key_Space: m_enable_animate = !m_enable_animate; break;
        default: break;
    }

    render_frame();
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event)
{
}

void RenderWindow::animate_frame()
{
    if (!m_scene) return;

    if (!m_enable_animate)
    {
        m_anim_ctrlr->stop();
    }
    else
    {
        auto  now         = std::chrono::steady_clock::now();
        float delta_time  = std::chrono::duration<float>(now - m_last_frame_time).count();
        m_last_frame_time = now;
        m_anim_ctrlr->update(delta_time);
    }
}

void RenderWindow::render_frame()
{
    if (width() == 0 || height() == 0) return;

    m_renderer->setup_target(m_swapchain.get());

    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterTriangle:
        case acre::RenderPath::rRasterGLTF: m_renderer->render((void*)m_raster_config.get()); break;
        case acre::RenderPath::rRayTriangle:
        case acre::RenderPath::rRayGLTF: m_renderer->render((void*)m_ray_config.get()); break;
        case acre::RenderPath::rPathGLTF: m_renderer->render((void*)m_path_config.get()); break;
    }

    m_swapchain->present();

    // If recording, grab native render target and submit to recorder
    if (m_recorder && m_recorder->is_recording())
    {
        void* nativeTarget = nullptr;
        m_renderer->get_native_target(&nativeTarget);
        if (nativeTarget)
        {
            if (m_recorder->submit_frame(nativeTarget))
            {
                m_record_frame_index++;
            }
        }
    }
}

std::string RenderWindow::profiler_info()
{
    if (!m_renderer) return "";

    if (!m_profiler)
    {
        m_renderer->profiler_count(m_profiler_count);
        m_profiler = new acre::Profiler[m_profiler_count];
    }

    render_frame();
    m_renderer->profiler_info(m_profiler);

    std::string profiler  = "";
    auto        totalTime = 0.0;
    auto        next      = m_profiler;
    for (int i = 0; i < m_profiler_count; i++)
    {
        profiler += "name:" + toNchar<15>(next->name) + "\ttime:" + std::to_string(next->time) + "ms\n";
        totalTime += next->time;
        next++;
    }

    profiler += "total time:\t" + std::to_string(totalTime) + "ms\n";

    return profiler;
}

std::string RenderWindow::pick_pixel(uint32_t x, uint32_t y)
{
    if (!m_renderer) return "";

    acre::PickUnit info;
    info.input.xPos = x;
    info.input.yPos = y;
    m_renderer->pick_to(&info);

    std::string result = "";
    result += "Entity: " + std::to_string(info.entity) + "\n";
    result += "Geometry: " + std::to_string(info.geometry) + "\n";
    result += "Material: " + std::to_string(info.material) + "\n";

    return result;
}

void RenderWindow::save_frame(const std::string& fileName)
{
    // Debug code: this costs more time than only rendering
    // auto start = std::chrono::high_resolution_clock::now();
    // for (auto index = 0; index < 4096; ++index)
    // {
    //     render_frame();
    // }
    // auto end      = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    // std::cout << "Time taken by function: " << duration << " seconds" << std::endl;

    acre::Pixels pixels;
    pixels.desc.width  = 1920;
    pixels.desc.height = 1080;
    pixels.desc.format = acre::Image::Format::RGBA8_UNORM;
    pixels.data        = new uint32_t[pixels.desc.width * pixels.desc.height];

    m_renderer->setup_target(&pixels.desc);

    m_camr_ctrlr->resize(pixels.desc.width, pixels.desc.height);
    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterGLTF: m_renderer->render((void*)m_raster_config.get(), 2048); break;
        case acre::RenderPath::rRayGLTF: m_renderer->render((void*)m_ray_config.get(), 2048); break;
        case acre::RenderPath::rPathGLTF: m_renderer->render((void*)m_path_config.get(), 2048); break;
    }

    m_renderer->copy_to(&pixels);
    m_exporter->save_frame(fileName, pixels.data, pixels.desc.width, pixels.desc.height, 4);

    delete[] pixels.data;

    // Note: need refresh to swapchain after render
    m_camr_ctrlr->resize(width(), height());
    render_frame();
}

void RenderWindow::reset_view()
{
    m_camr_ctrlr->reset();
}

void RenderWindow::_create_renderer()
{
    m_swapchain = std::make_unique<acre::Swapchain>(m_device_mgr.get(), (void*)(winId()), g_pixelRatio * width(), g_pixelRatio * height());
    m_renderer  = std::make_unique<acre::Renderer>(m_render_scene.get(), g_renderPath);
}

void RenderWindow::_init_scene()
{
    m_scene      = new SceneMgr(m_render_scene.get());
    m_camr_ctrlr = new CameraController(m_scene);
    m_anim_ctrlr = new AnimationController(m_scene);
    m_exporter   = new Exporter(m_scene);

    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterTriangle:
        case acre::RenderPath::rRasterGLTF: m_raster_config->camera = m_scene->camera_id(); break;
        case acre::RenderPath::rRayTriangle:
        case acre::RenderPath::rRayGLTF: m_ray_config->camera = m_scene->camera_id(); break;
        case acre::RenderPath::rPathGLTF: m_path_config->camera = m_scene->camera_id(); break;
    }
}

void RenderWindow::start_recording(const std::string& fileName)
{
    if (!m_renderer) return;
    if (!m_recorder) m_recorder = new RecorderController();
    if (m_recorder->is_recording()) return;

    // Get native device pointer from renderer
    void* nativeDevice = nullptr;
    m_renderer->get_native_device(&nativeDevice);

    uint32_t w = 0, h = 0;
    if (m_swapchain)
    {
        w = m_swapchain->width();
        h = m_swapchain->height();
    }
    else
    {
        w = width();
        h = height();
    }

    if (!m_recorder->start(fileName, nativeDevice, w, h))
    {
        std::cout << "RenderWindow: failed to start recorder." << std::endl;
        return;
    }

    m_record_frame_index = 0;
}

void RenderWindow::stop_recording()
{
    if (!m_recorder) return;
    if (!m_recorder->is_recording()) return;

    m_recorder->stop();
}
