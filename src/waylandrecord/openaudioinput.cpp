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


#include "openaudioinput.h"

OpenAudioInput::OpenAudioInput()
{
    m_auidoBuffer = nullptr;
    m_data = nullptr;

    m_sysData = nullptr;
    m_datasize = 0;

    init();
}

void OpenAudioInput::stop()
{
    m_stopped = 1;
}

void OpenAudioInput::start()
{
    if(m_isCaptureMic){
        QtConcurrent::run(this,&OpenAudioInput::captureMicAudio);
    }
    if(m_isCaptureSys){
        QtConcurrent::run(this,&OpenAudioInput::captureSysAudio);
    }
}

void OpenAudioInput::init()
{
    m_stopped = 0;
}

bool OpenAudioInput::initAudio()
{
    int audio = AUDIO_PORTAUDIO;
    if(m_isCaptureMic){
        qDebug() << " ======= 初始化麦克风音频 ";
        /*初始化输入音频context*/
        audio_context_t *input_audio_ctx = create_mic_audio_context(audio, 9);
        printf( "api = %d\n", input_audio_ctx->api );
        printf( "input_audio_ctx->device_name = %s\n", input_audio_ctx->device_name );
        printf( "input_audio_ctx->device_count = %d\n", input_audio_ctx->device_count );
        printf( "input_audio_ctx->device = %d\n", input_audio_ctx->device );
        printf( "input_audio_ctx->channels = %d\n", input_audio_ctx->channels );
        printf( "input_audio_ctx->samprate = %d\n", input_audio_ctx->samprate );
        printf( "input_audio_ctx->latency = %f\n", input_audio_ctx->latency );
        printf( "input_audio_ctx->current_ts = %ld\n", input_audio_ctx->current_ts );
        printf( "input_audio_ctx->last_ts = %ld\n", input_audio_ctx->last_ts );
        printf( "input_audio_ctx->snd_begintime = %ld\n", input_audio_ctx->snd_begintime );
        printf( "input_audio_ctx->ts_drift = %ld\n", input_audio_ctx->ts_drift );
        printf( "input_audio_ctx->capture_buff = %f\n", input_audio_ctx->capture_buff );
        printf( "input_audio_ctx->capture_buff_size = %d\n", input_audio_ctx->capture_buff_size );
        printf( "input_audio_ctx->capture_buff_level[2] = %f\n", input_audio_ctx->capture_buff_level[2] );
        printf( "input_audio_ctx->stream_flag = %d\n", input_audio_ctx->stream_flag );

        fflush(stdout);
        if (input_audio_ctx == NULL) {
            qDebug() << "cheese: couldn't get a valid audio context for the selected api - disabling audio\n" << stderr;
            return false;
        }
    }
    if(m_isCaptureSys){
        qDebug() << " ======= 初始化系统音频 ";
        /*初始化系统音频context*/
        audio_context_t *output_audio_ctx = create_sys_audio_context(audio, 9);
        printf( "api = %d\n", output_audio_ctx->api );
        printf( "output_audio_ctx->device_name = %s\n", output_audio_ctx->device_name );
        printf( "output_audio_ctx->device_count = %d\n", output_audio_ctx->device_count );
        printf( "output_audio_ctx->device = %d\n", output_audio_ctx->device );
        printf( "output_audio_ctx->channels = %d\n", output_audio_ctx->channels );
        printf( "output_audio_ctx->samprate = %d\n", output_audio_ctx->samprate );
        printf( "output_audio_ctx->latency = %f\n", output_audio_ctx->latency );
        printf( "output_audio_ctx->current_ts = %ld\n", output_audio_ctx->current_ts );
        printf( "output_audio_ctx->last_ts = %ld\n", output_audio_ctx->last_ts );
        printf( "output_audio_ctx->snd_begintime = %ld\n", output_audio_ctx->snd_begintime );
        printf( "output_audio_ctx->ts_drift = %ld\n", output_audio_ctx->ts_drift );
        printf( "output_audio_ctx->capture_buff = %f\n", output_audio_ctx->capture_buff );
        printf( "output_audio_ctx->capture_buff_size = %d\n", output_audio_ctx->capture_buff_size );
        printf( "output_audio_ctx->capture_buff_level[2] = %f\n", output_audio_ctx->capture_buff_level[2] );
        printf( "output_audio_ctx->stream_flag = %d\n", output_audio_ctx->stream_flag );

        fflush(stdout);
        if (output_audio_ctx == NULL) {
            qDebug() << "cheese: couldn't get a valid audio context for the selected api - disabling audio\n" << stderr;
            return false;
        }
    }
    return true;
}

void OpenAudioInput::captureSysAudio()
{
    qDebug() << " ======= 采集系统音频 ";
    int frameszie = 512;
    uint datasize = 0;
    audio_context_t *output_audio_ctx = get_sys_audio_context();
    if (!output_audio_ctx) {
        qDebug("no Sys audio context: skiping audio processing\n");
        return;
    }
    qDebug() << sizeof(sample_t);

    //初始化音频样本获取缓存大小
    audio_set_cap_buffer_size(output_audio_ctx,
                              frameszie * audio_get_channels(output_audio_ctx));
    audio_start(output_audio_ctx);

    datasize = output_audio_ctx->capture_buff_size * sizeof(sample_t);

    /*
     * alloc the buffer after audio_start
     * otherwise capture_buff_size may not
     * be correct
     * allocated data is big enough for float samples (32 bit)
     * although it may contain int16 samples (16 bit)
     */
    m_outputAudioBuffer = audio_get_buffer(output_audio_ctx);

    int sample_type = 1;

    FILE* pcm_data_file = fopen("/home/uos/Desktop/test/sys.pcm","wb");
    while (m_stopped == 0) {
        int ret = audio_get_next_buffer(output_audio_ctx, m_outputAudioBuffer, sample_type, get_audio_fx_mask());
        if (ret > 0) {
            /*
             * no buffers to process
             * sleep a couple of milisec
             */
            struct timespec req = {
                .tv_sec = 0,
                .tv_nsec = 1000000
            };/*nanosec*/
            nanosleep(&req, NULL);
        } else if (ret == 0) {
            if (m_outputDatasize != datasize) {
                m_outputDatasize = datasize;
                if (m_sysData != nullptr) {
                    delete [] m_sysData;
                    m_sysData = nullptr;
                }
                m_sysData = new uchar[datasize];
            }
            memcpy(m_sysData, m_outputAudioBuffer->data, datasize);

            fwrite(m_sysData,frameszie,sizeof(sample_t),pcm_data_file);
            m_rwMtxOutputData.lock();

            emit sendSysAudioProcessing(m_sysData, datasize);

            m_rwMtxOutputData.unlock();
        }
    }

    fclose(pcm_data_file);

    audio_stop(output_audio_ctx);
    audio_delete_buffer(m_outputAudioBuffer);

    return;
}

void OpenAudioInput::captureMicAudio()
{
    qDebug() << " ======= 采集麦克风音频 ";
    int frameszie = 512;
    uint datasize = 0;
    audio_context_t *audio_ctx = get_audio_context();
    if (!audio_ctx) {
        qDebug("no Mic context: skiping audio processing\n");
        return;
    }
    //初始化音频样本获取缓存大小
    audio_set_cap_buffer_size(audio_ctx,
                              frameszie * audio_get_channels(audio_ctx));
    audio_start(audio_ctx);

    datasize = audio_ctx->capture_buff_size * sizeof(sample_t);

    /*
     * alloc the buffer after audio_start
     * otherwise capture_buff_size may not
     * be correct
     * allocated data is big enough for float samples (32 bit)
     * although it may contain int16 samples (16 bit)
     */
    m_auidoBuffer = audio_get_buffer(audio_ctx);

    uint sample_type = 1;

    FILE* pcm_data_file = fopen("/home/uos/Desktop/test/mic.pcm","wb");
    uint yuvsize = 0;
    while (m_stopped == 0) {
        int ret = audio_get_next_buffer(audio_ctx, m_auidoBuffer, sample_type, get_audio_fx_mask());
        if (ret > 0) {
            /*
             * no buffers to process
             * sleep a couple of milisec
             */
            struct timespec req = {
                .tv_sec = 0,
                .tv_nsec = 1000000
            };/*nanosec*/
            nanosleep(&req, NULL);
        } else if (ret == 0) {
            if (m_datasize != datasize) {
                m_datasize = datasize;
                if (m_data != nullptr) {
                    delete [] m_data;
                    m_data = nullptr;
                }
                m_data = new uchar[datasize];
            }
            memcpy(m_data, m_auidoBuffer->data, datasize);

            fwrite(m_data,frameszie,sizeof(sample_t),pcm_data_file);
            m_rwMtxData.lock();

            emit sendMicAudioProcessing(m_data, datasize);

            m_rwMtxData.unlock();
        }
    }

    fclose(pcm_data_file);

    audio_stop(audio_ctx);
    audio_delete_buffer(m_auidoBuffer);

    return;
}

OpenAudioInput::~OpenAudioInput()
{
    qDebug() << "~OpenAudioInput";
}
