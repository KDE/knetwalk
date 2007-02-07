
add_subdirectory( pics ) 
add_subdirectory( sounds ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

########### next target ###############

set(knetwalk_SRCS 
   cell.cpp 
   highscores.cpp 
   main.cpp 
   mainwindow.cpp )

kde4_automoc(${knetwalk_SRCS})

kde4_add_kcfg_files(knetwalk_SRCS settings.kcfgc )

kde4_add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk  ${KDE4_KDEUI_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS} )

install(TARGETS knetwalk  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES knetwalk.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalkui.rc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalk.desktop  DESTINATION  ${XDG_APPS_DIR} )




#original Makefile.am contents follow:

#SUBDIRS  = pics sounds
#
#INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
#
#
#bin_PROGRAMS       = knetwalk
#knetwalk_LDFLAGS     = $(all_libraries) $(KDE_RPATH)
#knetwalk_LDADD       = $(LIB_KDEGAMES) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_QT)
#knetwalk_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#knetwalk_SOURCES     = cell.cpp highscores.cpp main.cpp mainwindow.cpp settings.kcfgc
#
#knetwalk_METASOURCES = AUTO
#rcdir   = $(kde_datadir)/knetwalk
#rc_DATA = knetwalkui.rc
#
#appdatadir = $(kde_datadir)/knetwalk
#appdata_DATA = eventsrc
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp *.cpp -o $(podir)/knetwalk.pot
#
## for system-wide highscore file
#DESTBIN = $(DESTDIR)$(bindir)/$(bin_PROGRAMS)
#DESTHIGHSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)
#DESTSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)/$(bin_PROGRAMS).scores
#
#install-data-local:
#	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
#	  echo "********************************************************" ;\
#	  echo "" ;\
#	  echo "This game is installed sgid \"games\" to use the" ;\
#	  echo "system-wide highscore file (in "$(HIGHSCORE_DIRECTORY)")." ;\
#	  echo "" ;\
#	  echo "If the system-wide highscore file does not exist, it is" ;\
#          echo "created with the correct ownership and permissions. See the" ;\
#          echo "INSTALL file in \"kdegames/libkdegames/highscore\" for details." ;\
#	  echo "" ;\
#	  echo "********************************************************" ;\
#	fi 
#
#install-exec-hook:
#	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
#	  chown $(highscore_user):$(highscore_group) $(DESTBIN) \
#	  || echo "Error: Could not install the game with correct permissions !!" ;\
#	fi
#
#	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
#		mkdir -p $(DESTHIGHSCORES) && \
#		chown $(highscore_user):$(highscore_group) $(DESTHIGHSCORES) \
#		&& chmod 750 $(DESTHIGHSCORES) \
#	        || echo "Error: Could not create the highscore directory with correct permissions !!" ;\
#	fi
#
#	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
#	   chown $(highscore_user):$(highscore_group) $(DESTBIN) \
#	   || echo "Error: Could not install the game with correct permissions !!" ;\
#	fi
#
#	@if test ${setgid} = true; then \
#	   chmod 2755  $(DESTBIN) \
#	   || echo "Error: Could not install the game with correct permissions !!" ;\
#	fi
#
#	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
#	   touch $(DESTSCORES) && chown $(highscore_user):$(highscore_group) $(DESTSCORES) \
#	   && chmod 0660 $(DESTSCORES) \
#	   || echo "Error: Could not create system-wide highscore file with correct permissions !!" ;\
#	fi
#