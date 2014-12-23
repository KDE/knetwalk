project(knetwalk)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 REQUIRED COMPONENTS
  CoreAddons 
  Config 
  ItemModels 
  WidgetsAddons 
  WindowSystem 
  Codecs 
  Archive
  Config 
  DBusAddons
  DNSSD
  Declarative
  I18n
  GuiAddons
  Service
  ConfigWidgets
  ItemViews
  IconThemes
  Completion
  JobWidgets
  TextWidgets
  GlobalAccel
  XmlGui
  Crash
  Bookmarks
  KIO
  NotifyConfig
  NewStuff
  KDELibs4Support)
find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})


#if(NOT COMPILING_KDEGAMES)	
	#find_package(KDE4 REQUIRED)
	#include(KDE4Defaults)
	
	#include(MacroLibrary)
	#find_package(KDEGames 4.9.0 REQUIRED)

	#add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	#add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	#include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})
#endif(NOT COMPILING_KDEGAMES)

add_subdirectory(src) 
add_subdirectory(themes) 
add_subdirectory(doc)

if(KDE4_BUILD_TESTS)
    add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)
