#include <config.h>
#include <ui/renderWindow.h>
#include <scene/triangleScene.h>
#include <scene/gltfScene.h>

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

static constexpr float g_ratio       = 1.5f;
static constexpr float g_rotateSpeed = 100.0f;

RenderWindow::RenderWindow() :
    QWindow()
{
#if USE_VULKAN
    m_deviceMgr = acre::createDeviceMgr(acre::DeviceType::rVulkan);
#else
    m_deviceMgr = acre::createDeviceMgr(acre::DeviceType::rDX12);
#endif

#ifndef _DEBUG
    m_renderScene = acre::createScene(m_deviceMgr, QDir::currentPath().toStdString().c_str());
#else
    auto srcDir   = SRC_DIR;
    auto dstDir   = DST_DIR;
    m_renderScene = acre::createScene(m_deviceMgr, srcDir, dstDir);
#endif

    m_renderConfig = new acre::config::Raster;

    initScene();
}

RenderWindow::~RenderWindow()
{
    acre::destoryRenderer(m_renderer);
    acre::destoryScene(m_renderScene);
    acre::destorySwapchain(m_swapchain);
    acre::destroyDeviceMgr(m_deviceMgr);
}

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
        acre::PickInfo info;
        info.xPos = m_mousePosition.x() * g_ratio;
        info.yPos = m_mousePosition.y() * g_ratio;
        m_renderer->pickPixel(&info);
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
        default:
            break;
    }

    render();
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event)
{
}

void RenderWindow::render()
{
    if (width() == 0 || height() == 0) return;
    m_renderer->render(m_swapchain, (void*)m_renderConfig);
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
    pixels.width  = 1920;
    pixels.height = 1080;
    pixels.data   = new uint32_t[pixels.width * pixels.height];
    pixels.format = acre::ImageFormat::RGBA8;

    m_scene->resize(pixels.width, pixels.height);
    m_renderer->render(&pixels, (void*)m_renderConfig, 2048);
    m_scene->saveFrame(fileName, &pixels);

    delete[] pixels.data;

    // Note: need refrash to swapchain after render
    m_scene->resize(width(), height());
    render();
}

void RenderWindow::createRenderer()
{
    m_swapchain = acre::createSwapchain(m_deviceMgr, (void*)(winId()), g_ratio * width(), g_ratio * height());
    m_renderer  = acre::createRenderer(m_renderScene, acre::RenderPath::rRasterGLTF);
}

void RenderWindow::initScene()
{
    // m_scene = new TriangleScene(m_renderScene);
    m_scene = new GLTFScene(m_renderScene);
}
