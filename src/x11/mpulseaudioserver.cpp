
#include <pulse/simple.h>
#include <pulse/error.h>
#include "../AudioDevices.c"

#include "mpulseaudioserver.h"

MPulseAudioServer::MPulseAudioServer()
{

}

MPulseAudioServer::~MPulseAudioServer()
{

}



bool MPulseAudioServer::isAvailable()
{
    // pulseaudio connection
    pa_simple *paConnection = NULL;

    // format specifier
    static pa_sample_spec sspec;
    sspec.channels = 2;
    sspec.format = PA_SAMPLE_S16LE;
    sspec.rate = 44100;

    int error = 0;

    paConnection = pa_simple_new(
                       NULL, // default PA server
                       "Test libpulse", // app name
                       PA_STREAM_RECORD,  // stream direction
                       NULL, // default device
                       "record", // stream name
                       &sspec,  // format spec
                       NULL, // default channel map
                       NULL, // may be NULL for defaults, but we want tweak!
                       &error);

    bool value;
    if (! paConnection) {
        //fprintf( stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror( error ) );
        value = false;
    } else {
        pa_simple_free(paConnection);
        //printf( "Connected to PulseAudio server ok.\n" );
        value = true;
    }

    return value;
}

QStringList MPulseAudioServer::getAllDevices()
{
    QStringList list;
    if (isAvailable()) {
        const char *ss = get_all_audio_devices();
        QString s1 = QString::fromUtf8(ss);
        QString s2 = s1.left(QString::fromUtf8(ss).count() - 3);
        list = s2.split("---");
    }
    return list;
}
