project(knetwalk)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test Declarative)
find_package(KF5 REQUIRED COMPONENTS
  CoreAddons 
  Config 
  WidgetsAddons 
  Config 
  DBusAddons
  Declarative
  I18n
  GuiAddons
  ConfigWidgets
  ItemViews
  IconThemes
  Completion
  JobWidgets
  TextWidgets
  XmlGui
  KIO
  NotifyConfig
  KDELibs4Support)
find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)


add_subdirectory(src) 
add_subdirectory(themes) 
add_subdirectory(doc)

if(KDE4_BUILD_TESTS)
    add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
