#pragma once

#include <QMainWindow>

class QStatusBar;
class QTabBar;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QStackedWidget;

class MenuBar;
class SideBar;
class BottomBar;
class RenderWindow;
class ShaderEditor;

class MainWindow : public QMainWindow
{
    QWidget*     m_topBar    = nullptr;
    QHBoxLayout* m_topLayout = nullptr;
    MenuBar*     m_menuBar   = nullptr;

    QWidget*        m_centerWidget = nullptr;
    QVBoxLayout*    m_centerLayout = nullptr;
    QTabBar*        m_pageTab      = nullptr;
    QStackedWidget* m_pageStack    = nullptr;

    // Viewport page
    RenderWindow* m_renderWindow = nullptr;
    QWidget*      m_renderWidget = nullptr;
    SideBar*      m_sideBar      = nullptr;
    BottomBar*    m_bottomBar    = nullptr;
    // ShaderEditor page
    ShaderEditor* m_shaderEditor = nullptr;

    QStatusBar* m_statusBar = nullptr;

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow();

private:
    void _init();

    void setupMainLayout();

    void setupPagesLayout();
    void setupViewportPage();
    void setupShaderEditorPage();

    void setupCallbacks();
};
