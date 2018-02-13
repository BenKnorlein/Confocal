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
	conf::ConfocalMainWindow* widget = new conf::ConfocalMainWindow();
	widget->show();
	return app.exec();
}

//#ifdef _MSC_VER
//	#define _CRT_SECURE_NO_WARNINGS
//	#ifndef WITH_CONSOLE
//	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
//	#endif
//	#define NOMINMAX
//	#include <windows.h>
//	#include <GL/gl.h>
//	#include <gl/GLU.h>
//	#define M_PI 3.14159265358979323846
//	#define OS_SLASH "\\"
//#elif defined(__APPLE__)
//	#include <OpenGL/OpenGL.h>
//	#include <OpenGL/glu.h>
//	#define OS_SLASH "/"
//#else
//	#define GL_GLEXT_PROTOTYPES
//	#include <GL/gl.h>
//	#include <GL/glu.h>
//	#define OS_SLASH "/"
//#endif

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "dirent.h"
//#include <iostream>
//#include <fstream>
//
//#include "Volume.h"
//#include "SphereFit.h"
//#include "MSAC.h"




//int main(int argc, char **argv) {


	//	//Setup functions
	//	conf::SphereFit spherefit;
	//	auto fit_function = std::bind(&conf::SphereFit::fit_function, spherefit, std::placeholders::_1);
	//	auto eval_function = std::bind(&conf::SphereFit::eval_function, spherefit, std::placeholders::_1, std::placeholders::_2);
	//	auto check_function = std::bind(&conf::SphereFit::check_function, spherefit, std::placeholders::_1);

	//	//run MSAC
	//	conf::MSAC msac;
	//	model = msac.run(spherefit.getMinSamples(), pts, fit_function, eval_function, check_function,4,1000000,99);
	//}

	//{
	//	std::vector<cv::Point3d> sample_points;
	//	// draw unit sphere points (r=1 center=(0,0,0)) ... your rays directions
	//	int ia, na, ib, nb;
	//	double x, y, z, r;
	//	double a, b, da, db;
	//	na = 16;                                  // number of slices
	//	da = M_PI / double(na - 1);                   // latitude angle step
	//	for (a = -0.5*M_PI, ia = 0; ia<na; ia++, a += da) // slice sphere to circles in xy planes
	//	{
	//		r = cos(a);                           // radius of actual circle in xy plane
	//		z = sin(a);                           // height of actual circle in xy plane
	//		nb = ceil(2.0*M_PI*r / da);
	//		db = 2.0*M_PI / double(nb);             // longitude angle step
	//		if ((ia == 0) || (ia == na - 1)) { nb = 1; db = 0.0; }  // handle edge cases
	//		for (b = 0.0, ib = 0; ib<nb; ib++, b += db)   // cut circle to vertexes
	//		{
	//			x = r*cos(b);                     // compute x,y of vertex
	//			y = r*sin(b);
	//			if(z<=0)sample_points.push_back(cv::Point3d(x,y,z));
	//		}
	//	}

	//	std::ofstream points_file("points.csv");
	//	std::ofstream normed_file("value_normed.csv");
	//	std::ofstream ch1_file("value_ch1.csv");
	//	std::ofstream ch2_file("value_ch2.csv");
	//	points_file.precision(12);
	//	normed_file.precision(12);
	//	ch1_file.precision(12);
	//	ch2_file.precision(12);

	//	cv::Point3d center = cv::Point3d(model[0], model[1], model[2]);
	//	for (auto dir : sample_points){
	//		points_file << dir.x << " , " << dir.y << " , " << dir.z << std::endl;

	//		for (int i = 0; i < model[3] + 30 ; i++)
	//		{
	//			cv::Point3d pt = center+ dir * i;

	//			if (pt.x > 0 && pt.y > 0 && pt.z > 0 &&
	//				pt.x / vol.get_x_scale() < vol.get_width() - 1 &&
	//				pt.y / vol.get_y_scale() < vol.get_height() - 1 &&
	//				pt.z / vol.get_z_scale() < vol.get_depth() - 1){
	//				double ch1 = vol.getInterpolated(pt.x, pt.y, pt.z, 0);
	//				double ch2 = vol.getInterpolated(pt.x, pt.y, pt.z, 1);

	//				normed_file << ch2 / ch1;
	//				ch1_file << ch1;
	//				ch2_file << ch2;
	//			}
	//			else
	//			{
	//				normed_file << 0;
	//				ch1_file << 0;
	//				ch2_file << 0;
	//			}
	//			if (i + 1 < model[3] + 30)
	//			{
	//				normed_file << " , ";
	//				ch1_file << " , ";
	//				ch2_file << " , ";
	//			}
	//		}
	//		normed_file << std::endl;
	//		ch1_file << std::endl;
	//		ch2_file << std::endl;
	//	}
	//	points_file.close();
	//	normed_file.close();
	//	ch1_file.close();
	//	ch2_file.close();
	//}

//	exit(0);
//}

