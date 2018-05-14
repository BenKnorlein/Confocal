#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#ifndef WITH_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

#include <QApplication>
#include "ConfocalMainWindow.h"

#include <opencv/cv.h>
#include <iostream>
#include "Data.h"
#include <mclmcrrt.h>

bool m_Initialized_MCR = false;
//using namespace conf;

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

int main(int argc, char** argv)
{
	conf::Data<float>::getInstance();

	MApplication app(argc, argv);
	conf::ConfocalMainWindow* widget = conf::ConfocalMainWindow::getInstance();
	widget->show();
	bool exit_code = app.exec();

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
