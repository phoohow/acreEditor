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

    std::function<void()> m_flushFrame;

    QVBoxLayout* m_mainLayout;
    InfoWidget*  m_infoWidget;
    CmdWidget*   m_cmdWidget;

public:
    explicit BottomBar(SceneMgr* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func);

    void flushState();
};
