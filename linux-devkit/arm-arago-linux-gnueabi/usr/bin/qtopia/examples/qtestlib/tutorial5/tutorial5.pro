SOURCES = benchmarking.cpp
CONFIG  += qtestlib

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial5
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial5
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000C60F
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}
