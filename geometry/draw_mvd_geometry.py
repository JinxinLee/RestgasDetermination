#!/usr/bin/env python3
import sys
import ROOT


def find_geometry_object(root_file, preferred_name="MVD_HV_MAPS"):
    if preferred_name:
        obj = root_file.Get(preferred_name)
        if obj:
            return obj

    for key in root_file.GetListOfKeys():
        cls = key.GetClassName()
        if "TGeoManager" in cls or "TGeoVolume" in cls:
            return key.ReadObj()
    return None


def draw_mvd_geometry(filename="MVD_HV_MAPS.root",
                      objname="MVD_HV_MAPS",
                      use_ogl=True,
                      vis_level=20,
                      save_as=""):
    ROOT.gSystem.Load("libGeom")

    f = ROOT.TFile.Open(filename, "READ")
    if not f or f.IsZombie():
        raise OSError(f"Cannot open file: {filename}")

    obj = find_geometry_object(f, objname)
    if not obj:
        f.ls()
        raise RuntimeError("No TGeoManager/TGeoVolume object found in file")

    geom = None
    top = None

    if obj.InheritsFrom(ROOT.TGeoManager.Class()):
        geom = obj
        top = geom.GetTopVolume()
    elif obj.InheritsFrom(ROOT.TGeoVolume.Class()):
        top = obj
        geom = ROOT.TGeoManager("geom_from_volume", "Temporary geometry manager built from top volume")
        ROOT.gGeoManager = geom
        geom.SetTopVolume(top)
        geom.CloseGeometry()
    else:
        raise TypeError(f"Unsupported object type: {obj.ClassName()}")

    if not geom or not top:
        raise RuntimeError("Failed to build drawable geometry")

    ROOT.gGeoManager = geom
    geom.SetVisLevel(vis_level)
    geom.SetVisOption(0)
    geom.DefaultColors()

    top.SetVisibility(True)
    top.VisibleDaughters(True)

    print(f"[INFO] Drawing object: {top.GetName()} (class = {top.ClassName()})")
    print(f"[INFO] Total daughter nodes in geometry tree: {top.CountNodes()}")

    canvas = ROOT.TCanvas("c_mvd_geom", "MVD geometry", 1400, 1000)
    if use_ogl:
        ROOT.gStyle.SetCanvasPreferGL(True)
        top.Draw("ogl")
    else:
        top.Draw()
    canvas.Modified()
    canvas.Update()

    if save_as:
        canvas.SaveAs(save_as)
        print(f"[INFO] Saved canvas to: {save_as}")

    return canvas, geom, f


if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else "MVD_HV_MAPS.root"
    objname = sys.argv[2] if len(sys.argv) > 2 else "MVD_HV_MAPS"
    use_ogl = True
    save_as = sys.argv[3] if len(sys.argv) > 3 else ""

    canvas, geom, root_file = draw_mvd_geometry(filename, objname, use_ogl=use_ogl, save_as=save_as)

    print("[INFO] Close the ROOT window or press Ctrl+C in terminal to exit.")
    if ROOT.gApplication:
        ROOT.gApplication.Run(True)
