TEMPLATE = app
TARGET   = xalm 
LANGUAGE = C++
DESTDIR  = $$(DFDZPLAT)/bin

QT           += xml network sql
CONFIG       += qt thread debug

TMP          = ../tmp/$$TARGET
OBJECTS_DIR  = $$TMP/obj
MOC_DIR      = $$TMP/moc

FORMS       += forms/wtdlg.ui \
               forms/testdlg.ui \
               forms/remarkdlg.ui

HEADERS     += mainwin.h \
               displaysetting.h \
               mdichild.h \
               alarmchild.h \
               eventchild.h \
               textchild.h \
	       wtdlg.h \
	       testdlg.h \
	       remarkdlg.h \
	       alarmevent.h 

SOURCES     += main.cpp \
               displaysetting.cpp \
               mainwin.cpp \
               mdichild.cpp \
               alarmchild.cpp \
               eventchild.cpp \
               textchild.cpp \
	       wtdlg.cpp \
	       remarkdlg.cpp \
	       testdlg.cpp \
	       alarmevent.cpp

RESOURCES   += xalm.qrc

