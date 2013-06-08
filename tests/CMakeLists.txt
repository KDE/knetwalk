set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

kde4_add_unit_test( abstractgridtest abstractgridtest.cpp ../src/abstractgrid.cpp )
target_link_libraries( abstractgridtest ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} )
