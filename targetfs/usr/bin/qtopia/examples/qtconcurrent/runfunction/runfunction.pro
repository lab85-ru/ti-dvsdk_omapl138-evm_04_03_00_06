TEMPLATE = app
TARGET += 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
