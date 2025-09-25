QT -= core gui

TARGET = Graphics1DisplayDLL
TEMPLATE = lib
CONFIG += dll

# Define export macro
DEFINES += GRAPHICS1DISPLAYDLL_LIBRARY

# Graphics1 DLL files
SOURCES += \
    Graphics1DisplayDLL.cpp

HEADERS += \
    Graphics1DisplayDLL.h

# Windows specific
win32 {
    LIBS += -luser32 -lsetupapi
}

# Graphics1 DLL is already compiled
