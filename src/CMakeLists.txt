add_subdirectory(pics)
add_subdirectory(sounds)

add_executable(knetwalk)

target_sources(knetwalk PRIVATE
    abstractgrid.cpp
    abstractgrid.h
    gameview.cpp
    gameview.h
    globals.h
    main.cpp
    mainwindow.cpp
    mainwindow.h

    knetwalk.qrc
)

ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX KNETWALK VERSION_HEADER knetwalk_version.h)

ki18n_wrap_ui(knetwalk general.ui customgame.ui)

kconfig_add_kcfg_files(knetwalk settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-knetwalk.png")
ecm_add_app_icon(knetwalk ICONS ${ICONS_SRCS})

target_link_libraries(knetwalk
    KDEGames6
    KF6::CoreAddons
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::TextWidgets
    KF6::XmlGui
    Qt6::QuickWidgets
)

install(TARGETS knetwalk  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/knetwalk)
install(PROGRAMS org.kde.knetwalk.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.knetwalk.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
