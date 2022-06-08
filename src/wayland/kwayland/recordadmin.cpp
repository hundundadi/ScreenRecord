#include "recordadmin.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <cstring>
#include <math.h>
#include <unistd.h>
#include <qtimer.h>
#include <QDebug>

RecordAdmin::RecordAdmin(QStringList list, WaylandIntegration::WaylandIntegrationPrivate *context, QObject *parent): QObject(parent),
    m_writeFrameThread(nullptr),
    m_context(context)
{
    m_delay = 0;
    m_gifBuffersize = 0;
    if (list.size() > 7) {
        m_videoType = list[0].toInt();
        //录屏不支持奇数，转偶数
        m_selectWidth = list[1].toInt() / 2 * 2;
        m_selectHeight = list[2].toInt() / 2 * 2;
        m_x = list[3].toInt();
        m_y = list[4].toInt();
        m_filePath = list[5];
    }
    m_writeFrameThread = new WriteFrameThread(context);
}

RecordAdmin::~RecordAdmin()
{
    if (nullptr != m_writeFrameThread) {
        delete m_writeFrameThread;
        m_writeFrameThread = nullptr;
    }

}

void RecordAdmin::setRecordAudioType(int audioType)
{
    switch (audioType) {
    case audioType::MIC:
        setMicAudioRecord(true);
        setSysAudioRecord(false);
        break;
    case audioType::SYS:
        setMicAudioRecord(false);
        setSysAudioRecord(true);
        break;
    case audioType::MIC_SYS:
        setMicAudioRecord(true);
        setSysAudioRecord(true);
        break;
    case audioType::NOS:
        setMicAudioRecord(false);
        setSysAudioRecord(false);
        break;
    default: {
        setMicAudioRecord(false);
        setSysAudioRecord(false);
    }
    }
}

void  RecordAdmin::setMicAudioRecord(bool bRecord)
{
    Q_UNUSED(bRecord);
}

void  RecordAdmin::setSysAudioRecord(bool bRecord)
{
    Q_UNUSED(bRecord);
}

void RecordAdmin::init(int width, int height)
{
    qDebug() << __LINE__ << " RecordAdmin::init ";
    m_selectWidth = width;
    m_selectHeight = height;
    //initGStreamer();
    initAudioWatcher();
    initGStreamer();
    setRecordAudioType(m_audioType);
    pthread_create(&m_mainThread, nullptr, stream, static_cast<void *>(this));
    pthread_detach(m_mainThread);
}

void RecordAdmin::initGStreamer()
{
    qDebug() << QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << __LINE__ << " RecordAdmin::initGStreamer start!";

    m_gstVideoWriter = new GstVideoWriter();

    Global::videoArgs videoArgs ;
    videoArgs.width = m_selectWidth;
    videoArgs.height = m_selectHeight;
    videoArgs.framerate = 5;
    m_gstVideoWriter->setVideoArgs(videoArgs);
    m_gstVideoWriter->init();
    qDebug() << __LINE__ << " RecordAdmin::initGStreamer end!";
}
int RecordAdmin::startStream()
{
    qDebug() << __LINE__ << " RecordAdmin::startStream";
    m_gstVideoWriter->setOutputDevice(m_audioWatcher->getDeviceName(static_cast<AudioWatcher::AudioMode>(0)));
    m_gstVideoWriter->setInputDevice(m_audioWatcher->getDeviceName(static_cast<AudioWatcher::AudioMode>(1)));
    m_gstVideoWriter->start();


//设置写mp4/mkv视频帧
    m_writeFrameThread->setBWriteFrame(true);
    m_writeFrameThread->start();
    return 0;
}

void *RecordAdmin::stream(void *param)
{
    qDebug() << __LINE__ << " RecordAdmin::stream";
    RecordAdmin *recordAdmin = static_cast<RecordAdmin *>(param);
    recordAdmin->startStream();
    return nullptr;
}

int RecordAdmin::stopStream()
{
    //设置关闭视频数据采集，将视频数据采集到队列中
    m_context->setBGetFrame(false);
    //设置是否写MP4/MKV视频帧，将数据从队列中取出
    m_writeFrameThread->setBWriteFrame(false);
    return 0;
}

void RecordAdmin::setBoardVendor(int boardVendorType)
{
    m_boardVendorType = boardVendorType;
}

/**
 * @brief VNoteRecordBar::initAudioWatcher
 */
void RecordAdmin::initAudioWatcher()
{
    m_audioWatcher = new AudioWatcher(this);
//    connect(m_audioWatcher, &AudioWatcher::sigDeviceChange,
//            this, &VNoteRecordBar::onAudioDeviceChange);
//    connect(m_audioWatcher, &AudioWatcher::sigVolumeChange,
//            this, &VNoteRecordBar::onAudioVolumeChange);
}



