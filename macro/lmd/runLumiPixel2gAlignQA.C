/*
 * runs align QA Taks and strores results to PDF, root
 *
 *	Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 * 
 */
using namespace std;

#include <string>

enum runParameter{kNormal, kPlotByOverlapID, kPlotByModule, kPlotCombinedPXResiduals, kPlotCombinedCMResiduals, kPlotCMMatrixResiduals, kPlotPXMatrixResiduals, kPlotCMvsPX, kPlotPXvsCMResiduals, kHistPixelDistances, kCalcOverlap, kCyclicCheckPX, kCyclicCheckCM};

void runLumiPixel2gAlignQA(TString LMDmatrixPath="/home/arbeit/RedPro3TB/simulationData/boxtest-50u-1.5/binaryPairs-cut-160/LMDmatrices", const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	//TStopwatch timer;
	//timer.Start();

	//TODO: convert this so it actually uses the parameters from above!

	/*
	 * what does it do? read matrices found by ICP and compare with PndLmdDim matrices.
	 * That means matrices must already be there from previous steps, right? So check that.
	 */

	// ---------------------- init parameters

	bool useCorrectionMatrix = false;
	bool inCentimeters = false;

	// ---------------------- init QA Task

	PndLmdAlignQA qaTask;

	qaTask.setCorrection(useCorrectionMatrix);
	qaTask.setInCentimeters(inCentimeters);
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-1.5/binaryPairs-cut-80/LMDmatrices");

	//TODO: Actually I don't want a dimension object here, the Manager should take care of that. See three lines down.
	//dimension is a singleton, so you can load matrices even after a different object requested a dimension object
	PndLmdDim *dimension = PndLmdDim::Instance();
	//TODO: the matrices here are also important for the AlignQA class, which also needs to know if we use 10u or 50u. So this is messy design.
	//change that!
	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd.dat", true);
	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd_misaligned-100u.dat", false);

	// DELETE THIS AFTERWARDS
	//qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/PDFout");
	//qaTask.compareMatrices(kHistPixelDistances);
	//return;
	// DELETE THIS AFTERWARDS

	//calculate all overlapping areas
	if(false){
		qaTask.compareMatrices(kCalcOverlap);
	}

	// check if matrices exist (matrix finder was run)
	bool matrixPresent = qaTask.checkForMatrixFiles();
	if(matrixPresent){
		cout << "\nINFO: all matrix files accounted for!\n";
	}
	else{
		cout << "\nINFO: at least one matrix file could not be found!\n";
	}

	/*
	 * prepare global settings
	 */
	qaTask.setAlignOption(2);	//0=aligned, other is misaligned. change together with LmdMatPath!
	qaTask.setPairsRequired(300e3);

	//check combined matrices
	if(false){
		qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-1.5/AlignQA/");
		//qaTask.compareMatrices(kPlotCombinedPXResiduals);
		//qaTask.compareMatrices(kPlotCombinedCMResiduals);

		qaTask.compareMatrices(kCyclicCheckCM);
		qaTask.compareMatrices(kCyclicCheckPX);

		return;
	}

	/*
	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-1.5/AlignQA-80/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-1.5/binaryPairs-cut-80/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-15/AlignQA-80/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-0u-15/binaryPairs-cut-80/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

*/

	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd_misaligned-10u.dat", false);
	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-10u-1.5/AlignQA-80/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-10u-1.5/binaryPairs-cut-80/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-10u-15/AlignQA-80/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-10u-15/binaryPairs-cut-80/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);


	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd_misaligned-50u.dat", false);
	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-50u-1.5/AlignQA-640/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-50u-1.5/binaryPairs-cut-640/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);


	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-50u-15/AlignQA-640/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-50u-15/binaryPairs-cut-640/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd_misaligned-100u.dat", false);
	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-100u-1.5/AlignQA-dynamic/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-100u-1.5/binaryPairs-cut-dynamic/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);


	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-100u-15/AlignQA-dynamic/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-100u-15/binaryPairs-cut-dynamic/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	
	dimension->Read_transformation_matrices("/geometry/MisalignTestGeometryMatrices/trafo_matrices_lmd_misaligned-200u.dat", false);
	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-200u-1.5/AlignQA-dynamic/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-200u-1.5/binaryPairs-cut-dynamic/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	qaTask.setPdfOutPath("/home/arbeit/RedPro3TB/simulationData/boxtest-200u-15/AlignQA-dynamic/");
	qaTask.setLmdMatPath("/home/arbeit/RedPro3TB/simulationData/boxtest-200u-15/binaryPairs-cut-dynamic/LMDmatrices"); //aligned, change together with setAlignOption!
	//compare (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotCMMatrixResiduals);
	//compare (matPX-matTargetPX)
	qaTask.compareMatrices(kPlotPXMatrixResiduals);
	//compare (matCM*matrixTarget-transFormToLMD(matPX))
	qaTask.compareMatrices(kPlotCMvsPX);
	//compare transformToLMD(matPX-matTargetPX) - (matCM-matTargetCM)
	qaTask.compareMatrices(kPlotPXvsCMResiduals);
	// compare combined matrices from 0 to 1-9
	qaTask.compareMatrices(kPlotCombinedPXResiduals);

	// TODO: compare lmdlocal -> sensor matrices with target matrices from PndLmdDim (maybe optional, because they are already in PndLmdDim?)



	// TODO: generate report, pdf or root file or w/e

	cout << "Macro finished successfully.\n";

	return;
}
