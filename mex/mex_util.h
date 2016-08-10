/*
 * mex_util.h
 */

#ifndef _MEX_UTIL_H_
#define _MEX_UTIL_H_

#include <map>
#include <string>
#include "mex.h" 

 //
 // Helper functions.
 //
inline std::string to_string(const mxArray* mat) 
{
	if (!mxIsChar(mat))
		mexErrMsgTxt("Expected a string argument");

	char* cstr = mxArrayToString(mat);
	std::string str(cstr);

	mxFree(cstr);
	return str;
}

//
// Define macro for mexFunction.
//
typedef void(*MatlabFunctionHandle)(int, mxArray**, int, const mxArray**);
typedef std::pair<std::string, MatlabFunctionHandle> Command;

#define DECLARE_MEX_COMMAND_BEGIN Command command_list[] = {
#define DECLARE_MEX_COMMAND(fn) Command(#fn, fn),
#define DECLARE_MEX_COMMAND_END }; std::map<std::string, MatlabFunctionHandle> commands(command_list, command_list + sizeof(command_list) / sizeof(command_list[0]));

#define DECLARE_MEX_FUNCTION void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {\
	if (nrhs == 0) {\
		mexErrMsgTxt("An API command is required.");\
		return;\
	}\
	std::string cmd = ::to_string(prhs[0]);\
	auto it = commands.find(cmd);\
	if (it == commands.end()) {\
		mexErrMsgTxt("API command not recognized.");\
		return;\
	}\
	it->second(nlhs, plhs, nrhs - 1, prhs + 1);\
}

#endif // _MEX_UTIL_H_