TEMPLATE = subdirs
SUBDIRS = tutorial1 tutorial2 tutorial3 tutorial4 tutorial5

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS qtestlib.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/qtestlib
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
