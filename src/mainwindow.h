#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QDebug>
#include <QDir>
#include <QLibraryInfo>
#include "utils.h"
#include "waylandrecord/waylandrecord.h"
#include "x11/x11record.h"

extern "C" {
#include "waylandrecord/load_libs.h"
}

#include <DPushButton>
#include <QObject>

DWIDGET_USE_NAMESPACE
class MainWindow : public DWidget
{
    Q_OBJECT
public:
    MainWindow();

    void initDynamicLibPath();
    QString libPath(const QString &strlib);
private:
    void initMainWindow();

public slots:
    void onStartRecord();
    void onStopRecord();
private:
    DPushButton *m_startBtn;
    DPushButton *m_stopBtn;

    WaylandRecord *m_waylandRecord;

    X11Record *m_x11Record;


};

#endif // MAINWINDOW_H
