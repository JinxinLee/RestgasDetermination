/** 
 * @defgroup PndEmc PndEmc
 * @brief Electromagnetic calorimeter
 *
 * @section Simulation
 * Simulation is handled by PndEmc. The energy deposit creates an PndEmcPoint, from those the 
 * task PndEmcHitProducer creates PndEmcHit%s.
 * 
 * @section Geometry
 * Geometry is handled by the PndEmcMapper. The class PndEmcTwoCoordIndex is used to store the crystal 
 * coordinates and check for neighbors.
 * 
 * @section Tasks
 * The standard way to create PndEmcRecoHit from detector hits (PndEmcPoint) involves the following tasks:
 * * PndEmcHitProducer: creates PndEmcHit%s from PndEmcPoint%s
 * * PndEmcHitsToWaveform: creates PndEmcWaveform%s from PndEmcHit%s
 * * PndEmcWaveformToDigi: creates PndEmcDigi%s from PndEmcWaveform%s
 * * PndEmcMakeCluster: creates PndEmcCluster%s from PndEmcDigi%s
 * * PndEmcMakeBump: creates PndEmcBump%s from PndEmcCluster%s
 * * PndEmcMakeRecoHit: creates PndEmcRecoHit%s from PndEmcBump%s
 * 
 * As an alternative, one can use PndEmcMakeDigi to create PndEmcDigi%s directly from PndEmcHit%s, 
 * bypassing the waveform generation and pulse shape analysis.
 * 
 * @section Waveforms
 * To simulate the full detector readout chain, the waveform of the detector and the following pulse
 * shape analysis are simulated. The task PndEmcHitsToWaveform uses the pulseshape PndEmcAsicPulseshape
 * for barrel, FwEndcap and BwEndcap and PndEmcCRRCPulseshape for the shashlyk. The task PndEmcWaveformToDigi
 * then uses PndEmcPSAMatchedDigiFilter for the pulse shape analysis of barrel, FwEndcap and BwEndcap and
 * PndEmcPSAParabolic for the shashlyk.
 * @subsection Alternative
 * An alternative way to generate waveforms is PndEmcFWEndcapTimebasedWaveforms, which was developed 
 * for the FwEndcap, but can be used for all of the EMC.
 * 
 */
