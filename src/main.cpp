
#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <QProcessEnvironment>
#include <DLog>

#include "utils.h"
#include "mainwindow.h"
DWIDGET_USE_NAMESPACE
bool isWaylandProtocol()
{
    QProcessEnvironment e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));
    return XDG_SESSION_TYPE == QLatin1String("wayland") ||  WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive);
}

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setTheme("light");
    a.setOrganizationName("deepin");
    a.setApplicationName("dtk application");
    a.setApplicationVersion("1.0");
    a.setProductIcon(QIcon(":/images/logo.svg"));
    a.setProductName("Dtk Application");
    a.setApplicationDescription("This is a dtk template application.");
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    gst_init(&argc, &argv);

    Utils::isWaylandMode = isWaylandProtocol();
    qDebug() <<  QDateTime::currentDateTime().toString(Qt::DateFormat::LocalDate) <<"当前的系统环境是否是wayland： " << Utils::isWaylandMode;

    MainWindow *w = new MainWindow ();
    w->setMinimumSize(500, 500);
    w->show();

    Dtk::Widget::moveToCenter(w);

    return a.exec();
}
