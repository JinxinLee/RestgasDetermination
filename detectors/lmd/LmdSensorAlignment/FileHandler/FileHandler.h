/*
 * FileHandler.h
 *
 *  Created on: Aug 15, 2013
 *      Author: roman
 *
 *
 *      Helper class to read text files to a stringstream, or
 *      cvs data to a vector< vector<double> >. Handles IO opperation,
 *      error handling etc.
 *
 */

#ifndef FILEHANDLER_H_
#define FILEHANDLER_H_

#include "icp/matrix.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <string>
#include <time.h>

namespace Roman {

class FileHandler {

private:
	static boost::mutex singleFileRead;
	static boost::mutex singleFileWrite;

public:

	static clock_t lastLoadBarTime;
	static int lastLoadBarIteration;

	//is directory named already there? (BOOST)
	static bool directoryExists(std::string name);

	//create (recursively, if necessary) directory (BOOST)
	static bool mkdir(std::string name);

	//returns path of file
	static std::string pwd(std::string file);

	//returns current working directory (cwd)
	static std::string cwd();

	//recursively searches files by extension in curr_directory and adds to list
	static int searchFiles(std::string curr_directory, std::string extension, std::vector<std::string>* list, bool includeSubDirs = true);

	//searches directories in curr_directory, adds to list
	static int searchDirectories(std::string curr_directory, std::vector<std::string>* list, bool includeSubDirs = true);

	//reads file to stringstream
	static int readFile(std::stringstream* ss, std::string filename, bool singleIOaccess = true);

	//reads file to vector<string>
	static std::vector<std::string> readFile(std::string filename, bool singleIOaccess = true);

	//writes stringstream to file, REPLACING file
	static bool writeFile(std::stringstream *ss , std::string filename, bool singleIOaccess = true);

	//appends stringstream to file, ADDING TO EXISTING file
	static bool appendToFile(std::string content, std::string filename, bool singleIOaccess = true);

	//read selection of a list of directories, returns path chosen
	static std::string selectDirectory(std::string sourceDirectory="", bool includeSubDirs = false);


	static bool writeMatrix(Matrix &mat, std::string filename, std::stringstream* log = NULL, bool singleIOaccess = true);
	static std::vector< std::vector<double> > readFromCSVfile(std::string filename, int maxLines=0, bool progressBar=false,bool singleIOaccess=true);
	static Matrix readMatrix(std::string filename);
	static std::vector<double > readFromCSVline(std::string line);
	static void loadBar(int current, int total, int resolution, int width, std::string message="");
	static int countLines(std::stringstream* ss);
	static int countLinesOfFile(std::string filename);
	static std::vector<std::string> findRegex(std::string source, std::string regex);
};

} /* namespace Roman */
#endif /* FILEHANDLER_H_ */
