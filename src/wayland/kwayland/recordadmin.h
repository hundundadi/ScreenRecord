
#ifndef RECORDADMIN_H
#define RECORDADMIN_H
#include "writeframethread.h"
#include "../gstvideowriter.h"
#include <sys/time.h>
#include <map>
#include <qimage.h>
#include "waylandintegration_p.h"
#include "writeframethread.h"
#include "../audio/audiowatcher.h"


#define AUDIO_INPUT_DEVICE    "hw:0,0"
#define VIDEO_INPUT_DEVICE    "/dev/video0"
#include <QThread>

//pts: （int64_t）显示时间，结合AVStream->time_base转换成时间戳
//dts: （int64_t）解码时间，结合AVStream->time_base转换成时间戳
using namespace std;
class RecordAdmin : public QObject
{
    Q_OBJECT

public:
    RecordAdmin(QStringList list, WaylandIntegration::WaylandIntegrationPrivate *context, QObject *parent = nullptr);
    virtual ~RecordAdmin();


public:
    /**
     * @brief init:初始化录屏管理
     * @param width:宽度
     * @param height:高度
     */
    void init(int width, int height);
    void initGStreamer();


    /**
     * @brief stopStream:停止录屏
     * @return
     */
    int stopStream();

    void setBoardVendor(int boardVendorType);

protected:
    void  setRecordAudioType(int audioType);
    void  setMicAudioRecord(bool bRecord);
    void  setSysAudioRecord(bool bRecord);
    int   startStream();
    static void *stream(void *param);

public:
    //CAVOutputStream                          *m_pOutputStream;
    WriteFrameThread                         *m_writeFrameThread;
    QMutex m_cacheMutex;
    GstVideoWriter *m_gstVideoWriter;

    void initAudioWatcher();
private:
    int m_boardVendorType = 0;
    WaylandIntegration::WaylandIntegrationPrivate *m_context;
    //参数列表：程序名称，视频类型，视频宽，视频高，视频x坐标，视频y坐标，视频帧率，视频保存路径，音频类型
    QList<QString> argvList;
    //文件路径
    QString m_filePath;
    //帧率
    int m_fps;
    //音频类型
    int m_audioType;
    //视频类型
    int  m_videoType;
    //x坐标
    int m_x;
    //y坐标
    int m_y;
    //帧宽
    int m_selectWidth;
    //帧高
    int m_selectHeight;
    pthread_t  m_mainThread;
    int m_delay;
    QMutex m_oldFrameMutex;
    int m_gifBuffersize;

    AudioWatcher *m_audioWatcher {nullptr};

};

#endif // RECORDADMIN_H
