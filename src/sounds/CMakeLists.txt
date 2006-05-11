kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/knetwalk/sounds FILES click.wav connect.wav start.wav turn.wav win.wav )

kde4_footer()



#original Makefile.am contents follow:

#
#sounds_DATA = click.wav connect.wav start.wav turn.wav win.wav
#
#soundsdir = $(kde_datadir)/knetwalk/sounds
