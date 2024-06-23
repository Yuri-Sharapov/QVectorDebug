#include <QApplication>
#include <QStyleFactory>

#include "platform.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    int result = 0;
    do
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.setWindowTitle("QVectorDebug");
        w.show();

        result = a.exec();
    } while (result == APP_RESTART_CODE);
    
    return result;
}
