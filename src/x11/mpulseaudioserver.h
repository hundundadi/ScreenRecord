#ifndef PULSEAUDIOSERVER_H
#define PULSEAUDIOSERVER_H

#include <QObject>

class MPulseAudioServer: public QObject
{
    Q_OBJECT
public:
    MPulseAudioServer();
    virtual ~MPulseAudioServer();
    static bool isAvailable();
    static QStringList getAllDevices();
};

#endif // PULSEAUDIOSERVER_H
