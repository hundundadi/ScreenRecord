#pragma once
#include "../global.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/base/gstbaseparse.h>

#include <QString>
#include <QObject>
#include <QList>
#include <QMutex>
#include <QAudioBuffer>
#include <QAudioDeviceInfo>

class GstVideoWriter  : public QObject
{
    Q_OBJECT
public:
    GstVideoWriter(const QString &videoPath = "/home/uos/Desktop/test/wayland_123.webm");
    ~GstVideoWriter();

    void init();
    void start();
    void stop();
    void setVideoPath(const QString &videoPath);
    bool writeVideo(Global::waylandFrame frame);


    void setVideoArgs(Global::videoArgs videoArgs);
    void setAudioArgs(Global::audioArgs audioArgs);
    void objectUnref(gpointer object);

    GstElement *m_pipeline;
    GMainLoop *m_gloop;

    bool doBusMessage(GstMessage *message);
    bool doBufferProbe(GstBuffer *buffer);
    void initFormat();
    void setOutputDevice(QString device);
    void setInputDevice(QString device);
    void bufferProbed();
protected:
    void loadAppSrcCaps();
    bool createPipe1();
    void setStateToNull();

    static void print_pad_capabilities(GstElement *element, gchar *pad_name);
    static void print_caps(const GstCaps *caps, const gchar *pfx);
    static gboolean print_field(GQuark field, const GValue *value, gpointer pfx);
    gboolean link_pulse_element_with_filter(GstElement *element1, GstElement *element2);
signals:
    //录音过程中发生错误，发送错误信息
    void errorMsg(QString msg);

    void audioBufferProbed(const QAudioBuffer &buffer);
private:
    QString m_videoPath;

    Global::videoArgs m_videoArgs;
    Global::audioArgs m_audioArgs;

    GstElement *m_appsrc;
    GstElement *m_audsrc;

    GstElement *m_filesink;
    GstBus *m_bus;


    QString m_currentOutputDevice;
    QString m_currentInputDevice;

    QMutex m_bufferMutex;
    QAudioBuffer m_pendingBuffer;

    QAudioFormat m_format;

};
