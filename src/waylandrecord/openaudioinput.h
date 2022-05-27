/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangcong <wangcong@uniontech.com>
* Maintainer: wangcong <wangcong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OpenAudioInput_H
#define OpenAudioInput_H

#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QtConcurrent>


#ifdef __cplusplus
extern "C" {
#endif
#include "libcam/libcam/camview.h"
#include "libcam/libcam_audio/audio.h"
#ifdef __cplusplus
}
#endif

/**
 * @brief 音频处理线程
 */
class OpenAudioInput : public QObject
{
    Q_OBJECT
public:
    OpenAudioInput();

    ~OpenAudioInput();

    /**
     * @brief stop 停止线程
     */
    void stop();

    void start();

    /**
     * @brief init 线程初始化
     */
    void init();

    bool initAudio();

    /**
     * @brief getStatus 获取状态
     */
    QAtomicInt getStatus()
    {
        return m_stopped;
    }

protected:

    void captureSysAudio();

    void captureMicAudio();

signals:
    void sendMicAudioProcessing(uchar *data, uint size);
    void sendSysAudioProcessing(uchar *data, uint size);

public:
    QMutex            m_rwMtxData;
    QMutex            m_rwMtxOutputData;
private:

    pthread_t m_inputAudioThread,m_outputAudioThread;
    QAtomicInt        m_stopped;
    audio_buff_t      *m_auidoBuffer;
    uchar             *m_data;
    uint              m_datasize;

    audio_buff_t      *m_outputAudioBuffer;
    uchar             *m_sysData;
    uint              m_outputDatasize;

    bool m_isCaptureMic = false;
    bool m_isCaptureSys = true;
};

#endif // AudioProcessingThread_H
