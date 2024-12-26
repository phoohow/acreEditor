#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include <functional>

class BaseScene;
class BottomBar : public QWidget
{
    BaseScene* m_scene = nullptr;

    std::function<void()> m_flushFrame;

    QTabWidget* m_selector;
    QLabel*     m_stateLabel;
    QLineEdit*  m_cmdLineEdit;
    QTextEdit*  m_logTextEdit;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_stateLayout;
    QWidget*     m_stateTab;
    QVBoxLayout* m_cmdLayout;
    QWidget*     m_cmdTab;
    QTextEdit*   m_cmdResultTextEdit;
    QVBoxLayout* m_logLayout;
    QWidget*     m_logTab;

public:
    explicit BottomBar(BaseScene* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

private:
    void initState();
    void initCmd();
    void initLog();

    void onUpdateTab();

    void onStateTabChanged();
    void onCmdTabChanged();
    void onLogTabChanged();
    void updateStateInfo();
    void updateCmdInfo();
    void updateLogInfo();
    void onCmdSubmitted();

    QString processCommand(const QString& command);
};
