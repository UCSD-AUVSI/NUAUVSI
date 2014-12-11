#include "PythonSaliency.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/PythonCVMatConvert.h"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <fstream>
#include <boost/python.hpp>
namespace bp = boost::python;
using std::cout;
using std::endl;


static bool read_file_contents(std::string filename, std::string & returnedFileContents)
{
	returnedFileContents.clear();
	std::ifstream myfile(filename);
	if(myfile.is_open() && myfile.good()) {
		std::string line;
		while(std::getline(myfile,line)) {
			returnedFileContents = (returnedFileContents + std::string("\n") + line);
		}
		myfile.close();
		return true;
	}
	return false;
}


template<class T>
bp::list std_vector_to_py_list(const std::vector<T>& v)
{
	bp::list l;
	typename std::vector<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it)
		l.append(*it);   
	return l;  
}


static void AddPathToPythonSys(std::string path)
{
	// now insert the current working directory into the python path so module search can take advantage
	// this must happen after python has been initialised
	
	//cout << "adding to python path: \"" << path << "\"" << endl;
	PyObject* sysPath = PySys_GetObject("path");
	PyList_Insert( sysPath, 0, PyString_FromString(path.c_str()));
	
	//print python's search paths to confirm that it was added
	/*PyRun_SimpleString(	"import sys\n"
						"from pprint import pprint\n"
						"pprint(sys.path)\n");*/
}


void pythonSaliency(std::string saliencyModuleFolderName,
					std::string pythonFilename,
					std::string pythonFunctionName,
					cv::Mat fullsizeImage,
					std::vector<cv::Mat> & returnedCrops,
					std::vector<std::pair<double,double>> & returned_geolocations,
					std::vector<double> *additional_args/*=nullptr*/)
{
	returnedCrops.clear();
	returned_geolocations.clear();
	
	if(path_to_HeimdallBuild_directory == nullptr) {
		std::cout << "ERROR: pythonSaliency: \"path_to_HeimdallBuild_directory\" not set!" << std::endl;
		return;
	}
	
	//initialize embedded Python interpreter
	if(!Py_IsInitialized()) {
		cout<<"INITIALIZING PYTHON INTERPETER........"<<endl;
		Py_Initialize();
	}
	if(!Py_IsInitialized()) {
		cout << "ERROR: pythonSaliency: COULD NOT INITIALIZE PYTHON INTERPRETER: Python Saliency" << endl;
	}
	else {
		// setup directories and namespaces for Python interpeter
		NDArrayConverter cvt;
		bp::object main = bp::import("__main__");
		bp::object global(main.attr("__dict__"));
		std::string dirToAdd = (*path_to_HeimdallBuild_directory)+std::string("/Saliency/PythonSaliency/")+saliencyModuleFolderName+std::string("/Python");
		if(check_if_directory_exists(dirToAdd) == false) {
			std::cout << "ERROR: pythonSaliency: couldn't find \"" << dirToAdd << "\"" << std::endl;
			return;
		}
		
		
		// load main python file into C++
		std::string loadedPyFile;
		if(read_file_contents(dirToAdd+std::string("/")+pythonFilename,loadedPyFile)==false) {
			std::cout<<"ERROR: COULD NOT FIND PYTHON FILE \""<<pythonFilename<<"\""<<std::endl;
			return;
		}
		AddPathToPythonSys(dirToAdd);
		
		
		// load main python file into the interpreter
		try {
			PyRun_SimpleString(loadedPyFile.c_str());
		}
		catch(bp::error_already_set) {
			std::cout<<"PYTHON SALIENCY: ERROR RUNNING PYTHON FILE LOADED INTO C++: PYTHON ERROR REPORT:"<<std::endl;
			PyErr_Print(); return;
		}
		
		
		//get handle to the function
		bp::object pythoncvfunctionhandle;
		try {
			pythoncvfunctionhandle = global[pythonFunctionName];
		}
		catch(bp::error_already_set) {
			std::cout << "PYTHON SALIENCY ERROR: function \"" << pythonFunctionName << "\" not found in python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
			PyErr_Print(); return;
		}
		//std::cout<<"done getting handle to function!"<<std::endl;
		
		
		//convert C++ cv::Mat to Python
		PyObject* givenImgCpp = cvt.toNDArray(fullsizeImage);
		bp::object givenImgPyObj( bp::handle<>(bp::borrowed(givenImgCpp)) );
		
		
		//call the Python function, return a list of crops
		bp::object resultobj;
		if(additional_args != nullptr) {
			bp::list extraArgs = std_vector_to_py_list<double>(*additional_args);
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj, extraArgs);
			}
			catch(bp::error_already_set) {
				std::cout << "PYTHON SALIENCY ERROR: error when running python file \"" << pythonFilename << "\" WITH EXTRA ARGS! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		else {
			try {
				resultobj = pythoncvfunctionhandle(givenImgPyObj);
			}
			catch(bp::error_already_set) {
				std::cout << "PYTHON SALIENCY ERROR: error when running python file \"" << pythonFilename << "\"! PYTHON ERROR REPORT:" << std::endl;
				PyErr_Print(); return;
			}
		}
		Py_DECREF(givenImgCpp);
		
		
		// extract each crop from Python to C++
		bp::list resultBPList(resultobj);
		int numElementsInReturnedList = ((int)bp::len(resultBPList));
		for(int ii=0; ii<numElementsInReturnedList; ii++)
		{
			bp::tuple thisCropTuple(resultBPList[ii]);
			if(((int)bp::len(thisCropTuple)) == 3) {
				bp::object thisImgObj = boost::python::extract<boost::python::object>(thisCropTuple[0])();
				returnedCrops.push_back(cvt.toMat(thisImgObj.ptr()));
				
				returned_geolocations.push_back(std::pair<double,double>(bp::extract<double>(thisCropTuple[1]),bp::extract<double>(thisCropTuple[2])));
			} else {
				std::cout << "PYTHON SALIENCY ERROR: expects return value to be a list of tuples," << std::endl
						<< "    where each tuple has 3 elements: (crop_image, lat, long)" << std::endl;
				return;
			}
		}
	}
}
