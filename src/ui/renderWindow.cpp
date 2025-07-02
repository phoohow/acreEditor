#include <config.h>
#include <ui/renderWindow.h>
#include <scene/sceneMgr.h>

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

static constexpr float  g_ratio       = 1.5f;
static constexpr float  g_rotateSpeed = 100.0f;
static acre::RenderPath g_renderPath  = acre::RenderPath::rRasterGLTF;

RenderWindow::RenderWindow() :
    QWindow()
{
#if USE_VULKAN
    m_deviceMgr = std::make_unique<acre::DeviceMgr>(acre::DeviceMgr::Type::rVulkan);
#else
    m_deviceMgr = std::make_unique<acre::DeviceMgr>(acre::DeviceMgr::Type::rDX12);
#endif

#ifndef _DEBUG
    m_renderScene = std::make_unique<acre::Scene>(m_deviceMgr.get(), QDir::currentPath().toStdString().c_str());
#else
    auto srcDir   = SRC_DIR;
    auto dstDir   = DST_DIR;
    m_renderScene = std::make_unique<acre::Scene>(m_deviceMgr.get(), srcDir, dstDir);
#endif

    m_rasterConfig = std::make_unique<acre::config::Raster>();
    m_rayConfig    = std::make_unique<acre::config::RayTracing>();
    m_pathConfig   = std::make_unique<acre::config::PathTracing>();

    initScene();
}

RenderWindow::~RenderWindow() = default;

void RenderWindow::exposeEvent(QExposeEvent* event)
{
    // TODO: there is a bug
    // if (!m_swapchain)
    // {
    //     createRenderer();
    // }

    // render();
}

void RenderWindow::resizeEvent(QResizeEvent* event)
{
    if (!m_swapchain)
        createRenderer();

    if (!m_renderer)
        return;

    if (width() == 0 || height() == 0) return;

    if (!isVisible()) return;

    m_swapchain->resize(g_ratio * width(), g_ratio * height());

    m_scene->resize(width(), height());

    render();
}

void RenderWindow::paintEvent(QPaintEvent* event)
{
    if (!m_swapchain)
        createRenderer();

    render();
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_swapchain)
        createRenderer();

    if (m_enableRotate)
    {
        auto currentPosition = event->position();
        auto delta           = currentPosition - m_mousePosition;
        m_mousePosition      = currentPosition;
        if (delta.x() != 0 || delta.y() != 0)
        {
            m_scene->cameraRotateY(delta.x() / float(width()) * g_rotateSpeed);
            m_scene->cameraRotateX(delta.y() / float(height()) * g_rotateSpeed);
        }
    }

    render();
}

void RenderWindow::mousePressEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enableRotate  = true;
    m_mousePosition = event->position();

    // Pick operate
    {
        acre::PickUnit info;
        info.input.xPos = m_mousePosition.x() * g_ratio;
        info.input.yPos = m_mousePosition.y() * g_ratio;
        m_renderer->pickTo(&info);

        // m_scene->setHighlightGeometry(info.geometryID);
        // std::cout << "GeometryID: " << info.geometryID << std::endl;
    }
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_swapchain) return;

    m_enableRotate = false;

    auto currentPosition = event->position();
    auto delta           = currentPosition - m_mousePosition;
    if (delta.x() != 0 || delta.y() != 0)
    {
        m_scene->cameraRotateY(delta.x() / float(width()) * g_rotateSpeed);
        m_scene->cameraRotateX(delta.y() / float(height()) * g_rotateSpeed);
    }

    render();
}

void RenderWindow::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta();

    if (numDegrees.y() < 0)
    {
        m_scene->cameraForward();
    }
    else if (numDegrees.y() > 0)
    {
        m_scene->cameraBack();
    }

    render();
}

void RenderWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_A:
            m_scene->cameraMove(acre::math::float3(1.0f, 0, 0));
            break;
        case Qt::Key_D:
            m_scene->cameraMove(acre::math::float3(-1.0f, 0, 0));
            break;
        case Qt::Key_W:
            m_scene->cameraMove(acre::math::float3(0, 0, 1.0f));
            break;
        case Qt::Key_S:
            m_scene->cameraMove(acre::math::float3(0, 0, -1.0f));
            break;
        case Qt::Key_Q:
            m_scene->cameraMove(acre::math::float3(0, 1.0f, 0));
            break;
        case Qt::Key_E:
            m_scene->cameraMove(acre::math::float3(0, -1.0f, 0));
            break;
        case Qt::Key_Left:
            m_scene->cameraRotateY(1.0f);
            break;
        case Qt::Key_Right:
            m_scene->cameraRotateY(-1.0f);
            break;
        case Qt::Key_Up:
            m_scene->cameraRotateX(1.0f);
            break;
        case Qt::Key_Down:
            m_scene->cameraRotateX(-1.0f);
            break;
        case Qt::Key_2:
            m_scene->forwardView();
            break;
        case Qt::Key_8:
            m_scene->backView();
            break;
        case Qt::Key_4:
            m_scene->leftView();
            break;
        case Qt::Key_6:
            m_scene->rightView();
            break;
        case Qt::Key_5:
            m_scene->topView();
            break;
        case Qt::Key_0:
            m_scene->bottomView();
            break;
        case Qt::Key_R:
            m_renderer->markShaderDirty();
            break;
        case Qt::Key_P:
            render();
            showProfiler();
            break;
        default:
            break;
    }

    render();
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event)
{
}

void RenderWindow::showProfiler()
{
    if (!m_renderer) return;

    if (!m_profiler)
    {
        m_renderer->getProfilerCount(m_profilerCount);
        m_profiler = new acre::Profiler[m_profilerCount];
    }

    m_renderer->getProfiler(m_profiler);
    auto next      = m_profiler;
    auto totalTime = 0.0;

    for (int i = 0; i < m_profilerCount; i++)
    {
        std::cout << "name:" << next->name << " time:" << next->time << "ms" << std::endl;
        totalTime += next->time;
        next++;
    }

    std::cout << "total time:" << totalTime << "ms" << std::endl;
}

void RenderWindow::render()
{
    if (width() == 0 || height() == 0) return;

    m_renderer->setupTarget(m_swapchain.get());

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

void RenderWindow::saveFrame(const std::string& fileName)
{
    // Note: Debug code, it cost more time than only render
    // auto start = std::chrono::high_resolution_clock::now();
    // for (auto index = 0; index < 4096; ++index)
    // {
    //     render();
    // }
    // auto end      = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    // std::cout << "Time taken by function: " << duration << " seconds" << std::endl;

    acre::Pixels pixels;
    pixels.desc.width  = 1920;
    pixels.desc.height = 1080;
    pixels.desc.format = acre::Image::Format::RGBA8_UNORM;
    pixels.data        = new uint32_t[pixels.desc.width * pixels.desc.height];

    m_renderer->setupTarget(&pixels.desc);

    m_scene->resize(pixels.desc.width, pixels.desc.height);
    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterGLTF: m_renderer->render((void*)m_rasterConfig.get(), 2048); break;
        case acre::RenderPath::rRayGLTF: m_renderer->render((void*)m_rayConfig.get(), 2048); break;
        case acre::RenderPath::rPathGLTF: m_renderer->render((void*)m_pathConfig.get(), 2048); break;
    }

    m_renderer->copyTo(&pixels);
    m_scene->saveFrame(fileName, &pixels);

    delete[] pixels.data;

    // Note: need refrash to swapchain after render
    m_scene->resize(width(), height());
    render();
}

void RenderWindow::createRenderer()
{
    m_swapchain = std::make_unique<acre::Swapchain>(m_deviceMgr.get(), (void*)(winId()), g_ratio * width(), g_ratio * height());
    m_renderer  = std::make_unique<acre::Renderer>(m_renderScene.get(), g_renderPath);
}

void RenderWindow::initScene()
{
    // m_scene = new TriangleLoader(m_renderScene);
    m_scene = new SceneMgr(m_renderScene.get());
    switch (g_renderPath)
    {
        case acre::RenderPath::rRasterTriangle:
        case acre::RenderPath::rRasterGLTF: m_rasterConfig->camera = m_scene->getCameraID(); break;
        case acre::RenderPath::rRayTriangle:
        case acre::RenderPath::rRayGLTF: m_rayConfig->camera = m_scene->getCameraID(); break;
        case acre::RenderPath::rPathGLTF: m_pathConfig->camera = m_scene->getCameraID(); break;
    }
}
