TEMPLATE  = subdirs
CONFIG   += ordered
QT += widgets
SUBDIRS   = \
    qscispinbox\
    aggregation \
    extensionsystem \
    utils \
    tlmapcontrol \
    qwt \
    runguard

BREAKPAD {
    SUBDIRS += libcrashreporter-qt
}

win32 {
SUBDIRS   += \
    zlib
}
SUBDIRS   += \
    quazip
SDL {
SUBDIRS += sdlgamepad
}

!LIGHTWEIGHT_GCS {
    SUBDIRS += glc_lib
}

SUBDIRS +=
