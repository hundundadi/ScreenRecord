#ifndef WAYLANDRECORD_H
#define WAYLANDRECORD_H
#include <QObject>
#include <gst/gst.h>
#include "writeframethread.h"

/**
 * @brief 此类是在wayland协议上录制视频的类
 */
class WaylandRecord : public QObject
{
    Q_OBJECT
public:
    WaylandRecord();

    /**
     * @brief 开始录屏
     */
    void startRecord();
    /**
     * @brief 停止录屏
     */
    void stopRecord();

    /**
     * @brief 设置录屏窗口
     */
    void setRecordWinArg(QStringList widArg);
    void initVariable();
    void initWaylandRecord();
    void initGStreamer();


public slots:
    /**
     * @brief 开始录屏
     */
    void onStartRecord(QString fd);

signals:
    void signalsStartRecord(QString fd);

protected:
    void checkTempFileArm();
    void whileCheckTempFileArm();

private:
    QString getRecordArea();
    QString getRecordEncoder();
    QString getRecordMuxer();
    QString Pipeline_structured_output(QString pipeline);

private :
    QString m_fd;
    QString m_path;

    QStringList m_winArg;
    GstElement *pipeline;

    std::string m_testFilePaht;

    bool m_tempFlag = false;


    WriteFrameThread* m_writeFrameThread;

};



#endif // WAYLANDRECORD_H
