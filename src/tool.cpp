#include "tool.h"

#include <QDebug>
#include <QDateTime>
Tool::Tool()
{

}

void Tool::out(QString mes)
{
    qDebug() << QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) << __LINE__ << mes;
}
