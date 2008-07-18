#ifndef DEDXHISTOPAINTER_H
#define DEDXHISTOPAINTER_H


class PDGMap;
class GausFitCenterInitiator;

class DEDXHistoPainter
{
public:
	DEDXHistoPainter();
	~DEDXHistoPainter();
	void SetPDGMap(PDGMap *pPDGMap);
	void SetMomentaV2();
	void LoadHistograms();
	void DrawSeparationPower();
	void DrawResolution();
	void DrawBBFitting();
	void DrawBBSummary();
	void DrawGausFitting();
private:
	PDGMap *fPDGMap;
	GausFitCenterInitiator *fCenter;
	
};

#endif
