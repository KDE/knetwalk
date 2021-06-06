set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( abstractgridtest abstractgridtest.cpp ../src/abstractgrid.cpp )
add_test(NAME abstractgridtest COMMAND abstractgridtest)
ecm_mark_as_test(abstractgridtest)
target_link_libraries( abstractgridtest Qt::Gui Qt::Test KF5::XmlGui)
