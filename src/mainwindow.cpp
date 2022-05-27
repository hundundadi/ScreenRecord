#include "mainwindow.h"

MainWindow::MainWindow()
{
    initMainWindow();
}


void MainWindow::initMainWindow()
{
    setWindowTitle(tr("Screen Capture"));

    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);   // make MouseMove can response
    installEventFilter(this);  // add event filter

    m_startBtn = new DPushButton(this);
    m_startBtn->setText("start");
    m_startBtn->move(10, 5);
    m_startBtn->show();
    m_startBtn->setAttribute(Qt::WA_TranslucentBackground, false);
    connect(m_startBtn, SIGNAL(clicked()), this, SLOT(onStartRecord()));
    m_stopBtn = new DPushButton(this);
    m_stopBtn->setText("stop");
    m_stopBtn->move(80, 5);
    m_stopBtn->show();
    m_stopBtn->setAttribute(Qt::WA_TranslucentBackground, false);
    connect(m_stopBtn, SIGNAL(clicked()), this, SLOT(onStopRecord()));
    Utils::getInputEvent(
        static_cast<int>(this->winId()),
        static_cast<short>(this->x()),
        static_cast<short>(this->y() + 5 + m_stopBtn->height()),
        static_cast<unsigned short>(this->width()),
        static_cast<unsigned short>(this->height() - 5 - m_stopBtn->height()));

    m_waylandRecord = nullptr;
    m_x11Record = nullptr;
    //qDebug() << Utils::getCpuModelName();
}
void MainWindow::initDynamicLibPath()
{
    LoadLibNames tmp;
    QByteArray portaudio = libPath("libportaudio.so").toLatin1();
    tmp.chPortaudio = portaudio.data();
    setLibNames(tmp);
}
QString MainWindow::libPath(const QString &strlib)
{
    QDir  dir;
    QString path  = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    dir.setPath(path);
    QStringList list = dir.entryList(QStringList() << (strlib + "*"), QDir::NoDotAndDotDot | QDir::Files); //filter name with strlib

    if (list.contains(strlib))
        return strlib;

    list.sort();
    Q_ASSERT(list.size() > 0);
    return list.last();
}
void MainWindow::onStartRecord()
{
    qDebug() << QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << "开始录屏！";

    m_startBtn->setEnabled(false);
    m_startBtn->setCheckable(false);
    QStringList winArg;
    winArg << QString::number(this->winId());
    winArg << QString::number(this->x());
    winArg << QString::number(this->y());
    winArg << QString::number(this->x() + this->width());
    winArg << QString::number(this->y() + this->height());
    if (Utils::isWaylandMode) {
        initDynamicLibPath();
        m_waylandRecord = new WaylandRecord();
        m_waylandRecord->setRecordWinArg(winArg);
//        m_waylandRecord->initWaylandRecord();
        m_waylandRecord->startRecord();
    } else {
        m_x11Record = new X11Record();
        m_x11Record->setRecordWinArg(winArg);
        m_x11Record->startRecord();
    }

}


void MainWindow::onStopRecord()
{
    qDebug() <<  QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << "结束录屏！";
    if (Utils::isWaylandMode) {
        m_waylandRecord->stopRecord();
    } else {
        m_x11Record->stopRecord();
    }
    m_stopBtn->setEnabled(false);
    //close();
}
