QT       += core gui charts network
greaterThan(QT_MAJOR_VERSION, 6): QT += widgets

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Testy_UAR.cpp \
    ARX.cpp \
    RegulatorPID.cpp \
    UAR.cpp \
    Generator.cpp \
    GeneratorSinusoida.cpp \
    GeneratorProstokatny.cpp \
    State.cpp \
    Test.cpp \
    DialogARX.cpp \
    ListWithWxtremes.cpp \
    QSaveState.cpp \
    DialogDebug.cpp \
    ARXCoefficientItem.cpp \
    GeneratorSkokJednostkowy.cpp \
    QTimerState.cpp \
    networkmanager.cpp \
    tcp.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    workdialog.cpp
    
HEADERS += \
    BUILD_DEFINE.h \
    MainWindow.h \
    ARX.h \
    RegulatorPID.h \
    UAR.h \
    Generator.h \
    GeneratorSinusoida.h \
    GeneratorProstokatny.h \
    GeneratorSkokJednostkowy.hpp \
    ListWithWxtremes.hpp \
    QTimerState.hpp \
    State.h \
    Test.h \
    DialogARX.h \
    QSaveState.hpp \
    DialogDebug.hpp \
    ARXCoefficientItem.hpp \
    networkmanager.h \
    tcp.h \
    tcpclient.h \
    tcpserver.h \
    workdialog.h


FORMS += \
    DialogARX.ui \
    MainWindow.ui \
    ARXCoefficientItem.ui \
    DialogDebug.ui \
    workdialog.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

