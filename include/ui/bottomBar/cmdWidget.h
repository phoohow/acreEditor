#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include <functional>

class BaseScene;
class CmdWidget : public QWidget
{
    BaseScene* m_scene = nullptr;

    std::function<void()> m_flushFrame;

    QTabWidget* m_selector;
    QLineEdit*  m_cmdLineEdit;
    QTextEdit*  m_historyTextEdit;

    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_cmdLayout;
    QWidget*     m_cmdTab;

public:
    explicit CmdWidget(BaseScene* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

private:
    void initCmd();

    void onUpdateTab();

    void onCmdTabChanged();
    void updateCmdInfo();
    void onCmdSubmitted();

    void processCommand(QString& command);
};
