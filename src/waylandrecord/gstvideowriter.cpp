#include "gstvideowriter.h"
extern "C" {
#include <gst/app/gstappsrc.h>
}

#include <QtConcurrent>
#include <QImage>
#include <QString>

GST_DEBUG_CATEGORY(appsrc_pipeline_debug);

static const QString audioEncoderStr = "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! lamemp3enc name=enc target=1 cbr=true bitrate=192";;
//static const QString audioEncoderStr = "capsfilter caps=audio/x-raw,rate=44100,channels=2 ! vorbisenc name=enc bitrate=6000";;


static gboolean
bus_message(GstBus *bus, GstMessage   *message, GstVideoWriter *app)
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

        g_error_free(err);
        g_free(dbg_info);

        // 结束录制任务
        g_main_loop_quit(app->m_gloop);

        // 停止管道
        gst_element_set_state(app->m_pipeline, GST_STATE_NULL);
        break;
    }
    case GST_MESSAGE_EOS: {
        // 结束录制任务
        g_main_loop_quit(app->m_gloop);
        // 停止管道
        gst_element_set_state(app->m_pipeline, GST_STATE_NULL);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

/**
 * @brief bufferProbe
 * @param pad
 * @param info
 * @param user_data 用户数据
 * @return 处理结果
 */
GstPadProbeReturn bufferProbe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    Q_UNUSED(pad);
    GstVideoWriter *recorder = static_cast<GstVideoWriter *>(user_data);
    if (GstBuffer *buffer = gst_pad_probe_info_get_buffer(info))
        return recorder->doBufferProbe(buffer) ? GST_PAD_PROBE_OK : GST_PAD_PROBE_DROP;
    return GST_PAD_PROBE_OK;
}

GstVideoWriter::GstVideoWriter(const QString &videoPath)
{
    m_videoPath = videoPath;
    m_pipeline = nullptr;
    m_gloop = nullptr;
    m_appsrc = nullptr;
    m_audsrc = nullptr;
    m_bus = nullptr;

}

GstVideoWriter::~GstVideoWriter()
{
    gst_object_unref(m_pipeline);
    gst_object_unref(m_appsrc);
    gst_object_unref(m_audsrc);
    gst_object_unref(m_filesink);
    gst_object_unref(m_bus);
}

void GstVideoWriter::start()
{
    qDebug() << "启动管道!";
    // 加载注入gstreamer的帧数据格式信息
//    loadAppSrcCaps();
    if (!m_format.isValid()) {
        initFormat();
    }
    // 启动管道
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    qDebug() << "启动管道!2";
    // 将录制任务放到线程中运行，bus回调收到eos信号，结束录制任务
    QtConcurrent::run(g_main_loop_run, m_gloop);
}

void GstVideoWriter::stop()
{
    GstFlowReturn ret;

//    if (m_audsrc)
//        g_signal_emit_by_name(m_audsrc, "end-of-stream", &ret);
//    if (m_appsrc)
//        g_signal_emit_by_name(m_appsrc, "end-of-stream", &ret);


        if (m_pipeline) {
            setStateToNull();
        }
}
/**
 * @brief GstreamRecorder::setStateToNull
 */
void GstVideoWriter::setStateToNull()
{
    GstState cur_state = GST_STATE_NULL, pending = GST_STATE_NULL;
    gst_element_get_state(m_pipeline, &cur_state, &pending, 0);
    if (cur_state == GST_STATE_NULL) {
        if (pending != GST_STATE_VOID_PENDING) {
            gst_element_set_state(m_pipeline, GST_STATE_NULL);
        }
        return;
    }
    gst_element_set_state(m_pipeline, GST_STATE_READY);
    gst_element_get_state(m_pipeline, nullptr, nullptr, static_cast<GstClockTime>(-1));
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

gboolean GstVideoWriter::link_pulse_element_with_filter(GstElement *element1, GstElement *element2)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("audio/x-raw",
                               "format", G_TYPE_STRING, "F32LE",
                               "layout", G_TYPE_STRING, "interleaved",
                               "rate", G_TYPE_INT, 44100,
                               "channels", G_TYPE_INT,2,
                               NULL);

    link_ok = gst_element_link_filtered(element1,element2,caps);
    gst_caps_unref(caps);
    return link_ok;
}
void GstVideoWriter::setVideoPath(const QString &videoPath)
{
    if (videoPath.isEmpty() || m_videoPath == videoPath)
        return;

    m_videoPath = videoPath;

    if (!m_filesink)
        return;

    g_object_set(m_filesink, "location", videoPath.toStdString().c_str(), NULL);
}

static int globalCount = 0;
bool GstVideoWriter::writeVideo(Global::waylandFrame frame)
{
    if (!m_pipeline)
        return false;

    if (!m_appsrc)
        return false;
    qDebug() << " ======= 取视频帧进行编码 ";

    GstFlowReturn ret;
    GstBuffer *buffer;
    guint8 *ptr;
    int size = frame._width * frame._height * 4;
    ptr = (guint8 *)g_malloc(size * sizeof(uchar));
    if (NULL == ptr) {
        qDebug("GStreamerRecorder::writeFrame malloc failed!");
        return false;
    } else {
        memcpy(ptr, frame._frame, size);
        buffer = gst_buffer_new_wrapped((void *)ptr, size);
        QImage *img = new QImage(frame._frame, frame._width, frame._height, QImage::Format::Format_RGBA8888);
        img->save(QString("/home/uos/Desktop/test/image/test_%1.png").arg(globalCount));
        globalCount++;

        //设置时间戳
        GST_BUFFER_PTS(buffer) = gst_clock_get_time(m_pipeline->clock) - m_pipeline->base_time;
        //注入视频帧数据
        g_signal_emit_by_name(m_appsrc, "push-buffer", buffer, &ret);

        gst_buffer_unref(buffer);
    }

    return ret == GST_FLOW_OK;
}

void GstVideoWriter::setVideoArgs(Global::videoArgs videoArgs)
{
    qDebug() << "视频参数: " << videoArgs.getStr();
    m_videoArgs = videoArgs;
}

void GstVideoWriter::setAudioArgs(Global::audioArgs audioArgs)
{
    m_audioArgs = audioArgs;
}

void GstVideoWriter::objectUnref(gpointer object)
{
    if (object != nullptr) {
        gst_object_unref(object);
        object = nullptr;
    }
}

void GstVideoWriter::init()
{
    qDebug() << "GstVideoWriter::init()";
    // vp8编码在高分辨下(800*600以上）存在巨大编码耗时，
    // 因vp8编码效率跟不上，不停堆积的帧数据会导致相机内存疯涨，并且录制用时会成几何倍增长
    // 现象是：目标视频文件需要在停止录制后较长时间才能完全写入。
    m_gloop = g_main_loop_new(NULL, TRUE);
    createPipe1();

}

void GstVideoWriter::loadAppSrcCaps()
{
    if (!m_appsrc)
        return;

    //有图像的像素格式RGB8P、
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "RGBA",
                                        "width", G_TYPE_INT, m_videoArgs.width/*800*/,
                                        "height", G_TYPE_INT, m_videoArgs.height/*600*/,
                                        "framerate", GST_TYPE_FRACTION, 5, 1,
                                        NULL);

    gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);
}
gboolean GstBusMessageCb(GstBus *bus, GstMessage *msg, void *userdata)
{
    Q_UNUSED(bus);
    GstVideoWriter *recorder = reinterpret_cast<GstVideoWriter *>(userdata);
    return recorder->doBusMessage(msg);
}

/**
 * @brief GstreamRecorder::doBusMessage
 * @param message 消息
 * @return true 成功
 */
bool GstVideoWriter::doBusMessage(GstMessage *message)
{
    if (!message)
        return true;
    switch (message->type) {
    case GST_MESSAGE_ERROR: {
        GError *error = nullptr;
        gchar *dbg = nullptr;

        gst_message_parse_error(message, &error, &dbg);

        qCritical() << "Got pipeline 调试信息 >>>>> :" << dbg;
        if (dbg) {
            g_free(dbg);
        }

        if (error) {
            QString errMsg = error->message;
            emit errorMsg(errMsg);
            qCritical() << "Got pipeline error:" << errMsg;
            g_error_free(error);
        }
        break;
    }
    default:
        break;
    }
    return true;
}



/**
 * @brief GstreamRecorder::doBufferProbe
 * @param buffer 录音数据
 * @return true 成功
 */
bool GstVideoWriter::doBufferProbe(GstBuffer *buffer)
{
    if (buffer) {
        qint64 position = static_cast<qint64>(buffer->pts);
        position = position >= 0
                   ? position / (1000 * 1000) // 毫秒
                   : -1;
        QByteArray data;
        GstMapInfo info;
        if (gst_buffer_map(buffer, &info, GST_MAP_READ)) {
            data = QByteArray(reinterpret_cast<const char *>(info.data),
                              static_cast<int>(info.size));
            gst_buffer_unmap(buffer, &info);
        } else {
            return true;
        }

        QMutexLocker locker(&m_bufferMutex);
        if (!m_pendingBuffer.isValid())
            QMetaObject::invokeMethod(this, "bufferProbed", Qt::QueuedConnection);
        m_pendingBuffer = QAudioBuffer(data, m_format, position);
    }
    return true;
}

bool GstVideoWriter::createPipe1()
{
//    QString pipDesc = QString("webmmux name=mux ! filesink location=%1 name=filename "
//                              "appsrc name=source ! queue ! videoconvert primaries-mode=2  name=convert ! "
//                              "queue ! vp8enc threads=8 min-quantizer=40 name=encoder ! queue ! mux.video_0"
//                              " adder name=mix ! queue ! audiorate ! audioconvert ! vorbisenc name=audioEncoder ! mux.audio_0"
//                              " pulsesrc name=micSrc ! queue ! audioresample ! capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix."
//                              " pulsesrc name=sysSrc ! queue ! audioresample ! capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix.").arg(m_videoPath);

    QString pipDesc = QString("webmmux name=mux ! filesink location=%1 name=filename "
                                  "appsrc name=source ! queue ! videoconvert primaries-mode=2  name=convert ! "
                                  "queue ! vp8enc threads=8 name=encoder ! queue ! mux.video_0"
                                  " adder name=mix ! queue ! audiorate ! audioconvert ! vorbisenc name=audioEncoder ! mux.audio_0"
                                  " pulsesrc name=micSrc ! queue ! audioresample ! capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix."
                                  " pulsesrc name=sysSrc ! queue ! audioresample ! capsfilter caps=audio/x-raw,rate=44100,channels=2 ! queue ! mix.").arg(m_videoPath);
     m_pipeline = gst_parse_launch(pipDesc.toStdString().c_str(), NULL);

    qDebug() << "GstVideoWriter::createPipe()";
     m_filesink = gst_bin_get_by_name(GST_BIN(m_pipeline), "filename");

      /* 设置 视频src 属性 */
      m_appsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "source");
      //g_assert(m_appsrc);
      loadAppSrcCaps();

      g_object_set(m_appsrc, "format", GST_FORMAT_TIME, NULL);
      g_object_set(m_appsrc, "is-live", TRUE, NULL);

      GstElement *audioEncoder = nullptr; //编码器
      GstElement *sysSrc = nullptr; //数据源
      sysSrc = gst_bin_get_by_name(GST_BIN(m_pipeline),"sysSrc");
      if (!m_currentOutputDevice.isEmpty()) {
          g_object_set(reinterpret_cast<gpointer *>(sysSrc), "device", m_currentOutputDevice.toLatin1().data(), nullptr);
      }

      GstElement *micSrc = nullptr; //数据源
      micSrc = gst_bin_get_by_name(GST_BIN(m_pipeline),"micSrc");
      if (!m_currentInputDevice.isEmpty()) {
          g_object_set(reinterpret_cast<gpointer *>(micSrc), "device", m_currentInputDevice.toLatin1().data(), nullptr);
      }

    audioEncoder = gst_bin_get_by_name(GST_BIN(m_pipeline),"audioEncoder");
      GstPad *pad = gst_element_get_static_pad(audioEncoder, "sink");
       if (pad) {
           gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, bufferProbe, this, nullptr);
           gst_object_unref(pad);
       } else {
           qCritical() << "Error!!! sink pad make error";
           return false;
       }
   return true;
}

/**
 * @brief GstreamRecorder::initFormat
 */
void GstVideoWriter::initFormat()
{
    //未压缩数据
    m_format.setCodec("audio/pcm");
    //通道，采样率
    m_format.setChannelCount(2);
    m_format.setSampleRate(44100);
    //lamemp3enc 编码器插件格式为S16LE
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setSampleSize(16);
}


/**
 * @brief GstreamRecorder::setDevice
 * @param device 设备名称
 */
void GstVideoWriter::setOutputDevice(QString device)
{
    if (device != m_currentOutputDevice) {
        m_currentOutputDevice = device;
        if (m_pipeline != nullptr) {
            GstElement *audioSrc = gst_bin_get_by_name(reinterpret_cast<GstBin *>(m_pipeline), "sysSrc");
            g_object_set(reinterpret_cast<gpointer *>(audioSrc), "device", device.toLatin1().data(), nullptr);
        }
        qInfo() << device;
    }
}
/**
 * @brief GstreamRecorder::setDevice
 * @param device 设备名称
 */
void GstVideoWriter::setInputDevice(QString device)
{
    if (device != m_currentOutputDevice) {
        m_currentOutputDevice = device;
        if (m_pipeline != nullptr) {
            GstElement *audioSrc = gst_bin_get_by_name(reinterpret_cast<GstBin *>(m_pipeline), "micSrc");
            g_object_set(reinterpret_cast<gpointer *>(audioSrc), "device", device.toLatin1().data(), nullptr);
        }
        qInfo() << device;
    }
}
/**
 * @brief GstreamRecorder::bufferProbed
 */
void GstVideoWriter::bufferProbed()
{
    QAudioBuffer audioBuffer;
    {
        QMutexLocker locker(&m_bufferMutex);
        if (!m_pendingBuffer.isValid())
            return;
        audioBuffer = m_pendingBuffer;
        m_pendingBuffer = QAudioBuffer();
    }
    emit audioBufferProbed(audioBuffer);
}

void GstVideoWriter::print_pad_capabilities (GstElement *element, gchar *pad_name) {
  GstPad *pad = NULL;
  GstCaps *caps = NULL;

  /* Retrieve pad */
  pad = gst_element_get_static_pad (element, pad_name);
  if (!pad) {
    g_printerr ("Could not retrieve pad '%s'\n", pad_name);
    return;
  }

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_current_caps (pad);
  if (!caps)
    caps = gst_pad_query_caps (pad, NULL);

  /* Print and free */
  g_print ("Caps for the %s pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);
  gst_object_unref (pad);
}

void GstVideoWriter::print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

 gboolean GstVideoWriter::print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);

  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}
