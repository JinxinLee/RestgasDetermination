to use these classes copy:

- CMakeLists.txt
- MvdTrkLinkDef.h


from this folder to trunk/mvd/
e compile pandaroot again.
In your macros:
gSystem->Load("libMvdTrk");
will load also these libs.