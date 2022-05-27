#ifndef GLOBAL_H
#define GLOBAL_H
#include <QObject>
class Global: public QObject
{
public:
    //缓存帧
    struct waylandFrame {
        //时间戳
        int64_t _time;
        //索引
        int _index;
        int _width;
        int _height;
        int _stride;
        unsigned char *_frame;
    };

    struct videoArgs {
        int width;
        int height;
        QString format = "RGBA";
        int framerate;
        int videotype;
        int drawmouse;

        QList<QString> getStr(){
            QList<QString> list;
            list.append("width: "); list << QString::number(width);
            list.append("height: "); list << QString::number(height);
            list.append("format: "); list << format;
            list.append("framerate: "); list << QString::number(framerate);
            list.append("videotype: "); list << QString::number(videotype);
            list.append("drawmouse: "); list << QString::number(drawmouse);
            return list;
        }
    };

    struct audioArgs {
        int framerate = 48000;
        int audiotype; //0:mic 1:sys 2:mix
        QString format = "F32LE";
        int channel;
        int layout;
    };
};


#endif // GLOBAL_H
