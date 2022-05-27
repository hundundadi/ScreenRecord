/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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

#ifndef LOAD_LIBS_H
#define LOAD_LIBS_H
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <pthread.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
//#include <libavutil/dict.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libffmpegthumbnailer/videothumbnailerc.h>
#include <portaudio.h>
#include <libv4l2.h>
#include <libudev.h>

typedef struct _LoadLibNames {
    char *chPortaudio;
} LoadLibNames;

void setLibNames(LoadLibNames tmp);


//PaDeviceIndex Pa_GetDeviceCount( void );
typedef PaDeviceIndex(*uos_Pa_GetDeviceCount)(void);
//PaError Pa_IsStreamStopped( PaStream *stream );
typedef PaError(*uos_Pa_IsStreamStopped)(PaStream *stream);
//const PaDeviceInfo* Pa_GetDeviceInfo( PaDeviceIndex device );
typedef const PaDeviceInfo *(*uos_Pa_GetDeviceInfo)(PaDeviceIndex device);
//PaError Pa_Initialize( void );
typedef PaError(*uos_Pa_Initialize)(void);
//PaError Pa_IsStreamActive( PaStream *stream );
typedef PaError(*uos_Pa_IsStreamActive)(PaStream *stream);
//PaError Pa_Terminate( void );
typedef PaError(*uos_Pa_Terminate)(void);
//PaError Pa_AbortStream( PaStream *stream );
typedef PaError(*uos_Pa_AbortStream)(PaStream *stream);
//PaError Pa_StopStream( PaStream *stream );
typedef PaError(*uos_Pa_StopStream)(PaStream *stream);
//PaError Pa_CloseStream( PaStream *stream );
typedef PaError(*uos_Pa_CloseStream)(PaStream *stream);
//PaDeviceIndex Pa_GetDefaultInputDevice( void );
typedef PaDeviceIndex(*uos_Pa_GetDefaultInputDevice)(void);
//const PaHostApiInfo * Pa_GetHostApiInfo( PaHostApiIndex hostApi );
typedef const PaHostApiInfo *(*uos_Pa_GetHostApiInfo)(PaHostApiIndex hostApi);
//PaDeviceIndex Pa_GetDefaultOutputDevice( void );
typedef PaDeviceIndex(*uos_Pa_GetDefaultOutputDevice)(void);
/*PaError Pa_OpenStream( PaStream** stream,
                       const PaStreamParameters *inputParameters,
                       const PaStreamParameters *outputParameters,
                       double sampleRate,
                       unsigned long framesPerBuffer,
                       PaStreamFlags streamFlags,
                       PaStreamCallback *streamCallback,
                       void *userData );*/
typedef PaError(*uos_Pa_OpenStream)(PaStream **stream,
                                    const PaStreamParameters *inputParameters,
                                    const PaStreamParameters *outputParameters,
                                    double sampleRate,
                                    unsigned long framesPerBuffer,
                                    PaStreamFlags streamFlags,
                                    PaStreamCallback *streamCallback,
                                    void *userData);
//PaError Pa_StartStream( PaStream *stream );
typedef PaError(*uos_Pa_StartStream)(PaStream *stream);
//const PaStreamInfo* Pa_GetStreamInfo( PaStream *stream );
typedef const PaStreamInfo *(*uos_Pa_GetStreamInfo)(PaStream *stream);
//const char *Pa_GetErrorText( PaError errorCode );
typedef const char *(*uos_Pa_GetErrorText)(PaError errorCode);

typedef struct _LoadPortAudio {
    uos_Pa_GetDeviceCount m_Pa_GetDeviceCount;
    uos_Pa_IsStreamStopped m_Pa_IsStreamStopped;
    uos_Pa_GetDeviceInfo m_Pa_GetDeviceInfo;
    uos_Pa_Initialize m_Pa_Initialize;
    uos_Pa_IsStreamActive m_Pa_IsStreamActive;
    uos_Pa_Terminate m_Pa_Terminate;
    uos_Pa_AbortStream m_Pa_AbortStream;
    uos_Pa_StopStream m_Pa_StopStream;
    uos_Pa_CloseStream m_Pa_CloseStream;
    uos_Pa_GetDefaultInputDevice m_Pa_GetDefaultInputDevice;
    uos_Pa_GetHostApiInfo m_Pa_GetHostApiInfo;
    uos_Pa_GetDefaultOutputDevice m_Pa_GetDefaultOutputDevice;
    uos_Pa_OpenStream m_Pa_OpenStream;
    uos_Pa_StartStream m_Pa_StartStream;
    uos_Pa_GetStreamInfo m_Pa_GetStreamInfo;
    uos_Pa_GetErrorText m_Pa_GetErrorText;
} LoadPortAudio;

LoadPortAudio *getPortAudio();

#endif//LOAD_LIBS_H
