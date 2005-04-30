SUBDIRS  = pics sounds

INCLUDES = $(all_includes)

bin_PROGRAMS       = knetwalk
knetwalk_LDFLAGS     = $(all_libraries) $(KDE_RPATH)
knetwalk_LDADD       = -lkdegames $(LIB_KDEGAMES) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
knetwalk_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
knetwalk_SOURCES     = cell.cpp highscores.cpp main.cpp mainwindow.cpp settings.kcfgc

knetwalk_METASOURCES = AUTO
rcdir   = $(kde_datadir)/knetwalk
rc_DATA = knetwalkui.rc

appdatadir = $(kde_datadir)/knetwalk
appdata_DATA = eventsrc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cpp $(podir)/knetwalk.pot

