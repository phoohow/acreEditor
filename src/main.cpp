#include <ui/renderWindow.h>
#include <ui/menuBar.h>
#include <ui/sideBar.h>
#include <ui/bottomBar.h>

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
        statusBar->showMessage((std::string("Saved frame succeed: ") + fileName).c_str(), 5000);
    };

    // MenuBar
    auto menuBar = new MenuBar(window->getScene(), mainWindow);
    mainWindow->setMenuBar(menuBar->getMenuBar());
    menuBar->setFlushFrameCallBack(flushFrameFunc);
    menuBar->setSaveFrameCallBack(saveFrameFunc);

    // SideBar
    auto sideBar = new SideBar(window->getScene());
    sideBar->setFlushFrameCallBack(flushFrameFunc);

    // BottomBar
    auto bottomBar = new BottomBar(window->getScene());
    bottomBar->setFlushFrameCallBack(flushFrameFunc);
    auto flushStateFunc = [bottomBar]() {
        bottomBar->flushState();
    };
    menuBar->setFlushStateCallBack(flushStateFunc);

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