#-------------------------------------------------
#
# Project created by QtCreator 2016-11-17T14:31:23
#
#-------------------------------------------------

QT       += core gui serialport serialbus printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DemoIoTPhase2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stationform.cpp \
    qcustomplot.cpp \
    uhv4serialinterface.cpp \
    stationfunctions.cpp \
    canint.cpp \
    email/emailaddress.cpp \
    email/mimeattachment.cpp \
    email/mimecontentformatter.cpp \
    email/mimefile.cpp \
    email/mimehtml.cpp \
    email/mimeinlinefile.cpp \
    email/mimemessage.cpp \
    email/mimemultipart.cpp \
    email/mimepart.cpp \
    email/mimetext.cpp \
    email/quotedprintable.cpp \
    email/smtpclient.cpp

HEADERS  += mainwindow.h \
    stationform.h \
    qcustomplot.h \
    config.h \
    uhv4errormessage.h \
    uhv4receivemessage.h \
    uhv4serialinterface.h \
    windowcommand.h \
    canint.h \
    email/emailaddress.h \
    email/mimeattachment.h \
    email/mimecontentformatter.h \
    email/mimefile.h \
    email/mimehtml.h \
    email/mimeinlinefile.h \
    email/mimemessage.h \
    email/mimemultipart.h \
    email/mimepart.h \
    email/mimetext.h \
    email/quotedprintable.h \
    email/smtpclient.h \
    email/smtpexports.h \
    email/SmtpMime

FORMS    += mainwindow.ui \
    stationform.ui

RESOURCES += \
    rsc.qrc
