#include "waylandrecord.h"
#include "waylandintegration.h"

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QtConcurrent>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
WaylandRecord::WaylandRecord()
{
    initVariable();
}

void WaylandRecord::startRecord()
{

    //初始化kwayland相关的画面帧获取
    initWaylandRecord();
}



void WaylandRecord::stopRecord()
{
    qDebug() << "wayland录屏 结束！";

//    // send EOS to pipeline
//    gst_element_send_event(pipeline, gst_event_new_eos());

//    // wait for the EOS to traverse the pipeline and is reported to the bus
//    GstBus *bus = gst_element_get_bus(pipeline);
//    gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS);
//    gst_object_unref(bus);

//    gst_element_set_state(pipeline, GST_STATE_NULL);
//    gst_object_unref(pipeline);

    WaylandIntegration::stopStreaming();
    qDebug() << "wayland录屏 完成！";

}

void WaylandRecord::setRecordWinArg(QStringList widArg)
{
    if (widArg.isEmpty()) {
        m_winArg << QString::number(0);
        m_winArg << QString::number(0);
        m_winArg << QString::number(0);
        m_winArg << QString::number(1920);
        m_winArg << QString::number(1080);
    } else {
        m_winArg = widArg;
    }
}

void WaylandRecord::initVariable()
{
    m_fd = "";
    m_path = "";
    pipeline = nullptr;
    m_testFilePaht = "";
    m_tempFlag = false;

    m_writeFrameThread = nullptr;
}

void WaylandRecord::initWaylandRecord()
{
    //connect(this,SIGNAL(signalsStartRecord(QString)),this,SLOT(onStartRecord(QString)));
    QStringList arguments;
    arguments << QString("%1").arg(1);
    arguments << m_winArg[3] << m_winArg[4]; //width,height
    arguments << m_winArg[1] << m_winArg[2]; //x,y
    arguments << QString("%1").arg(24);
    arguments << "/tmp/test/wayland_test.mov";
    arguments << QString("%1").arg(1);
    WaylandIntegration::init(arguments);
    //checkTempFileArm();
}

void WaylandRecord::checkTempFileArm()
{
    qDebug() << __LINE__ << " WaylandRecord::checkTempFileArm";
    m_testFilePaht = "/tmp/test/";
    //判断文件是否存在，若存在则先删除文件，启动录屏时不应该存在该文件
    std::string tempFile = m_testFilePaht + "1.txt";
    tempFile += "1.txt";
    QFile mFile(tempFile.c_str());
    if (mFile.exists()) {
        remove(tempFile.c_str());
    }
    QtConcurrent::run(this, &WaylandRecord::whileCheckTempFileArm);
}
void WaylandRecord::whileCheckTempFileArm()
{
    qDebug() << __LINE__ << " WaylandRecord::checkTempFileArm";
    m_tempFlag = true;

    while (m_tempFlag) {
        QDir tdir(m_testFilePaht.c_str());
        //判断文件夹路径是否存在
        if (tdir.exists()) {
            std::string tempFile = m_testFilePaht + "1.txt";
            //打开文件
            int fd = open(tempFile.c_str(), O_RDWR, 0644);
            if (fd == -1) {
                qDebug() << "open file fail!" << strerror(errno);
                ::close(fd);
                if (m_tempFlag) {
                    QThread::msleep(500);
                }
                continue;
            }
            //文件加锁
            int flock = lockf(fd, F_TLOCK, 0);
            if (flock == -1) {
                qDebug() << "lock file fail!" << strerror(errno);
                ::close(fd);
                if (m_tempFlag) {
                    QThread::msleep(500);
                }
                continue;
            }
            ssize_t ret = -1;
            char rBuffer[2];
            memset(rBuffer, 0, 2);
            //读文件
            ret = read(fd, rBuffer, 2);
            if (ret < 0) {
                qDebug() << "read file fail!";
            } else {
                qDebug() << "file: " << rBuffer;
                emit signalsStartRecord(QString(rBuffer));
                m_tempFlag = false;

                //                if (QString(rBuffer).toInt() == 1) {
                //                    qDebug() << "read file to stop Record!" ;
                //                    //stopRecord();
                //                    emit signalsStartRecord(QString(rBuffer));
                //                    tempFlag = false;
                //                } else {
                //                    qDebug() << "file: " << rBuffer;
                //                }
            }
            //文件解锁
            flock = lockf(fd, F_ULOCK, 0);
            ::close(fd);
            qDebug() << "close file!";
            //移除文件
            remove(tempFile.c_str());
            qDebug() << "remove file!";
        }
    }
}

void WaylandRecord::onStartRecord(QString fd)
{
    QStringList stringList;
    //    stringList << QString("pipewiresrc fd=").append(m_fd).append(" path=").append(m_path).append(" do-timestamp=true");
    stringList << QString("pipewiresrc ").append(" path=").append(fd).append(" do-timestamp=true");
    stringList << "videoconvert";
    stringList << "videorate";
    stringList << getRecordArea();
    stringList << "video/x-raw, framerate=24/1";
    stringList << getRecordEncoder();
    stringList << getRecordMuxer();
    stringList << "filesink location=/tmp/test/wayland_test.mov";
    QString launch = stringList.join(" ! ");

    qDebug() << "录屏命令： " << launch;
    qDebug() << " ";
    qDebug().noquote() << Pipeline_structured_output(launch);
    qDebug() << " ";

    pipeline = gst_parse_launch(launch.toUtf8(), nullptr);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}
QString WaylandRecord::getRecordArea()
{
    QString videocrop = "";
    videocrop = "videocrop top=" + m_winArg[2] + " " +
            "right=" + m_winArg[1] + " " +
            "bottom=" + m_winArg[4] + " " +
            "left=" + m_winArg[3];
    return videocrop;
}
QString WaylandRecord::getRecordEncoder()
{
    QString value = "";
    QStringList list;
    list << "vp8enc";
    list << "min_quantizer=20";
    list << "max_quantizer=20";
    list << "cpu-used=" + QString::number(QThread::idealThreadCount());
    list << "deadline=1000000";
    list << "threads=" + QString::number(QThread::idealThreadCount());
    value = list.join(" ");
    return value;

}
QString WaylandRecord::getRecordMuxer()
{
    QString value = "qtmux name=mux";

    return value;
}
QString WaylandRecord::Pipeline_structured_output(QString pipeline)
{
    QString string;
    QString nl;
    nl = "\\";
    string = pipeline.prepend("gst-launch-1.0 -e " + nl + "\n    ");
    string = pipeline.replace("mux.", "mux. " + nl + "\n   ");
    string = pipeline.replace("mix.", "mix. " + nl + "\n   ");
    string = pipeline.replace("!", nl + "\n        !");
    string.append("\n");
    return string;
}
