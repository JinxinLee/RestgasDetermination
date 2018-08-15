/*
 * FileHandler.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: roman
 */

#include "FileHandler.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <dirent.h>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <iomanip>
#include <time.h>

using namespace std;

namespace Roman {

int FileHandler::lastLoadBarIteration = 0;
clock_t FileHandler::lastLoadBarTime = clock();
boost::mutex FileHandler::singleFileRead;
boost::mutex FileHandler::singleFileWrite;

std::vector< std::vector<double> > FileHandler::readFromCSVfile(std::string filename, int maxLines, bool progressBar, bool singleIOaccess) {

	stringstream ss;
	stringstream iss;
	int i=0;
	int lines = readFile(&ss, filename, singleIOaccess);

	if(maxLines > lines){
		maxLines = lines;
	}
	if(maxLines < 1){
		maxLines = lines;
	}

	string line, token;
	std::vector<std::vector<double> > data;
	data.reserve(maxLines);

	//cout << "reading " << maxLines << " lines." << endl;

	while(getline(ss, line)){

		if(progressBar){
			FileHandler::loadBar(i, maxLines, 100,30);
		}

		iss << line;
		std::vector<double> tempvec;
		while(getline(iss, token, ',')) {
			tempvec.push_back(boost::lexical_cast<double>(token));
		}
		data.push_back(tempvec);
		++i;
		iss.clear();

		if(i > maxLines){
			break;
		}
	}
	ss.str("");
	iss.str("");
	return data;
}

std::vector<double> FileHandler::readFromCSVline(std::string line) {

	stringstream iss;
	string token;
	std::vector<double> data;

	while(getline(iss, token, ',')) {
		data.push_back(boost::lexical_cast<double>(token));
	}
	return data;
}

bool FileHandler::writeFile(std::stringstream* ss, std::string filename, bool singleIOAccess) {

	mkdir(pwd(filename));

	if(singleIOAccess){
		singleFileWrite.lock();
	}

	std::ofstream outFileStream;
	outFileStream.open(filename.c_str());
	outFileStream << ss->str();
	outFileStream.close();

	if(singleIOAccess){
		singleFileWrite.unlock();
	}

	return true;
}

bool FileHandler::appendToFile(std::string content, std::string filename, bool singleIOAccess) {


	if(singleIOAccess){
		singleFileWrite.lock();
	}

	mkdir(pwd(filename));

	std::ofstream outFileStream;
	outFileStream.open(filename.c_str(), fstream::app);
	outFileStream << content;
	outFileStream.close();

	if(singleIOAccess){
		singleFileWrite.unlock();
	}

	return true;
}

//FIXME: Error reporting to log and handling
bool FileHandler::writeMatrix(Matrix &mat, std::string filename, stringstream* log, bool singleIOaccess){

	if(singleIOaccess){
		singleFileWrite.lock();
	}

	mkdir(pwd(filename));

	//mat.val[0][3] *= 80;
	//mat.val[1][3] *= 80;

	std::ofstream outFileStream;
	outFileStream.open(filename.c_str());\
	outFileStream << std::setprecision(16);
	outFileStream << mat;
	outFileStream.close();

	if(singleIOaccess){
		singleFileWrite.unlock();
	}

	*log << "successfully written matrix " << filename << endl;
	return true;
}

// Process has done i out of n rounds,
// and we want a bar of width w and resolution r.
void FileHandler::loadBar(int i, int n, int r, int w, std::string message){

	// Only update r times.
	if(n==0) return;
	if(n==1) return;

	if(r>n){
		r = n;
	}

	//calculate ev / sec every 100000 iterations
	if ( i % (n/r) != 0 ){
		return;
	}

	flush(cout);

	// Calculate the ratio of complete-to-incomplete.
	float ratio = i/(float)n;
	int   c     = ratio * w;

	clock_t timePassed = clock() - lastLoadBarTime;
	int eventsPassed = i - lastLoadBarIteration;
	float time = (float)timePassed / CLOCKS_PER_SEC;
	float eventsPerSecond = eventsPassed / time;
	float minutesLeft = (n-i) / eventsPerSecond / 60;

	lastLoadBarIteration = i;
	lastLoadBarTime = clock();

	// Show the percentage complete.
	printf("%3d%% [", (int)(ratio*100) );

	// Show the load bar.
	for (int x=0; x<c; x++)
		printf("=");

	for (int x=c; x<w; x++)
		printf(" ");

	// ANSI Control codes to go back to the
	// previous line and clear it.
	if(minutesLeft > 1)
		printf("] %s %d of %d ( %.1f/sec, %.1f minutes left )  \n\033[F\033[J", message.c_str(), i, n, eventsPerSecond, minutesLeft);
	else
		printf("] %s %d of %d ( %.1f/sec, %.0f seconds left )  \n\033[F\033[J", message.c_str(), i, n, eventsPerSecond, minutesLeft*60);
}

// recursive search algorithm, using BOOST
int FileHandler::searchFiles(std::string path, std::string extension, std::vector<std::string>* list, bool includeSubDirs) {
	if(!boost::filesystem::exists(path)){
		return 0;
	}
	boost::filesystem::directory_iterator iterator(path);
	for(; iterator != boost::filesystem::directory_iterator(); ++iterator){
		if(boost::filesystem::is_directory(iterator->path()) && includeSubDirs){
			list->push_back(iterator->path().string());
			searchFiles(iterator->path().string(), extension, list, includeSubDirs);
		}
		else if(boost::filesystem::is_regular_file(iterator->path())){
			if( iterator->path().string().find(extension, (iterator->path().string().length() - extension.length())) != std::string::npos ){
				list->push_back(iterator->path().string());
			}
		}
	}
	sort(list->begin(), list->end());
	return list->size();
}

// recursive search algorithm, using BOOST
int FileHandler::searchDirectories(std::string curr_directory,	std::vector<std::string>* list, bool includeSubDirs) {
	if(!boost::filesystem::exists(curr_directory)){
		return 0;
	}
	boost::filesystem::directory_iterator iterator(curr_directory);
	for(; iterator != boost::filesystem::directory_iterator(); ++iterator){
		if(boost::filesystem::is_directory(iterator->path())){
			list->push_back(iterator->path().string());
			//recursively call
			if(includeSubDirs){
				searchDirectories(iterator->path().string(), list, includeSubDirs);
			}
		}
	}
	sort(list->begin(), list->end());
	return list->size();
}

//read contents of specified file, appends to stringstream provided, returns no of lines
int FileHandler::readFile(std::stringstream* valueStream, std::string filename, bool singleIOaccess) {

	int lines=0;

	//get events from text file. later: directly from root file
	//cout << "reading info from file.." << endl;
	//stringstream* valueStream = ss;

	if(singleIOaccess){
		singleFileRead.lock();
	}

	ifstream ifs;
	ifs.open(filename.c_str());
	if(!ifs.is_open()){
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << endl;
		return 0;
	}
	string linebuffer;// = new string();
	while(std::getline(ifs,linebuffer)){
		*valueStream << linebuffer << endl;
		lines++;
	}
	ifs.close();

	if(singleIOaccess){
		singleFileRead.unlock();
	}
	//cout << "file read to ram. "<< lines << " lines read." << endl;

	return lines;
}

int FileHandler::countLines(stringstream* ss){

	int lines=0;
	string content;
	while(getline(*ss, content)){
		lines++;
	}

	ss->clear();
	ss->seekg(0, ios::beg);
	return lines;

}

int FileHandler::countLinesOfFile(std::string filename) {

	int lines=0;

	//get events from text file. later: directly from root file
	cout << "reading lines of file.." << endl;

	ifstream ifs;
	ifs.open(filename.c_str());
	if(!ifs.is_open()){
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << endl;
		return 0;
	}
	string linebuffer;
	while(std::getline(ifs,linebuffer)){
		lines++;
	}
	ifs.close();
	cout << "file" << filename << " has " << lines << " lines." << endl;
	return lines;

}

Matrix FileHandler::readMatrix(std::string filename) {

	vector< vector<double> > temp = readFromCSVfile(filename);

	if(temp.size() < 1){
		cout << "warning! can't read matrix from file " << filename << endl;
		exit(1);
	}
	if(temp[0].size() < 1){
		cout << "warning! can't read matrix from file " << filename << endl;
		exit(1);
	}

	int rows, columns;
	rows = temp.size();
	columns = temp[0].size();

	Matrix result(rows, columns);

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			result.val[i][j] = temp[i][j];
		}
	}
	return result;
}

std::string FileHandler::pwd(std::string file) {

	unsigned found = file.find_last_of("/\\");
	return file.substr(0,found+1);
}

bool FileHandler::directoryExists(std::string name) {
	return boost::filesystem::exists(name);
}

bool FileHandler::mkdir(std::string name) {

	if(directoryExists(name)){
		return true;
	}
	boost::filesystem::path dir(name);
	if(boost::filesystem::create_directories(dir)) {
		return true;
	}
	else{
		return false;
	}
}

std::string FileHandler::selectDirectory(std::string sourceDirectory, bool includeSubDirs) {

	vector<string> list;
	searchDirectories(sourceDirectory, &list, includeSubDirs);
	int selection;

	if(sourceDirectory == ""){

		cout << "warning! you called searchDirectories() without argument. this does not work yet" << endl;
		//sourceDirectory == "./";
		//sourceDirectory = cwd();
		//sourceDirectory += "/";
	}


	if(list.size()<1){
		cout << "no directories found in " << sourceDirectory << "!" << endl;
		cout << "this error seems critical, you should cancel the program!" << endl;
		return "";
	}
	if(list.size()==1){
		return list[0];
	}
	cout << "select directory:" << endl;
	for(int i=0; i<list.size(); i++){
		cout << i << ": " << list[i] << endl;
	}
	cin >> selection;
	if(selection > list.size()){
		cout << "invalid choice! (too high)" << endl;
		exit(1);
	}
	if(selection < 0){
		cout << "invalid choice! (negative value)" << endl;
		exit(1);
	}

	return list[selection];
}

std::vector<std::string> FileHandler::readFile(std::string filename, bool singleIOaccess) {

	//get events from text file. later: directly from root file
	//cout << "reading info from file.." << endl;
	//stringstream* valueStream = ss;

	vector<string> result;

	if(singleIOaccess){
		singleFileRead.lock();
	}

	ifstream ifs;
	ifs.open(filename.c_str());
	if(!ifs.is_open()){
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << endl;
		return result;
	}
	string linebuffer;// = new string();
	while(std::getline(ifs,linebuffer)){
		result.push_back(linebuffer);

	}
	ifs.close();

	if(singleIOaccess){
		singleFileRead.unlock();
	}

	return result;
}

vector<string> FileHandler::findRegex(std::string source, std::string regex){

	boost::regex expression(regex);

	std::string::const_iterator start, end;
	start = source.begin();
	end = source.end();
	boost::match_results<std::string::const_iterator> result;
	boost::match_flag_type flags = boost::match_default;

	vector<string> resultStrings;

	while(regex_search(start, end, result, expression, flags))
	{
		// update search position:
		start = result[0].second;

		for(int j=0; j<result.size(); j++){
			resultStrings.push_back( boost::lexical_cast<string>(result[j]));
		}

		// update flags:
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}

	return resultStrings;
}

std::string FileHandler::cwd() {
	return boost::filesystem::current_path().string();
}

} /* namespace Roman */

