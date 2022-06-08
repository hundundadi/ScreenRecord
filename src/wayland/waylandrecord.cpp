#include "waylandrecord.h"
#include "kwayland/waylandintegration.h"

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

}

void WaylandRecord::initWaylandRecord()
{
    //connect(this,SIGNAL(signalsStartRecord(QString)),this,SLOT(onStartRecord(QString)));
    QStringList arguments;
    arguments << QString("%1").arg(1);
    arguments << m_winArg[3] << m_winArg[4]; //width,height
    arguments << m_winArg[1] << m_winArg[2]; //x,y
    arguments << QString("%1").arg(24);
    arguments << m_winArg[5];
    arguments << QString("%1").arg(1);
    WaylandIntegration::init(arguments);
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
