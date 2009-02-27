double CalcEventsPerYear(Int_t simEvents, Int_t interactionRate = 2*1E7, Double_t dutyCycle = 0.5)
{
	Int_t secondsPerYear = 60*60*24*365;
	return (double)interactionRate/simEvents * secondsPerYear * dutyCycle;
}
