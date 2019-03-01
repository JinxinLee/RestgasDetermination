/**
 @class PndFilteredPrimaryGenerator

 @author Klaus Goetzen <k [dot] goetzen (at) gsi [dot] de>

 @brief Primary generator with added event filtering capabilities.

 This class adds event filtering capabilities to FairPrimaryGenerator which is used internally for handling the event generators.
 The event filtering is performed after the event generation and before the particle transport through the detector model.
 
 The filter is configured with a string of the form "Filt_1.1 && Filt_1.2  ||  Filt_2.1  ||  Filt_3.1 && Filt_3.2 && Filt_3.2", where the 
 logical && (AND) has higher priority than the logical || (OR). Each filter can be negated with a leading '!'. 
 
 There are two types of filters possible: particle type counters and invariant mass filters. The syntax is
 
 count filter : C(<particle> ; <mult> ; [kinematic requirements])
 				==> requires <mult> particles of type <particle> (counted in MC list) to meet certain kinematic conditions (p, pt, pz, theta, phi)
 				
 mass filter  : M(<particle 1> <particle 2> [<particle 3> ... <particle 5>] ; <mult> ; <mass requirement> ; [kinematic requirements])
 				==> takes neutral particles and charged tracks, applies the mass hypotheses from <particle 1..5>, combines them and requires <mult> candidates with invariant mass to be in certain window
 
 <particle>   : e+, e-, e+-, mu+, mu-, mu+-, pi+, pi-, pi+-, K+, K-, K+-, p+, p-, p+-, n0, n0b, n00b (=n0 or n0b), gam, nt (neutral = gamma), t+ (positive tracks), t- (neg. tracks), t+- (chg. tracks), any (total multiplicity)
 <mult>       : a..b, a (= a..a), ..a (= 0..a), a.. = (a..10000), not given (>=1 = 1..10000)
 <mass req.>  : m[center,window] : center - window/2 < inv. mass < center + window/2
 [kin. req.]  : p[min,max] ; pt[min,max] ; pz[min,max] ; tht[min,max] ; phi[min,max]. Can be shortened to [,max] = [0.0,max] and [min,] = [min,1e8]
                
 Example: C(any ; 4..) && C(tr+- ; 2 ; tht[5,20] ; p[1.0,]) && M(pi+ pi- gam gam ; m[0.782,0.1] ; pt[0.5,2.0])
   - at least >=4 particles in event
   - AND exactly two charged tracks with 5° < theta < 20° and p > 1.0 GeV/c
   - AND at least one possible pi+ pi- gamma gamma combination with |m - 0.782| < 0.1 GeV/c2 and 0.5 < pt < 2.0 GeV/c
 
 
 Concerning FairPrimaryGenerator:

 The PndFilteredPrimaryGenerator is responsible for the handling of the MC input.
 Several input generators can be registered to it; these have to be
 derived from the FairGenerator class. The PndFilteredPrimaryGenerator defines
 position and (optionally) smearing of the primary vertex.
 This class should be instantiated only once.
 */


#ifndef PndFilteredPrimaryGenerator_H
#define PndFilteredPrimaryGenerator_H

#include "FairPrimaryGenerator.h"
#include "FairEvtFilterParams.h"
#include "FairRunSim.h"

#include "PndSmpFilt.h"
#include "PndSmpCand.h"

#include "TFile.h"                     // for TFile
#include "TLorentzVector.h"
#include "TString.h"
#include "Rtypes.h"                   // for Double_t, Bool_t, Int_t, etc

//#include <iosfwd>                     // for ostream
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <vector>
#include <unordered_set>
#include <map>


class FairGenericStack;
class TDatabasePDG;


typedef vector<PndSmpFilt> PndSmpFilterSet;
typedef std::vector<PndSmpCand> PndSmpCandList;
typedef std::vector<TString> StrVec;


class PndFilteredPrimaryGenerator : public FairPrimaryGenerator
{

public:

	/** @brief Default constructor. **/
	PndFilteredPrimaryGenerator();

	/** @brief Constructor with filter-set string **/
	PndFilteredPrimaryGenerator(TString filterset);

	/** @brief Destructor. **/
	virtual ~PndFilteredPrimaryGenerator() {};

	/** @brief Initialize the event generator(s) and the event (veto) filter(s). **/
	virtual Bool_t Init();

	/** @brief Registers a filter as a string to be parsed 
	Each filter set consist of one filter definition or a number of filters connected with logical && (AND) (= a certain event type).
	All filter sets are combined with a logical || (OR) (all event types to be accepted). 
	Syntax: "Filt_1.1 && Filt_1.2  ||  Filt_2.1  ||  Filt_3.1 && Filt_3.2 && Filt_3.2"
	*/
	void AddFilter(TString filterStr);

	/** @brief Calls event generators and the event filters.
	 *
        To be called at the beginning of each event from FairMCApplication.
        Generates an event vertex and calls the ReadEvent methods from the
        registered generators.
        Calls defined event (veto) filters to decide whether to process the event
        or to call the event generators again.
	 *@param pStack The particle stack
	 *@return kTRUE if successful, kFALSE if not
	 **/
	virtual Bool_t GenerateEvent(FairGenericStack* pStack);

	/** @brief Define the maximum number of times that this object should try to find an event which suits all event filters. **/
	void SetFilterMaxTries(Int_t maxTries=99999)
	{
		if ( maxTries > 0 ){
			fEvtFilterStat.fFilterMaxTries = maxTries;
			std::cout << "PndFilteredPrimaryGenerator: maxTries is now set to " << fEvtFilterStat.fFilterMaxTries << "\n";
		} else {
			std::cout << "\n\n\n  -WARNING from PndFilteredPrimaryGenerator: maxTries must be a positive number! Check your SetFilterMaxTries call!\n\n\n";
		}
	}

	/** @brief returns the maximum number of times that this object should try to find an event which suits all event filters. */
	Int_t GetNumberOfFilterMaxTries(){
		return fEvtFilterStat.fFilterMaxTries;
	}

	/** @brief returns the total (accepted + rejected) number of events generated by the event generators. If no event filters are used this number is equal to the number of simulated events. */
	Int_t GetNumberOfGeneratedEvents(){
		return fEvtFilterStat.fGeneratedEvents;
	}

	/** @brief Sets the frequency (accepted events) for printout (verbose>0) of accepted and generated events. */
	void SetEventPrintFrequency(int freq)
	{
		fEventPrintFreq = freq;
	}

	/** @brief Returns the number of cases in which no matching event was found within the set max. tries.
	 *
		 This method returns 0 if everything works fine.
		 If it returns a value >0 it means that you should set a higher limit in SetFilterMaxTries.
		 If it returns a value which is equal to the number of events that you requested, it means that either the max. number of tries is set way too low
		 or that the generator does not create such events that you are interested in
		 or that your event filters cannot be satisfied at all (logical error).
	 */
	Int_t GetNumberOfFilterFailedEvents(){
		return fEvtFilterStat.fFailedFilterEvents;
	}

	/** @brief Writes all relevant event filter information to the output root file */
	void WriteEvtFilterStatsToRootFile(TFile* outputFile=NULL){
		std::cout << "\n\nGenerated Events = " << GetNumberOfGeneratedEvents() << "\n";
		if (0 < GetNumberOfFilterFailedEvents() ) {
			std::cout << "WARNING: Number of events where the event filter FAILED " << GetNumberOfFilterFailedEvents() << "\n\n\n";
			std::cout << "Random events were accepted to avoid infinite loops. \n";
			std::cout <<  "Try increasing the max. number of tries or change your filter (maybe the generators do not produce such events as you want).\n\n";
		}
		if(outputFile==NULL) outputFile = FairRunSim::Instance()->GetOutputFile();
		outputFile->cd();
		outputFile->mkdir("FairEvtFilter");
		outputFile->cd("FairEvtFilter");
		fEvtFilterStat.Write();
		outputFile->cd();
	}

	/**@brief Set the level of commenting output.
	 * @param verbose Level of commenting output, 0 means no output, higher gives more output.
	 */
	void SetVerbose(Int_t verbose=12)
	{
		if ( verbose >= 0 )
		{
			fVerbose = verbose;
			std::cout << "PndFilteredPrimaryGenerator: fVerbose is now set to " << verbose << "\n";
		}
		else 
		{
			std::cout << "\n\n\n  -WARNING from PndFilteredPrimaryGenerator: verbose must be a positive number! Check your SetVerbose call!\n\n\n";
		}
	}



protected:

	/** @brief Combines upt to five particle lists of PndSmpCand with overlap and double counting prevention **/
    PndSmpCandList CombineList(int pdg, PndSmpCandList *l0, PndSmpCandList *l1, PndSmpCandList *l2=0, PndSmpCandList *l3=0, PndSmpCandList *l4=0);

	/** @brief Prints a candidate lits **/
	void PrintSmpCandList(PndSmpCandList l, TString name="");
	
	/** @brief Splits a TString to substrings **/
	StrVec SplitString(TString s, TString delim=" ");
	
	/** @brief Checks whether P4 kinematics match the criteria of a PndSmpFilt  **/
	bool   CheckKinematic(const PndSmpFilt &f, const TLorentzVector &p4);
	
	/** @brief Gets anti-pdg code, if exists. If not returns the code itself (particle is its anti-particle) **/
	int    AntiPdgCode(int pdg);
	
	/** @brief Turns a string of the form <float><delim><float> (e.g. "124.2,178.3") to a range a,b  **/
	void   GetRangeDouble(TString s, double &a, double &b, TString delim=",", bool forceset=false);
	
	/** @brief Turns a string of the form <int><delim><int> (e.g. "3..8") to a range a,b **/
	void   GetRangeInt(TString s, int &a, int &b, TString delim="..");

	/** @brief Contains the filter-sets.
	Each filter set consist of one filter definition or a number of filters connected with logical AND (= a certain event type).
	All filter sets are combined with a logical OR (all event types to be accepted). 
	Structure is like: (Filt_1.1 AND Filt_1.2)  OR  (Filt_2.1)  OR  (Filt_3.1 AND Filt_3.2 AND Filt_3.2) 
	*/
	std::vector<PndSmpFilterSet> fFilterSets;

	/** @brief Contains the statistics of the event filtering process. */
	FairEvtFilterParams fEvtFilterStat;

	/** @brief Level of commenting output, 0 means no output, higher gives more output. */
	Int_t fVerbose;

	/** @brief Event number (Set by the filtered primary generator **/
	Int_t fEventNrFiltered;

	/** @brief Print frequency for filtered events **/
	Int_t fEventPrintFreq; 

	/** @brief Shortcut to TDatabasePDG **/
	TDatabasePDG *fdbPdg;

	/** @brief particle names for particles to count (with and w/o charged specification, also simply tracks and neutrals) **/
	std::vector<TString> fPartNames;
	
	/** @brief particle codes for particles to count (with and w/o charged specification, also simply tracks and neutrals) **/
	std::vector<int>     fNamePdg;

	/** @brief  particle codes for the lists used for combinatorics; !!! the codes are not selected from MC truth, just the corresponding mass hypos are applied to charged tracks **/
	std::vector<int>     fCombFsPdg;

	/** @brief  set to identify particles from MC truth list which can be combined	**/
	std::unordered_set<int>    fSetFsPdg;

	/** @brief  mapes names to (pdg) codes	**/
	std::map<TString, int>     fNameCodeMap;
	
	/** @brief  mapes (pdg) codes to names	**/
	std::map<int, TString>     fCodeNameMap;


private:
	PndFilteredPrimaryGenerator(const PndFilteredPrimaryGenerator&);
	PndFilteredPrimaryGenerator& operator=(const  PndFilteredPrimaryGenerator&);


	ClassDef(PndFilteredPrimaryGenerator,1);
};

#endif
