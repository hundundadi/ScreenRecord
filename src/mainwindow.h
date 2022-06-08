#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDebug>
#include <QtConcurrent>

#include "wayland/kwayland/waylandintegration.h"
#include "common/utils.h"
#include "common/audioutils.h"

#include <QDir>
#include <QLibraryInfo>

#include <DPushButton>
#include <QObject>

DWIDGET_USE_NAMESPACE
class MainWindow : public DWidget
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
private:
    /**
     * @brief 初始化成员变量
     */
    void initMemberVariables();
    /**
     * @brief 初始化界面
     */
    void initMainWindow();

    void releaseMemberVariables();

public slots:
    void onStartRecord();
    void onStopRecord();
protected:
    void GstStartRecord();
    void GstStopRecord();
private:
    DPushButton *m_startBtn;
    DPushButton *m_stopBtn;
    QString saveBaseName;
    QString savePath;

    /**
     * @brief 录屏的帧率
     */
    int m_framerate;
    /**
     * @brief 录屏的范围
     */
    QRect m_recordRect;

    /**
     * @brief 录制的声音类型： 混音 单麦克风音频 单系统音频
     */
    int recordAudioInputType;
    /**
     * @brief 是否录制鼠标
     */
    bool m_isRecordMouse;
    GstRecordX *m_gstRecordX;

};

#endif // MAINWINDOW_H
