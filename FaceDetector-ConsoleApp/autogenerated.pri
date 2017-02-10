TOP_DIR = $$PWD/.. 
TARGET_DIR = $$TOP_DIR/target 
BIN_DIR = $$TARGET_DIR/bin
LIB_DIR = $$TARGET_DIR/lib
INCLUDE_DIR = $$TARGET_DIR/include
DESTDIR = $$BIN_DIR 

CONFIG (release, debug|release) { TARGET = fd }
CONFIG (debug, debug|release) { TARGET = fd-d }


INCLUDEPATH += \
	$$INCLUDE_DIR

CONFIG (release, debug|release) {
	LIBS += \ 
	-L$$BIN_DIR/ -lFaceDetector-Core-1.0
}
CONFIG (debug, debug|release) {
	LIBS += \ 
	-L$$BIN_DIR/ -lFaceDetector-Core-1.0-d
}