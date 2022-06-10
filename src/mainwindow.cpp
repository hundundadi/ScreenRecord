#include "mainwindow.h"
#include <QStandardPaths>

// system
// system
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
MainWindow::MainWindow()
{
    initMemberVariables();
    initMainWindow();
}

MainWindow::~MainWindow()
{

    releaseMemberVariables();
}

void MainWindow::initMemberVariables()
{
    m_startBtn = nullptr;
    m_stopBtn = nullptr;
    saveBaseName = "";
    savePath = "";
    m_framerate = 0;
    m_recordRect = QRect(0, 0, 0, 0);
    recordAudioInputType = 0;
    m_isRecordMouse = false;
    m_gstRecordX = nullptr;
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
}



void MainWindow::releaseMemberVariables()
{
    if (m_gstRecordX) {
        delete m_gstRecordX;
        m_gstRecordX = nullptr;
    }
}

void MainWindow::onStartRecord()
{
    qDebug() << QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << "开始录屏！";

    m_startBtn->setEnabled(false);
    m_startBtn->setCheckable(false);

    m_recordRect = QRect(this->x(), this->y(), this->x() + this->width(), this->y() + this->height());
    m_framerate = 24;
    recordAudioInputType = 3;
    Utils::isPipewireMode = true;
    GstStartRecord();

}
void MainWindow::onStopRecord()
{
    qDebug() <<  QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << "结束录屏！";
    GstStopRecord();
}


void MainWindow::GstStartRecord()
{
    int argc = 1;
//    char *mock[1] = {QString("empty").toLatin1().data()};
//    char **argv[1];
//    *argv = mock;
    //gstreamer接口初始化
    gst_init(&argc, nullptr);
    qDebug() << "Gstreamer 录屏开始！";
    GstRecordX::VideoType videoType = GstRecordX::VideoType::webm;
    GstRecordX::AudioType audioType = GstRecordX::AudioType::NoVoice;
    m_gstRecordX = new GstRecordX(this);
    //设置参数
    m_gstRecordX->setFramerate(m_framerate);
    m_gstRecordX->setRecordArea(m_recordRect);
    //这里设置音频设备名称（输入和输出），即使名称为空也不影响。
    AudioUtils audioUtils;
    m_gstRecordX->setInputDeviceName(audioUtils.getDefaultDeviceName(AudioUtils::DefaultAudioType::Source));
    m_gstRecordX->setOutputDeviceName(audioUtils.getDefaultDeviceName(AudioUtils::DefaultAudioType::Sink));
    //这里才会设置究竟采集哪些音频设备的音频数据
    if (recordAudioInputType == 3) {
        audioType =  GstRecordX::AudioType::Mix;
    } else if (recordAudioInputType == 1) {
        audioType =  GstRecordX::AudioType::Mic;
    } else if (recordAudioInputType == 2) {
        audioType =  GstRecordX::AudioType::Sys;
    }
    m_gstRecordX->setAudioType(audioType);
    QDateTime date = QDateTime::currentDateTime();
    QString fileExtension = "webm";
    videoType =  GstRecordX::VideoType::webm;
    fileExtension = "webm";

    saveBaseName = QString("%1_%3.%4").arg("录屏").arg(date.toString("yyyyMMddhhmmss")).arg(fileExtension);
    savePath = QDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first()).filePath(saveBaseName);
    // Remove same cache file first.
    QFile file(savePath);
    file.remove();
    m_gstRecordX->setVidoeType(videoType);
    m_gstRecordX->setSavePath(savePath);
    m_gstRecordX->setX11RecordMouse(m_isRecordMouse);
    //开始录制
    if (Utils::isWaylandMode) {
        if(!Utils::isPipewireMode){
            //wayland下停止录屏需要通过信号槽控制，避免Gstreamer管道数据未写完程序就被退出了
    //        connect(m_gstRecordX, &GstRecordX::waylandGstRecrodFinish, this, &RecordProcess::onExitGstRecord);
            //wayland模式需打开视频画面采集
            QStringList arguments;
            arguments << QString("%1").arg(videoType);
            arguments << QString("%1").arg(m_recordRect.width()) << QString("%1").arg(m_recordRect.height());
            arguments << QString("%1").arg(m_recordRect.x()) << QString("%1").arg(m_recordRect.y());
            arguments << QString("%1").arg(m_framerate);
            arguments << QString("%1").arg(savePath);
            arguments << QString("%1").arg(recordAudioInputType);
            qDebug() << arguments;
            WaylandIntegration::init(arguments, m_gstRecordX);
        }else{
            qDebug() << "pipewire录屏!!!";
            portal_wl_one = new Portal_wl("1111111");
            portal_wl_one->setRequestToken(1);
            portal_wl_one->setSessionToken(1);
//            portal_wl_two = new Portal_wl("2222222");
//            portal_wl_two->setRequestToken(2);
//            portal_wl_two->setSessionToken(2);
            connect( portal_wl_one, SIGNAL( signal_portal_fd_path( QString, QString ) ), m_gstRecordX, SLOT( slot_start_gst( QString, QString ) ) );
            connect( portal_wl_one, SIGNAL( signal_portal_fd_path( QString, QString ,QString) ), m_gstRecordX, SLOT( slot_start_gst( QString, QString, QString) ) );
//            connect( portal_wl_one, SIGNAL( signal_portal_fd_path( QString, QString ) ), this, SLOT( saveImage1( QString, QString ) ) );
//            connect( portal_wl_two, SIGNAL( signal_portal_fd_path( QString, QString ) ), this, SLOT( saveImage2( QString, QString ) ) );
//            portal_wl_one->requestScreenSharing(1,m_isRecordMouse? 1:2);
//            portal_wl_two->requestScreenSharing(1,m_isRecordMouse? 1:2);

            QtConcurrent::run(this,&MainWindow::createScreenRecord1);
//            QtConcurrent::run(this,&MainWindow::createScreenRecord2);
        }
    } else {
        m_gstRecordX->x11GstStartRecord();
    }
}

void MainWindow::createScreenRecord1(){
    qDebug() << "创建屏幕共享1 ！！！！";
    portal_wl_one->requestScreenSharing(0,m_isRecordMouse? 1:2);

}
void MainWindow::createScreenRecord2(){
    qDebug() << "创建屏幕共享2 ！！！！";
    portal_wl_two->requestScreenSharing(1,m_isRecordMouse? 1:2);

}

void MainWindow::saveImage1(QString dma_fd, QString path)
{
    qDebug() << __FUNCTION__ << "dma_fd: " << dma_fd << " , path: " << path;
   }

void MainWindow::saveImage2(QString dma_fd, QString path)
{
    qDebug() << __FUNCTION__ << "dma_fd: " << dma_fd << " , path: " << path;
   }
//gstreamer停止录制视频
void MainWindow::GstStopRecord()
{
    if (Utils::isWaylandMode) {
        if(!Utils::isPipewireMode){

        WaylandIntegration::stopStreaming();
        }
        else {
            m_gstRecordX->x11GstStopRecord();

        }
    } else {
        //x11 gstreamer录屏
        m_gstRecordX->x11GstStopRecord();
    }
}
