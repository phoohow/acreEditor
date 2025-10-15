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

    setupMainLayout();
    setupPagesLayout();

    setupCallbacks();
}

MainWindow::~MainWindow() = default;

void MainWindow::_init()
{
    m_topBar    = new QWidget(this);
    m_topLayout = new QHBoxLayout();
    m_pageTab   = new QTabBar(this);
    m_pageTab->addTab("Viewport");
    m_pageTab->addTab("ShaderEditor");
    m_pageTab->setExpanding(false);

    m_centerLayout = new QVBoxLayout();
    m_pageStack    = new QStackedWidget(this);
    m_centerWidget = new QWidget(this);

    m_renderWindow = new RenderWindow;
    m_renderWidget = QWidget::createWindowContainer(m_renderWindow);
    m_renderWidget->setStyleSheet("background-color: lightGreen;");

    m_menuBar      = new MenuBar(m_renderWindow->getScene(), this);
    m_sideBar      = new SideBar(m_renderWindow->getScene(), this);
    m_bottomBar    = new BottomBar(m_renderWindow->getScene(), this);
    m_shaderEditor = new ShaderEditor(this);

    m_statusBar = new QStatusBar(this);
}

void MainWindow::setupMainLayout()
{
    m_topLayout->setContentsMargins(0, 0, 0, 0);
    m_topLayout->setSpacing(0);
    m_topLayout->addWidget(m_menuBar->getMenuBar(), 0);
    m_topLayout->addStretch(1);
    m_topLayout->addWidget(m_pageTab, 0);
    m_topLayout->addStretch(1);
    m_topBar->setLayout(m_topLayout);
    this->setMenuWidget(m_topBar);

    m_centerLayout->setContentsMargins(0, 0, 0, 0);
    m_centerLayout->setSpacing(0);
    m_centerLayout->addWidget(m_pageStack, 1);
    m_centerWidget->setLayout(m_centerLayout);
    this->setCentralWidget(m_centerWidget);

    this->setStatusBar(m_statusBar);
}

void MainWindow::setupPagesLayout()
{
    setupViewportPage();
    setupShaderEditorPage();
}

void MainWindow::setupViewportPage()
{
    auto hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(m_renderWidget);
    hSplitter->addWidget(m_sideBar);
    hSplitter->setSizes({700, 200});

    auto vSplitter = new QSplitter(Qt::Vertical);
    vSplitter->addWidget(hSplitter);
    vSplitter->addWidget(m_bottomBar);
    vSplitter->setSizes({700, 200});

    m_pageStack->addWidget(vSplitter);
}

void MainWindow::setupShaderEditorPage()
{
    m_pageStack->addWidget(m_shaderEditor);
}

void MainWindow::setupCallbacks()
{
    auto renderFrameFunc = [this]() {
        m_renderWindow->renderFrame();
    };
    auto saveFrameFunc = [this](const std::string& fileName) {
        m_statusBar->showMessage("Start saving frame ...", 0);
        m_renderWindow->saveFrame(fileName);
        m_statusBar->showMessage((std::string("Saved frame succeed: ") + fileName).c_str(), 10000);
    };

    m_menuBar->setRenderFrameCallBack(renderFrameFunc);
    m_menuBar->setSaveFrameCallBack(saveFrameFunc);
    m_menuBar->setResetViewCallBack([this]() { m_renderWindow->resetView(); });
    m_menuBar->setFlushStateCallBack([this]() { m_bottomBar->flushState(); });

    QObject::connect(m_pageTab, &QTabBar::currentChanged, m_pageStack, &QStackedWidget::setCurrentIndex);

    m_sideBar->setRenderFrameCallBack(renderFrameFunc);

    m_bottomBar->setRenderFrameCallBack(renderFrameFunc);
    m_bottomBar->setSaveFrameCallBack([this]() { m_menuBar->saveFrame(); });
    m_bottomBar->setShowProfilerCallBack([this]() { m_bottomBar->showProfiler(m_renderWindow->profiler_info()); });
    m_bottomBar->setPickPixelCallBack([this](uint32_t x, uint32_t y) { m_bottomBar->showPickInfo(m_renderWindow->pickPixel(x, y)); });
}
