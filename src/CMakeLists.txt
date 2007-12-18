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

set(knetwalk_icons
	${CMAKE_CURRENT_SOURCE_DIR}/pics/hi16-app-knetwalk.png
	${CMAKE_CURRENT_SOURCE_DIR}/pics/hi32-app-knetwalk.png
	${CMAKE_CURRENT_SOURCE_DIR}/pics/hi48-app-knetwalk.png
	${CMAKE_CURRENT_SOURCE_DIR}/pics/hi64-app-knetwalk.png
	${CMAKE_CURRENT_SOURCE_DIR}/pics/hi128-app-knetwalk.png
)
kde4_add_win32_app_icon(knetwalk_SRCS ${knetwalk_icons})
kde4_add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk  ${KDE4_KDEUI_LIBS} kdegames ${KDE4_KNOTIFYCONFIG_LIBS} )

install(TARGETS knetwalk  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES knetwalk.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalkui.rc  DESTINATION  ${DATA_INSTALL_DIR}/knetwalk )
install( FILES knetwalk.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )




