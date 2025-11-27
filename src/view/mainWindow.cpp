#include "view/mainWindow.h"
#include "view/renderWindow.h"
#include "view/menuBar.h"
#include "view/sideBar.h"
#include "view/bottomBar.h"
#include "view/shaderEditor.h"

#include <QTabBar>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QWidget>

#include <string>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent)
{
    this->resize(900, 900);

    _init();

    _setup_main_layout();
    _setup_pages_layout();

    _setup_callbacks();
}

MainWindow::~MainWindow() = default;

void MainWindow::_init()
{
    m_top_bar    = new QWidget(this);
    m_top_layout = new QHBoxLayout();
    m_page_tab   = new QTabBar(this);
    m_page_tab->addTab("Viewport");
    m_page_tab->addTab("ShaderEditor");
    m_page_tab->setExpanding(false);

    m_center_layout = new QVBoxLayout();
    m_page_stack    = new QStackedWidget(this);
    m_center_widget = new QWidget(this);

    m_render_window = new RenderWindow;
    m_render_widget = QWidget::createWindowContainer(m_render_window);
    m_render_widget->setStyleSheet("background-color: lightGreen;");

    m_menu_bar      = new MenuBar(m_render_window->get_scene(), this);
    m_side_bar      = new SideBar(m_render_window->get_scene(), this);
    m_bottom_bar    = new BottomBar(m_render_window->get_scene(), this);
    m_shader_editor = new ShaderEditor(this);

    m_status_bar = new QStatusBar(this);
}

void MainWindow::_setup_main_layout()
{
    m_top_layout->setContentsMargins(0, 0, 0, 0);
    m_top_layout->setSpacing(0);
    m_top_layout->addWidget(m_menu_bar->getMenuBar(), 0);
    m_top_layout->addStretch(1);
    m_top_layout->addWidget(m_page_tab, 0);
    m_top_layout->addStretch(1);
    m_top_bar->setLayout(m_top_layout);
    this->setMenuWidget(m_top_bar);

    m_center_layout->setContentsMargins(0, 0, 0, 0);
    m_center_layout->setSpacing(0);
    m_center_layout->addWidget(m_page_stack, 1);
    m_center_widget->setLayout(m_center_layout);
    this->setCentralWidget(m_center_widget);

    this->setStatusBar(m_status_bar);
}

void MainWindow::_setup_pages_layout()
{
    _setup_viewport_page();
    _setup_shader_editor_page();
}

void MainWindow::_setup_viewport_page()
{
    auto hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(m_render_widget);
    hSplitter->addWidget(m_side_bar);
    hSplitter->setSizes({700, 200});

    auto vSplitter = new QSplitter(Qt::Vertical);
    vSplitter->addWidget(hSplitter);
    vSplitter->addWidget(m_bottom_bar);
    vSplitter->setSizes({700, 200});

    m_page_stack->addWidget(vSplitter);
}

void MainWindow::_setup_shader_editor_page()
{
    m_page_stack->addWidget(m_shader_editor);
}

void MainWindow::_setup_callbacks()
{
    auto renderFrameFunc = [this]() {
        m_render_window->render_frame();
    };
    auto saveFrameFunc = [this](const std::string& fileName) {
        m_status_bar->showMessage("Start saving frame ...", 0);
        m_render_window->save_frame(fileName);
        m_status_bar->showMessage((std::string("Saved frame succeed: ") + fileName).c_str(), 10000);
    };

    m_menu_bar->set_renderframe_callback(renderFrameFunc);
    m_menu_bar->set_saveframe_callback(saveFrameFunc);
    m_menu_bar->set_start_record_callback([this](const std::string& fileName) { m_render_window->start_recording(fileName); });
    m_menu_bar->set_stop_record_callback([this]() { m_render_window->stop_recording(); });
    m_menu_bar->set_resetview_callback([this]() { m_render_window->reset_view(); });
    m_menu_bar->set_flushstate_callback([this]() { m_bottom_bar->flush_state(); });

    QObject::connect(m_page_tab, &QTabBar::currentChanged, m_page_stack, &QStackedWidget::setCurrentIndex);

    m_side_bar->set_renderframe_callback(renderFrameFunc);

    m_bottom_bar->set_renderframe_callback(renderFrameFunc);
    m_bottom_bar->set_saveframe_callback([this]() { m_menu_bar->save_frame(); });
    m_bottom_bar->set_showprofiler_callback([this]() { m_bottom_bar->show_profiler(m_render_window->profiler_info()); });
    m_bottom_bar->set_pickpixel_callback([this](uint32_t x, uint32_t y) { m_bottom_bar->show_pick_info(m_render_window->pick_pixel(x, y)); });
}
