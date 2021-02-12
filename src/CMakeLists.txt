add_subdirectory(pics)
add_subdirectory(sounds)


########### next target ###############

set(knetwalk_SRCS 
    main.cpp
    gameview.cpp
    mainwindow.cpp
    abstractgrid.cpp
    knetwalk.qrc
)

ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX KNETWALK VERSION_HEADER knetwalk_version.h)

ki18n_wrap_ui( knetwalk_SRCS general.ui customgame.ui)

kconfig_add_kcfg_files(knetwalk_SRCS settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-knetwalk.png")
ecm_add_app_icon(knetwalk_SRCS ICONS ${ICONS_SRCS})
add_executable(knetwalk ${knetwalk_SRCS})

target_link_libraries(knetwalk
    KF5::Crash
    KF5::DBusAddons
    KF5::TextWidgets
    KF5::XmlGui
    KF5KDEGames
)

install(TARGETS knetwalk  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/knetwalk)
install(PROGRAMS org.kde.knetwalk.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.knetwalk.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
