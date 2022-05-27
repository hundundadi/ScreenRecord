#pragma once
#include "../global.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/base/gstbaseparse.h>

#include <QString>
#include <QThread>
#include <QImage>

typedef struct _tagAppSrcOption {
    _tagAppSrcOption()
        : pipeline(nullptr)
        , shareScreenAppsrc(nullptr)
        , sendAudioAppsrc(nullptr)
        , bus(nullptr)
        , gloop(nullptr)
        , width(0)
        , height(0)
    {}

    GstElement *pipeline;
    GstElement *shareScreenAppsrc;
    GstElement *sendAudioAppsrc;
    GstElement *receiveAudioAppsrc;
    GstElement *rtmp;
    GstBus *bus;
    GMainLoop *gloop;

    QString recordFileName;

//    iLiveSucCallback sucCallback;
//    iLiveErrCallback errCallback;
    void *callbackData;

    uint width;
    uint height;

} AppSrcOption;

//int gstreamerInit(AppSrcOption *app, int argc, char *argv[]);


class GStreamProcess : public QThread
{
    Q_OBJECT
public:
    GStreamProcess();
    void run() Q_DECL_OVERRIDE;

    /**
     * @brief 初始化
     */
    void init();


    void recordFrame(Global::waylandFrame frame);

    void stopRecordFrame();

public:
    AppSrcOption *app;
    int m_width;
    int m_height;
private:
//    GstElement *m_pipeline;
//    GstElement *m_appsrc, *m_videoconvert, *m_vp8enc, *m_webmmux, *m_filesink;
//    GstBus *m_bus;

    GstClockTime timestamp = 0;

signals:
    void resultReady(const QString &s);

private:
    GstClockTime m_lastFramePts = 0;
};
