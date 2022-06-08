#ifndef UTILS_H
#define UTILS_H

#include <DWidget>
#include <DSysInfo>
#include <DWindowManagerHelper>

#include<QObject>
#include <QtX11Extras/QX11Info>
#include <X11/extensions/shape.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

class Utils: public QObject
{
public:
    static bool isWaylandMode;

public:
    Utils();

    static void passInputEvent(int wid);

    /**
     * @brief 取消对目标区域的穿透处理
     * @param wid  窗口id
     * @param x  区域位置x坐标
     * @param y  区域位置y坐标
     * @param width  区域宽
     * @param height  区域高
     */
    static void cancelInputEvent(const int wid, const short x, const short y, const unsigned short width, const unsigned short height);

    /**
     * @brief 对目标区域做穿透处理
     * @param 窗口id
     * @param 区域位置x坐标
     * @param 区域位置y坐标
     * @param 区域宽
     * @param 区域高
     */
    static void getInputEvent(const int wid, const short x, const short y, const unsigned short width, const unsigned short height);

    static QString getCpuModelName();

};

#endif // UTILS_H
