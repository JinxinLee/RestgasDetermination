#include <iostream>

#include "PndIOStreamUtils.h"
using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::ios;

void IOStreamUtilsTest(){
  cout << "This is thirty-one in hex:          " << hex << 31 << ".\n";
  cout << "This is thirty-one in decimal:      " << dec << 31 << ".\n";
  {
    panda::IOStream::all_format_guard guard( cout );
    cout << "This is thirty-one in hex again:    " << hex << 31 
         << ", inside a block with an all_format_guard.\n";
  }
  cout << "This is thirty-one after the block: " << dec << 31 << "." << endl;
  exit(0);    
}
