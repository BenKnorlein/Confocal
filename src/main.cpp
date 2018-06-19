#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#ifndef WITH_CONSOLE
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#ifdef WITH_UI
	#include <QApplication>
	#include "ConfocalMainWindow.h"
#else 
	#include <fstream>
#endif

#ifdef _MSC_VER
	#define OS_SLASH "\\"
#else
	#define OS_SLASH "//"
#endif

#include <opencv/cv.h>
#include <iostream>
#include "Data.h"
#include <mclmcrrt.h>
#include "LoadDataAction.h"
#include "SphereFit.h"
#include "GenerateMasksAction.h"
#include "UpdateResultsAction.h"
#include "SaveProcessedDataAction.h"

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
	std::string input_foldername;
	std::string output_foldername;
	double region_size = 10.0f;
	double smoothness = 0;
	int iterations = 300;
	double contractionbias = 0;

	//parse parameters
	for (int index = 0; index < argc; ++index)
	{
		std::string c = argv[index];
		if (c == "-i")
		{
			index++;
			if (index < argc)
				input_foldername = argv[index];
		}
		if (c == "-o")
		{
			index++;
			if (index < argc)
				output_foldername = argv[index];
		}
		if (c == "-r")
		{
			index++;
			if (index < argc)
				region_size = atof(argv[index]);
		}
		if (c == "-s")
		{
			index++;
			if (index < argc)
				smoothness = atof(argv[index]);
		}
		if (c == "-n")
		{
			index++;
			if (index < argc)
				iterations = atoi(argv[index]);
		}
		if (c == "-c")
		{
			index++;
			if (index < argc)
				contractionbias = atof(argv[index]);
		}
		if (c == "-h")
		{
			std::cerr << "Options for command line are : " << std::endl;
			std::cerr << "-i INPUTFOLDER" << std::endl;
			std::cerr << "-o OUTPUTFOLDER" << std::endl;
			std::cerr << "-r REGIONSIZE (range: positive floating point - default: 10)" << std::endl;
			std::cerr << "-s SMOOTHNESS (range: positive floating point - default : 0)" << std::endl;
			std::cerr << "-n ITERATIONS (range: positive integer - default : 300)" << std::endl;
			std::cerr << "-c CONTRACTIONBIAS (range: -1 : 1 : default: 0)" << std::endl;
			exit(1);
		}
	}

	if (input_foldername.empty() || output_foldername.empty())
	{
		std::cerr << "You have to at least provide an input and outputfolder. Run with option -h to see available arguments" << std::endl;
		exit(1);
	}

	std::cerr << "Starting Run" << std::endl;
	std::cerr << "Input : " << input_foldername << std::endl;
	std::cerr << "Output : " <<output_foldername << std::endl;
	std::cerr << "Regionsize : " << region_size << std::endl;
	std::cerr << "Iterations : " << iterations << std::endl;
	std::cerr << "Smoothness : " << smoothness << std::endl;
	std::cerr << "Contractionbias : "<< contractionbias << std::endl;

	//chack validity of input
	std::cerr << "Loading folder " << input_foldername << std::endl;
	//start processing
	conf::LoadDataAction(input_foldername).run();
	conf::SphereFit().run();
	conf::GenerateMasksAction(iterations,contractionbias,smoothness).run();
	conf::UpdateResultsAction(region_size).run();
	conf::SaveProcessedDataAction(output_foldername).run();

	std::ofstream run_Details;
	// Red (pixel normalized) - Red (region normalized)- Red - Green - NbPixel - sd Red (normalized) - sd Red - sd Green
	run_Details.open(output_foldername + OS_SLASH + "settings.csv", std::ios::out);
	run_Details << "Input , " << input_foldername << std::endl;
	run_Details << "Output , " << output_foldername << std::endl;
	run_Details << "Regionsize , " << region_size << std::endl;
	run_Details << "Iterations , " << iterations << std::endl;
	run_Details << "Smoothness , " << smoothness << std::endl;
	run_Details << "Contractionbias , " << contractionbias << std::endl;
	run_Details.close();
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
