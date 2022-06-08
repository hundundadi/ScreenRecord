#include "x11record.h"
#include <QThread>
X11Record::X11Record()
{
}

void X11Record::initX11Record()
{
    m_recordArg << getRecordArea();
    m_recordArg << getRecordFrames();
    m_recordArg << "videoconvert";
    m_recordArg << "videorate";
    m_recordArg << "queue max-size-bytes=1073741824 max-size-time=10000000000 max-size-buffers=1000";
    m_recordArg << getRecordEncoder();
    m_recordArg << "queue";
    m_recordArg << "mux.";

//    m_recordArg << "pulsesrc device=alsa_input.usb-SHENZHEN_AONI_ELECTRONIC_CO._LTD_Full_HD_webcam_20200406001-02.analog-mono";
//    m_recordArg << "audio/x-raw, channels=2";
//    m_recordArg << "queue";
//    m_recordArg << "mix.";

//    m_recordArg << "pulsesrc device=alsa_output.pci-0000_00_14.0.analog-stereo.monitor";
    m_recordArg << "pulsesrc device=alsa_input.usb-Generic_USB2.0_PC_CAMERA-02.analog-mono";
    m_recordArg << "audio/x-raw, channels=2";
    m_recordArg << "queue";
    m_recordArg << "mix.";

//    m_recordArg << "pulsesrc device=alsa_input.pci-0000_00_14.0.analog-stereo";
    m_recordArg << "pulsesrc device=alsa_output.pci-0000_00_1f.3.analog-stereo";
    m_recordArg << "audio/x-raw, channels=2";
    m_recordArg << "queue";
    m_recordArg << "mix.";




    //可以录到声音
//    //arguments << "pulsesrc device=alsa_input.usb-Generic_USB2.0_PC_CAMERA-02.analog-mono";
//    arguments << "pulsesrc device=alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! vorbisenc ! queue ! mux.";

//    arguments << "pulsesrc device=alsa_input.usb-Generic_USB2.0_PC_CAMERA-02.analog-mono";
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2";
//    arguments << "queue ! mix.";

//    arguments << "pulsesrc device=alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix.";

    //可以录到声音
//    arguments << "alsasrc device=front:0"; //系统音频
//    arguments << "alsasrc device=default"; //麦克风
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! vorbisenc ! queue ! mux.";

//    arguments << "alsasrc device=default"; //麦克风
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix.";

//    arguments << "alsasrc device=front:0"; //系统音频
//    arguments << "queue ! audioconvert ! audioresample";
//    arguments << "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix.";

    m_recordArg << "audiomixer name=mix";
    m_recordArg << "audioconvert";
    m_recordArg << "audiorate";
    m_recordArg << "queue";
    m_recordArg << "vorbisenc";
    m_recordArg << "queue";
    m_recordArg << "mux.";

    m_recordArg << getRecordMuxer();
    m_recordArg.removeAll("");
    m_recordArg << QString("filesink location=%1").arg(m_winArg[5]);
    QString recordArg = m_recordArg.join(" ! ");
    recordArg = recordArg.replace("mix. !", "mix.");
    recordArg = recordArg.replace("mux. !", "mux.");

    qDebug() << "录屏命令： " << recordArg;
    qDebug().noquote() << Pipeline_structured_output(recordArg);

    QByteArray byteArray = recordArg.toUtf8();
    const gchar *line = byteArray.constData();
    GError *error = Q_NULLPTR;
    pipeline = gst_parse_launch(line, &error);

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qDebug() << "Unable to set the pipeline to the playing state.";
        gst_object_unref(pipeline);
        return;
    }

    //emit signal_newVideoFilename(newVideoFilename);
}

void X11Record::startRecord()
{
    qDebug() << "x11录屏 开始！";
    initX11Record();
}

void X11Record::setRecordWinArg(QStringList widArg)
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

void X11Record::stopRecord()
{
    qDebug() << "x11录屏 结束！";
    bool a = gst_element_send_event(pipeline, gst_event_new_eos());
    Q_UNUSED(a);

    GstClockTime timeout = 5 * GST_SECOND;
    GstMessage *msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipeline), timeout, GST_MESSAGE_EOS);
    Q_UNUSED(msg);

    GstStateChangeReturn ret ;
    Q_UNUSED(ret);
    ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
    Q_UNUSED(ret);
    ret = gst_element_set_state(pipeline, GST_STATE_READY);
    Q_UNUSED(ret);
    ret = gst_element_set_state(pipeline, GST_STATE_NULL);
    Q_UNUSED(ret);
    gst_object_unref(pipeline);
    qDebug().noquote() << "Stop record";

    qDebug() << "x11录屏 完成！";
}

QString X11Record::getRecordArea()
{
    QString value = "";
    QStringList stringList;
    stringList << "ximagesrc"
               << "display-name=" + qgetenv("DISPLAY")
               << "use-damage=false"
               << "show-pointer=true"     //是否录制光标
               << "startx=" + m_winArg[1]
               << "starty=" + m_winArg[2]
               << "endx=" + m_winArg[3]
               << "endy=" + m_winArg[4];

    value = stringList.join(" ");
    return value;
}

QString X11Record::getRecordFrames()
{
    QStringList stringList;
    stringList << "video/x-raw, framerate="
               << QString::number(24)
               << "/1";
    return QString(stringList.join(""));
}

QString X11Record::getRecordEncoder()
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

QString X11Record::getRecordAudio()
{

    return "";
}

QString X11Record::getRecordMuxer()
{
    //QString value = "qtmux name=mux";
    QString value = "webmmux name=mux";

    return value;
}
QString X11Record::Pipeline_structured_output(QString pipeline)
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

