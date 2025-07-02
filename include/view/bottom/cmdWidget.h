#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QCompleter>

#include <functional>

class SceneMgr;
class CmdWidget : public QWidget
{
    SceneMgr* m_scene = nullptr;

    std::function<void()> m_flushFrame;
    std::function<void()> m_showProfiler;

    QTabWidget* m_selector;
    QLineEdit*  m_cmdLineEdit;
    QCompleter* m_completer;
    QTextEdit*  m_historyTextEdit;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_cmdLayout;
    QWidget*     m_cmdTab;

public:
    explicit CmdWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void setShowProfilerCallback(std::function<void()> func) { m_showProfiler = func; }

private:
    void initCmd();

    void onUpdateTab();

    void onCmdTabChanged();
    void updateCmdInfo();
    void onCmdSubmitted();

    void processCommand(QString& command);

    void moveEntity(const std::string& entity, const std::string& x, const std::string& y, const std::string& z);
    void moveGeometry(const std::string& geometry, const std::string& x, const std::string& y, const std::string& z);

    void resetView();
    void resetScene();
    void loadScene(const char* scene);
    void saveFrame();
};
