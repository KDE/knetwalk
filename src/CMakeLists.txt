add_subdirectory( pics ) 
add_subdirectory( sounds ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )

########### next target ###############

set(knetwalk_SRCS 
   main.cpp
   gameview.cpp
   mainwindow.cpp
   abstractgrid.cpp)

kde4_add_ui_files( knetwalk_SRCS general.ui customgame.ui )

kde4_add_kcfg_files(knetwalk_SRCS settings.kcfgc )

kde4_add_app_icon(knetwalk_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-knetwalk.png")
kde4_add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk  ${KDE4_KDEUI_LIBS} ${QT_QTDECLARATIVE_LIBRARY} ${KDECLARATIVE_LIBRARIES} kdegames)

install(TARGETS knetwalk  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES knetwalkui.rc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/knetwalk)
install( PROGRAMS knetwalk.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )





