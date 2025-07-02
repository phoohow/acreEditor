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

    auto flushFrameFunc = [window]() {
        window->render();
    };

    auto saveFrameFunc = [window, statusBar](const std::string& fileName) {
        statusBar->showMessage("Start saving frame ...", 0);
        window->saveFrame(fileName);
        statusBar->showMessage((std::string("Saved frame succeed: ") + fileName).c_str(), 10000);
    };

    // MenuBar
    auto menuBar = new MenuBar(window->getScene(), mainWindow);
    mainWindow->setMenuBar(menuBar->getMenuBar());
    menuBar->setFlushFrameCallBack(flushFrameFunc);
    menuBar->setSaveFrameCallBack(saveFrameFunc);
    menuBar->setResetViewCallBack([window]() { window->resetView(); });

    // SideBar
    auto sideBar = new SideBar(window->getScene());
    sideBar->setFlushFrameCallBack(flushFrameFunc);

    // BottomBar
    auto bottomBar = new BottomBar(window->getScene());
    bottomBar->setFlushFrameCallBack(flushFrameFunc);
    auto showProfilerFunc = [window, bottomBar]() {
        auto profiler = window->getProfiler();
        bottomBar->showProfiler(profiler);
    };
    bottomBar->setShowProfilerCallback(showProfilerFunc);
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