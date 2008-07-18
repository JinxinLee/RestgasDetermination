// -------------------------------------------------------------------------
// -----                  PndDchDigiInf source file                    -----
// -----                Created 30.06.08  by A. Bubak                -----
// -------------------------------------------------------------------------

#include "PndDchDigiInf.h"

#include "PndDchDigiPar.h"

#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TSystem.h"
#include "TMap.h"

#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

PndDchDigiInf* PndDchDigiInf::fgInfInstance = 0;

PndDchDigiInf* PndDchDigiInf::Instance() {
	if (0 == fgInfInstance) {
		cout << "First call of PndDchDigiInf::Instance(...)"
				<< " must be with filename" << endl;
		abort();
	} else
		return fgInfInstance;
}

PndDchDigiInf* PndDchDigiInf::Instance(TString filename) {
	if (0 != fgInfInstance) {
		cout << "An instance of PndDchDigiInf already existed, "
				<< "I replace it with a new one" << endl;
		delete fgInfInstance;
	}

	return fgInfInstance = new PndDchDigiInf(filename);
}

PndDchDigiInf::~PndDchDigiInf() {
	;
}

Double_t PndDchDigiInf::GetVariableStrict(const char* key) {
	TObjString* value = (TObjString*) fVariables->GetValue(key);
	if (value == NULL) {
		cerr << "Can't find variable named \"" << key << "\"";
		cerr << ". Exiting..." << endl;
		Fatal("", "");
	}
	Double_t val;
	char* err = NULL;
	val = strtod(value->GetString(), &err);
	if (err[0] != '\0') {
		cerr << "Can't convert variable named \"" << key;
		cerr << "\" to floating point. Value is \"";
		cerr << value->GetString() << "\"." << endl;
		cerr << "Exiting..." << endl;
		Fatal("", "");
	}
	return val;
}

Double_t PndDchDigiInf::GetVariable(const char* key) {
	TObjString* value = (TObjString*) fVariables->GetValue(key);
	if (value == NULL)
		return -1111;
	Double_t val;
	char* err = NULL;
	val = strtod(value->GetString(), &err);
	if (err[0] != '\0')
		return -1111;
	return val;
}

void PndDchDigiInf::AddVariable(const char* key, const char* value) {
	TObjString* skey = (TObjString*) fVariables->FindObject(key);
	//Value for this key already exists!!!
	if (skey != NULL)
		return;
	skey = new TObjString(key);
	skey->String().ToLower();
	TObjString* svalue = new TObjString(value);
	fVariables->Add(skey, svalue);
}

PndDchDigiInf::PndDchDigiInf(const char* filename) {

	ifstream file(filename);
	Int_t linenum;
	Double_t val;
	string buffer;
	string message;
	string variable;
	string value;
	TObjString* str = NULL;
	char** err = NULL;
	int ssize = -1;

	fSuccess = 1;

	if (!file) {
		cerr << "PndDchDigiInf: Can't open information file " << filename
				<< "!" << endl;
		fSuccess = 0;
		return;
	}

	fVariables = new TMap(200);
	fFileName = filename;
	linenum = 0;
	while (getline(file, buffer)) {
		linenum++;
		message = buffer.substr(buffer.find_first_not_of("    ")); //Skiping initial spaces
		message = message.substr(0, message.find("#")); //Removing comments
		if (message.empty())
			continue; //This was just a comment


		variable = message.substr(0, message.find("   "));
		value = message.substr(message.find("   ") + 3);

		AddVariable(variable.c_str(), value.c_str());
		//cout <<"(Distance, Time) : "<< variable <<", "<< value << endl;
	}
	file.close();
	InitVariables();
}

void PndDchDigiInf::InitVariables() {
	fNumberOfPoints = (Int_t) GetVariableStrict("numberofpoints");
	fTimeUnit2ns = GetVariableStrict("timeunit2ns");
	fDistanceUnit2cm = GetVariableStrict("distanceunit2cm");
}

void PndDchDigiInf::FillDigiPar(PndDchDigiPar* par) const {
	TObjString* key;
	TIterator* iter = fVariables->MakeIterator();
	par->setChanged();

	TMap* parVariables = new TMap();
	par->SetVariables(parVariables);

	while ((key = (TObjString*) iter->Next()) != NULL) {
		parVariables->Add(key, fVariables->GetValue(key));

		//TObjString *value = (TObjString*)fVariables->GetValue(key);
		//cout <<"PndDchDigiInf::(Distance, Time) : "<< key->GetString() <<", "<< value->GetString() << endl;
	}
}

