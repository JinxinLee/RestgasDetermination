#include "Riostream.h"

int treeMerge(Char_t *treename, Char_t *infilelist, Char_t *outfile)
{
  Int_t nlines = 0;
  Char_t filename[64];

  ifstream in;
  in.open(infilelist);

  TChain ch(treename);

  while (1) 
    {
      in >> filename;
      if (!in.good()) break;
      printf("Adding %s\n",filename);
      ch.AddFile(filename);
    }

  ch.Merge(outfile);

  return 0;
}
