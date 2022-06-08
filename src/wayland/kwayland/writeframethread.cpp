#include "writeframethread.h"
#include "waylandintegration.h"
#include "waylandintegration_p.h"
#include "recordadmin.h"

#include <QDebug>
#include <QImage>
WriteFrameThread::WriteFrameThread(WaylandIntegration::WaylandIntegrationPrivate *context, QObject *parent) :
    QThread(parent),
    m_bWriteFrame(false)
{
    m_context = context;
}

//int test = 0;
void WriteFrameThread::run()
{
    if (nullptr == m_context)
        return;

    m_context->m_recordAdmin->m_cacheMutex.lock();
    Global::waylandFrame frame;
    qDebug() << "WriteFrameThread::run()";
    while (m_context->isWriteVideo()) {
        //qDebug() << "m_context->isWriteVideo() " << m_context->isWriteVideo();
        //qDebug() << "m_context->getFrame(frame) " << m_context->getFrame(frame);
        if (m_context->getFrame(frame)) {
            //m_context->m_recordAdmin->m_gstreamProcess->recordFrame(frame._frame,frame._width*frame._height*3/2,frame._time);
            m_context->m_recordAdmin->m_gstVideoWriter->writeVideo(frame);
        }
    }
    m_context->m_recordAdmin->m_cacheMutex.unlock();
}

bool WriteFrameThread::bWriteFrame()
{
    QMutexLocker locker(&m_writeFrameMutex);
    return m_bWriteFrame;
}

void WriteFrameThread::setBWriteFrame(bool bWriteFrame)
{
    QMutexLocker locker(&m_writeFrameMutex);
    m_bWriteFrame = bWriteFrame;
}
