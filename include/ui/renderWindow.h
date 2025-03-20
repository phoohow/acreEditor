
#pragma once

#include <QWindow>
#include <QPoint>

namespace acre
{
class Swapchain;
class DeviceMgr;
class Renderer;
class Scene;
struct RasterConfig;
} // namespace acre

class BaseScene;
class RenderWindow : public QWindow
{
    acre::DeviceMgr*    m_deviceMgr    = nullptr;
    acre::Swapchain*    m_swapchain    = nullptr;
    acre::Scene*        m_renderScene  = nullptr;
    acre::Renderer*     m_renderer     = nullptr;
    acre::RasterConfig* m_renderConfig = nullptr;


    QPointF m_mousePosition;
    bool    m_enableRotate = false;

    BaseScene* m_scene = nullptr;

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

    void render();

    void saveFrame(const std::string& fileName);

private:
    void createRenderer();

    void initScene();
};
