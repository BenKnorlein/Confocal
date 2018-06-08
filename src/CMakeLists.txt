project(Confocal_src)
set(CMAKE_CXX_STANDARD 11)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

OPTION(WITH_UI "Build with a command line for debugging" OFF)

if(WITH_UI)
	FIND_PACKAGE(Qt5 COMPONENTS Core Widgets GUI PrintSupport REQUIRED)
	find_package(OpenGL REQUIRED)
	find_package(GLEW REQUIRED)
	ADD_DEFINITIONS(-DWITH_UI)	
ENDIF()

FIND_PACKAGE(OpenCV REQUIRED )
FIND_PACKAGE(glm REQUIRED )
FIND_PACKAGE(Matlab REQUIRED)

#Set Matlab mclmcrrt library
IF(WIN32)
	list(GET Matlab_LIBRARIES 0 Matlab_LIBRARY)
	get_filename_component(Matlab_LIBRARY_DIR ${Matlab_LIBRARY} DIRECTORY)
	SET(Matlab_LIBRARIES ${Matlab_LIBRARY_DIR}/mclmcrrt.lib)
ELSE()
	list(GET Matlab_LIBRARIES 0 Matlab_LIBRARY)
	get_filename_component(Matlab_LIBRARY_DIR ${Matlab_LIBRARY} DIRECTORY)
	SET(Matlab_LIBRARIES ${Matlab_LIBRARY_DIR}/../../runtime/glnxa64/libmwmclmcrrt.so)
ENDIF()

#Allow a console window on Windows for debugging
IF(WIN32)
	IF(WITH_CONSOLE)
		SET(GUI_TYPE WIN32)
		SET(QT_USE_QMAIN true)
		OPTION(WITH_CONSOLE "Build with a command line for debugging" ON)
		IF(WITH_CONSOLE)
			ADD_DEFINITIONS(-DWITH_CONSOLE)		
		ENDIF()
	ELSE()
		ADD_DEFINITIONS(-DWITH_CONSOLE)
	ENDIF()
ENDIF(WIN32)

if(WITH_UI)
	set(CMAKE_AUTOMOC ON)
	SET(Confocal_FORMS
		ui/ConfocalDockWidget.ui
		ui/ConfocalMainWindow.ui
		ui/PlotWindow.ui
		ui/ConfocalSliceView.ui
	)
	QT5_WRAP_UI(Confocal_FORMS_HEADERS_GEN ${Confocal_FORMS})

	include_directories(
	  ${OPENGL_INCLUDE_DIR}
	  ${GLEW_INCLUDE_DIRS}
	  ${Qt5Core_INCLUDE_DIRS}
	  ${Qt5Widgets_INCLUDE_DIRS}
	  ${Qt5GUI_INCLUDE_DIRS}
	  ${Qt5PrintSupport_INCLUDE_DIRS}
	)
ENDIF()

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${OpenCV_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${Matlab_INCLUDE_DIRS}
)

set(SOURCES
	main.cpp
	MSAC.cpp
	MSAC.h
	SphereFit.cpp
    SphereFit.h
    Volume.cpp
    Volume.h
	LoadDataAction.cpp
    LoadDataAction.h
	RefineMeshAction.h
    RefineMeshAction.cpp
	Data.cpp
    Data.h
	IcoSphereCreator.cpp
	IcoSphereCreator.h
	GenerateMasksAction.h
    GenerateMasksAction.cpp
    UpdateResultsAction.h
    UpdateResultsAction.cpp
)

if(WITH_UI)
	set(SOURCES
		${SOURCES}
		  ConfocalDockWidget.h
		  ConfocalDockWidget.cpp
		  ConfocalMainWindow.h
		  ConfocalMainWindow.cpp
		  ConfocalOpenGLWidget.h
		  ConfocalOpenGLWidget.cpp
		  VolumeSliceRender.h
		  VolumeSliceRender.cpp
		  VolumeSliceRenderShader.h
		  VolumeSliceRenderShader.cpp
		  Shader.cpp
		  Shader.h
		  PlotWindow.cpp
		  PlotWindow.h
		  ConfocalSliceView.h
		  ConfocalSliceView.cpp
		  ConfocalSliceViewWidget.h
		  ConfocalSliceViewWidget.cpp
		  external/qcustomplot.cpp
		  external/qcustomplot.h
		  ${Confocal_FORMS_HEADERS_GEN}
	)
endif()

add_executable(Confocal
	${SOURCES}
)

set(LIBRARIES
	${OpenCV_LIBS}
	${GLM_LIBRARIES}
	${Matlab_LIBRARIES}
)

IF(WIN32)
	set(LIBRARIES
	${LIBRARIES}
	${CMAKE_CURRENT_SOURCE_DIR}/matlab/msvc/libIso
)
ELSE()
set(LIBRARIES
	${LIBRARIES}
	${CMAKE_CURRENT_SOURCE_DIR}/matlab/linux/libIso.so
)
ENDIF()

if(WITH_UI)
	set(LIBRARIES
		${LIBRARIES}
		    ${GLEW_LIBRARY}
			${OPENGL_LIBRARY}
			${Qt5Core_LIBRARIES}
			${Qt5Widgets_LIBRARIES}
			${Qt5PrintSupport_LIBRARIES}
			${Qt5GUI_LIBRARIES}
	)
endif()

target_link_libraries(Confocal
  ${LIBRARIES}
)

