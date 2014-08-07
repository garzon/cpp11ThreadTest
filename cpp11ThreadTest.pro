TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt
QMAKE_CXXFLAGS += -Wl,--no-as-needed -std=c++11 -pthread
SOURCES += main.cpp

