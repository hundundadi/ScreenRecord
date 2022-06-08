QT += core gui
QT += x11extras
QT += dtkgui dtkwidget
QT += KWaylandClient KI18n multimedia

CONFIG += c++ link_pkgconfig
PKGCONFIG += xcb xcb-util dframeworkdbus dtkwidget libpulse wayland-client gstreamer-1.0 gstreamer-app-1.0


TARGET = ScreenRecord
TEMPLATE = app

LIBS += -lX11 -lXext -lXtst -lXfixes -lXcursor
LIBS += -lepoxy -lpulse-simple -ldl

HEADERS += \
    src/mainwindow.h \
    src/audio/audiowatcher.h \
    src/common/global.h \
    src/common/tool.h \
    src/common/utils.h \
    src/gstrecord/gstrecordx.h \
    src/wayland/kwayland/recordadmin.h \
    src/wayland/kwayland/waylandintegration_p.h \
    src/wayland/kwayland/waylandintegration.h \
    src/wayland/kwayland/waylandrecord.h \
    src/wayland/kwayland/writeframethread.h \
    src/wayland/pipewire/portal_wl.h \
    src/wayland/waylandrecord.h \
    src/x11/x11record.h




SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/audio/audiowatcher.cpp \
    src/common/tool.cpp \
    src/common/utils.cpp \
    src/gstrecord/gstrecordx.cpp \
    src/wayland/kwayland/recordadmin.cpp \
    src/wayland/kwayland/waylandintegration.cpp \
    src/wayland/kwayland/waylandrecord.cpp \
    src/wayland/kwayland/writeframethread.cpp \
    src/wayland/pipewire/portal_wl.cpp \
    src/wayland/waylandrecord.cpp \
    src/x11/x11record.cpp



RESOURCES +=         resources.qrc

DISTFILES +=


