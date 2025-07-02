#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include <functional>

class SceneMgr;
class InfoWidget : public QWidget
{
    SceneMgr* m_scene = nullptr;

    std::function<void()> m_flushFrame;

    QTabWidget* m_selector;
    QLabel*     m_stateLabel;
    QTextEdit*  m_logTextEdit;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_stateLayout;
    QWidget*     m_stateTab;
    QVBoxLayout* m_logLayout;
    QWidget*     m_logTab;

public:
    explicit InfoWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void flushState();

private:
    void initState();
    void initLog();

    void onUpdateTab();

    void onStateTabChanged();
    void onLogTabChanged();
    void updateStateInfo();
    void updateLogInfo();
};
