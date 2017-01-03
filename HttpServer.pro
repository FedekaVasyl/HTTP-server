QT += core
QT -= gui

CONFIG += c++11

TARGET = HttpServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    socket.cpp \
    client.cpp \
    log.cpp

HEADERS += \
    client.h \
    server.h \
    socket.h \
    log.h
