/*
 * runs align QA Taks and strores results to PDF, root
 */
using namespace std;

#include <string>

void runLumiPixel2gAlignQA(TString storePath="/home/arbeit/fairsoft_may16p1/pandaroot/geometry/LMDmatrices/", const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	//TODO: convert this so it actually uses the parameters from above!

	/*
	 * what does it do? read matrices found by ICP and compare with PndLmdDim matrices.
	 * That means matrices must already be there from previous steps, right? So check that.
	 */

	// ---------------------- init parameters

	bool useCorrectionMatrix = false;
	bool inCentimeters = false;
	string matrixDir = storePath.Data();
	string reportOutDir = matrixDir + "/../QAReport";	//where should PDFs be saved? leading / is important!

	// ---------------------- init QA Task

	PndLmdAlignQA qaTask;
	qaTask.setMatrixDir(matrixDir);
	qaTask.setCorrection(useCorrectionMatrix);
	qaTask.setInCentimeters(inCentimeters);
	qaTask.setOutputPath(reportOutDir);

	//dimension is a singleton, so you can load matrices even after a different object requested a dimension object
	PndLmdDim *dimension = PndLmdDim::Instance();
	//TODO: the matrices here are also important for the AlignQA class, which also needs to know if we use 10u or 50u. So this is messy design.
	//change that!
	dimension->Read_transformation_matrices("/home/arbeit/simulationData/boxtest-0u-1.5/GeometryMatrices/trafo_matrices_lmd_misaligned-10u.dat", false);
	dimension->Read_transformation_matrices("/home/arbeit/simulationData/boxtest-0u-1.5/GeometryMatrices/trafo_matrices_lmd.dat", true);


	// check if matrices exist (matrix finder was run)
	bool matrixPresent = qaTask.checkForMatrixFiles();
	if(matrixPresent){
		cout << "\nINFO: all matrix files accounted for!\n";
	}
	else{
		cout << "\nINFO: at least one matrix file could not be found!\n";
	}

	// compare overlap matrices with icp matrices
	//qaTask.compareMatrices(0);

	qaTask.compareMatrices(6);


	// TODO: compare lmdlocal -> sensor matrices with target matrices from PndLmdDim (maybe optional, because they are already in PndLmdDim?)



	// TODO: generate report, pdf or root file or w/e

	if(false){

		PndLmdAlignQA qaTask;
		qaTask.setCorrection(false);

		/*
		qaTask.setMatrixDir("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts0-ss0/");
		qaTask.setOutputPath("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts0-ss0/pdf");
		qaTask.setInCentimeters(true);
		qaTask.compareMatrices();
		qaTask.setInCentimeters(false);
		qaTask.compareMatrices();
		 */

		qaTask.setMatrixDir("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts0-ss1/");
		qaTask.setOutputPath("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts0-ss1/pdf");
		qaTask.setInCentimeters(true);
		qaTask.compareMatrices();
		qaTask.setInCentimeters(false);
		qaTask.compareMatrices();

		/*
		qaTask.setMatrixDir("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts1-ss0/");
		qaTask.setOutputPath("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts1-ss0/pdf");
		qaTask.setInCentimeters(true);
		qaTask.compareMatrices();
		qaTask.setInCentimeters(false);
		qaTask.compareMatrices();
		 */

		qaTask.setMatrixDir("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts1-ss1/");
		qaTask.setOutputPath("/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/timestampTest/matrices-ts1-ss1/pdf");
		qaTask.setInCentimeters(true);
		qaTask.compareMatrices();
		qaTask.setInCentimeters(false);
		qaTask.compareMatrices();

		return;

	}

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
	cout << endl;
}
