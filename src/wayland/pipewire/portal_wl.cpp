/*
 * Copyright © 2016 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Jan Grulich <jgrulich@redhat.com>
 */

#include "portal_wl.h"
#include "../../common/global.h"


#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusUnixFileDescriptor>
#include <QDBusInterface>

#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusObjectPath>
#include <QVariantMap>
#include <QLatin1String>

Q_DECLARE_METATYPE(Portal_wl::Stream);
Q_DECLARE_METATYPE(Portal_wl::Streams);

const QDBusArgument &operator >> (const QDBusArgument &arg, Portal_wl::Stream &stream)
{
    arg.beginStructure();
    arg >> stream.node_id;

    arg.beginMap();
    while (!arg.atEnd()) {
        QString key;
        QVariant map;
        arg.beginMapEntry();
        arg >> key >> map;
        arg.endMapEntry();
        stream.map.insert(key, map);
    }
    arg.endMap();
    arg.endStructure();

    return arg;
}


Portal_wl::Portal_wl(QString name) : m_sessionTokenCounter(0), m_requestTokenCounter(0)
{
    m_name = name;
}


Portal_wl::~Portal_wl()
{
}
void Portal_wl::setSessionToken(int sessionToken)
{
    m_sessionTokenCounter =sessionToken;
}

void Portal_wl::setRequestToken(int requestToken)
{
    m_requestTokenCounter=requestToken;
}

void Portal_wl::requestScreenSharing(int value, int mouseOnOff)
{
    qDebug()<< m_name << "请求共享屏幕 ！！！！value: " << value << " , mouseOnOff: " << mouseOnOff;
    Selection_Screen_Window_Area = value;
    record_mouse_onOff = mouseOnOff;

    QDBusInterface *createSession = new QDBusInterface(QLatin1String("org.freedesktop.portal.Desktop"),
                                 QLatin1String("/org/freedesktop/portal/desktop"),
                                 QLatin1String("org.freedesktop.portal.ScreenCast"),
                                 QDBusConnection::sessionBus());
    QList<QVariant> arg;

    arg << QVariantMap { { QLatin1String("session_handle_token"),getSessionToken() }, { QLatin1String("handle_token"), getRequestToken() } };
    QDBusMessage result = createSession->callWithArgumentList(QDBus::Block,"CreateSession",arg);


//    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
//                                                          QLatin1String("/org/freedesktop/portal/desktop"),
//                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
//                                                          QLatin1String("CreateSession"));

//    message << QVariantMap { { QLatin1String("session_handle_token"),getSessionToken() }, { QLatin1String("handle_token"), getRequestToken() } };
//    qDebug() << m_name <<__FUNCTION__ << ">>>>>> message: " << message.arguments();
//    QDBusMessage result = QDBusConnection::sessionBus().call(message);
//    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.type(): " <<result.type();
//    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.arguments(): " <<result.arguments();

    bool bo = QDBusConnection::sessionBus().connect(QString(),
                                                    result.path(),
                                                    QLatin1String("org.freedesktop.portal.Request"),
                                                    QLatin1String("Response"),
                                                    this,
                                                    SLOT(slot_gotCreateSessionResponse(uint, QVariantMap)));
    qDebug() << m_name << ">>> CreateSession！！！" << bo;

//    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
//    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * watcher) {
//        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
//        if (reply.isError()) {
//            qWarning()<< m_name << "Couldn't get reply";
//            qWarning() << m_name << "Error: " << reply.error().message();
//        } else {
//            qDebug() << m_name << "Begin create portal session";

//            bool bo = QDBusConnection::sessionBus().connect(QString(),
//                                                            reply.value().path(),
//                                                            QLatin1String("org.freedesktop.portal.Request"),
//                                                            QLatin1String("Response"),
//                                                            this,
//                                                            SLOT(slot_gotCreateSessionResponse(uint, QVariantMap)));
//            qDebug() << m_name << ">>> QDBusConnection::sessionBus().connect: " << bo;
//        }
//    });
}


void Portal_wl::slot_gotCreateSessionResponse(uint response, const QVariantMap &results)
{
    qDebug()<< m_name  << __FUNCTION__ <<">>>>>> results: " << results << " , response: " << response;
    if(results.count() == 0) return;

    qDebug()<< m_name  << "Got response from portal CreateSession";

    if (response != 0) {
        qWarning()<< m_name << "Failed to create session: " << response;
        return;
    }
    m_session = results.value(QLatin1String("session_handle")).toString();

    QDBusInterface *createSession = new QDBusInterface(QLatin1String("org.freedesktop.portal.Desktop"),
                                 QLatin1String("/org/freedesktop/portal/desktop"),
                                 QLatin1String("org.freedesktop.portal.ScreenCast"),
                                 QDBusConnection::sessionBus());
    QList<QVariant> arg;

    arg << QVariant::fromValue(QDBusObjectPath(m_session))
        << QVariantMap { { QLatin1String("multiple"), true},
            { QLatin1String("types"), (uint)Selection_Screen_Window_Area },
            { QLatin1String("cursor_mode"), (uint)record_mouse_onOff },
            { QLatin1String("handle_token"), getRequestToken() } };

    qDebug() << m_name <<__FUNCTION__ << ">>>>>> arg: " <<arg;
    QDBusMessage result = createSession->callWithArgumentList(QDBus::Block,"SelectSources",arg);



//    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
//                                                          QLatin1String("/org/freedesktop/portal/desktop"),
//                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
//                                                          QLatin1String("SelectSources"));

//    m_session = results.value(QLatin1String("session_handle")).toString();

//    qDebug()<< m_name  << __FUNCTION__ <<">>> m_session" << m_session;


//    message << QVariant::fromValue(QDBusObjectPath(m_session))
//    << QVariantMap { { QLatin1String("multiple"), true},
//        { QLatin1String("types"), (uint)Selection_Screen_Window_Area },
//        { QLatin1String("cursor_mode"), (uint)record_mouse_onOff },
//        { QLatin1String("handle_token"), getRequestToken() } };

//    qDebug() << m_name  <<__FUNCTION__ << ">>>>>> message: " << message;

//    QDBusMessage result = QDBusConnection::sessionBus().call(message);

    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.type(): " <<result.type();
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result: " <<result;

    bool bo = QDBusConnection::sessionBus().connect(QString(),
                                                    result.path(),
                                                    QLatin1String("org.freedesktop.portal.Request"),
                                                    QLatin1String("Response"),
                                                    this,
                                                    SLOT(slot_gotSelectSourcesResponse(uint, QVariantMap)));
    qDebug() << m_name << ">>> SelectSources！！！ " << bo;

//    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
//    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * watcher) {
//        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
//        if (reply.isError()) {
//            qWarning() << m_name << "Couldn't get reply";
//            qWarning() << m_name << "Error: " << reply.error().message();
//        } else {
//            qDebug()<< m_name  <<__FUNCTION__ << ">>>>>> SelectSources！！！";
//            QDBusConnection::sessionBus().connect(QString(),
//                                                  reply.value().path(),
//                                                  QLatin1String("org.freedesktop.portal.Request"),
//                                                  QLatin1String("Response"),
//                                                  this,
//                                                  SLOT(slot_gotSelectSourcesResponse(uint, QVariantMap)));
//        }
//    });
}


void Portal_wl::slot_gotSelectSourcesResponse(uint response, const QVariantMap &results)
{
    qDebug()<< m_name  << __FUNCTION__ <<">>>>>> results: " << results << " , response: " << response;
//    if(results.count() == 0) return;

    qDebug()<< m_name  << "Got response from portal SelectSources";

    if (response != 0) {
        qWarning() << m_name << "Failed to select sources: " << response;
        return;
    }

    QDBusInterface *createSession = new QDBusInterface(QLatin1String("org.freedesktop.portal.Desktop"),
                                 QLatin1String("/org/freedesktop/portal/desktop"),
                                 QLatin1String("org.freedesktop.portal.ScreenCast"),
                                 QDBusConnection::sessionBus());
    QList<QVariant> arg;

    arg <<  QVariant::fromValue(QDBusObjectPath(m_session))
         << QString() // parent_window
 << QVariantMap { { QLatin1String("handle_token"), getRequestToken() } };

    QDBusMessage result = createSession->callWithArgumentList(QDBus::Block,"Start",arg);

//    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
//                                                          QLatin1String("/org/freedesktop/portal/desktop"),
//                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
//                                                          QLatin1String("Start"));

//    message << QVariant::fromValue(QDBusObjectPath(m_session))
//            << QString() // parent_window
//    << QVariantMap { { QLatin1String("handle_token"), getRequestToken() } };

//    qDebug()<< m_name  <<__FUNCTION__ << "message: " << message.arguments();

//    QDBusMessage result = QDBusConnection::sessionBus().call(message);

    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.type(): " <<result.type();
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result: " <<result;

    bool bo = QDBusConnection::sessionBus().connect(QString(),
                                                    result.path(),
                                                    QLatin1String("org.freedesktop.portal.Request"),
                                                    QLatin1String("Response"),
                                                    this,
                                                    SLOT(slot_gotStartResponse(uint, QVariantMap)));
    qDebug() << m_name << ">>> Start！！！ " << bo;

//    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
//    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * watcher) {
//        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
//        if (reply.isError()) {
//            qWarning() << m_name << "Couldn't get reply";
//            qWarning() << m_name << "Error: " << reply.error().message();
//        } else {
//            qDebug()<< m_name  <<__FUNCTION__ << ">>>>>> Start！！！";
//            QDBusConnection::sessionBus().connect(QString(),
//                                                  reply.value().path(),
//                                                  QLatin1String("org.freedesktop.portal.Request"),
//                                                  QLatin1String("Response"),
//                                                  this,
//                                                  SLOT(slot_gotStartResponse(uint, QVariantMap)));
//        }
//    });
}


void Portal_wl::slot_gotStartResponse(uint response, const QVariantMap &results)
{
    qDebug()<< m_name  << __FUNCTION__ <<">>>>>> results: " << results << " , response: " << response;
    if(results.count() == 0) return;

    qDebug()<< m_name  << "Got response from portal Start";

    if (response != 0) {
        // The system Desktop dialog was canceled
        qDebug()<< m_name  << "Failed to start or cancel dialog: " << response;
        emit signal_portal_cancel(response);
        return;
    }

    Streams streams = qdbus_cast<Streams>(results.value(QLatin1String("streams")));
    if (streams.size() == 0) {
        qDebug()<< m_name  << ">>> 无法获取流";
    }

    qDebug()<< m_name << __FUNCTION__  << "streams.size() : "<<streams.size();
    Stream stream1 = streams.first();
    Stream stream2 = streams.last();

    QDBusInterface *createSession = new QDBusInterface(QLatin1String("org.freedesktop.portal.Desktop"),
                                 QLatin1String("/org/freedesktop/portal/desktop"),
                                 QLatin1String("org.freedesktop.portal.ScreenCast"),
                                 QDBusConnection::sessionBus());
    QList<QVariant> arg;
    arg << QVariant::fromValue(QDBusObjectPath(m_session)) << QVariantMap();
    QDBusMessage result = createSession->callWithArgumentList(QDBus::Block,"OpenPipeWireRemote",arg);
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.type(): " <<result.type();
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result: " <<result;
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> result.arguments().takeFirst(): " << QString::number(result.arguments().takeFirst().value<QDBusUnixFileDescriptor>().fileDescriptor());
    QString vk_fd = QString::number(result.arguments().takeFirst().value<QDBusUnixFileDescriptor>().fileDescriptor());

//    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
//                                                          QLatin1String("/org/freedesktop/portal/desktop"),
//                                                          QLatin1String("org.freedesktop.portal.ScreenCast"),
//                                                          QLatin1String("OpenPipeWireRemote"));
//    message << QVariant::fromValue(QDBusObjectPath(m_session)) << QVariantMap();
//    qDebug()<< m_name << __FUNCTION__  << "message: "<< message;
//    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
//    QDBusPendingReply<QDBusUnixFileDescriptor> reply = pendingCall.reply();
//    qDebug()<< m_name << __FUNCTION__  << "等待回复！！";
//    reply.waitForFinished();
//    qDebug()<< m_name << __FUNCTION__  << "已回复！！";
//    if (reply.isError()) {
//        qWarning() << m_name << "Failed to get fd for node_id";
//    }
//    QString vk_fd = QString::number(reply.value().fileDescriptor());


    QString vk_path = QString::number(stream1.node_id);
    QString vk_path2 = QString::number( stream2.node_id );
    qDebug()<< m_name << __FUNCTION__  << "vk_fd: " << vk_fd << " , vk_path: " << vk_path<< " , vk_path2: " << vk_path2;


    qDebug() << m_name <<__FUNCTION__ << ">>>>>> stream1.map: " << stream1.map;
    qDebug() << m_name <<__FUNCTION__ << ">>>>>> stream2.map: " << stream2.map;

    if(streams.size() == 2){
        emit signal_portal_fd_path( vk_fd, vk_path, vk_path2 );
       }else {
        emit signal_portal_fd_path( vk_fd, vk_path );
    }
    qDebug()<< m_name << __FUNCTION__  << "Success to get fd for node_id";

}


QString Portal_wl::getSessionToken()
{
    m_sessionTokenCounter += 1;
    return QString("u%1").arg(m_sessionTokenCounter);
}

QString Portal_wl::getRequestToken()
{
    m_requestTokenCounter += 1;
    return QString("u%1").arg(m_requestTokenCounter);
}
