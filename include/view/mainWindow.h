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
    QWidget*     m_top_bar    = nullptr;
    QHBoxLayout* m_top_layout = nullptr;
    MenuBar*     m_menu_bar   = nullptr;

    QWidget*        m_center_widget = nullptr;
    QVBoxLayout*    m_center_layout = nullptr;
    QTabBar*        m_page_tab      = nullptr;
    QStackedWidget* m_page_stack    = nullptr;

    // Viewport page
    RenderWindow* m_render_window = nullptr;
    QWidget*      m_render_widget = nullptr;
    SideBar*      m_side_bar      = nullptr;
    BottomBar*    m_bottom_bar    = nullptr;
    // ShaderEditor page
    ShaderEditor* m_shader_editor = nullptr;

    QStatusBar* m_status_bar = nullptr;

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow();

private:
    void _init();

    void _setup_main_layout();

    void _setup_pages_layout();
    void _setup_viewport_page();
    void _setup_shader_editor_page();

    void _setup_callbacks();
};
