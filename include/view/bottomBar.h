#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include <functional>

class SceneMgr;
class InfoWidget;
class CmdWidget;
class BottomBar : public QWidget
{
    SceneMgr* m_scene = nullptr;

    QVBoxLayout* m_mainLayout;
    InfoWidget*  m_infoWidget;
    CmdWidget*   m_cmdWidget;

public:
    explicit BottomBar(SceneMgr* scene, QWidget* parent = nullptr);

    void setRenderFrameCallBack(std::function<void()> func);

    void setShowProfilerCallBack(std::function<void()> func);

    void setSaveFrameCallBack(std::function<void()> func);

    void setPickPixelCallBack(std::function<void(uint32_t, uint32_t)> func);

    void flushState();

    void showProfiler(const std::string& profiler);

    void showPickInfo(const std::string& info);
};
