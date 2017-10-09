cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(QPOP)


#set(CMAKE_CXX_FLAGS "-std=c++17")


find_package(wxWidgets)
find_path(websocketpp_INCLUDE websocketpp/)
find_path(boost_library_directory C:/boost)
include_directories(${wxWidgets_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${boost_library_directory})
include_directories(${websocketpp_INCLUDE})
include_directories(./Header)
set(EXECUTABLE_OUTPUT_PATH ../executable/)
if(wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})
	add_executable(
		QPOP WIN32
		Source/qpop_app.cpp
		Source/img_processing.cpp
		Source/qpop_frame.cpp
		Source/screen_capturer.cpp
		Source/settings_class.cpp
		Source/qpop_server.cpp
	)
	target_link_libraries(QPOP 
		${wxWidgets_LIBRARIES}
		${Boost_LIBRARIES})
	target_compile_definitions(QPOP PRIVATE -DBOOST_NO_EXCEPTIONS)

endif()
