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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include "load_libs.h"

void PrintError(){
    char *error;
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
}
static LoadLibNames g_ldnames;


static LoadPortAudio *pPortAudio = NULL;
static LoadPortAudio *newPortAudio(void)
{
    pPortAudio = (LoadPortAudio *)malloc(sizeof(LoadPortAudio));
    //libportaudio
    void *handle = dlopen(g_ldnames.chPortaudio/*"libportaudio.so.2"*/,RTLD_LAZY);
    if (!handle) {
        PrintError();
    }
    pPortAudio->m_Pa_GetDeviceCount = (uos_Pa_GetDeviceCount)dlsym(handle, "Pa_GetDeviceCount");
    PrintError();
    pPortAudio->m_Pa_IsStreamStopped = (uos_Pa_IsStreamStopped)dlsym(handle, "Pa_IsStreamStopped");
    PrintError();
    pPortAudio->m_Pa_GetDeviceInfo = (uos_Pa_GetDeviceInfo)dlsym(handle, "Pa_GetDeviceInfo");
    PrintError();
    pPortAudio->m_Pa_Initialize = (uos_Pa_Initialize)dlsym(handle, "Pa_Initialize");
    PrintError();
    pPortAudio->m_Pa_IsStreamActive = (uos_Pa_IsStreamActive)dlsym(handle, "Pa_IsStreamActive");
    PrintError();
    pPortAudio->m_Pa_Terminate = (uos_Pa_Terminate)dlsym(handle, "Pa_Terminate");
    PrintError();
    pPortAudio->m_Pa_AbortStream = (uos_Pa_AbortStream)dlsym(handle, "Pa_AbortStream");
    PrintError();
    pPortAudio->m_Pa_StopStream = (uos_Pa_StopStream)dlsym(handle, "Pa_StopStream");
    PrintError();
    pPortAudio->m_Pa_CloseStream = (uos_Pa_CloseStream)dlsym(handle, "Pa_CloseStream");
    PrintError();
    pPortAudio->m_Pa_GetDefaultInputDevice = (uos_Pa_GetDefaultInputDevice)dlsym(handle, "Pa_GetDefaultInputDevice");
    PrintError();
    pPortAudio->m_Pa_GetHostApiInfo = (uos_Pa_GetHostApiInfo)dlsym(handle, "Pa_GetHostApiInfo");
    PrintError();
    pPortAudio->m_Pa_GetDefaultOutputDevice = (uos_Pa_GetDefaultOutputDevice)dlsym(handle, "Pa_GetDefaultOutputDevice");
    PrintError();
    pPortAudio->m_Pa_OpenStream = (uos_Pa_OpenStream)dlsym(handle, "Pa_OpenStream");
    PrintError();
    pPortAudio->m_Pa_StartStream = (uos_Pa_StartStream)dlsym(handle, "Pa_StartStream");
    PrintError();
    pPortAudio->m_Pa_GetStreamInfo = (uos_Pa_GetStreamInfo)dlsym(handle, "Pa_GetStreamInfo");
    PrintError();
    pPortAudio->m_Pa_GetErrorText = (uos_Pa_GetErrorText)dlsym(handle, "Pa_GetErrorText");
    PrintError();
    assert(pPortAudio != NULL);
    return pPortAudio;
}

LoadPortAudio *getPortAudio()
{
    static pthread_mutex_t mutexPortAudio;
    //双检锁
    if (pPortAudio == NULL) {
        // 这里要对pUSB加锁
        pthread_mutex_lock(&mutexPortAudio);
        if (pPortAudio == NULL)
        {
            pPortAudio = newPortAudio();
        }
        //退出时解锁
        pthread_mutex_unlock(&mutexPortAudio);
    }

    return pPortAudio;
}


void setLibNames(LoadLibNames tmp)
{

    g_ldnames.chPortaudio = (char*)malloc(strlen(tmp.chPortaudio)+1);
    strcpy(g_ldnames.chPortaudio,tmp.chPortaudio);
}
