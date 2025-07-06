#include <view/mainWindow.h>
#include <QApplication>

int runEditor(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow   mainWindow;
    mainWindow.show();
    return app.exec();
}

int main(int argc, char* argv[])
{
    return runEditor(argc, argv);
}