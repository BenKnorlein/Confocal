#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#ifndef WITH_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

#ifdef WITH_UI
	#include <QApplication>
	#include "ConfocalMainWindow.h"
#endif


#include <opencv/cv.h>
#include <iostream>
#include "Data.h"
#include <mclmcrrt.h>
#include "LoadDataAction.h"
#include "SphereFit.h"
#include "GenerateMasksAction.h"
#include "UpdateResultsAction.h"

bool m_Initialized_MCR = false;
//using namespace conf;

#ifdef WITH_UI
class MApplication : public QApplication
{
public:
	MApplication(int& argc, char** argv) : QApplication(argc, argv)
	{
	};

	~MApplication()
	{
	};

	bool notify(QObject* object, QEvent* event)
	{
		try
		{
			return QApplication::notify(object, event);
		}
		catch (cv::Exception& e)
		{
			std::cerr << e.err << " in " << e.file << " in function " << e.func << " at line " << e.line << std::endl;
		}
		catch (std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
		return false;
	}
};
#endif

int main(int argc, char** argv)
{
	conf::Data<float>::getInstance();
	bool exit_code = 0;

#ifdef WITH_UI
	MApplication app(argc, argv);
	conf::ConfocalMainWindow* widget = conf::ConfocalMainWindow::getInstance();
	widget->show();
	exit_code = app.exec();
#else
	//required batch parameters
	std::string filename;


	//parse parameters
	for (int index = 0; index < argc; ++index)
	{
		std::string c = argv[index];
		if (c == "-i")
		{
			index++;
			std::cerr << "set" << std::endl;
			if (index < argc)
				filename = argv[index]; 
		}
	}
	//chack validity of input
	std::cerr << "Loading folder " << filename << std::endl;
	//start processing
	conf::LoadDataAction(filename).run();
	conf::SphereFit().run();
	conf::GenerateMasksAction().run();
	conf::UpdateResultsAction(10).run();
#endif

	if (m_Initialized_MCR)
	{
		if (!mclTerminateApplication())
		{
			std::cerr << "Could not terminate Matlab runtime properly"<< std::endl;
		} 
		else
		{
			std::cerr << "Matlab runtime terminated" << std::endl;
		}
	}

	return exit_code;
}
