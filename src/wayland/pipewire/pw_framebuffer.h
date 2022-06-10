/* This file is part of the KDE project
   Copyright (C) 2018 Oleg Chernovskiy <kanedias@xaker.ru>
   Copyright (C) 2018-2020 Jan Grulich <jgrulich@redhat.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.
*/
#ifndef KRFB_FRAMEBUFFER_XCB_XCB_FRAMEBUFFER_H
#define KRFB_FRAMEBUFFER_XCB_XCB_FRAMEBUFFER_H

#include <QWidget>
#include <QObject>
#include <QVariantMap>
#include <QMutex>

/**
 * @brief The PWFrameBuffer class - framebuffer implementation based on XDG Desktop Portal ScreenCast interface.
 *        The design relies heavily on a presence of XDG D-Bus service and PipeWire daemon.
 *
 * @author Oleg Chernovskiy <kanedias@xaker.ru>
 */
class PWFrameBuffer: public QObject
{
    Q_OBJECT
public:
    typedef struct {
        uint nodeId;
        QVariantMap map;
    } Stream;
    typedef QList<Stream> Streams;
    quint32 screenWidth = 0;
    quint32 screenHeight = 0;
    uint pwStreamNodeId = 0;
    char *fb = nullptr;

    PWFrameBuffer( QObject *parent = nullptr);
    virtual ~PWFrameBuffer() override;

    void initPW();
    int  depth() ;
    int  height() ;
    int  width() ;
    int  paddedWidth() ;
    void startMonitor() ;
    void stopMonitor();


    bool isValid() const;
    bool isShareFlag;

private:
    class Private;
    const QScopedPointer<Private> d;

    QMutex m_mutex;
    QList<QRect> tiles;

};

#endif
