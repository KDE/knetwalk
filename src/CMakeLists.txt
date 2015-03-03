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

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-apps-knetwalk.png")
ecm_add_app_icon(knetwalk_SRCS ICONS ${ICONS_SRCS})
add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk KF5::XmlGui KF5::TextWidgets KF5KDEGames KF5::DBusAddons)

install(TARGETS knetwalk  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES knetwalkui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/knetwalk )
install( DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/knetwalk)
install( PROGRAMS org.kde.knetwalk.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )





