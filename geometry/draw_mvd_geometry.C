// Compatible with older ROOT/FairSoft setups.
// No geometry headers are included on purpose to avoid Cling/autoparse issues.

namespace {
TObject* find_geometry_object(TFile* f, const char* preferred_name)
{
  if (!f) return 0;

  if (preferred_name && preferred_name[0]) {
    TObject* obj = f->Get(preferred_name);
    if (obj) return obj;
  }

  TIter nextkey(f->GetListOfKeys());
  TKey* key = 0;
  while ((key = (TKey*)nextkey())) {
    const char* cls = key->GetClassName();
    if (!cls) continue;
    if (!strcmp(cls, "TGeoManager") || !strcmp(cls, "TGeoVolume") || !strcmp(cls, "TGeoVolumeAssembly")) {
      return key->ReadObj();
    }
  }
  return 0;
}

TGeoManager* build_wrapped_geometry(TGeoVolume* payload, Double_t world_half)
{
  if (!payload) return 0;

  TGeoManager* geom = new TGeoManager("geom_from_volume", "temporary geometry manager built from top volume");
  gGeoManager = geom;

  TGeoMaterial* matVac = new TGeoMaterial("Vacuum", 0., 0., 0.);
  TGeoMedium* medVac = new TGeoMedium("Vacuum", 1, matVac);

  TGeoVolume* world = geom->MakeBox("world", medVac, world_half, world_half, world_half);
  world->SetVisibility(kFALSE);
  world->VisibleDaughters(kTRUE);
  world->AddNode(payload, 1);

  geom->SetTopVolume(world);
  geom->CloseGeometry();
  geom->SetTopVisible(kFALSE);
  return geom;
}

void draw_impl(const char* filename,
               const char* objname,
               Bool_t use_ogl,
               Int_t vis_level,
               const char* save_as,
               Double_t world_half)
{
  gSystem->Load("libGeom");

  TFile* f = TFile::Open(filename, "READ");
  if (!f || f->IsZombie()) {
    Error("draw_mvd_geometry", "Cannot open file: %s", filename);
    return;
  }

  TObject* obj = find_geometry_object(f, objname);
  if (!obj) {
    Error("draw_mvd_geometry", "No TGeoManager/TGeoVolume object found in file");
    f->ls();
    return;
  }

  TGeoManager* geom = 0;
  TGeoVolume* draw_vol = 0;

  if (obj->InheritsFrom("TGeoManager")) {
    geom = (TGeoManager*)obj;
    gGeoManager = geom;
    draw_vol = geom->GetTopVolume();
  } else if (obj->InheritsFrom("TGeoVolume")) {
    geom = build_wrapped_geometry((TGeoVolume*)obj, world_half);
    draw_vol = geom ? geom->GetTopVolume() : 0;
  } else {
    Error("draw_mvd_geometry", "Unsupported object type: %s", obj->ClassName());
    return;
  }

  if (!geom || !draw_vol) {
    Error("draw_mvd_geometry", "Failed to prepare geometry for drawing");
    return;
  }

  geom->SetVisLevel(vis_level);
  geom->SetVisOption(0);
  geom->SetMaxVisNodes(100000);
  geom->DefaultColors();

  printf("[INFO] Drawing top volume: %s (class = %s)\n", draw_vol->GetName(), draw_vol->ClassName());
  printf("[INFO] Visible depth = %d\n", vis_level);
  printf("[INFO] world_half = %.1f\n", world_half);

  TCanvas* c = new TCanvas("c_mvd_geom", "MVD geometry", 1400, 1000);

  if (use_ogl) {
    Warning("draw_mvd_geometry", "OGL mode may still crash on some old ROOT builds; try use_ogl = kFALSE first");
    gStyle->SetCanvasPreferGL(kTRUE);
    draw_vol->Draw("ogl");
  } else {
    draw_vol->Draw();
  }

  c->Modified();
  c->Update();

  if (save_as && save_as[0]) {
    c->SaveAs(save_as);
    printf("[INFO] Saved canvas to: %s\n", save_as);
  }
}
} // namespace

// Main entry matching the macro file name.
void draw_mvd_geometry(const char* filename = "MVD_HV_MAPS.root",
                       const char* objname = "MVD_HV_MAPS",
                       Bool_t use_ogl = kFALSE,
                       Int_t vis_level = 20,
                       const char* save_as = "",
                       Double_t world_half = 100.)
{
  draw_impl(filename, objname, use_ogl, vis_level, save_as, world_half);
}
