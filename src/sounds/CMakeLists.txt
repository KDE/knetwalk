kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/knetwalk/sounds FILES click.wav connect.wav start.wav turn.wav win.wav )

kde4_footer()



#original Makefile.am contents follow:

#
#sounds_DATA = click.wav connect.wav start.wav turn.wav win.wav
#
#soundsdir = $(kde_datadir)/knetwalk/sounds
