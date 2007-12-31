add_subdirectory( pics ) 
add_subdirectory( sounds ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )

########### next target ###############

set(knetwalk_SRCS 
   cell.cpp 
   main.cpp 
   mainwindow.cpp
   renderer.cpp )

kde4_add_kcfg_files(knetwalk_SRCS settings.kcfgc )

kde4_add_app_icon(knetwalk_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-knetwalk.png")
kde4_add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk  ${KDE4_KDEUI_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS} )

install(TARGETS knetwalk  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES knetwalk.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalkui.rc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalk.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )





