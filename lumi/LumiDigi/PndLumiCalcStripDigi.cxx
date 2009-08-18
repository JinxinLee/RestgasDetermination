#include "PndLumiCalcStripDigi.h"
//TRandom3 *fRNG = new TRandom3();
PndLumiCalcStripDigi::PndLumiCalcStripDigi()
{
	 fPitch = 0.0;
	 fOrient = 0.0;
	 fWidth = 0.0;
	 fLength = 0.0;
	 fThreshold = 0.0;
	 fNoise = 0.;
	 fSigma = -1.;
	 fStripZeroId = TVector2 (0.0, 0.0);
}

PndLumiCalcStripDigi::PndLumiCalcStripDigi(Double_t pitch, Double_t orient,
		Double_t width, Double_t length, Double_t threshold, Double_t noise,
		Double_t sigma,TVector2 stripzeroId)
{
	fPitch = pitch;
	fOrient = orient;
	fWidth = width;
	fLength = length;
	fThreshold = threshold;
	fNoise = noise;
	fSigma = sigma;
	fStripZeroId = stripzeroId;
}

PndLumiCalcStripDigi::~PndLumiCalcStripDigi()
{

}

std::vector<PndLumiStrip> PndLumiCalcStripDigi::GetStrips(FairGeoVector in, FairGeoVector out,
		Double_t eLoss)
{
	// Get the 2d-Projection of trajectory
    TVector2 in_2d  (in.getX(),in.getY());
    TVector2 out_2d (out.getX(),out.getY());
    TVector2 path_2d = out_2d - in_2d;
    //Real Path
    Double_t path = path_2d.Mod();

    std::vector<PndLumiStrip> strip;

    Double_t inStripId  = CalcStripFromHit(in.getX() , in.getY());
    Double_t outStripId = CalcStripFromHit(out.getX() , out.getY());

    Int_t nuIn = Int_t(inStripId);
    Int_t nuOut = Int_t(outStripId);

    //Orthogonal distance between the entry point and exit point
    Double_t d;

    //Track direction
    Double_t dir = 0.;
    if (outStripId > inStripId) dir=1.;
    else dir = -1.;

    Double_t Q = eLoss;//

    //Variable:
    Double_t dQ; // fraction charge collected in one strip
    Double_t dp; // fraction path inside one strip
    Double_t sQ;

    //******Collect Information in each strip fired

    // only one strip hit ?
    if (nuIn == nuOut){
    	// this strip collected the entire charge
    	strip.push_back(PndLumiStrip( nuIn , path , Q ));
		//cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;

    } else {
    	Int_t nrHits = 0;

    	// info @ the first strip
    	d = TMath::Abs(inStripId-outStripId);
    	
    	if (dir>0.){
        	dp = ((nuIn + 1) - inStripId) / d;
        	dQ = Q * dp;
        	strip.push_back(PndLumiStrip( nuIn , (dp * path), dQ ));
            //cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        	nrHits++;
            Q -= dQ;
            path -=(dp * path);
            inStripId = inStripId + 1.;
        }else {
        	dp = (inStripId - nuIn) / d ;
        	dQ = Q * dp;
            strip.push_back(PndLumiStrip( nuIn , (dp * path), dQ ));
            //cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
            nrHits++;
            Q -= dQ;
            path -=(dp * path);
            inStripId = inStripId;
        }
    	d = TMath::Abs(Int_t(inStripId)-outStripId);
    	
	// info @ the last strip
        if (dir<0.){
        	dp = (( nuOut + 1) - outStripId) / d;
        	dQ = Q * dp;
        	strip.push_back(PndLumiStrip( nuOut , (dp * path) , dQ ));
            //cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        	nrHits++;
            Q -= dQ;
            path -= (dp * path);
            outStripId = outStripId + 1.;
        }else {
        	dp = (outStripId - nuOut) / d;
        	dQ = Q * dp;
        	strip.push_back(PndLumiStrip( nuOut , (dp * path) , dQ ));
            //cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        	nrHits++;
            Q -= dQ;
            path -= (dp * path);
            outStripId = outStripId;
        }
        
        // Distribute the charge among the Int_termediate strips
        d = TMath::Abs(Int_t(inStripId)-Int_t(outStripId));
        
        if (d != 0){
        	dp = 1./d ;
        	dQ = Q * dp;
        	sQ = AddNoise(dQ);
        	if (dir>0.){
        		for ( Int_t iStrip = nuIn +1 ; iStrip < nuOut; iStrip++){
        			if (sQ >= fThreshold)
        				strip.push_back(PndLumiStrip( iStrip , (dp * path) , sQ));
        			//cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        			nrHits++;
        		}
        	}else {
        		for (Int_t iStrip = nuOut +1 ; iStrip < nuIn; iStrip++){
        			if (sQ >= fThreshold)
        				strip.push_back(PndLumiStrip( iStrip , (dp * path) , sQ));
        			//cout << "PndLumiCalcStrip : "<<strip.back( ) << endl;
        			nrHits++;
        		}
        	}
        }
    }
   return strip;
}


std::vector<PndLumiStrip> PndLumiCalcStripDigi::GetStripsDigi(FairGeoVector in,
		FairGeoVector out, Double_t eLoss)
{
	std::vector<PndLumiStrip> strips = GetStrips(in,out,eLoss);
	//cout << " Number of strips initially fired : "<< strips.size()<<endl;

	Double_t inStripId = CalcStripFromHit(in.getX(),in.getY());
	Double_t outStripId = CalcStripFromHit(out.getX(),out.getY());

	Int_t nuIn = Int_t(inStripId);
	Int_t nuOut = Int_t(outStripId);

	//Path direction
	Double_t dir=0.;
	if ( outStripId > inStripId) dir=1.;
	else dir = -1.;

	Double_t Y1 = (inStripId - nuIn) * fPitch;
	Double_t Y2 = (outStripId - nuOut) * fPitch;

	//PseudoPath : path projection on y-axis in strip coord syst
	Double_t path = (TMath::Abs(inStripId-outStripId))*fPitch;

	Int_t size = strips.size();

	if (size==1){

		// Variable need for the charge sharing
		Double_t q_head, q_tail;
		Double_t sQ;
		Int_t id;
		Double_t charge_1, charge_2;

		//Collect information from the hit strip
		PndLumiStrip strip = strips.at(0);
		Double_t path_real = strip.GetPath();

		Double_t Q = strip.GetCharge();

		Int_t index = strip.GetIndex();
		strips.clear();

		// Charge ratio collected by the left (dq_0) and
		// by the right (dq_p) of the hit strip respectively
		Double_t dq_0 = ChargeDiffusion(0.,Y1, path, dir, Q);
		Double_t dq_p = Q - ChargeDiffusion(fPitch,Y1, path, dir, Q);
		Double_t dq_c = Q - dq_0 - dq_p; // charge left in the center

		//Add noise for dq_0 and dq_p
		Double_t dQ_0 = AddNoise(dq_0);

		Double_t dQ_p = AddNoise(dq_p);

		//If only 1 strip is involved on the digitization
		if (!(AboveThreshold(dQ_0)) &&
				!(AboveThreshold(dQ_p))){

			sQ = AddNoise(dq_c);
			if (sQ >= fThreshold)
				strips.push_back(PndLumiStrip(index,path_real,sQ));

		}	else{
			Int_t i = 0;
			Int_t j = 1;

			//Charge distribution at the HEAD of the track
			while (AboveThreshold(dQ_0))
			{
				charge_1 = ChargeDiffusion(((i-1)*fPitch),Y1, path, dir, Q);
				charge_2 = ChargeDiffusion((i*fPitch),Y1, path, dir, Q);

				q_head = charge_2 - charge_1;
				if (q_head == 0) break;
				id = index + i-1;
				sQ = AddNoise(q_head);
				if (AboveThreshold(sQ))
					strips.push_back(PndLumiStrip( id , 0. , sQ ));
				dq_0 -=  q_head;

				dQ_0 = AddNoise(dq_0);
				i--;

			}

			//Charge distribution at the TAIL of the track
			while (AboveThreshold(dQ_p))
			{
				charge_1 = ChargeDiffusion((j*fPitch),Y1, path, dir, Q);
				charge_2 = ChargeDiffusion(((j+1)*fPitch),Y1, path, dir, Q);
				q_tail = charge_2 -charge_1;
				if (q_tail == 0) break;
				id = index + j;
				sQ = AddNoise(q_tail);
				if (AboveThreshold(sQ))
					strips.push_back(PndLumiStrip( id , 0. , sQ ));
				dq_p -= q_tail;
				dQ_p = AddNoise(dq_p);
				j++;

			}

			//Charge left in the strip originally fired
			Double_t restcharge =  ChargeDiffusion(fPitch,Y1, path, dir, Q)-
			ChargeDiffusion(0.,Y1, path, dir, Q);
			strips.push_back (PndLumiStrip(index,path_real,AddNoise(restcharge)));


		}


	}else {

		Double_t Y_head, Y_tail;
		std::vector<PndLumiStrip> str_head;
		std::vector<PndLumiStrip> str_tail;
		PndLumiStrip strip_head;
		PndLumiStrip strip_tail;
		Int_t id_head, id_tail;
		Double_t Q_head, Q_tail, Q;
		Double_t p_head, p_tail;

		Double_t sQ_head, sQ_tail;
		Double_t sQ;
		Double_t charge_1, charge_2;
		Double_t q_head, q_tail;
		Int_t id;

		if (nuIn < nuOut ) {
			strip_head = strips.at(0);
			strip_tail = strips.at(1);
			Y_head = Y1;
			Y_tail = (size-1)*fPitch+ Y2;
		} else{
			strip_head = strips.at(1);
			strip_tail = strips.at(0);
			Y_head = Y2;
			Y_tail = (size-1)*fPitch+Y1;
		}

		id_head = strip_head.GetIndex();
		p_head = strip_head.GetPath();
		Q_head = strip_head.GetCharge();

		id_tail= strip_tail.GetIndex();
		p_tail= strip_tail.GetPath();
		Q_tail= strip_tail.GetCharge();

		Q = eLoss;
		cout << endl;

		strips.erase(strips.begin(),strips.begin()+2);

		Double_t dq_head = ChargeDiffusion(0.,Y_head, path, 1., Q);
		Double_t dq_tail = Q - ChargeDiffusion(size*fPitch, Y_tail, path, -1., Q);
		Double_t dq_head_hit = Q_head - dq_head;
		Double_t dq_tail_hit = Q_tail - dq_tail;

		Double_t dQ_head = AddNoise(dq_head);

		Double_t dQ_tail = AddNoise(dq_tail);


		if (!(AboveThreshold(dQ_head)) &&
				!(AboveThreshold(dQ_tail))){

			sQ_head = AddNoise(dq_head_hit);

			sQ_tail = AddNoise(dq_tail_hit);
			if (AboveThreshold(sQ_head))
				str_head.push_back(PndLumiStrip(id_head, p_head , sQ_head));
			if (AboveThreshold(sQ_tail))
				str_tail.push_back(PndLumiStrip(id_tail, p_tail , sQ_tail));

			strips.insert(strips.begin(),str_head.begin(),str_head.end());
			strips.insert(strips.end(),str_tail.begin(),str_tail.end());

		} else{

			// Charge distribution at the HEAD (lowest stripID) of the track
			Int_t i = 0;
			while(AboveThreshold(dQ_head)){
				charge_1 = ChargeDiffusion(((i-1)*fPitch),Y_head, path, 1., Q);
				charge_2 = ChargeDiffusion((i*fPitch),Y_head, path, 1., Q);
				q_head = charge_2 - charge_1;
				if (q_head == 0) break;
				id = id_head + i-1;

				if (AboveThreshold(sQ_head)){
					str_head.push_back(PndLumiStrip(id, 0., sQ_head));
				}
				dq_head -= q_head;

				dQ_head = AddNoise(dq_head);
				i--;

			}
			//charge left on the head strip hit

			Double_t restQ_head = ChargeDiffusion(fPitch,Y_head, path, 1., Q)-
			ChargeDiffusion(0.,Y_head, path, 1., Q);
			sQ_head = AddNoise(restQ_head);
			if (AboveThreshold(sQ_head)){
				str_head.push_back(PndLumiStrip(id_head, p_head, sQ_head));
			}
			strips.insert(strips.begin(),str_head.begin(), str_head.begin()+str_head.size());

			// Charge distribution at the TAIL (highest stripID) of the track
			Int_t j = 1;
			while(AboveThreshold(dQ_tail)){
				id = id_tail + j;
				charge_1 = ChargeDiffusion(((j+(size-1))*fPitch),Y1, path, -1., Q);
				charge_2 = ChargeDiffusion(((j+1+(size-1) )*fPitch),Y1, path, -1., Q);
				q_tail = charge_2 -charge_1;
				if (q_head == 0) break;
				if (AboveThreshold(sQ_tail)){
					str_tail.push_back(PndLumiStrip(id, 0, sQ_tail));
				}
				dq_tail = Q_tail - charge_2;

				dQ_tail = AddNoise(dq_tail);
				j++;

			}
			//charge left on the tail strip hit

			Double_t restQ_tail = ChargeDiffusion(size*fPitch ,Y_tail, path, -1., Q)-
			ChargeDiffusion((size-1)*fPitch ,Y_tail, path, -1., Q);
			sQ_tail = AddNoise(restQ_tail);
			if (AboveThreshold(sQ_tail)){
				str_tail.push_back(PndLumiStrip(id_tail, p_tail, sQ_tail));
			}
			strips.insert(strips.end(),str_tail.begin(), str_tail.begin()+str_tail.size());

		}


	}


	return strips;
}


Double_t PndLumiCalcStripDigi:: AddNoise(Double_t charge)
{
	Double_t Q_n = fRND->Gaus(charge, fNoise);
    return Q_n;
}


Double_t PndLumiCalcStripDigi::ChargeDiffusion(Double_t x, Double_t y,
		Double_t path, Double_t dir, Double_t eLoss)
{
	Int_t N = 100000;
	Double_t DQ = 0.;
	Double_t ds = path/N;
	Double_t dQ = eLoss/N;
	Double_t phi; //part of gauss area from -infinity to x

	if (path == 0.){
		phi = 0.5*(1+TMath::Erf((x-y)/(fSigma*std::sqrt(2.))));
		DQ = (phi * dQ);
	}else{
		if (dir > 0.){
			for (Double_t dy = y ; dy <= y + path ; dy += ds){
				phi = 0.5*(1+TMath::Erf((x-dy)/(fSigma*std::sqrt(2.))));
				DQ += (phi * dQ);
			}
		}
		if (dir < 0.){
			for (Double_t dy = y ; dy >= y - path ; dy -= ds){
				phi = 0.5*(1+TMath::Erf((x-dy)/(fSigma*std::sqrt(2.))));
				DQ += (phi * dQ);
			}
		}
	}

	return DQ;
}


Double_t PndLumiCalcStripDigi::CalcStripFromHit(Double_t x, Double_t y)
{

	Double_t a = fStripZeroId.X();
	Double_t b = fStripZeroId.Y();
	Double_t zero;
	zero =TMath::Abs(( -a*sin(fOrient) + b*cos(fOrient))/fPitch);

	Double_t nr = (-x*sin(fOrient) + y *cos(fOrient))/fPitch;

	return zero + nr;

}


std::map<Int_t,PndLumiStrip> PndLumiCalcStripDigi::GetClusters(std::vector<PndLumiStrip> strip)
{
	std::map<Int_t,PndLumiStrip> clust;
	std::map<Int_t,PndLumiStrip>::iterator it;
	clust.clear();

	for (Int_t j = 0; j < strip.size(); j++){
		clust[strip[j].GetIndex()] = strip[j];
	}

	
	return clust;

}

bool PndLumiCalcStripDigi::AboveThreshold(Double_t Q)
{
	if (Q >= fThreshold){
		return true;
	} else {
		return false;
	}
}

ClassImp(PndLumiCalcStripDigi)
