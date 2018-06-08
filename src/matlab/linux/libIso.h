//
// MATLAB Compiler: 6.5 (R2017b)
// Date: Fri Jun  8 13:15:59 2018
// Arguments:
// "-B""macro_default""-W""cpplib:libIso""-T""link:lib""-d""/gpfs/runtime/opt/co
// nfocal/1.0/src/Confocal/matlab/libIso/for_testing""-v""/gpfs/runtime/opt/conf
// ocal/1.0/src/Confocal/matlab/processActiveContour.m"
//

#ifndef __libIso_h
#define __libIso_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libIso_C_API 
#define LIB_libIso_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_libIso_C_API 
bool MW_CALL_CONV libIsoInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libIso_C_API 
bool MW_CALL_CONV libIsoInitialize(void);

extern LIB_libIso_C_API 
void MW_CALL_CONV libIsoTerminate(void);

extern LIB_libIso_C_API 
void MW_CALL_CONV libIsoPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_libIso_C_API 
bool MW_CALL_CONV mlxProcessActiveContour(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                          *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#ifdef __cplusplus
}
#endif


/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__MINGW64__)

#ifdef EXPORTING_libIso
#define PUBLIC_libIso_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libIso_CPP_API __declspec(dllimport)
#endif

#define LIB_libIso_CPP_API PUBLIC_libIso_CPP_API

#else

#if !defined(LIB_libIso_CPP_API)
#if defined(LIB_libIso_C_API)
#define LIB_libIso_CPP_API LIB_libIso_C_API
#else
#define LIB_libIso_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libIso_CPP_API void MW_CALL_CONV processActiveContour(int nargout, mwArray& Surface, mwArray& DistanceMap, const mwArray& ImageStack, const mwArray& Mask, const mwArray& Params, const mwArray& Aspect);

/* C++ INTERFACE -- WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */
#endif

#endif
