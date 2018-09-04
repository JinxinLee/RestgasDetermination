#include "TFile.h"
#include "TGeoArb8.h"
#include "TGeoCompositeShape.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoMatrix.h"
#include "TGeoPcon.h"
#include "TGeoTube.h"
#include "TGeoVolume.h"
#include "TMath.h"
#include "TSystem.h"
#include "TVector3.h"
#include "TView.h"
#include "TVirtualPad.h"
#include "TTree.h"

#include <iostream>
using namespace std;

TGeoManager* geoMan;
TGeoTrap* crystals_L[11];
TGeoTrap* crystals_R[11];
TGeoCompositeShape* wrappings_L[11];
TGeoCompositeShape* wrappings_R[11];
TGeoCompositeShape* front_inserts_L[11];
TGeoCompositeShape* front_inserts_R[11];
TGeoCompositeShape* alveoles_L_plus[43];
TGeoCompositeShape* alveoles_R_plus[43];
TGeoCompositeShape* alveoles_L_minus[28];
TGeoCompositeShape* alveoles_R_minus[28];
TGeoCombiTrans* matrices_minus_left[28];
TGeoCombiTrans* matrices_minus_right[28];
TGeoCombiTrans* matrices_plus_left[43];
TGeoCombiTrans* matrices_plus_right[43];
TGeoTranslation* matrices_fe2cry_left[11]; // front ends to crystals
TGeoTranslation* matrices_fe2cry_right[11];
TGeoMatrix* matrices_alveole_minus_left[28];
TGeoMatrix* matrices_alveole_minus_right[28];
TGeoMatrix* matrices_alveole_plus_left[43];
TGeoMatrix* matrices_alveole_plus_right[43];
Double_t crystal_dz[43];
Double_t crystal_theta[44];
TGeoMatrix* matrices_phi[5];
TGeoTranslation* matrices_crystal2alveole_L[11];
TGeoTranslation* matrices_crystal2alveole_R[11];

Double_t AF[11] = {2.121, 2.118, 2.117, 2.117, 2.117, 2.119, 2.122, 2.123, 2.123, 2.125, 2.125};
Double_t BF[11] = {2.128, 2.128, 2.128, 2.128, 2.128, 2.128, 2.128, 2.128, 2.128, 2.128, 2.128};
Double_t CF[11] = {2.127, 2.139, 2.151, 2.160, 2.169, 2.178, 2.186, 2.191, 2.195, 2.200, 2.202};
Double_t AR[11] = {2.904, 2.878, 2.836, 2.790, 2.735, 2.672, 2.623, 2.570, 2.514, 2.470, 2.435};
Double_t BR[11] = {2.875, 2.875, 2.875, 2.722, 2.722, 2.722, 2.547, 2.547, 2.547, 2.442, 2.442};
Double_t CR[11] = {2.912, 2.907, 2.881, 2.845, 2.801, 2.747, 2.699, 2.651, 2.600, 2.556, 2.523};
Double_t theta_hole_corr = -(4.997 + 3.862213309 + 90.);

void geom();
void ConstructSlice(TGeoVolume* vol);
void ConstructTargetSlice(TGeoVolume* vol);
void ConstructSuperModule(TGeoVolume* vol, Int_t id, Bool_t is_target = kFALSE);
void ConstructModule(TGeoVolume* vol, Int_t id, Int_t sign, Bool_t is_target = kFALSE);
void CalculateCrystalMatricesZ();
void CalculateCrystalMatricesPhi();
void CreateFrontInsertShapesAndMatricesZ();
void CreateAlveoleShapesAndMatricesZ();
//TGeoTranslation* local2master_vector(const TGeoTranslation* local, const TGeoMatrix* matrix);
TGeoTrap* create_trap(TString name, Int_t type, Double_t af, Double_t bf, Double_t cf,
    Double_t ar, Double_t br, Double_t cr, Double_t l);
TGeoTrap* create_trap(TString name, Int_t type, Double_t af, Double_t bf, Double_t cf,
    Double_t ar, Double_t br, Double_t cr, Double_t l, Int_t i);
void get_trap_vertices(TGeoTrap*, TVector3* points);
void raytrace_x(TGeoManager* geom, TString filename);
void raytrace_z(TGeoManager* geom, TString filename);


int main() {
    geom();
}

void geom() {
    //--- Definition of a simple geometry
    gSystem->Load("libGeom");
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libPndData");
    gSystem->Load("libPassive");

    // load create materials/media
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile("media_pnd.geo");
    geoFace->readMedia();
    geoFace->print();

    FairGeoMedia* Media = geoFace->getMedia();
    FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();

    FairGeoMedium* PWO = Media->getMedium("PWO");
    FairGeoMedium* Prepreg = Media->getMedium("Prepreg");
    FairGeoMedium* VM2000 = Media->getMedium("VM2000");
    FairGeoMedium* ABS = Media->getMedium("ABS");
    FairGeoMedium* Polyurethane = Media->getMedium("Polyurethane");
    FairGeoMedium* Aluminum = Media->getMedium("Aluminum");
    geobuild->createMedium(PWO);
    geobuild->createMedium(Prepreg);
    geobuild->createMedium(VM2000);
    geobuild->createMedium(ABS);
    geobuild->createMedium(Polyurethane);
    geobuild->createMedium(Aluminum);

    //--- make the top container volume
    geoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    TGeoVolume *top = new TGeoVolumeAssembly("BarrelEMC");
    geoMan->SetTopVolume(top);

    /* Crystals */
    for (Int_t i = 0; i < 11; i++) {
        crystals_L[i] = create_trap(TString::Format("crystal%d_L", i+1), 1,
            AF[i], BF[i], CF[i], AR[i], BR[i], CR[i], 20.);
        crystals_R[i] = create_trap(TString::Format("crystal%d_R", i+1), -1,
            AF[i], BF[i], CF[i], AR[i], BR[i], CR[i], 20.);
    }
    CalculateCrystalMatricesZ();
    CalculateCrystalMatricesPhi();

    /* Wrapping */
    for (Int_t i = 0; i < 11; i++) {
        Double_t l, dl, af, bf, cf, ar, br, cr, thickness;

        // out
        thickness = 0.0065;
        l = 20. + thickness*2;
        dl = (l - 20.) / 2;
        af = AF[i] - (AR[i] - AF[i]) * dl / 20. + thickness * 2;
        bf = BF[i] - (BR[i] - BF[i]) * dl / 20. + thickness * 2;
        cf = CF[i] - (CR[i] - CF[i]) * dl / 20. + thickness * 2;
        ar = AR[i] + (AR[i] - AF[i]) * dl / 20. + thickness * 2;
        br = BR[i] + (BR[i] - BF[i]) * dl / 20. + thickness * 2;
        cr = CR[i] + (CR[i] - CF[i]) * dl / 20. + thickness * 2;

        TGeoTrap* wrapping_L_out = create_trap(TString::Format("wrapping%d_L_out", i+1),
            1, af, bf, cf, ar, br, cr, l);
        TGeoTrap* wrapping_R_out = create_trap(TString::Format("wrapping%d_R_out", i+1),
            -1, af, bf, cf, ar, br, cr, l);

        // in
        thickness = 0.00001;
        l = 20. + thickness*2;
        dl = (l - 20.) / 2;
        af = AF[i] - (AR[i] - AF[i]) * dl / 20. + thickness * 2;
        bf = BF[i] - (BR[i] - BF[i]) * dl / 20. + thickness * 2;
        cf = CF[i] - (CR[i] - CF[i]) * dl / 20. + thickness * 2;
        ar = AR[i] + (AR[i] - AF[i]) * dl / 20. + thickness * 2;
        br = BR[i] + (BR[i] - BF[i]) * dl / 20. + thickness * 2;
        cr = CR[i] + (CR[i] - CF[i]) * dl / 20. + thickness * 2;

        TGeoTrap* wrapping_L_in = create_trap(TString::Format("wrapping%d_L_in", i+1),
            1, af, bf, cf, ar, br, cr, l);
        TGeoTrap* wrapping_R_in = create_trap(TString::Format("wrapping%d_R_in", i+1),
            -1, af, bf, cf, ar, br, cr, l);

        // boolean shape
        wrappings_L[i] = new TGeoCompositeShape(TString::Format("wrapping%d_L", i+1),
            TString::Format("wrapping%d_L_out - wrapping%d_L_in", i+1, i+1));
        wrappings_R[i] = new TGeoCompositeShape(TString::Format("wrapping%d_R", i+1),
            TString::Format("wrapping%d_R_out - wrapping%d_R_in", i+1, i+1));
    }

    /* Front inserts */
    CreateFrontInsertShapesAndMatricesZ();

    /* Alveoles */
    CreateAlveoleShapesAndMatricesZ();

    /* construct whole barrel */
    TGeoVolumeAssembly* Slice = new TGeoVolumeAssembly("Slice");
    ConstructSlice(Slice);
    TGeoVolumeAssembly* Slice_target = new TGeoVolumeAssembly("Slice_target");
    ConstructTargetSlice(Slice_target);
    for (Int_t i = 0; i < 1; i++) {
        double theta = 360./16 * i;
        TGeoRotation* rot = new TGeoRotation(TString::Format("rot_slice_%d", i),
                90, theta, 90, 90+theta, 0, 0);
        if (i == 0 || i == 8) top->AddNode(Slice_target, i+1, rot);
        else top->AddNode(Slice, i+1, rot);
    }

    //--- close the geometry
    geoMan->CloseGeometry();

    /* draw geometry */
    geoMan->SetVisLevel(4);
    geoMan->SetVisOption(0);
    top->Draw("ogl");
    //TView* view = gPad->GetView();
    //view->ShowAxis();

    /* overlap checking */
    //geoMan->CheckOverlaps(1e-5);

    /* raytrace */
    //raytrace_z(geoMan, "emc_raytrace_xz.root");
    //raytrace_x(geoMan, "emc_raytrace_xy.root");

    TFile* f = new TFile("emc_module12_2018v1.root", "recreate");
    top->Write();
    f->Close();
}

void ConstructSlice(TGeoVolume* vol) {
    TGeoVolumeAssembly* SuperModule1 = new TGeoVolumeAssembly("SuperModule1");
    TGeoVolumeAssembly* SuperModule2 = new TGeoVolumeAssembly("SuperModule2");
    TGeoVolumeAssembly* SuperModule3 = new TGeoVolumeAssembly("SuperModule3");
    TGeoVolumeAssembly* SuperModule4 = new TGeoVolumeAssembly("SuperModule4");
    TGeoVolumeAssembly* SuperModule5 = new TGeoVolumeAssembly("SuperModule5");
    TGeoVolumeAssembly* SuperModule6 = new TGeoVolumeAssembly("SuperModule6");
    TGeoVolumeAssembly* SuperModule7 = new TGeoVolumeAssembly("SuperModule7");

    ConstructSuperModule(SuperModule1, 1);
    ConstructSuperModule(SuperModule2, 2);
    ConstructSuperModule(SuperModule3, 3);
    ConstructSuperModule(SuperModule4, 4);
    ConstructSuperModule(SuperModule5, 5);
    ConstructSuperModule(SuperModule6, 6);
    ConstructSuperModule(SuperModule7, 7);

    vol->AddNode(SuperModule1, 1);
    vol->AddNode(SuperModule2, 1);
    vol->AddNode(SuperModule3, 1);
    vol->AddNode(SuperModule4, 1);
    vol->AddNode(SuperModule5, 1);
    vol->AddNode(SuperModule6, 1);
    vol->AddNode(SuperModule7, 1);

    // Simplified front end structures
    TGeoTubeSeg* front_logement = new TGeoTubeSeg(55.0, 55.1, 106.6, -theta_hole_corr-19.89, -theta_hole_corr+2.21);
    TGeoVolume* FrontLogement = new TGeoVolume("Front-Logement", front_logement, geoMan->GetMedium("Polyurethane"));
    FrontLogement->SetLineColor(kBlue);
    TGeoTranslation* mat_logement = new TGeoTranslation("mat-Front-Logement", 0., 0., 36.9);
    vol->AddNode(FrontLogement, 1, mat_logement);

    TGeoTubeSeg* front_al_plate = new TGeoTubeSeg(54.2, 54.3, 106.6, -theta_hole_corr-19.89, -theta_hole_corr+2.21);
    TGeoVolume* FrontAlPlate = new TGeoVolume("Front-Al-Plate", front_al_plate, geoMan->GetMedium("Aluminum"));
    FrontAlPlate->SetLineColor(kDarkTerrain);
    TGeoTranslation* mat_al_plate = new TGeoTranslation("mat-Front-Al-Plate", 0., 0., 36.9);
    vol->AddNode(FrontAlPlate, 1, mat_al_plate);
}

void ConstructTargetSlice(TGeoVolume* vol) {
    TGeoVolumeAssembly* SuperModule1 = new TGeoVolumeAssembly("SuperModule1_Target");
    TGeoVolumeAssembly* SuperModule2 = new TGeoVolumeAssembly("SuperModule2_Target");
    TGeoVolumeAssembly* SuperModule3 = new TGeoVolumeAssembly("SuperModule3_Target");
    TGeoVolumeAssembly* SuperModule4 = new TGeoVolumeAssembly("SuperModule4_Target");
    TGeoVolumeAssembly* SuperModule5 = new TGeoVolumeAssembly("SuperModule5_Target");
    TGeoVolumeAssembly* SuperModule6 = new TGeoVolumeAssembly("SuperModule6_Target");
    TGeoVolumeAssembly* SuperModule7 = new TGeoVolumeAssembly("SuperModule7_Target");

    ConstructSuperModule(SuperModule1, 1, kTRUE);
    ConstructSuperModule(SuperModule2, 2, kTRUE);
    ConstructSuperModule(SuperModule3, 3, kTRUE);
    ConstructSuperModule(SuperModule4, 4, kTRUE);
    ConstructSuperModule(SuperModule5, 5, kTRUE);
    ConstructSuperModule(SuperModule6, 6, kTRUE);
    ConstructSuperModule(SuperModule7, 7, kTRUE);

    vol->AddNode(SuperModule1, 1);
    vol->AddNode(SuperModule2, 1);
    vol->AddNode(SuperModule3, 1);
    vol->AddNode(SuperModule4, 1);
    vol->AddNode(SuperModule5, 1);
    vol->AddNode(SuperModule6, 1);
    vol->AddNode(SuperModule7, 1);

    /* Simplified front end structures */
    // Logement for cooling tubes
    TGeoTubeSeg* front_logement = new TGeoTubeSeg("front_logement", 55.0, 55.1, 106.6, -theta_hole_corr-19.89, -theta_hole_corr+2.21);
    TGeoTranslation* traLogement = new TGeoTranslation("traLogement", 0., 0., 36.9);
    traLogement->RegisterYourself();
    TGeoTube* holeLogement = new TGeoTube("holeLogement", 0., 7.6, 100.);
    TGeoRotation* rotHoleLogement = new TGeoRotation("rotHoleLogement", 90, 0, 0, 0, 90, 270);
    TGeoCombiTrans* matHoleLogement = new TGeoCombiTrans("matHoleLogement", 0., 0., 3.7, rotHoleLogement);
    matHoleLogement->RegisterYourself();
    TGeoCompositeShape* front_logement_target = 
        new TGeoCompositeShape("front_logement_target", "front_logement:traLogement - holeLogement:matHoleLogement");
    TGeoVolume* FrontLogement = new TGeoVolume("Front-Logement-Target", front_logement_target, geoMan->GetMedium("Polyurethane"));
    FrontLogement->SetLineColor(kBlue);
    vol->AddNode(FrontLogement, 1);

    // Aluminum plate for vacuum
    TGeoTubeSeg* front_al_plate = new TGeoTubeSeg("front_al_plate", 54.2, 54.3, 106.6, -theta_hole_corr-19.89, -theta_hole_corr+2.21);
    TGeoTranslation* traAlPlate = new TGeoTranslation("traAlPlate", 0., 0., 36.9);
    traAlPlate->RegisterYourself();
    TGeoTube* holeAlPlate = new TGeoTube("holeAlPlate", 0., 7.6, 100.);
    TGeoRotation* rotHoleAlPlate = new TGeoRotation("rotHoleAlPlate", 90, 0, 0, 0, 90, 270);
    TGeoCombiTrans* matHoleAlPlate = new TGeoCombiTrans("matHoleAlPlate", 0., 0., 3.7, rotHoleAlPlate);
    matHoleAlPlate->RegisterYourself();
    TGeoCompositeShape* front_al_plate_target = 
        new TGeoCompositeShape("front_al_plate_target", "front_al_plate:traAlPlate - holeAlPlate:matHoleAlPlate");
    TGeoVolume* FrontAlPlate = new TGeoVolume("Front-Al-Plate-Target", front_al_plate_target, geoMan->GetMedium("Aluminum"));
    FrontAlPlate->SetLineColor(kDarkTerrain);
    vol->AddNode(FrontAlPlate, 1);
}

void ConstructSuperModule(TGeoVolume* vol, Int_t id, Bool_t is_target) {
    if (id == 1) {
        TGeoVolumeAssembly* Module7m = new TGeoVolumeAssembly("Module7-");
        ConstructModule(Module7m, 7, -1, is_target);
        vol->AddNode(Module7m, 1);
    }
    else if (id == 2) {
        TGeoVolumeAssembly* Module6m = new TGeoVolumeAssembly("Module6-");
        ConstructModule(Module6m, 6, -1, is_target);
        vol->AddNode(Module6m, 1);

        TGeoVolumeAssembly* Module5m = new TGeoVolumeAssembly("Module5-");
        ConstructModule(Module5m, 5, -1, is_target);
        vol->AddNode(Module5m, 1);

        TGeoVolumeAssembly* Module4m = new TGeoVolumeAssembly("Module4-");
        ConstructModule(Module4m, 4, -1, is_target);
        vol->AddNode(Module4m, 1);
    }
    else if (id == 3) {
        TGeoVolumeAssembly* Module3m = new TGeoVolumeAssembly("Module3-");
        ConstructModule(Module3m, 3, -1, is_target);
        vol->AddNode(Module3m, 1);

        TGeoVolumeAssembly* Module2m = new TGeoVolumeAssembly("Module2-");
        ConstructModule(Module2m, 2, -1, is_target);
        vol->AddNode(Module2m, 1);

        TGeoVolumeAssembly* Module1m = new TGeoVolumeAssembly("Module1-");
        ConstructModule(Module1m, 1, -1, is_target);
        vol->AddNode(Module1m, 1);
    }
    else if (id == 4) {
        TGeoVolumeAssembly* Module1p = new TGeoVolumeAssembly("Module1+");
        ConstructModule(Module1p, 1, +1, is_target);
        vol->AddNode(Module1p, 1);

        TGeoVolumeAssembly* Module2p = new TGeoVolumeAssembly("Module2+");
        ConstructModule(Module2p, 2, +1, is_target);
        vol->AddNode(Module2p, 1);

        TGeoVolumeAssembly* Module3p = new TGeoVolumeAssembly("Module3+");
        ConstructModule(Module3p, 3, +1, is_target);
        vol->AddNode(Module3p, 1);
    }
    else if (id == 5) {
        TGeoVolumeAssembly* Module4p = new TGeoVolumeAssembly("Module4+");
        ConstructModule(Module4p, 4, +1, is_target);
        vol->AddNode(Module4p, 1);

        TGeoVolumeAssembly* Module5p = new TGeoVolumeAssembly("Module5+");
        ConstructModule(Module5p, 5, +1, is_target);
        vol->AddNode(Module5p, 1);

        TGeoVolumeAssembly* Module6p = new TGeoVolumeAssembly("Module6+");
        ConstructModule(Module6p, 6, +1, is_target);
        vol->AddNode(Module6p, 1);
    }
    else if (id == 6) {
        TGeoVolumeAssembly* Module7p = new TGeoVolumeAssembly("Module7+");
        ConstructModule(Module7p, 7, +1, is_target);
        vol->AddNode(Module7p, 1);

        TGeoVolumeAssembly* Module8p = new TGeoVolumeAssembly("Module8+");
        ConstructModule(Module8p, 8, +1, is_target);
        vol->AddNode(Module8p, 1);

        TGeoVolumeAssembly* Module9p = new TGeoVolumeAssembly("Module9+");
        ConstructModule(Module9p, 9, +1, is_target);
        vol->AddNode(Module9p, 1);
    }
    else if (id == 7) {
        TGeoVolumeAssembly* Module10p = new TGeoVolumeAssembly("Module10+");
        ConstructModule(Module10p, 10, +1, is_target);
        vol->AddNode(Module10p, 1);

        TGeoVolumeAssembly* Module11p = new TGeoVolumeAssembly("Module11+");
        ConstructModule(Module11p, 11, +1, is_target);
        vol->AddNode(Module11p, 1);
    }
}

void ConstructModule(TGeoVolume* vol, Int_t id, Int_t sign, Bool_t is_target) {
    // Volumes
    TGeoVolume* Crystal1;
    TGeoVolume* Crystal2;
    TGeoVolume* Wrapping1;
    TGeoVolume* Wrapping2;
    TGeoVolume* FrontInsert1;
    TGeoVolume* FrontInsert2;
    TGeoVolume* Alveole1;
    TGeoVolume* Alveole2;
    //TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
    //TGeoMedium *Vacuum = new TGeoMedium("Vacuum",0, matVacuum);

    Int_t nCrystals = (id == 11) ? 3 : 4;
    char char_sign = (sign > 0) ? 'p' : 'm';
    char char_type1 = (sign > 0) ? 'R' : 'L';
    char char_type2 = (sign > 0) ? 'L' : 'R';

    Int_t j_start = (id-1) * 4;
    Int_t j_end = j_start + nCrystals;
    for (Int_t i = 0; i < 5; i++) {
        for(Int_t j = j_start; j < j_end; j++) {
            if (is_target) {
                if (id == 1 && (0 < i && i < 4)) continue;
            }

            TString crystal_name1 = TString::Format("Crystal-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1);
            TString crystal_name2 = TString::Format("Crystal-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1);
            TString wrapping_name1 = TString::Format("Wrapping-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1);
            TString wrapping_name2 = TString::Format("Wrapping-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1);
            TString front_insert_name1 = TString::Format("FrontInsert-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1);
            TString front_insert_name2 = TString::Format("FrontInsert-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1);
            TString alveole_name1 = TString::Format("Alveole-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1);
            TString alveole_name2 = TString::Format("Alveole-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1);
            if (sign < 0) {
                Crystal1 = new TGeoVolume(crystal_name1, crystals_L[id-1], geoMan->GetMedium("PWO"));
                Crystal2 = new TGeoVolume(crystal_name2, crystals_R[id-1], geoMan->GetMedium("PWO"));
                Wrapping1 = new TGeoVolume(wrapping_name1, wrappings_L[id-1], geoMan->GetMedium("VM2000"));
                Wrapping2 = new TGeoVolume(wrapping_name2, wrappings_R[id-1], geoMan->GetMedium("VM2000"));
                FrontInsert1 = new TGeoVolume(front_insert_name1, front_inserts_L[id-1], geoMan->GetMedium("ABS"));
                FrontInsert2 = new TGeoVolume(front_insert_name2, front_inserts_R[id-1], geoMan->GetMedium("ABS"));
                Alveole1 = new TGeoVolume(alveole_name1, alveoles_L_minus[j], geoMan->GetMedium("Prepreg"));
                Alveole2 = new TGeoVolume(alveole_name2, alveoles_R_minus[j], geoMan->GetMedium("Prepreg"));
            }
            else {
                Crystal1 = new TGeoVolume(crystal_name1, crystals_R[id-1], geoMan->GetMedium("PWO"));
                Crystal2 = new TGeoVolume(crystal_name2, crystals_L[id-1], geoMan->GetMedium("PWO"));
                Wrapping1 = new TGeoVolume(wrapping_name1, wrappings_R[id-1], geoMan->GetMedium("VM2000"));
                Wrapping2 = new TGeoVolume(wrapping_name2, wrappings_L[id-1], geoMan->GetMedium("VM2000"));
                FrontInsert1 = new TGeoVolume(front_insert_name1, front_inserts_R[id-1], geoMan->GetMedium("ABS"));
                FrontInsert2 = new TGeoVolume(front_insert_name2, front_inserts_L[id-1], geoMan->GetMedium("ABS"));
                Alveole1 = new TGeoVolume(alveole_name1, alveoles_R_plus[j], geoMan->GetMedium("Prepreg"));
                Alveole2 = new TGeoVolume(alveole_name2, alveoles_L_plus[j], geoMan->GetMedium("Prepreg"));
            }
            Crystal1->SetLineColor(kCyan);
            Crystal2->SetLineColor(kCyan);
            Wrapping1->SetLineColor(kViolet);
            Wrapping2->SetLineColor(kViolet);
            FrontInsert1->SetLineColor(kWhite);
            FrontInsert2->SetLineColor(kWhite);
            Alveole1->SetLineColor(kYellow);
            Alveole2->SetLineColor(kYellow);

            // Matrices
            TGeoHMatrix* mat1_crystal;
            TGeoHMatrix* mat2_crystal;
            if (sign < 0) {
                mat1_crystal = new TGeoHMatrix((*matrices_phi[i])*(*matrices_minus_left[j]));
                mat2_crystal = new TGeoHMatrix((*matrices_phi[i])*(*matrices_minus_right[j]));
            }
            else {
                mat1_crystal = new TGeoHMatrix((*matrices_phi[i])*(*matrices_plus_right[j]));
                mat2_crystal = new TGeoHMatrix((*matrices_phi[i])*(*matrices_plus_left[j]));
            }
            mat1_crystal->SetName(TString::Format("mat-crystal-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1));
            mat2_crystal->SetName(TString::Format("mat-crystal-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1));

            TGeoHMatrix* mat1_front_insert;
            TGeoHMatrix* mat2_front_insert;
            if (sign < 0) {
                mat1_front_insert = new TGeoHMatrix((*matrices_phi[i])*(*matrices_minus_left[j])*(*matrices_fe2cry_left[id-1]));
                mat2_front_insert = new TGeoHMatrix((*matrices_phi[i])*(*matrices_minus_right[j])*(*matrices_fe2cry_right[id-1]));
            }
            else {
                mat1_front_insert = new TGeoHMatrix((*matrices_phi[i])*(*matrices_plus_right[j])*(*matrices_fe2cry_right[id-1]));
                mat2_front_insert = new TGeoHMatrix((*matrices_phi[i])*(*matrices_plus_left[j])*(*matrices_fe2cry_left[id-1]));
            }
            mat1_front_insert->SetName(TString::Format("mat-front-insert-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1));
            mat2_front_insert->SetName(TString::Format("mat-front-insert-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1));

            TGeoHMatrix* mat1_alveole;
            TGeoHMatrix* mat2_alveole;
            if (sign < 0) {
                mat1_alveole = new TGeoHMatrix((*matrices_phi[i])*
                    (*matrices_alveole_minus_left[j]));
                mat2_alveole = new TGeoHMatrix((*matrices_phi[i])*
                    (*matrices_alveole_minus_right[j]));
            }
            else {
                mat1_alveole = new TGeoHMatrix((*matrices_phi[i])*
                    (*matrices_alveole_plus_right[j]));
                mat2_alveole = new TGeoHMatrix((*matrices_phi[i])*
                    (*matrices_alveole_plus_left[j]));
            }
            mat1_alveole->SetName(TString::Format("mat-alveole-%d%c-%c%d", id, char_sign, char_type1, j-j_start+1));
            mat2_alveole->SetName(TString::Format("mat-alveole-%d%c-%c%d", id, char_sign, char_type2, j-j_start+1));


            // Nodes
            vol->AddNode(Alveole1, i+1, mat1_alveole);
            vol->AddNode(Alveole2, i+1, mat2_alveole);
            vol->AddNode(FrontInsert1, i+1, mat1_front_insert);
            vol->AddNode(FrontInsert2, i+1, mat2_front_insert);
            vol->AddNode(Wrapping1, i+1, mat1_crystal);
            vol->AddNode(Wrapping2, i+1, mat2_crystal);
            vol->AddNode(Crystal1, i+1, mat1_crystal);
            vol->AddNode(Crystal2, i+1, mat2_crystal);
        }
    }
}

void CalculateCrystalMatricesZ() {
    for (Int_t type = 0; type < 4; type++) { // type: 0 left-, 1 right+, 2 right-, 3 left+
        const Double_t crystal_gap = 0.068;
        const Double_t alveole_gap = 0.09;
        Double_t module_gap[] = {0.12, 0.25, 0.3, 0.33}; // half of the first gap

        Double_t trans_z = 0;
        Double_t trans_theta = 0;

        Int_t nModule;
        if (type == 0 || type == 2) nModule = 3;
        else nModule = 4;
        Int_t i = 0;
        crystal_theta[0] = 0.;
        for (Int_t iModule = 0; iModule < nModule; iModule++) {
            for (Int_t iAlveole = 0; iAlveole < 3; iAlveole++) {
                for (Int_t iCrystal = 0; iCrystal < 4; iCrystal++) {
                    Int_t module_id;
                    if (type == 0 || type == 2) module_id = 3 - iModule;
                    else module_id = iModule + 4;
                    Int_t alveole_id = iModule * 3 + iAlveole + 1;
                    Int_t crystal_id = iCrystal + 1;

                    if (module_id == 1 && iAlveole > 0) break; // SuperModule1 only have 1 alveole
                    if (module_id == 7 && iAlveole > 1) break; // Module7 only have 2 alveole
                    if (module_id == 7 && iAlveole == 1 && iCrystal > 2) break;  // The last alveole has 3 crystal colomns

                    // calculate crystal type
                    TGeoTrap* crystal;
                    if (type == 0 || type == 3) crystal = crystals_L[alveole_id-1];
                    else crystal = crystals_R[alveole_id-1];

                    Double_t gap = crystal_gap;
                    if (iCrystal == 0) {
                        gap = alveole_gap;
                        if (iAlveole == 0) gap = module_gap[iModule];
                    }

                    Double_t bf = crystal->GetH1()*2;
                    Double_t br = crystal->GetH2()*2;
                    Double_t l = crystal->GetDz()*2;
                    Double_t theta = TMath::ATan((br-bf)/l);

                    Double_t bl1 = crystal->GetBl1();
                    Double_t bl2 = crystal->GetBl2();
                    Double_t tl1 = crystal->GetTl1();
                    Double_t tl2 = crystal->GetTl2();
                    Double_t h1 = crystal->GetH1();
                    Double_t h2 = crystal->GetH2();
                    Double_t dz = crystal->GetDz();

                    // calculate the center of the crystal
                    TVector3 center2pivot;
                    TVector3 vertices[8];
                    get_trap_vertices(crystal, vertices);
                    if (type == 0) {
                        trans_z -= (gap + bf) / TMath::Cos(trans_theta); // trans_z < 0
                        center2pivot = -vertices[1]; // rotate c2p from ROOT coordinate to experimental coordinate
                    }
                    else if (type == 1) {
                        trans_z += (gap + bf) / TMath::Cos(trans_theta); // trans_z < 0
                        center2pivot = -vertices[2]; // rotate c2p from ROOT coordinate to experimental coordinate
                    }
                    else if (type == 2) {
                        trans_z -= (gap + bf) / TMath::Cos(trans_theta); // trans_z < 0
                        center2pivot = -vertices[2];
                    }
                    else {
                        trans_z += (gap + bf) / TMath::Cos(trans_theta); // trans_z < 0
                        center2pivot = -vertices[1];
                    }
                    if (type == 0) {
                        center2pivot.RotateX(-TMath::Pi()/2.);
                        center2pivot.RotateZ(-TMath::Pi()/2.);
                    }
                    else if (type == 1) {
                        center2pivot.RotateZ(TMath::Pi());
                        center2pivot.RotateX(-TMath::Pi()/2.);
                        center2pivot.RotateZ(-TMath::Pi()/2.);
                    }
                    else if (type == 2) {
                        center2pivot.RotateZ(TMath::Pi());
                        center2pivot.RotateX(TMath::Pi()/2.);
                        center2pivot.RotateZ(TMath::Pi()/2.);
                    }
                    else {
                        center2pivot.RotateX(TMath::Pi()/2.);
                        center2pivot.RotateZ(TMath::Pi()/2.);
                    }
                    center2pivot.RotateY(trans_theta);
                    TVector3 pivot;
                    if (type == 0 || type == 1)
                        pivot = TVector3(57.+0.112126393, -0.068, trans_z + 3.7); // EMC radius is 570mm, crystal offset w.r.t. target is 37mm
                    else
                        pivot = TVector3(57., 0, trans_z + 3.7);
                    TVector3 center = pivot + center2pivot;

                    // calculate the rotation matrix and place the crystal
                    Double_t trans_theta_deg = trans_theta/TMath::Pi()*180.;
                    TGeoRotation* crystal_rot;
                    char avleole_sign;
                    char crystal_type;
                    if (type == 0) {
                        avleole_sign = 'm';
                        crystal_type = 'L';
                    }
                    else if (type == 1) {
                        avleole_sign = 'p';
                        crystal_type = 'R';
                    }
                    else if (type == 1) {
                        avleole_sign = 'm';
                        crystal_type = 'R';
                    }
                    else {
                        avleole_sign = 'p';
                        crystal_type = 'L';
                    }
                    if (type == 0) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 270, 180-trans_theta_deg, 180, 90+trans_theta_deg, 0);
                    }
                    else if (type == 1) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 90, trans_theta_deg, 0, 90+trans_theta_deg, 0);
                    }
                    else if (type == 2) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 270, 180-trans_theta_deg, 180, 90+trans_theta_deg, 0);
                    }
                    else {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 90, trans_theta_deg, 0, 90+trans_theta_deg, 0);
                    }
                    crystal_rot->RegisterYourself();

                    TGeoCombiTrans* crystal_ctrans = new TGeoCombiTrans(TString::Format("trans-%d%c-%c%d",
                        alveole_id, avleole_sign, crystal_type, crystal_id),
                        center.X(), center.Y(), center.Z(), crystal_rot);

                    if (type == 0)
                        matrices_minus_left[i] = crystal_ctrans;
                    else if (type == 1)
                        matrices_plus_right[i] = crystal_ctrans;
                    else if (type == 2)
                        matrices_minus_right[i] = crystal_ctrans;
                    else
                        matrices_plus_left[i] = crystal_ctrans;

                    if (type == 0 || type == 2)
                        trans_theta += theta; // trans_theta > 0
                    else
                        trans_theta -= theta;

                    if (type == 1) {
                        crystal_theta[i+1] = -trans_theta;
                        crystal_dz[i] = trans_z;
                    }

                    i++;
                } // crystal
            } // module (alveole)
        } // supermodule
    } // type
}

void CalculateCrystalMatricesPhi() {
    Double_t alpha_alveole[] = {3.862213309, 3.945818621, 4.028979744, 4.11168967, 4.193941577};
    Double_t theta_alveole[] = {0, 4.416394688, 4.416838877, 4.417290073, 4.417748093};
    Double_t theta = theta_hole_corr;
    for (Int_t i = 0; i < 5; i++) {
        theta += theta_alveole[i];
        TGeoRotation rot(TString::Format("rot_alveole_row_%d", i),
                90, 360-theta, 90, 90-theta, 0, 0); // clockwise

        TGeoTranslation tra1(-57., 0, 0);
        TGeoRotation rot1("rot1_row2", 90, 360-alpha_alveole[i], 90, 90-alpha_alveole[i], 0, 0);
        TGeoTranslation tra2(57., 0, 0);
        matrices_phi[i] = new TGeoHMatrix(rot * tra2 * rot1 * tra1);
        matrices_phi[i]->SetName(TString::Format("mat_phi_%d", i));
    }
}

void CreateFrontInsertShapesAndMatricesZ() {
    // shapes
    TGeoTrap* front_inserts_L_out[11];
    TGeoTrap* front_inserts_R_out[11];
    for (Int_t i = 0; i < 11; i++) {
        Double_t l, dl, af, bf, cf, ar, br, cr, thickness;

        dl = 0.0065 + 0.00001; // wrapping + safe gap
        cr = CF[i] - (CR[i] - CF[i]) * dl / 20.;
        //ar = cr;
        ar = AF[i] - (AR[i] - AF[i]) * dl / 20.;
        br = BF[i] - (BR[i] - BF[i]) * dl / 20.;

        dl += 0.3;
        cf = CF[i] - (CR[i] - CF[i]) * dl / 20.;
        //af = cf;
        af = AF[i] - (AR[i] - AF[i]) * dl / 20.;
        bf = BF[i] - (BR[i] - BF[i]) * dl / 20.;

        l = 0.3;

        front_inserts_L_out[i] = create_trap(TString::Format("front_insert_%d_L_out", i+1),
            1, af, bf, cf, ar, br, cr, l);
        front_inserts_R_out[i] = create_trap(TString::Format("front_insert_%d_R_out", i+1),
            -1, af, bf, cf, ar, br, cr, l);

        TGeoTube* big_hole = new TGeoTube("front_insert_big_hole", 0., 0.6, 0.151);
        TGeoTube* small_hole = new TGeoTube("front_insert_small_hole", 0., 0.2, 0.151);
        TGeoTranslation* trans_big_hole_L = new TGeoTranslation("trans_front_insert_big_hole_L", -0.13, 0.13, 0.);
        trans_big_hole_L->RegisterYourself();
        TGeoTranslation* trans_small_hole_L = new TGeoTranslation("trans_front_insert_small_hole_L", 0.67, -0.67, 0.);
        trans_small_hole_L->RegisterYourself();
        TGeoTranslation* trans_big_hole_R = new TGeoTranslation("trans_front_insert_big_hole_R", 0.13, 0.13, 0.);
        trans_big_hole_R->RegisterYourself();
        TGeoTranslation* trans_small_hole_R = new TGeoTranslation("trans_front_insert_small_hole_R", -0.67, -0.67, 0.);
        trans_small_hole_R->RegisterYourself();

        front_inserts_L[i] = new TGeoCompositeShape(TString::Format("front_insert_%d_L", i+1),
            TString::Format("front_insert_%d_L_out - front_insert_big_hole:trans_front_insert_big_hole_L", i+1));
            //TString::Format("front_insert_%d_L_out - front_insert_big_hole:trans_front_insert_big_hole_L - front_insert_small_hole:trans_front_insert_small_hole_L", i+1));
        front_inserts_R[i] = new TGeoCompositeShape(TString::Format("front_insert_%d_R", i+1),
            TString::Format("front_insert_%d_R_out - front_insert_big_hole:trans_front_insert_big_hole_R", i+1));
            //TString::Format("front_insert_%d_R_out - front_insert_big_hole:trans_front_insert_big_hole_R - front_insert_small_hole:trans_front_insert_small_hole_R", i+1));

    }

    // matrices
    for (Int_t i = 0; i < 11; i++) {
        // left
        TGeoTrap* crystal = crystals_L[i];
        TGeoTrap* front_insert = front_inserts_L_out[i];

        TVector3 vertices[8];
        get_trap_vertices(crystal, vertices);
        TVector3 ccry_to_v0cry = vertices[0];                            // crystal's center point to vertex 0
        TVector3 v0cry_to_v4ins = TVector3(0., 0., -(0.0065 + 0.00001)); // crystal's vertex 0 to insert's vertex 4
        get_trap_vertices(front_insert, vertices);
        TVector3 v4ins_to_cins = -vertices[4];                                  // insert's vertex 4 to center point
        TVector3 ccry_to_cins = ccry_to_v0cry + v0cry_to_v4ins + v4ins_to_cins; // crystal's certer to insert's center

        matrices_fe2cry_left[i] = new TGeoTranslation(ccry_to_cins.X(), ccry_to_cins.Y(), ccry_to_cins.Z());

        // right
        crystal = crystals_R[i];
        front_insert = front_inserts_R_out[i];

        get_trap_vertices(crystal, vertices);
        TVector3 ccry_to_v3cry = vertices[3];                            // crystal's center point to vertex 3
        TVector3 v3cry_to_v7ins = TVector3(0., 0., -(0.0065 + 0.00001)); // crystal's vertex 3 to insert's vertex 7
        get_trap_vertices(front_insert, vertices);
        TVector3 v7ins_to_cins = -vertices[7];                                  // insert's vertex 7 to center point
        ccry_to_cins = ccry_to_v3cry + v3cry_to_v7ins + v7ins_to_cins; // crystal's certer to insert's center

        matrices_fe2cry_right[i] = new TGeoTranslation(ccry_to_cins.X(), ccry_to_cins.Y(), ccry_to_cins.Z());
    }
}

void CreateAlveoleShapesAndMatricesZ() {
    Double_t gap_phi = 0.014;
    Double_t gap_z = 0.014;
    Double_t thickness = 0.02;
    Double_t safety = 1e-4;
    Double_t dlf = 2.;
    Double_t dlr = 3.;

    /* Shapes */
    TGeoTrap* alveoles_L_out[43];
    TGeoTrap* alveoles_R_out[43];
    Int_t i = 0;
    Double_t top_plane_height[] = {21.773, 20.559, 16.696, 12.582};
    Double_t top_plane_angle[] = {88.574, 110.433, 131.197, 147.2625};
    for (Int_t iModule = 0; iModule < 4; iModule++) {
        for (Int_t iAlveole = 0; iAlveole < 3; iAlveole++) {
            for (Int_t iCrystal = 0; iCrystal < 4; iCrystal++) {
                Int_t module_id = iModule + 4;
                Int_t alveole_id = iModule * 3 + iAlveole + 1;
                Int_t crystal_id = iCrystal + 1;

                if (module_id == 1 && iAlveole > 0) break; // SuperModule1 only have 1 alveole
                if (module_id == 7 && iAlveole > 1) break; // Module7 only have 2 alveole
                if (module_id == 7 && iAlveole == 1 && iCrystal > 2) break;  // The last alveole has 3 crystal colomns

                Double_t l, dlr_i, af, bf, cf, ar, br, cr;
                Double_t theta_c = TMath::ATan((CR[0] - CF[0])/20.);

                // out
                l = dlf + 20. + dlr;
                dlr_i = 20. + dlr - (20.*TMath::Cos(crystal_theta[i]) +
                    BF[alveole_id-1]*TMath::Sin(crystal_theta[i]));
                bf = BF[alveole_id-1]/TMath::Cos(crystal_theta[i]) -
                    (BR[alveole_id-1]*TMath::Sin(TMath::Pi()/2+crystal_theta[i+1]-crystal_theta[i])/TMath::Sin(TMath::Pi()/2-crystal_theta[i+1]) -
                    BF[alveole_id-1]/TMath::Cos(crystal_theta[i])) * dlf /
                    (20.*TMath::Cos(crystal_theta[i])+BF[alveole_id-1]*TMath::Sin(crystal_theta[i])) +
                    (thickness + gap_z - safety) / TMath::Cos(crystal_theta[i+1]) +
                    (thickness + gap_z - safety) / TMath::Cos(crystal_theta[i]);
                cf = CF[0] - (CR[0] - CF[0]) * dlf / 20.
                    + (thickness + gap_phi - safety) / TMath::Cos(theta_c) + thickness + gap_phi - safety;
                af = cf;
                br = BR[alveole_id-1]*TMath::Sin(TMath::Pi()/2+crystal_theta[i+1]-crystal_theta[i])/TMath::Sin(TMath::Pi()/2-crystal_theta[i+1]) +
                    (BR[alveole_id-1]*TMath::Sin(TMath::Pi()/2+crystal_theta[i+1]-crystal_theta[i])/TMath::Sin(TMath::Pi()/2-crystal_theta[i+1]) -
                    BF[alveole_id-1]/TMath::Cos(crystal_theta[i])) * dlr_i /
                    (20.*TMath::Cos(crystal_theta[i])+BF[alveole_id-1]*TMath::Sin(crystal_theta[i])) +
                    (thickness + gap_z - safety) / TMath::Cos(crystal_theta[i+1]) +
                    (thickness + gap_z - safety) / TMath::Cos(crystal_theta[i]);
                cr = CR[0] + (CR[0] - CF[0]) * dlr / 20.
                    + (thickness + gap_phi - safety) / TMath::Cos(theta_c) + thickness + gap_phi - safety;
                ar = cr;

                TGeoTrap* alveole_L_out = create_trap(TString::Format("alveole%d_L_out", i+1),
                    1, af, bf, cf, ar, br, cr, l, i);
                TGeoTrap* alveole_R_out = create_trap(TString::Format("alveole%d_R_out", i+1),
                    -1, af, bf, cf, ar, br, cr, l, i);
                alveoles_L_out[i] = alveole_L_out;
                alveoles_R_out[i] = alveole_R_out;

                // top cut plane
                Double_t height, theta_top_plane;
                if (iAlveole == 0 && iCrystal == 0) {
                    height = top_plane_height[iModule];
                    theta_top_plane = 90.+crystal_theta[i]/TMath::Pi()*180.-top_plane_angle[iModule];
                }
                TVector3 vertices[8];
                get_trap_vertices(alveole_L_out, vertices);

                Double_t y_c1 = vertices[0].Y() +
                    (vertices[4].Y()-vertices[0].Y())/l*(dlf+height);
                Double_t z_c = vertices[0].Z() + height + dlf;
                TVector3 h0(0., y_c1, z_c);
                TVector3 c2center(0, 0, 100.);
                c2center.RotateX(-theta_top_plane/180*TMath::Pi());
                TVector3 center_c = h0 + c2center;
                TGeoRotation* rot_top_plane = new TGeoRotation(
                    TString::Format("rot_top_plane_%d", i+1), 90, 0, 90+theta_top_plane, 90, theta_top_plane, 90);
                TGeoCombiTrans* cmb_top_plane = new TGeoCombiTrans(TString::Format("cmb_top_plane_%d", i+1),
                    center_c.X(), center_c.Y(), center_c.Z(), rot_top_plane);
                cmb_top_plane->RegisterYourself();

                TGeoBBox* top_box = new TGeoBBox(
                    TString::Format("cut_box_%d", i+1), 100., 100., 100.);

                height = height - (bf+(br-bf)/l*(height+dlf))/
                    (1/TMath::Tan(theta_top_plane/180*TMath::Pi())+TMath::Tan(crystal_theta[i+1]));

                // bottom cut plane
                // left-
                get_trap_vertices(alveole_L_out, vertices);
                h0.SetXYZ(vertices[2].X(), vertices[2].Y(), vertices[2].Z() + dlf - 0.158);
                c2center.SetXYZ(0., 0., -100.);
                c2center.RotateY(-4./180*TMath::Pi());
                center_c = h0 + c2center;
                TGeoRotation* rot_bottom_plane_left_minus = new TGeoRotation(
                    TString::Format("rot_bottom_plane_left_minus_%d", i+1), 90-4, 0, 90, 90, 4, 180);
                TGeoCombiTrans* cmb_bottom_plane_left_minus = new TGeoCombiTrans(TString::Format("cmb_bottom_plane_left_minus_%d", i+1),
                    center_c.X(), center_c.Y(), center_c.Z(), rot_bottom_plane_left_minus);
                cmb_bottom_plane_left_minus->RegisterYourself();

                // right-
                get_trap_vertices(alveole_R_out, vertices);
                h0.SetXYZ(vertices[2].X(), vertices[2].Y(), vertices[2].Z() + dlf - 0.158);
                c2center.SetXYZ(0., 0., -100.);
                c2center.RotateY(-4./180*TMath::Pi());
                center_c = h0 + c2center;
                TGeoRotation* rot_bottom_plane_right_minus = new TGeoRotation(
                    TString::Format("rot_bottom_plane_right_minus_%d", i+1), 90-4, 0, 90, 90, 4, 180);
                TGeoCombiTrans* cmb_bottom_plane_right_minus = new TGeoCombiTrans(TString::Format("cmb_bottom_plane_right_minus_%d", i+1),
                    center_c.X(), center_c.Y(), center_c.Z(), rot_bottom_plane_right_minus);
                cmb_bottom_plane_right_minus->RegisterYourself();

                // left+
                get_trap_vertices(alveole_L_out, vertices);
                h0.SetXYZ(vertices[1].X(), vertices[1].Y(), vertices[1].Z() + dlf - 0.158);
                c2center.SetXYZ(0., 0., -100.);
                c2center.RotateY(4./180*TMath::Pi());
                center_c = h0 + c2center;
                TGeoRotation* rot_bottom_plane_left_plus = new TGeoRotation(
                    TString::Format("rot_bottom_plane_left_plus_%d", i+1), 90+4, 0, 90, 90, 4, 0);
                TGeoCombiTrans* cmb_bottom_plane_left_plus = new TGeoCombiTrans(TString::Format("cmb_bottom_plane_left_plus_%d", i+1),
                    center_c.X(), center_c.Y(), center_c.Z(), rot_bottom_plane_left_plus);
                cmb_bottom_plane_left_plus->RegisterYourself();

                // right+
                get_trap_vertices(alveole_R_out, vertices);
                h0.SetXYZ(vertices[1].X(), vertices[1].Y(), vertices[1].Z() + dlf - 0.158);
                c2center.SetXYZ(0., 0., -100.);
                c2center.RotateY(4./180*TMath::Pi());
                center_c = h0 + c2center;
                TGeoRotation* rot_bottom_plane_right_plus = new TGeoRotation(
                    TString::Format("rot_bottom_plane_right_plus_%d", i+1), 90+4, 0, 90, 90, 4, 0);
                TGeoCombiTrans* cmb_bottom_plane_right_plus = new TGeoCombiTrans(TString::Format("cmb_bottom_plane_right_plus_%d", i+1),
                    center_c.X(), center_c.Y(), center_c.Z(), rot_bottom_plane_right_plus);
                cmb_bottom_plane_right_plus->RegisterYourself();

                // in
                af -= thickness * 2;
                bf -= thickness * 2;
                cf -= thickness * 2;
                ar -= thickness * 2;
                br -= thickness * 2;
                cr -= thickness * 2;
                l += 0.01;

                TGeoTrap* alveole_L_in = create_trap(TString::Format("alveole%d_L_in", i+1),
                    1, af, bf, cf, ar, br, cr, l, i);
                TGeoTrap* alveole_R_in = create_trap(TString::Format("alveole%d_R_in", i+1),
                    -1, af, bf, cf, ar, br, cr, l, i);

                // composite shapes
                if (i < 28) {
                    alveoles_L_minus[i] = new TGeoCompositeShape(TString::Format("alveole%d_L", i+1),
                        TString::Format("alveole%d_L_out - alveole%d_L_in - cut_box_%d:cmb_top_plane_%d - cut_box_%d:cmb_bottom_plane_left_minus_%d", i+1, i+1, i+1, i+1, i+1, i+1));
                    alveoles_R_minus[i] = new TGeoCompositeShape(TString::Format("alveole%d_R", i+1),
                        TString::Format("alveole%d_R_out - alveole%d_R_in - cut_box_%d:cmb_top_plane_%d - cut_box_%d:cmb_bottom_plane_right_minus_%d", i+1, i+1, i+1, i+1, i+1, i+1));
                }
                alveoles_L_plus[i] = new TGeoCompositeShape(TString::Format("alveole%d_L", i+1),
                    TString::Format("alveole%d_L_out - alveole%d_L_in - cut_box_%d:cmb_top_plane_%d - cut_box_%d:cmb_bottom_plane_left_plus_%d", i+1, i+1, i+1, i+1, i+1, i+1));
                alveoles_R_plus[i] = new TGeoCompositeShape(TString::Format("alveole%d_R", i+1),
                    TString::Format("alveole%d_R_out - alveole%d_R_in - cut_box_%d:cmb_top_plane_%d - cut_box_%d:cmb_bottom_plane_right_plus_%d", i+1, i+1, i+1, i+1, i+1, i+1));

                i++;
            } // crystal
        } // module (alveole)
    } // supermodule

    /* Matrices */
    for (Int_t type = 0; type < 4; type++) { // type: 0 left-, 1 right+, 2 right-, 3 left+
        const Double_t crystal_gap = 2*safety;
        const Double_t alveole_gap = 0.09 - 2*thickness - 2*gap_z + 2*safety;
        Double_t module_gap[] = {
            0.12 - thickness   - gap_z   + safety,
            0.25 - 2*thickness - 2*gap_z + 2*safety,
            0.3  - 2*thickness - 2*gap_z + 2*safety,
            0.33 - 2*thickness - 2*gap_z + 2*safety
        }; // half of the first gap

        Double_t trans_z = 0;

        Int_t nModule;
        if (type == 0 || type == 2) nModule = 3;
        else nModule = 4;
        Int_t i = 0;
        for (Int_t iModule = 0; iModule < nModule; iModule++) {
            for (Int_t iAlveole = 0; iAlveole < 3; iAlveole++) {
                for (Int_t iCrystal = 0; iCrystal < 4; iCrystal++) {
                    Int_t module_id;
                    if (type == 0 || type == 2) module_id = 3 - iModule;
                    else module_id = iModule + 4;
                    Int_t alveole_id = iModule * 3 + iAlveole + 1;
                    Int_t crystal_id = iCrystal + 1;

                    if (module_id == 1 && iAlveole > 0) break; // SuperModule1 only have 1 alveole
                    if (module_id == 7 && iAlveole > 1) break; // Module7 only have 2 alveole
                    if (module_id == 7 && iAlveole == 1 && iCrystal > 2) break;  // The last alveole has 3 crystal colomns

                    // calculate crystal type
                    TGeoTrap* crystal;
                    if (type == 0 || type == 3) crystal = alveoles_L_out[i];
                    else crystal = alveoles_R_out[i];

                    Double_t gap = crystal_gap;
                    if (iCrystal == 0) {
                        gap = alveole_gap;
                        if (iAlveole == 0) gap = module_gap[iModule];
                    }

                    // calculate the center of the crystal
                    TVector3 pivot2center;
                    TVector3 vertices[8];
                    get_trap_vertices(crystal, vertices);
                    Double_t bf = crystal->GetH1()*2;
                    if (type == 0) {
                        trans_z -= gap/TMath::Cos(crystal_theta[i]) + bf; // trans_z < 0
                        pivot2center = -vertices[1]; // rotate c2p from ROOT coordinate to experimental coordinate
                    }
                    else if (type == 1) {
                        trans_z += gap/TMath::Cos(crystal_theta[i]) + bf; // trans_z < 0
                        pivot2center = -vertices[2]; // rotate c2p from ROOT coordinate to experimental coordinate
                    }
                    else if (type == 2) {
                        trans_z -= gap/TMath::Cos(crystal_theta[i]) + bf; // trans_z < 0
                        pivot2center = -vertices[2];
                    }
                    else {
                        trans_z += gap/TMath::Cos(crystal_theta[i]) + bf; // trans_z < 0
                        pivot2center = -vertices[1];
                    }
                    if (type == 0) {
                        pivot2center.RotateX(-TMath::Pi()/2.);
                        pivot2center.RotateZ(-TMath::Pi()/2.);
                    }
                    else if (type == 1) {
                        pivot2center.RotateZ(TMath::Pi());
                        pivot2center.RotateX(-TMath::Pi()/2.);
                        pivot2center.RotateZ(-TMath::Pi()/2.);
                    }
                    else if (type == 2) {
                        pivot2center.RotateZ(TMath::Pi());
                        pivot2center.RotateX(TMath::Pi()/2.);
                        pivot2center.RotateZ(TMath::Pi()/2.);
                    }
                    else {
                        pivot2center.RotateX(TMath::Pi()/2.);
                        pivot2center.RotateZ(TMath::Pi()/2.);
                    }
                    TVector3 pivot;
                    if (type == 0 || type == 1)
                        pivot = TVector3(57.+0.112126393-dlf,
                             -2*safety-(gap_phi+thickness-safety),
                             trans_z+3.7); // EMC radius is 570mm, crystal offset w.r.t. target is 37mm
                    else
                        pivot = TVector3(57.-dlf, -(gap_phi+thickness-safety),
                            trans_z+3.7);
                    TVector3 center = pivot + pivot2center;

                    // calculate the rotation matrix and place the crystal
                    TGeoRotation* crystal_rot;
                    char avleole_sign;
                    char crystal_type;
                    if (type == 0) {
                        avleole_sign = 'm';
                        crystal_type = 'L';
                    }
                    else if (type == 1) {
                        avleole_sign = 'p';
                        crystal_type = 'R';
                    }
                    else if (type == 1) {
                        avleole_sign = 'm';
                        crystal_type = 'R';
                    }
                    else {
                        avleole_sign = 'p';
                        crystal_type = 'L';
                    }
                    if (type == 0) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-alveole-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 270, 180, 180, 90, 0);
                    }
                    else if (type == 1) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-alveole-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 90, 0, 0, 90, 0);
                    }
                    else if (type == 2) {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-alveole-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 270, 180, 180, 90, 0);
                    }
                    else {
                        crystal_rot = new TGeoRotation(
                            TString::Format("rot-alveole-%d%c-%c%d",
                            alveole_id, avleole_sign, crystal_type, crystal_id),
                            90, 90, 0, 0, 90, 0);
                    }
                    crystal_rot->RegisterYourself();


                    TGeoCombiTrans* crystal_ctrans =
                        new TGeoCombiTrans(TString::Format("trans-alveole-%d%c-%c%d",
                        alveole_id, avleole_sign, crystal_type, crystal_id),
                        center.X(), center.Y(), center.Z(), crystal_rot);

                    if (type == 0)
                        matrices_alveole_minus_left[i] = crystal_ctrans;
                    else if (type == 1)
                        matrices_alveole_plus_right[i] = crystal_ctrans;
                    else if (type == 2)
                        matrices_alveole_minus_right[i] = crystal_ctrans;
                    else
                        matrices_alveole_plus_left[i] = crystal_ctrans;

                    i++;
                } // crystal
            } // module (alveole)
        } // supermodule
    } // type
}

/*TGeoTranslation* local2master_vector(const TGeoTranslation* local, const TGeoMatrix* matrix) {
    Double_t* vmaster = new Double_t[3];

    const Double_t* vlocal = local->GetTranslation();
    matrix->LocalToMasterVect(vlocal, vmaster);
    TGeoTranslation* master = new TGeoTranslation(vmaster[0], vmaster[1], vmaster[2]);

    delete[] vmaster;
    return master;
}*/

TGeoTrap* create_trap(TString name, Int_t type, Double_t af, Double_t bf, Double_t cf,
    Double_t ar, Double_t br, Double_t cr, Double_t l)
{
    Double_t x1 = (af + cf) / 4;
    Double_t y1 = bf / 2;
    Double_t z1 = 0.;
    Double_t x2 = (ar + cr) / 4;
    Double_t y2 = br / 2;
    Double_t z2 = l;
    Double_t x21 = x2 - x1;
    Double_t y21 = y2 - y1;
    Double_t z21 = z2 - z1;
    TVector3 v21(x21, y21, z21);

    Double_t dz = l / 2.;
    Double_t theta = v21.Theta() / TMath::Pi() * 180.;
    Double_t phi = v21.Phi() / TMath::Pi() * 180.;
    Double_t h1 = bf / 2;
    Double_t bl1 = cf / 2;
    Double_t tl1 = af / 2;
    Double_t alpha1 = -TMath::ATan((cf-af)/2/bf) / TMath::Pi() * 180.;
    Double_t h2 = br / 2;
    Double_t bl2 = cr / 2;
    Double_t tl2 = ar / 2;
    Double_t alpha2 = -TMath::ATan((cr-ar)/2/br) / TMath::Pi() * 180.;

    TGeoTrap* trap;
    if (type > 0)
        trap = new TGeoTrap(name, dz, theta, phi, h1, bl1, tl1, alpha1,
            h2, bl2, tl2, alpha2);
    else
        trap = new TGeoTrap(name, dz, theta, 180-phi, h1, bl1, tl1, -alpha1,
            h2, bl2, tl2, -alpha2);

    return trap;
}

TGeoTrap* create_trap(TString name, Int_t type, Double_t af, Double_t bf, Double_t cf,
    Double_t ar, Double_t br, Double_t cr, Double_t l, Int_t i)
{
    Double_t x1 = (af + cf) / 4;
    Double_t y1 = bf / 2;
    Double_t z1 = 0.;
    Double_t x2 = (ar + cr) / 4;
    Double_t y2 = br / 2 + l * TMath::Tan(crystal_theta[i]);
    Double_t z2 = l;
    Double_t x21 = x2 - x1;
    Double_t y21 = y2 - y1;
    Double_t z21 = z2 - z1;
    TVector3 v21(x21, y21, z21);

    Double_t dz = l / 2.;
    Double_t theta = v21.Theta() / TMath::Pi() * 180.;
    Double_t phi = v21.Phi() / TMath::Pi() * 180.;
    Double_t h1 = bf / 2;
    Double_t bl1 = cf / 2;
    Double_t tl1 = af / 2;
    Double_t alpha1 = -TMath::ATan((cf-af)/2/bf) / TMath::Pi() * 180.;
    Double_t h2 = br / 2;
    Double_t bl2 = cr / 2;
    Double_t tl2 = ar / 2;
    Double_t alpha2 = -TMath::ATan((cr-ar)/2/br) / TMath::Pi() * 180.;

    TGeoTrap* trap;
    if (type > 0)
        trap = new TGeoTrap(name, dz, theta, phi, h1, bl1, tl1, alpha1,
            h2, bl2, tl2, alpha2);
    else
        trap = new TGeoTrap(name, dz, theta, 180-phi, h1, bl1, tl1, -alpha1,
            h2, bl2, tl2, -alpha2);
    return trap;
}

void get_trap_vertices(TGeoTrap* trap, TVector3* points) {
    Double_t xy[8][2];
    Double_t* p = trap->GetVertices();
    Int_t off = 0;
    for (Int_t i = 0; i < 8; i++) {
        for (Int_t j = 0; j < 2; j++) {
            xy[i][j] = *(p + off);
            off++;
        }
    }
    for (Int_t i = 0; i < 8; i++) {
        if (i < 4) {
            points[i].SetXYZ(xy[i][0], xy[i][1], -trap->GetDz());
        }
        else {
            points[i].SetXYZ(xy[i][0], xy[i][1], trap->GetDz());
        }
    }
}

void raytrace_z(TGeoManager* geoMan, TString filename) {
    Double_t xmin = 50.;
    Double_t xmax = 100.;
    Double_t nx = 1e2;
    Double_t dx = (xmax - xmin) / nx;
    Double_t ymin = -5.;
    Double_t ymax = 5.;
    Double_t ny = 100;
    Double_t dy = (ymax - ymin) / ny;
    Double_t zmin = -100.;
    Double_t theta = 1;
    Double_t phi = 90.;

    TVector3 n(0., 0., 1.);
    n.RotateY(theta/180.*TMath::Pi());
    n.RotateZ(phi/180.*TMath::Pi());
    TFile* f = new TFile(filename, "recreate");
    TTree* t = new TTree("raytrace", "raytrace");
    Double_t _x, _y, _z;
    t->Branch("x", &_x, "x/D");
    t->Branch("y", &_y, "y/D");
    t->Branch("z", &_z, "z/D");
    Int_t count = 0.;
    for (Double_t x = xmin; x < xmax; x += dx) {
        for (Double_t y = ymin; y < ymax; y += dy) {
            Int_t unit = (Int_t)(nx*ny/100);
            if (count % unit == 0) {
                cout << count / unit << "% procecced" << endl;
            }
            geoMan->InitTrack(x, y, zmin, n.X(), n.Y(), n.Z());
            while (!geoMan->IsOutside()) {
                geoMan->FindNextBoundaryAndStep(10000., kTRUE);
                _x = geoMan->GetCurrentPoint()[0];
                _y = geoMan->GetCurrentPoint()[1];
                _z = geoMan->GetCurrentPoint()[2];
                t->Fill();
            }
            count++;
        }
    }
    t->Write();
    f->Close();
}

void raytrace_x(TGeoManager* geoMan, TString filename) {
    Double_t zmin = 4;
    Double_t zmax = 4.5;
    Double_t nz = 1E2;
    Double_t dz = (zmax - zmin) / nz;
    Double_t ymin = -100.;
    Double_t ymax = 100.;
    Double_t ny = 1E4;
    Double_t dy = (ymax - ymin) / ny;
    Double_t xmin = 0.;
    Double_t theta = 0.1;
    Double_t phi = 0.;

    TVector3 n(1., 0., 0.);
    n.RotateZ(theta/180.*TMath::Pi());
    n.RotateX(phi/180.*TMath::Pi());
    TFile* f = new TFile(filename, "recreate");
    TTree* t = new TTree("raytrace", "raytrace");
    Double_t _x, _y, _z;
    t->Branch("x", &_x, "x/D");
    t->Branch("y", &_y, "y/D");
    t->Branch("z", &_z, "z/D");
    Int_t count = 0.;
    for (Double_t y = ymin; y < ymax; y += dy) {
        for (Double_t z = zmin; z < zmax; z += dz) {
            Int_t unit = (Int_t)(ny*nz/100);
            if (count % unit == 0) {
                cout << count / unit << "% proceeded" << endl;
            }
            geoMan->InitTrack(xmin, y, z, n.X(), n.Y(), n.Z());
            do {
                geoMan->FindNextBoundaryAndStep();
                _x = geoMan->GetCurrentPoint()[0];
                _y = geoMan->GetCurrentPoint()[1];
                _z = geoMan->GetCurrentPoint()[2];
                t->Fill();
            } while (_x < 1000 && _x > -1000);
            //} while (!geoMan->IsOutside());
            count++;
        }
    }
    t->Write();
    f->Close();
}
