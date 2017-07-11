cmake_minimum_required(VERSION 2.8)
# Handling of relative directories by link_directories()
cmake_policy(SET CMP0015 NEW) 

set(PROJECT_NAME remote-camera)
set(SERVER_NAME server)

project( ${PROJECT_NAME} )

find_package( OpenCV REQUIRED )
if(OpenCV_FOUND)
  message(">> OpenCV version: ${OpenCV_VERSION}")
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  link_directories( ${OpenCV_LIBS} )
endif(OpenCV_FOUND)

set(SRC
	source/main.cc
	source/utility.h
)

add_executable( ${SERVER_NAME} ${SRC} )
target_link_libraries( ${SERVER_NAME} ${OpenCV_LIBS} )

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(TARGETS ${SERVER_NAME} RUNTIME DESTINATION "install")
install(FILES "${CMAKE_SOURCE_DIR}/source/_INSTALL/launch.bat" DESTINATION "install")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/source/_INSTALL/out" DESTINATION "install")

find_path(OpenCV_DLL_release NAMES opencv_core320.dll PATHS $ENV{OpenCV_DIR})
install(FILES "${OpenCV_DLL_release}/opencv_core320.dll" DESTINATION "install")
find_path(OpenCV_DLL_debug NAMES opencv_core320d.dll PATHS $ENV{OpenCV_DIR})
install(FILES "${OpenCV_DLL_debug}/opencv_core320d.dll" DESTINATION "install")
