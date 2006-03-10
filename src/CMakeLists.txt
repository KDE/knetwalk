kde4_header()

add_subdirectory( pics ) 
add_subdirectory( sounds ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(knetwalk_SRCS 
   cell.cpp 
   highscores.cpp 
   main.cpp 
   mainwindow.cpp )

kde4_automoc(${knetwalk_SRCS})

kde4_add_kcfg_files(knetwalk_SRCS settings.kcfgc )

kde4_add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk  ${LIB_KDEUI} kdegames )

install_targets(/bin knetwalk )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/knetwalk FILES eventsrc )
install_files( ${DATA_INSTALL_DIR}/knetwalk FILES knetwalkui.rc )

kde4_footer()



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
