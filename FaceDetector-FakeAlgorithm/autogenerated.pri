TOP_DIR = $$PWD/.. 
TARGET_DIR = $$TOP_DIR/target 
BIN_DIR = $$TARGET_DIR/bin
LIB_DIR = $$TARGET_DIR/lib
INCLUDE_DIR = $$TARGET_DIR/include
DESTDIR = $$BIN_DIR 

CONFIG (release, debug|release) { TARGET = FaceDetector-FakeAlgorithm-1.0 }
CONFIG (debug, debug|release)   { TARGET = FaceDetector-FakeAlgorithm-1.0-d }

V_MAJ = 1
V_MIN = 0
V_PAT = 0

unix: { 
VER_MAJ = $${V_MAJ}
VER_MIN = $${V_MIN}
VER_PAT = $${V_PAT}
}

DEFINES += 'VER_MAJ=$$V_MAJ'
DEFINES += 'VER_MIN=$$V_MIN'
DEFINES += 'VER_PAT=$$V_PAT'