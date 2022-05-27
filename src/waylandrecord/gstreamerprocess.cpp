#include "gstreamerprocess.h"
#include <QDebug>
extern "C" {
#include <gst/app/gstappsrc.h>
}
#define GST_CAT_DEFAULT appsrc_pipeline_debug_1
GST_DEBUG_CATEGORY(appsrc_pipeline_debug_1);

static gboolean
bus_message(GstBus *bus, GstMessage *message, AppSrcOption *app)
{
    GST_DEBUG("got message %s",
              gst_message_type_get_name(GST_MESSAGE_TYPE(message)));

    switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR: {
        GError *err = NULL;
        gchar *dbg_info = NULL;

        gst_message_parse_error(message, &err, &dbg_info);

        gchar *elename = GST_OBJECT_NAME(message->src);

        g_printerr("ERROR from element %s: %s\n",
                   elename, err->message);
        g_printerr("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");

        //app->errCallback(-90001, err->message, app->callbackData);

        g_error_free(err);
        g_free(dbg_info);
        g_main_loop_quit(app->gloop);
        break;
    }
    case GST_MESSAGE_EOS: {
        g_main_loop_quit(app->gloop);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

GStreamProcess::GStreamProcess()
{
    app = nullptr;
//    m_pipeline = nullptr ;
//    m_appsrc = nullptr ;
//    m_videoconvert = nullptr ;
//    m_vp8enc = nullptr ;
//    m_webmmux = nullptr ;
//    m_filesink = nullptr;
//    m_bus = nullptr;
    m_width = 0;
    m_height = 0;
}

void GStreamProcess::run()
{
    qDebug() << __LINE__ << " GStreamProcess::init";
    GST_DEBUG_CATEGORY_INIT(appsrc_pipeline_debug_1, "appsrc-pipeline", 0,
                            "appsrc pipeline example");

    GstElement *m_pipeline, *queue;
    GstElement *m_appsrc, *m_videoconvert, *m_vp8enc, *m_webmmux, *m_filesink;
    int argc = 1;
    char *mock[1] = {"empty"};
    char **argv[1];
    *argv = mock;

    gst_init(&argc, argv);
    app->gloop = g_main_loop_new(NULL, TRUE);

    m_pipeline = gst_pipeline_new("pipeline_test");
    m_appsrc = gst_element_factory_make("appsrc", "source_test");
    m_videoconvert = gst_element_factory_make("videoconvert", "convert_test");
    //    m_vp8enc = gst_element_factory_make("vp8enc", "encoder_test");
    m_vp8enc = gst_element_factory_make("x264enc", "encoder_test");
    m_filesink = gst_element_factory_make("filesink", "sink_test");
//    m_webmmux = gst_element_factory_make("qtmux", "codec_test");
    //    m_webmmux = gst_element_factory_make("webmmux", "codec_test");

    m_webmmux = gst_element_factory_make("flvmux", "codec_test");
    queue = gst_element_factory_make("queue", "queue_test");

    g_object_set(m_filesink, "location", "/home/uos/Desktop/test/wayland_123.flv");

    GstMessage *msg;

    app->bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    msg = gst_bus_timed_pop_filtered(app->bus, 1000, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            /* We should not reach here because we only asked for ERRORs and EOS */
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

    /* 设置 screen src 属性 */
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "RGBA",
                                        "width", G_TYPE_INT, 1000,
                                        "height", G_TYPE_INT, 800,
                                        "framerate", GST_TYPE_FRACTION, 24, 1,
                                        NULL);

    gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);

    g_object_set(m_appsrc, "format", GST_FORMAT_TIME, NULL);

    gst_bin_add_many(GST_BIN(m_pipeline), m_appsrc, m_videoconvert, queue, m_vp8enc, m_webmmux, m_filesink, NULL);

    gst_element_link_many(m_appsrc, m_videoconvert, queue, m_vp8enc, m_webmmux, m_filesink, NULL);

    app->pipeline = m_pipeline;
    app->shareScreenAppsrc = m_appsrc;
    /* go to playing */
    gst_element_set_state(app->pipeline, GST_STATE_PLAYING);

    //GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline_dot");

    g_main_loop_run(app->gloop);
    qDebug() << "============stopping============";

    gst_element_set_state(app->pipeline, GST_STATE_NULL);

    gst_object_unref(app->bus);
    g_main_loop_unref(app->gloop);
}

//void GStreamProcess::run()
//{
//    GError *error = NULL;

//    int argc = 1;
//    char *mock[1] = {"empty"};
//    char **argv[1];
//    *argv = mock;

//    gst_init(&argc, argv);

//    GST_DEBUG_CATEGORY_INIT(appsrc_pipeline_debug_1, "appsrc-pipeline", 0,
//        "appsrc pipeline example");

//    app->gloop = g_main_loop_new(NULL, TRUE);

//    //GstElement *pipeline = gst_parse_launch(" appsrc name=screen_src !videorate ! x264enc ! mux.video flvmux name=mux ! filesink name=file ", NULL);
////    m_pipeline = gst_parse_launch(" appsrc name=screen_src ! videoconvert !  videorate ! queue ! x264enc ! flvmux name=mux  ! filesink name=file", NULL);
//    m_pipeline = gst_parse_launch("flvmux name=mux appsrc name=screen_src ! queue  !  videorate ! x264enc ! mux.video ! queue ! filesink name=file", NULL);
//    g_assert(m_pipeline);

//    app->bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
//    g_assert(app->bus);

//    /* add watch for messages */
//    gst_bus_add_watch(app->bus, (GstBusFunc)bus_message, app);


//    /* 设置 screen src 属性 */

//    app->shareScreenAppsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "screen_src");
//    g_assert(app->shareScreenAppsrc);

//    GstCaps *caps = gst_caps_new_simple("video/x-raw",
//        "format", G_TYPE_STRING, "RGBA",
//        "width", G_TYPE_INT, 640,
//        "height", G_TYPE_INT, 480,
//        "framerate", GST_TYPE_FRACTION, 15, 1,
//        NULL);

//    gst_app_src_set_caps(GST_APP_SRC(app->shareScreenAppsrc), caps);

//    g_object_set(app->shareScreenAppsrc, "format", GST_FORMAT_TIME, NULL);
//    g_object_set(app->shareScreenAppsrc, "is-live", TRUE, NULL);


//    /* 设置 filesink 属性 */

//    GstElement *filesink = gst_bin_get_by_name(GST_BIN(m_pipeline), "file");
//    g_assert(filesink);

//    g_object_set(G_OBJECT(filesink), "location", "/tmp/test/wayland_test.flv", NULL);

//    qDebug() << "wayland录屏保存位置：/tmp/test/wayland_test.flv";
//    /* go to playing */
//    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

//    //GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline_dot");

//    app->pipeline = m_pipeline;

//    //app->sucCallback(app->callbackData);

//    qDebug() << "g_main_loop_run!" ;
//    g_main_loop_run(app->gloop);

//    GST_DEBUG("stopping");

//    gst_element_set_state(app->pipeline, GST_STATE_NULL);

//    gst_object_unref(app->bus);
//    g_main_loop_unref(app->gloop);
//}

void GStreamProcess::init()
{
    GstElement *m_pipeline, *queue;
    GstBus *m_bus;
    GstElement *m_appsrc, *m_videoconvert, *m_vp8enc, *m_webmmux, *m_filesink;
    m_pipeline = gst_pipeline_new("pipe");
    //     m_pipeline = gst_parse_launch("appsrc ! videoconvert ! queue ! vp8enc ! mux. alsasrc ! queue ! audioconvert ! audioresample ! vorbisenc ! mux. webmmux name=mux ! filesink location=test.mp4 sync=false", NULL);

    m_appsrc = gst_element_factory_make("appsrc", "source");
    //    GstCaps *caps = gst_caps_new_simple("video/x-raw",
    //                                        "format", G_TYPE_STRING, "RGBA",
    //                                        "width", G_TYPE_INT, m_width,
    //                                        "height", G_TYPE_INT, m_height,
    //                                        "framerate", GST_TYPE_FRACTION, 15, 1,
    //                                        NULL);
    //    gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);
    //    g_object_set(m_appsrc, "format", GST_FORMAT_TIME, NULL);
    //    g_object_set(m_appsrc, "is-live", TRUE, NULL);

    m_videoconvert = gst_element_factory_make("videoconvert", "convert");
    m_vp8enc = gst_element_factory_make("vp8enc", "encoder");
    m_filesink = gst_element_factory_make("filesink", "sink");
    g_object_set(m_filesink, "location", "/tmp/test/wayland_test.webm");
    GstElement *appsrc_queue = gst_element_factory_make("queue", "appsrc_queue");
    gst_bin_add_many(GST_BIN(m_pipeline), m_appsrc, appsrc_queue, m_videoconvert, m_vp8enc, m_filesink, NULL);
    gst_element_link_many(m_appsrc, m_videoconvert, appsrc_queue, m_vp8enc, m_filesink, NULL);

    GstMessage *msg;

    m_bus = gst_element_get_bus(m_pipeline);
    msg = gst_bus_timed_pop_filtered(m_bus, 1000, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    //app->pipeline = m_pipeline;
    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            /* We should not reach here because we only asked for ERRORs and EOS */
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

}

static int globalCount = 0;
void GStreamProcess::recordFrame(Global::waylandFrame frame)
{
    //    qDebug() << "frame._time: " << frame._time;
    int size = frame._width * frame._height * 3 ;
    if (app && app->pipeline) {
        GstBuffer *buffer;
        guint8 *ptr;
        ptr = (guint8 *)g_malloc(size * sizeof(uchar));
        if (NULL == ptr) {
            qDebug() << "失败!!!!!";
        } else {
            memcpy(ptr, frame._frame, size);
            buffer = gst_buffer_new_wrapped((void *)ptr, size);

            //if (!globalCount) {
            QImage *img = new QImage(frame._frame, frame._width, frame._height, QImage::Format::Format_RGBA8888);
            img->save(QString("/home/uos/Desktop/test/image/test_%1.png").arg(globalCount));
            //}
            qDebug() << " frame._width: " << frame._width << ",frame._height: " << frame._height;
            globalCount++;
            //设置时间戳
            GST_BUFFER_PTS(buffer) = timestamp;
            GST_BUFFER_DTS(buffer) = timestamp;

//            GST_BUFFER_PTS(buffer) = gst_clock_get_time(app->pipeline->clock) - app->pipeline->base_time;
//                        GST_BUFFER_DTS(buffer) = gst_clock_get_time(app->pipeline->clock) - app->pipeline->base_time;
//            buffer->pts = frame._time;
//            buffer->dts = frame._time;
            GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 24);
            timestamp += GST_BUFFER_DURATION(buffer);
            qDebug() << "buffer->pts: " << buffer->pts;
            qDebug() << "buffer->dts: " << buffer->dts;
            qDebug() << "buffer->duration: " << buffer->duration;

//            m_lastFramePts =  buffer->pts;
//            qDebug() << "app->shareScreenAppsrc: " << app->shareScreenAppsrc;
            GstFlowReturn ret;
            //注入视频帧数据
            g_signal_emit_by_name(app->shareScreenAppsrc, "push-buffer", buffer, &ret);

            //qDebug() << "ret: " << ret;

            gst_buffer_unref(buffer);
        }
    }
}

void GStreamProcess::stopRecordFrame()
{
    if (app && app->pipeline) {
        GstFlowReturn ret;
        g_signal_emit_by_name(app->shareScreenAppsrc, "end-of-stream", &ret);
        GST_DEBUG("stopping");
        qDebug() << "stopping!!!!";

        //        gst_element_set_state(app->pipeline, GST_STATE_NULL);

        //        gst_object_unref(app->bus);
        //        g_main_loop_unref(app->gloop);


        //        gst_object_unref(m_bus);

        //        gst_element_set_state(m_pipeline,GST_STATE_NULL);
        //        gst_object_unref(m_pipeline);
        //        g_signal_emit_by_name(app->sendAudioAppsrc, "end-of-stream", &ret);
        //        g_signal_emit_by_name(app->receiveAudioAppsrc, "end-of-stream", &ret);
    }
}
