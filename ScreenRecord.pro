QT += core gui
QT += x11extras
QT += dtkgui dtkwidget
QT += KWaylandClient KI18n multimedia

CONFIG += c++ link_pkgconfig
PKGCONFIG += xcb xcb-util dframeworkdbus dtkwidget libpulse wayland-client gstreamer-1.0 gstreamer-app-1.0 libpipewire-0.3


TARGET = ScreenRecord
TEMPLATE = app

LIBS += -lX11 -lXext -lXtst -lXfixes -lXcursor
LIBS += -lepoxy -lpulse-simple -ldl

HEADERS += \
    src/mainwindow.h \
    src/common/audioutils.h \
    src/common/global.h \
    src/common/tool.h \
    src/common/utils.h \
    src/gstrecord/gstrecordx.h \
    src/wayland/kwayland/waylandintegration_p.h \
    src/wayland/kwayland/waylandintegration.h \
    src/wayland/pipewire/portal_wl.h \
    src/wayland/pipewire/pw_framebuffer.h




SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/common/audioutils.cpp \
    src/common/tool.cpp \
    src/common/utils.cpp \
    src/gstrecord/gstrecordx.cpp \
    src/wayland/kwayland/waylandintegration.cpp \
    src/wayland/pipewire/portal_wl.cpp \
    src/wayland/pipewire/pw_framebuffer.cpp



RESOURCES +=         resources.qrc

DISTFILES +=


