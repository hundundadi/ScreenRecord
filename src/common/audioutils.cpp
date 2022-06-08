/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     He MingYang <hemingyang@uniontech.com>
 *
 * Maintainer: Liu Zheng <liuzheng@uniontech.com>
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

#include "audioutils.h"

#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusError>
#include <QDBusMessage>

#include <com_deepin_daemon_audio.h>
#include <com_deepin_daemon_audio_sink.h>
#include <com_deepin_daemon_audio_source.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

AudioUtils::AudioUtils(QObject *parent)
{
    Q_UNUSED(parent);
}

QString AudioUtils::currentAudioChannel()
{
    const QString serviceName {"com.deepin.daemon.Audio"};

    QScopedPointer<com::deepin::daemon::Audio> audioInterface;
    QScopedPointer<com::deepin::daemon::audio::Sink> defaultSink;

    audioInterface.reset(
        new com::deepin::daemon::Audio(
            serviceName,
            "/com/deepin/daemon/Audio",
            QDBusConnection::sessionBus(),
            this)
    );

    defaultSink.reset(
        new com::deepin::daemon::audio::Sink(
            serviceName,
            audioInterface->defaultSink().path(),
            QDBusConnection::sessionBus(),
            this)
    );
    if (defaultSink->isValid()) {
        QString sinkName = defaultSink->name();
        QStringList options;
        options << "-c";
        options << QString("pacmd list-sources | grep -PB 1 %1 | head -n 1 | perl -pe 's/.* //g'").arg(sinkName);

        QProcess process;
        process.start("bash", options);
        process.waitForFinished();
        process.waitForReadyRead();
        QString str_output = process.readAllStandardOutput();
        qDebug() << options << str_output;
        return str_output;
    }
    return "";
}

QString AudioUtils::getDefaultDeviceName(DefaultAudioType mode)
{
    QString device = "";
    const QString serviceName {"com.deepin.daemon.Audio"};

    QScopedPointer<com::deepin::daemon::Audio> audioInterface;
    audioInterface.reset(
        new com::deepin::daemon::Audio(
            serviceName,
            "/com/deepin/daemon/Audio",
            QDBusConnection::sessionBus(),
            this)
    );
    if (mode == DefaultAudioType::Sink) {
        QScopedPointer<com::deepin::daemon::audio::Sink> defaultSink;
        defaultSink.reset(
            new com::deepin::daemon::audio::Sink(
                serviceName,
                audioInterface->defaultSink().path(),
                QDBusConnection::sessionBus(),
                this)
        );
        if (defaultSink->isValid()) {
            qInfo() << "default sink name is : " << defaultSink->name();
            qInfo() << "default sink activePort name : " << defaultSink->activePort().name;
            qInfo() << "             activePort description : " << defaultSink->activePort().description;
            qInfo() << "             activePort availability : " << defaultSink->activePort().availability;
            device = defaultSink->name();
            if (!device.isEmpty() && !device.endsWith(".monitor")) {
                device += ".monitor";
            }
        }
    } else if (mode == DefaultAudioType::Source) {
        QScopedPointer<com::deepin::daemon::audio::Source> defaultSource;
        defaultSource.reset(
            new com::deepin::daemon::audio::Source(
                serviceName,
                audioInterface->defaultSource().path(),
                QDBusConnection::sessionBus(),
                this)
        );
        if (defaultSource->isValid()) {
            qInfo() << "default source name is : " << defaultSource->name();
            qInfo() << "default source activePort name : " << defaultSource->activePort().name;
            qInfo() << "               activePort description : " << defaultSource->activePort().description;
            qInfo() << "               activePort availability : " << defaultSource->activePort().availability;
            device = defaultSource->name();
            if (device.endsWith(".monitor")) {
                device.clear();
            }
        }
    } else {
        qCritical() << "The passed parameter is incorrect! Please pass in 1 or 2!";
    }
    return device;
}
