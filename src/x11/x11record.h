#ifndef X11RECORD_H
#define X11RECORD_H

#include <QObject>
#include <QDebug>

#include <gst/gst.h>

class X11Record : public QObject
{
public:
    X11Record();

    /**
     * @brief initX11Record
     */
    void initX11Record();

    /**
     * @brief 开始录屏
     */
    void startRecord();

    /**
     * @brief 设置录屏窗口
     */
    void setRecordWinArg(QStringList widArg);
    /**
     * @brief 停止录屏
     */
    void stopRecord();

private:
    /**
     * @brief 获取录屏的区域
     * @return
     */
    QString getRecordArea();

    /**
     * @brief 获取录屏的帧
     * @return
     */
    QString getRecordFrames();

    /**
     * @brief 获取录屏的编码器
     * @return
     */
    QString getRecordEncoder();

    /**
     * @brief 获取录屏的音频
     * @return
     */
    QString getRecordAudio();

    /**
     * @brief 获取录屏复用器
     * @return
     */
    QString getRecordMuxer();


    QString Pipeline_structured_output(QString pipeline);


private:
    QStringList m_recordArg;

    QStringList m_winArg;

    GstElement *pipeline;


};

#endif // X11RECORD_H
