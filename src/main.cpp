#include <view/renderWindow.h>
#include <view/menuBar.h>
#include <view/sideBar.h>
#include <view/bottomBar.h>

#include <QGuiApplication>
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QStatusBar>

#include <string>

int runEditor(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto mainWindow = new QMainWindow;
    mainWindow->resize(900, 900);

    // StatusBar
    auto statusBar = new QStatusBar(mainWindow);
    mainWindow->setStatusBar(statusBar);

    // Render window
    auto window    = new RenderWindow;
    auto container = QWidget::createWindowContainer(window);
    container->setStyleSheet("background-color: lightGreen;");

    auto renderFrameFunc = [window]() {
        window->renderFrame();
    };

    auto saveFrameFunc = [window, statusBar](const std::string& fileName) {
        statusBar->showMessage("Start saving frame ...", 0);
        window->saveFrame(fileName);
        statusBar->showMessage((std::string("Saved frame succeed: ") + fileName).c_str(), 10000);
    };

    // MenuBar
    auto menuBar = new MenuBar(window->getScene(), mainWindow);
    mainWindow->setMenuBar(menuBar->getMenuBar());
    menuBar->setRenderFrameCallBack(renderFrameFunc);
    menuBar->setSaveFrameCallBack(saveFrameFunc);
    menuBar->setResetViewCallBack([window]() { window->resetView(); });

    // SideBar
    auto sideBar = new SideBar(window->getScene());
    sideBar->setRenderFrameCallBack(renderFrameFunc);

    // BottomBar
    auto bottomBar = new BottomBar(window->getScene());
    bottomBar->setRenderFrameCallBack(renderFrameFunc);
    bottomBar->setSaveFrameCallBack([menuBar]() { menuBar->saveFrame(); });
    bottomBar->setShowProfilerCallBack([window, bottomBar]() {
        auto profiler = window->getProfiler();
        bottomBar->showProfiler(profiler);
    });
    bottomBar->setPickPixelCallBack([window, bottomBar](uint32_t x, uint32_t y) {
        auto info = window->pickPixel(x, y);
        bottomBar->showPickInfo(info);
    });
    menuBar->setFlushStateCallBack([bottomBar]() { bottomBar->flushState(); });

    auto hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(container);
    hSplitter->addWidget(sideBar);
    hSplitter->setSizes({700, 200});

    auto vSplitter = new QSplitter(Qt::Vertical);
    vSplitter->addWidget(hSplitter);
    vSplitter->addWidget(bottomBar);
    vSplitter->setSizes({700, 200});

    mainWindow->setCentralWidget(vSplitter);
    mainWindow->show();

    return app.exec();
}

int main(int argc, char* argv[])
{
    return runEditor(argc, argv);
}