add_subdirectory( pics ) 
add_subdirectory( sounds ) 


########### next target ###############

set(knetwalk_SRCS 
   main.cpp
   gameview.cpp
   mainwindow.cpp
   abstractgrid.cpp)

ki18n_wrap_ui( knetwalk_SRCS general.ui customgame.ui )

kconfig_add_kcfg_files(knetwalk_SRCS settings.kcfgc )

#kde4_add_app_icon(knetwalk_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-knetwalk.png")
add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk KF5::XmlGui KF5::TextWidgets KF5KDEGames KF5::DBusAddons)

install(TARGETS knetwalk  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES knetwalkui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/knetwalk )
install( DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/knetwalk)
install( PROGRAMS org.kde.knetwalk.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )





