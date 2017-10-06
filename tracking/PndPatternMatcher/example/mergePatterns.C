int mergePatterns() {
  PndPatternDBMerger dbMerger;
  dbMerger.SetInputFilename("patternDB.root");
  dbMerger.SetInputTreename("sectorPatterns");
  // dbMerger.SetInputTreename("trackPatterns");
  dbMerger.SetOutputFilename("patternDBclean.root");

  dbMerger.Execute();

  return 0;
}
