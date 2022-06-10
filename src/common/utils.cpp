#include "utils.h"
#include <QtX11Extras/QX11Info>
#include <X11/extensions/shape.h>

bool Utils::isWaylandMode = false;
bool Utils::isFFmpegEnv = false;
bool Utils::isPipewireMode = false;

Utils::Utils()
{

}

void Utils::passInputEvent(int wid)
{
    // Wayland 事件穿透
    if (Utils::isWaylandMode) {
        QWidget *widget = QWidget::find(static_cast<WId>(wid));
        if (widget && widget->windowHandle()) {
            widget->windowHandle()->setMask(QRegion(0, 0, 1, 1));
        }
        return;
    }


    XRectangle *reponseArea = new XRectangle;
    reponseArea->x = 0;
    reponseArea->y = 0;
    reponseArea->width = 0;
    reponseArea->height = 0;

    XShapeCombineRectangles(QX11Info::display(), static_cast<unsigned long>(wid), ShapeInput, 0, 0, reponseArea, 1, ShapeSet, YXBanded);
    delete reponseArea;

}

void Utils::cancelInputEvent(const int wid, const short x, const short y, const unsigned short width, const unsigned short height)
{
    if (Utils::isWaylandMode == true)
        return;
    XRectangle *reponseArea = new XRectangle;
    reponseArea->x = x;
    reponseArea->y = y;
    reponseArea->width = width;
    reponseArea->height = height;

    XShapeCombineRectangles(QX11Info::display(), static_cast<unsigned long>(wid), ShapeInput, 0, 0, reponseArea, 0, ShapeSet, YXBanded);
    delete reponseArea;

}

void Utils::getInputEvent(const int wid, const short x, const short y, const unsigned short width, const unsigned short height)
{
    // Wayland 事件穿透
    if (Utils::isWaylandMode) {
//        QWidget *widget = QWidget::find(static_cast<WId>(wid));
//        if (widget && widget->windowHandle()) {
//            widget->windowHandle()->setMask(QRegion(0, 0, 1, 1));
//        }
        return;
    }
    XRectangle *reponseArea = new XRectangle;
    reponseArea->x = x;
    reponseArea->y = y;
    reponseArea->width = width;
    reponseArea->height = height;

    /*
     *XShapeCombineRectangles:
     * param1:
     * param2:
     * param3:
     * param4:
     * param5:
     * param6:为NULL（参数为XRectangle*类型），那整个窗口都将被穿透
     * param7:控制设置穿透的，为0时表示设置鼠标穿透，为1时表示取消鼠标穿透，当取消设置鼠标穿透的时候，必须设置区域，
     * param8:
     * param9:
     */
    XShapeCombineRectangles(QX11Info::display(), static_cast<unsigned long>(wid), ShapeInput, 0, 0, reponseArea, 1, ShapeSubtract, YXBanded);
    delete reponseArea;

}

QString Utils::getCpuModelName()
{
    return DSysInfo::cpuModelName();

}


