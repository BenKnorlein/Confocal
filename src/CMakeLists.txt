project(Confocal_src)
set(CMAKE_CXX_STANDARD 11)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)


FIND_PACKAGE(Qt5 COMPONENTS Core Widgets GUI REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED )
FIND_PACKAGE(GLM REQUIRED )

#Allow a console window on Windows for debugging
OPTION(WITH_CONSOLE "Build with a command line for debugging" OFF)
IF(WIN32)
	IF(WITH_CONSOLE)
		ADD_DEFINITIONS(-DWITH_CONSOLE)
	ELSE()
		SET(GUI_TYPE WIN32)
		SET(QT_USE_QMAIN true)
	ENDIF()
ENDIF(WIN32)


set(CMAKE_AUTOMOC ON)
SET(Confocal_FORMS
	ui/ConfocalDockWidget.ui
	ui/ConfocalMainWindow.ui
)
QT5_WRAP_UI(Confocal_FORMS_HEADERS_GEN ${Confocal_FORMS})

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5GUI_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
)

add_executable(Confocal
  main.cpp
  MSAC.cpp
  MSAC.h
  SphereFit.cpp
  SphereFit.h
  Volume.cpp
  Volume.h
  ConfocalDockWidget.h
  ConfocalDockWidget.cpp
  ConfocalMainWindow.h
  ConfocalMainWindow.cpp
  ConfocalOpenGLWidget.h
  ConfocalOpenGLWidget.cpp
  LoadDataAction.cpp
  LoadDataAction.h
  VolumeSliceRender.h
  VolumeSliceRender.cpp
  VolumeSliceRenderShader.h
  VolumeSliceRenderShader.cpp
  Data.cpp
  Data.h
  Shader.cpp
  Shader.h
  IcoSphereCreator.cpp
  IcoSphereCreator.h
  ${Confocal_FORMS_HEADERS_GEN}
)

target_link_libraries(Confocal
  ${GLEW_LIBRARY}
  ${OPENGL_LIBRARY}
  ${OpenCV_LIBS}
  ${Qt5Core_LIBRARIES}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5GUI_LIBRARIES}
  ${GLM_LIBRARIES}
)

