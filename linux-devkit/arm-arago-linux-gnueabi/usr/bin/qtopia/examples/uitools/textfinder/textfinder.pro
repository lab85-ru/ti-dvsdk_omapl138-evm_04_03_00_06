CONFIG		+= uitools
HEADERS		= textfinder.h
RESOURCES	= textfinder.qrc
SOURCES		= textfinder.cpp main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/uitools/textfinder
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro forms
sources.path = $$[QT_INSTALL_EXAMPLES]/uitools/textfinder
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
