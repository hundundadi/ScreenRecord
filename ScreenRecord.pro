QT += core gui
QT += x11extras
QT += dtkgui dtkwidget
QT += KWaylandClient KI18n multimedia


TARGET = ScreenRecord
TEMPLATE = app
CONFIG += c++ link_pkgconfig
PKGCONFIG += xcb xcb-util dframeworkdbus dtkwidget libpulse wayland-client gstreamer-1.0 gstreamer-app-1.0

LIBS += -lX11 -lXext -lXtst -lXfixes -lXcursor
LIBS += -lepoxy -lpulse-simple -ldl

HEADERS += \
    src/utils.h \
    src/mainwindow.h \
    src/waylandrecord/waylandrecord.h \
    src/waylandrecord/waylandintegration.h \
    src/x11/x11record.h \
    src/x11/mpulseaudioserver.h \
    src/waylandrecord/waylandintegration_p.h \
    src/waylandrecord/gstreamerprocess.h \
    src/AudioDevices.h \
    src/waylandrecord/writeframethread.h \
    src/waylandrecord/recordadmin.h \
    src/global.h \
    src/tool.h \
    src/waylandrecord/gstvideowriter.h \
    src/waylandrecord/openaudioinput.h \
    src/waylandrecord/libcam/libcam/cameraconfig.h \
    src/waylandrecord/libcam/libcam/camview.h \
    src/waylandrecord/libcam/libcam/options.h \
    src/waylandrecord/libcam/libcam_audio/audio_portaudio.h \
    src/waylandrecord/libcam/libcam_audio/audio.h \
    src/waylandrecord/libcam/libcam_audio/gview.h \
    src/waylandrecord/libcam/libcam_audio/gviewaudio.h \
    src/waylandrecord/load_libs.h \
    src/waylandrecord/audio/audiowatcher.h \
    src/waylandrecord/audio/vnwaveform.h \
    src/gst/gstrecordx.h

SOURCES += \
    src/main.cpp \
    src/utils.cpp \
    src/mainwindow.cpp \
    src/waylandrecord/waylandrecord.cpp \
    src/waylandrecord/waylandintegration.cpp \
    src/x11/x11record.cpp \
    src/x11/mpulseaudioserver.cpp \
    src/waylandrecord/gstreamerprocess.cpp \
    src/AudioDevices.c \
    src/waylandrecord/writeframethread.cpp \
    src/waylandrecord/recordadmin.cpp \
    src/tool.cpp \
    src/waylandrecord/gstvideowriter.cpp \
    src/waylandrecord/openaudioinput.cpp \
    src/waylandrecord/libcam/libcam/cameraconfig.c \
    src/waylandrecord/libcam/libcam/camview.c \
    src/waylandrecord/libcam/libcam/options.c \
    src/waylandrecord/libcam/libcam_audio/audio_fx.c \
    src/waylandrecord/libcam/libcam_audio/audio_portaudio.c \
    src/waylandrecord/libcam/libcam_audio/audio.c \
    src/waylandrecord/load_libs.c \
    src/waylandrecord/audio/audiowatcher.cpp \
    src/waylandrecord/audio/vnwaveform.cpp \
    src/gst/gstrecordx.cpp


    src/waylandrecord/load_libs.c

RESOURCES +=         resources.qrc

DISTFILES +=


