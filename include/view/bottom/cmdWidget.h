#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QCompleter>

#include <functional>

class SceneMgr;
class CmdController;
class CmdWidget : public QWidget
{
    SceneMgr*      m_scene         = nullptr;
    CmdController* m_cmdController = nullptr;

    QTabWidget* m_selector;
    QLineEdit*  m_cmdLineEdit;
    QCompleter* m_completer;
    QTextEdit*  m_historyTextEdit;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_cmdLayout;
    QWidget*     m_cmdTab;

public:
    explicit CmdWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void setRenderFrameCallBack(std::function<void()> func);

    void setShowProfilerCallBack(std::function<void()> func);

    void setPickPixelCallBack(std::function<void(uint32_t, uint32_t)> func);

    void setSaveFrameCallBack(std::function<void()> func);

private:
    void initCmd();

    void onUpdateTab();

    void onCmdTabChanged();
    void updateCmdInfo();
    void onCmdSubmitted();

    void processCommand(QString& command);
};
