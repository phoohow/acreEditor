#include <config.h>

#include <view/renderWindow.h>
#include <controller/cameraController.h>
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
    m_renderScene = std::make_unique<acre::Scene>(m_device_mgr.get(), QDir::currentPath().toStdString().c_str());
#else
    auto src_dir  = SRC_DIR;
    auto dst_dir  = DST_DIR;
    m_renderScene = std::make_unique<acre::Scene>(m_device_mgr.get(), src_dir, dst_dir);
#endif

    m_rasterConfig = std::make_unique<acre::config::Raster>();
    m_rayConfig    = std::make_unique<acre::config::RayTracing>();
    m_pathConfig   = std::make_unique<acre::config::PathTracing>();

    initScene();

    // Recommended: use QTimer to drive animation and rendering
    m_lastFrameTime = std::chrono::steady_clock::now();
    m_timer         = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        auto  now       = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - m_lastFrameTime).count();
        m_lastFrameTime = now;
        if (m_scene) m_scene->updateAnimation(deltaTime);
        renderFrame();
    });
    m_timer->start(16); // About 60FPS
}

RenderWindow::~RenderWindow() = default;

void RenderWindow::exposeEvent(QExposeEvent* event)
{
    // TODO: there is a bug
    // if (!m_swapchain)
    // {
    //     createRenderer();
    // }

    // renderFrame();
}

void RenderWindow::resizeEvent(QResizeEvent* event)
{
    if (!m_swapchain) createRenderer();

    if (!m_swapchain) return;

    if (width() == 0 || height() == 0) return;

    if (!isVisible()) return;

    m_swapchain->resize(g_pixelRatio * width(), g_pixelRatio * height());

    m_cameraController->resize(width(), height());

    renderFrame();
}

void RenderWindow::paintEvent(QPaintEvent* event)
{
    if (!m_swapchain) createRenderer();

    if (!m_swapchain) return;

    renderFrame();
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_swapchain) createRenderer();

    if (m_enableRotate)
    {
        auto currentPosition = event->position();
        auto delta           = currentPosition - m_mousePosition;
        m_mousePosition      = currentPosition;
        if (delta.x() != 0 || delta.y() != 0)
        {
            m_cameraController->rotateY(delta.x() * g_degreeRatio / float(width()));
            m_cameraController->rotateX(delta.y() * g_degreeRatio / float(height()));
        }
    }

    renderFrame();
}

void RenderWindow::mousePressEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enableRotate  = true;
    m_mousePosition = event->position();

    pickPixel(m_mousePosition.x() * g_pixelRatio, m_mousePosition.y() * g_pixelRatio);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enableRotate = false;

    auto currentPosition = event->position();
    auto delta           = currentPosition - m_mousePosition;
    if (delta.x() != 0 || delta.y() != 0)
    {
        m_cameraController->rotateY(delta.x() * g_degreeRatio / float(width()));
        m_cameraController->rotateX(delta.y() * g_degreeRatio / float(height()));
    }

    renderFrame();
}

void RenderWindow::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta();

    if (numDegrees.y() < 0)
    {
        m_cameraController->moveForward();
    }
    else if (numDegrees.y() > 0)
    {
        m_cameraController->moveBack();
    }

    renderFrame();
}

void RenderWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_A: m_cameraController->move(1.0f, 0, 0); break;
        case Qt::Key_D: m_cameraController->move(-1.0f, 0, 0); break;
        case Qt::Key_W: m_cameraController->move(0, 0, 1.0f); break;
        case Qt::Key_S: m_cameraController->move(0, 0, -1.0f); break;
        case Qt::Key_Q: m_cameraController->move(0, 1.0f, 0); break;
        case Qt::Key_E: m_cameraController->move(0, -1.0f, 0); break;
        case Qt::Key_Left: m_cameraController->rotateY(1.0f); break;
        case Qt::Key_Right: m_cameraController->rotateY(-1.0f); break;
        case Qt::Key_Up: m_cameraController->rotateX(1.0f); break;
        case Qt::Key_Down: m_cameraController->rotateX(-1.0f); break;
        case Qt::Key_2: m_cameraController->frontView(); break;
        case Qt::Key_8: m_cameraController->backView(); break;
        case Qt::Key_4: m_cameraController->leftView(); break;
        case Qt::Key_6: m_cameraController->rightView(); break;
        case Qt::Key_5: m_cameraController->topView(); break;
        case Qt::Key_0: m_cameraController->bottomView(); break;
        case Qt::Key_R: m_renderer->mark_shader_dirty(); break;
        case Qt::Key_P: std::cout << profiler_info(); break;
        default: break;
    }

    renderFrame();
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event)
{
}

void RenderWindow::renderFrame()
{
    if (width() == 0 || height() == 0) return;

    m_renderer->setup_target(m_swapchain.get());

    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterTriangle:
        case acre::RenderPath::rRasterGLTF: m_renderer->render((void*)m_rasterConfig.get()); break;
        case acre::RenderPath::rRayTriangle:
        case acre::RenderPath::rRayGLTF: m_renderer->render((void*)m_rayConfig.get()); break;
        case acre::RenderPath::rPathGLTF: m_renderer->render((void*)m_pathConfig.get()); break;
    }

    m_swapchain->present();
}

std::string RenderWindow::profiler_info()
{
    if (!m_renderer) return "";

    if (!m_profiler)
    {
        m_renderer->profiler_count(m_profilerCount);
        m_profiler = new acre::Profiler[m_profilerCount];
    }

    renderFrame();
    m_renderer->profiler_info(m_profiler);

    std::string profiler  = "";
    auto        totalTime = 0.0;
    auto        next      = m_profiler;
    for (int i = 0; i < m_profilerCount; i++)
    {
        profiler += "name:" + toNchar<15>(next->name) + "\ttime:" + std::to_string(next->time) + "ms\n";
        totalTime += next->time;
        next++;
    }

    profiler += "total time:\t" + std::to_string(totalTime) + "ms\n";

    return profiler;
}

std::string RenderWindow::pickPixel(uint32_t x, uint32_t y)
{
    if (!m_renderer) return "";

    acre::PickUnit info;
    info.input.xPos = x;
    info.input.yPos = y;
    m_renderer->pick_to(&info);

    std::string result = "";
    result += "Entity: " + std::to_string(info.entityID) + "\n";
    result += "Geometry: " + std::to_string(info.geometryID) + "\n";
    result += "Material: " + std::to_string(info.materialID) + "\n";

    return result;
}

void RenderWindow::saveFrame(const std::string& fileName)
{
    // Debug code: this costs more time than only rendering
    // auto start = std::chrono::high_resolution_clock::now();
    // for (auto index = 0; index < 4096; ++index)
    // {
    //     renderFrame();
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

    m_cameraController->resize(pixels.desc.width, pixels.desc.height);
    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterGLTF: m_renderer->render((void*)m_rasterConfig.get(), 2048); break;
        case acre::RenderPath::rRayGLTF: m_renderer->render((void*)m_rayConfig.get(), 2048); break;
        case acre::RenderPath::rPathGLTF: m_renderer->render((void*)m_pathConfig.get(), 2048); break;
    }

    m_renderer->copy_to(&pixels);
    m_exporter->saveFrame(fileName, pixels.data, pixels.desc.width, pixels.desc.height, 4);

    delete[] pixels.data;

    // Note: need refresh to swapchain after render
    m_cameraController->resize(width(), height());
    renderFrame();
}

void RenderWindow::resetView()
{
    m_cameraController->reset();
}

void RenderWindow::createRenderer()
{
    m_swapchain = std::make_unique<acre::Swapchain>(m_device_mgr.get(), (void*)(winId()), g_pixelRatio * width(), g_pixelRatio * height());
    m_renderer  = std::make_unique<acre::Renderer>(m_renderScene.get(), g_renderPath);
}

void RenderWindow::initScene()
{
    m_scene            = new SceneMgr(m_renderScene.get());
    m_cameraController = new CameraController(m_scene);
    m_exporter         = new Exporter(m_scene);

    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterTriangle:
        case acre::RenderPath::rRasterGLTF: m_rasterConfig->camera = m_scene->getCameraID(); break;
        case acre::RenderPath::rRayTriangle:
        case acre::RenderPath::rRayGLTF: m_rayConfig->camera = m_scene->getCameraID(); break;
        case acre::RenderPath::rPathGLTF: m_pathConfig->camera = m_scene->getCameraID(); break;
    }
}
