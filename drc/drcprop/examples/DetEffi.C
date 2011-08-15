
#include "DetEffi.h"


//======
// ROOT
//======
#include <TRandom3.h>


//==========
// STD C/C++
//==========
#include <iostream>
#include <cstdlib>

using namespace std;


ClassImp(DetEffi) // ROOT linking stuff

TRandom3  DetEffi::rand;


DetEffi::DetEffi()
{
  rand.TRandom3::SetSeed( 4357 );


// taken from ../drcprop/PndDrcEffiBialkali.cxx

  for (int i = 0; i < 70; i++)
    _mcpEffiOld[i] = 0;

  // the argument is the wavelenght (nm) divided by 10.
  _mcpEffiOld[29] = 0.06;
  _mcpEffiOld[30] = 0.10;
  _mcpEffiOld[31] = 0.16;
  _mcpEffiOld[32] = 0.22;
  _mcpEffiOld[33] = 0.27;
  _mcpEffiOld[34] = 0.28;
  _mcpEffiOld[35] = 0.285;
  _mcpEffiOld[36] = 0.31;
  _mcpEffiOld[37] = 0.31;
  _mcpEffiOld[38] = 0.30;
  _mcpEffiOld[39] = 0.29;
  _mcpEffiOld[40] = 0.28;
  _mcpEffiOld[41] = 0.27;
  _mcpEffiOld[42] = 0.26;
  _mcpEffiOld[43] = 0.25;
  _mcpEffiOld[44] = 0.24;
  _mcpEffiOld[45] = 0.23;
  _mcpEffiOld[46] = 0.21;
  _mcpEffiOld[47] = 0.19;
  _mcpEffiOld[48] = 0.18;
  _mcpEffiOld[49] = 0.17;
  _mcpEffiOld[50] = 0.16;
  _mcpEffiOld[51] = 0.14;
  _mcpEffiOld[52] = 0.12;
  _mcpEffiOld[53] = 0.10;
  _mcpEffiOld[54] = 0.07;
  _mcpEffiOld[55] = 0.05;
  _mcpEffiOld[56] = 0.04;
  _mcpEffiOld[57] = 0.03;
  _mcpEffiOld[58] = 0.03;
  _mcpEffiOld[59] = 0.02;
  _mcpEffiOld[60] = 0.01;
  _mcpEffiOld[61] = 0.0075;
  _mcpEffiOld[62] = 0.005;
  _mcpEffiOld[63] = 0.0025;

  for (int i = 0; i < 70; i++)
    _mcpEffiOld[i] = _mcpEffiOld[i]*3; // to avoid the removing of too much photons



// quantum efficiency data from Alex Britting, Jan 25, 2011
// taken from ../drcprop/PndDrcHitProducerReal.cxx
// unit is percent
// index is the wavelength in nm + 200

  for(int i = 0; i < 501; i++)
    _mcpEffi[i] = 0;

  _mcpEffi[0]   = 231.84;
  _mcpEffi[1]   = 615.36;
  _mcpEffi[2]   = 657.4;
  _mcpEffi[3]   = 258.78;
  _mcpEffi[4]   = 9839.92;
  _mcpEffi[5]   = 44.67;
  _mcpEffi[6]   = 67.87;
  _mcpEffi[7]   = 51.01;
  _mcpEffi[8]   = 41.49;
  _mcpEffi[9]   = 5.36;
  _mcpEffi[10]  = 49.4;
  _mcpEffi[11]  = 2.13;
  _mcpEffi[12]  = 35.49;
  _mcpEffi[13]  = 8.66;
  _mcpEffi[14]  = 5.03;
  _mcpEffi[15]  = 7.51;
  _mcpEffi[16]  = 13.27;
  _mcpEffi[17]  = 18.71;
  _mcpEffi[18]  = 3.92;
  _mcpEffi[19]  = 3.66;
  _mcpEffi[20]  = 8.2;
  _mcpEffi[21]  = 0.56;
  _mcpEffi[22]  = 7.68;
  _mcpEffi[23]  = 2.87;
  _mcpEffi[24]  = 10.06;
  _mcpEffi[25]  = 3.47;
  _mcpEffi[26]  = 3.39;
  _mcpEffi[27]  = 6.99;
  _mcpEffi[28]  = 6.01;
  _mcpEffi[29]  = 4.92;
  _mcpEffi[30]  = 6.25;
  _mcpEffi[31]  = 5.97;
  _mcpEffi[32]  = 6.92;
  _mcpEffi[33]  = 8.29;
  _mcpEffi[34]  = 10.45;
  _mcpEffi[35]  = 8.68;
  _mcpEffi[36]  = 8.6;
  _mcpEffi[37]  = 9.79;
  _mcpEffi[38]  = 11.76;
  _mcpEffi[39]  = 9.53;
  _mcpEffi[40]  = 10.98;
  _mcpEffi[41]  = 9.9;
  _mcpEffi[42]  = 10.97;
  _mcpEffi[43]  = 11.31;
  _mcpEffi[44]  = 10.88;
  _mcpEffi[45]  = 10.78;
  _mcpEffi[46]  = 12.16;
  _mcpEffi[47]  = 12.38;
  _mcpEffi[48]  = 12.37;
  _mcpEffi[49]  = 13.04;
  _mcpEffi[50]  = 12.36;
  _mcpEffi[51]  = 13.18;
  _mcpEffi[52]  = 13.7;
  _mcpEffi[53]  = 13.85;
  _mcpEffi[54]  = 13.66;
  _mcpEffi[55]  = 13.98;
  _mcpEffi[56]  = 14.55;
  _mcpEffi[57]  = 14.93;
  _mcpEffi[58]  = 14.82;
  _mcpEffi[59]  = 14.97;
  _mcpEffi[60]  = 14.98;
  _mcpEffi[61]  = 15.14;
  _mcpEffi[62]  = 15.35;
  _mcpEffi[63]  = 15.37;
  _mcpEffi[64]  = 15.43;
  _mcpEffi[65]  = 15.49;
  _mcpEffi[66]  = 15.59;
  _mcpEffi[67]  = 15.84;
  _mcpEffi[68]  = 15.84;
  _mcpEffi[69]  = 15.92;
  _mcpEffi[70]  = 16.01;
  _mcpEffi[71]  = 16.22;
  _mcpEffi[72]  = 16.41;
  _mcpEffi[73]  = 16.42;
  _mcpEffi[74]  = 16.52;
  _mcpEffi[75]  = 16.86;
  _mcpEffi[76]  = 17.1;
  _mcpEffi[77]  = 17.17;
  _mcpEffi[78]  = 17.22;
  _mcpEffi[79]  = 17.46;
  _mcpEffi[80]  = 17.79;
  _mcpEffi[81]  = 17.99;
  _mcpEffi[82]  = 18.13;
  _mcpEffi[83]  = 18.33;
  _mcpEffi[84]  = 18.34;
  _mcpEffi[85]  = 18.53;
  _mcpEffi[86]  = 18.72;
  _mcpEffi[87]  = 18.95;
  _mcpEffi[88]  = 19.02;
  _mcpEffi[89]  = 19.15;
  _mcpEffi[90]  = 19.28;
  _mcpEffi[91]  = 19.45;
  _mcpEffi[92]  = 19.66;
  _mcpEffi[93]  = 19.69;
  _mcpEffi[94]  = 19.77;
  _mcpEffi[95]  = 19.73;
  _mcpEffi[96]  = 19.95;
  _mcpEffi[97]  = 19.98;
  _mcpEffi[98]  = 20.17;
  _mcpEffi[99]  = 20.29;
  _mcpEffi[100] = 20.33;
  _mcpEffi[101] = 20.37;
  _mcpEffi[102] = 20.47;
  _mcpEffi[103] = 20.48;
  _mcpEffi[104] = 20.57;
  _mcpEffi[105] = 20.75;
  _mcpEffi[106] = 20.8;
  _mcpEffi[107] = 20.84;
  _mcpEffi[108] = 20.86;
  _mcpEffi[109] = 20.88;
  _mcpEffi[110] = 21.0;
  _mcpEffi[111] = 21.06;
  _mcpEffi[112] = 21.0;
  _mcpEffi[113] = 21.06;
  _mcpEffi[114] = 21.06;
  _mcpEffi[115] = 21.04;
  _mcpEffi[116] = 21.1;
  _mcpEffi[117] = 21.14;
  _mcpEffi[118] = 21.08;
  _mcpEffi[119] = 21.17;
  _mcpEffi[120] = 21.3;
  _mcpEffi[121] = 21.38;
  _mcpEffi[122] = 21.49;
  _mcpEffi[123] = 21.58;
  _mcpEffi[124] = 21.69;
  _mcpEffi[125] = 21.77;
  _mcpEffi[126] = 21.87;
  _mcpEffi[127] = 22.02;
  _mcpEffi[128] = 22.13;
  _mcpEffi[129] = 22.29;
  _mcpEffi[130] = 22.35;
  _mcpEffi[131] = 22.45;
  _mcpEffi[132] = 22.53;
  _mcpEffi[133] = 22.55;
  _mcpEffi[134] = 22.64;
  _mcpEffi[135] = 22.67;
  _mcpEffi[136] = 22.73;
  _mcpEffi[137] = 22.74;
  _mcpEffi[138] = 22.71;
  _mcpEffi[139] = 22.79;
  _mcpEffi[140] = 22.76;
  _mcpEffi[141] = 22.77;
  _mcpEffi[142] = 22.76;
  _mcpEffi[143] = 22.75;
  _mcpEffi[144] = 22.78;
  _mcpEffi[145] = 22.7;
  _mcpEffi[146] = 22.68;
  _mcpEffi[147] = 22.72;
  _mcpEffi[148] = 22.66;
  _mcpEffi[149] = 22.64;
  _mcpEffi[150] = 22.7;
  _mcpEffi[151] = 22.67;
  _mcpEffi[152] = 22.71;
  _mcpEffi[153] = 22.67;
  _mcpEffi[154] = 22.75;
  _mcpEffi[155] = 22.77;
  _mcpEffi[156] = 22.83;
  _mcpEffi[157] = 22.84;
  _mcpEffi[158] = 22.93;
  _mcpEffi[159] = 22.97;
  _mcpEffi[160] = 23.0;
  _mcpEffi[161] = 23.08;
  _mcpEffi[162] = 23.16;
  _mcpEffi[163] = 23.27;
  _mcpEffi[164] = 23.25;
  _mcpEffi[165] = 23.37;
  _mcpEffi[166] = 23.44;
  _mcpEffi[167] = 23.49;
  _mcpEffi[168] = 23.55;
  _mcpEffi[169] = 23.52;
  _mcpEffi[170] = 23.58;
  _mcpEffi[171] = 23.64;
  _mcpEffi[172] = 23.63;
  _mcpEffi[173] = 23.58;
  _mcpEffi[174] = 23.64;
  _mcpEffi[175] = 23.63;
  _mcpEffi[176] = 23.62;
  _mcpEffi[177] = 23.64;
  _mcpEffi[178] = 23.63;
  _mcpEffi[179] = 23.66;
  _mcpEffi[180] = 23.59;
  _mcpEffi[181] = 23.59;
  _mcpEffi[182] = 23.56;
  _mcpEffi[183] = 23.58;
  _mcpEffi[184] = 23.63;
  _mcpEffi[185] = 23.57;
  _mcpEffi[186] = 23.66;
  _mcpEffi[187] = 23.62;
  _mcpEffi[188] = 23.67;
  _mcpEffi[189] = 23.64;
  _mcpEffi[190] = 23.54;
  _mcpEffi[191] = 23.57;
  _mcpEffi[192] = 23.51;
  _mcpEffi[193] = 23.53;
  _mcpEffi[194] = 23.45;
  _mcpEffi[195] = 23.3;
  _mcpEffi[196] = 23.41;
  _mcpEffi[197] = 23.25;
  _mcpEffi[198] = 23.21;
  _mcpEffi[199] = 23.08;
  _mcpEffi[200] = 23.01;
  _mcpEffi[201] = 22.92;
  _mcpEffi[202] = 22.9;
  _mcpEffi[203] = 22.76;
  _mcpEffi[204] = 22.76;
  _mcpEffi[205] = 22.61;
  _mcpEffi[206] = 22.53;
  _mcpEffi[207] = 22.48;
  _mcpEffi[208] = 22.39;
  _mcpEffi[209] = 22.29;
  _mcpEffi[210] = 22.24;
  _mcpEffi[211] = 22.2;
  _mcpEffi[212] = 22.12;
  _mcpEffi[213] = 22.07;
  _mcpEffi[214] = 21.96;
  _mcpEffi[215] = 21.89;
  _mcpEffi[216] = 21.87;
  _mcpEffi[217] = 21.76;
  _mcpEffi[218] = 21.74;
  _mcpEffi[219] = 21.58;
  _mcpEffi[220] = 21.49;
  _mcpEffi[221] = 21.48;
  _mcpEffi[222] = 21.37;
  _mcpEffi[223] = 21.29;
  _mcpEffi[224] = 21.2;
  _mcpEffi[225] = 21.17;
  _mcpEffi[226] = 21.03;
  _mcpEffi[227] = 20.98;
  _mcpEffi[228] = 20.92;
  _mcpEffi[229] = 20.85;
  _mcpEffi[230] = 20.76;
  _mcpEffi[231] = 20.69;
  _mcpEffi[232] = 20.58;
  _mcpEffi[233] = 20.56;
  _mcpEffi[234] = 20.47;
  _mcpEffi[235] = 20.37;
  _mcpEffi[236] = 20.32;
  _mcpEffi[237] = 20.24;
  _mcpEffi[238] = 20.13;
  _mcpEffi[239] = 20.08;
  _mcpEffi[240] = 19.9;
  _mcpEffi[241] = 19.84;
  _mcpEffi[242] = 19.77;
  _mcpEffi[243] = 19.69;
  _mcpEffi[244] = 19.63;
  _mcpEffi[245] = 19.51;
  _mcpEffi[246] = 19.41;
  _mcpEffi[247] = 19.27;
  _mcpEffi[248] = 19.06;
  _mcpEffi[249] = 19.01;
  _mcpEffi[250] = 18.87;
  _mcpEffi[251] = 18.7;
  _mcpEffi[252] = 18.49;
  _mcpEffi[253] = 18.41;
  _mcpEffi[254] = 18.17;
  _mcpEffi[255] = 17.98;
  _mcpEffi[256] = 17.84;
  _mcpEffi[257] = 17.69;
  _mcpEffi[258] = 17.5;
  _mcpEffi[259] = 17.25;
  _mcpEffi[260] = 17.15;
  _mcpEffi[261] = 16.98;
  _mcpEffi[262] = 16.79;
  _mcpEffi[263] = 16.66;
  _mcpEffi[264] = 16.48;
  _mcpEffi[265] = 16.32;
  _mcpEffi[266] = 16.19;
  _mcpEffi[267] = 16.02;
  _mcpEffi[268] = 15.88;
  _mcpEffi[269] = 15.77;
  _mcpEffi[270] = 15.67;
  _mcpEffi[271] = 15.5;
  _mcpEffi[272] = 15.39;
  _mcpEffi[273] = 15.23;
  _mcpEffi[274] = 15.09;
  _mcpEffi[275] = 15.04;
  _mcpEffi[276] = 14.92;
  _mcpEffi[277] = 14.75;
  _mcpEffi[278] = 14.7;
  _mcpEffi[279] = 14.5;
  _mcpEffi[280] = 14.45;
  _mcpEffi[281] = 14.34;
  _mcpEffi[282] = 14.25;
  _mcpEffi[283] = 14.16;
  _mcpEffi[284] = 14.13;
  _mcpEffi[285] = 14.0;
  _mcpEffi[286] = 13.92;
  _mcpEffi[287] = 13.84;
  _mcpEffi[288] = 13.76;
  _mcpEffi[289] = 13.73;
  _mcpEffi[290] = 13.61;
  _mcpEffi[291] = 13.54;
  _mcpEffi[292] = 13.52;
  _mcpEffi[293] = 13.45;
  _mcpEffi[294] = 13.41;
  _mcpEffi[295] = 13.39;
  _mcpEffi[296] = 13.31;
  _mcpEffi[297] = 13.22;
  _mcpEffi[298] = 13.17;
  _mcpEffi[299] = 13.13;
  _mcpEffi[300] = 13.06;
  _mcpEffi[301] = 13.2;
  _mcpEffi[302] = 13.09;
  _mcpEffi[303] = 12.97;
  _mcpEffi[304] = 12.92;
  _mcpEffi[305] = 12.73;
  _mcpEffi[306] = 12.65;
  _mcpEffi[307] = 12.4;
  _mcpEffi[308] = 12.22;
  _mcpEffi[309] = 12.02;
  _mcpEffi[310] = 11.79;
  _mcpEffi[311] = 11.59;
  _mcpEffi[312] = 11.33;
  _mcpEffi[313] = 11.03;
  _mcpEffi[314] = 10.68;
  _mcpEffi[315] = 10.46;
  _mcpEffi[316] = 10.14;
  _mcpEffi[317] = 9.88;
  _mcpEffi[318] = 9.62;
  _mcpEffi[319] = 9.36;
  _mcpEffi[320] = 9.14;
  _mcpEffi[321] = 8.87;
  _mcpEffi[322] = 8.63;
  _mcpEffi[323] = 8.51;
  _mcpEffi[324] = 8.24;
  _mcpEffi[325] = 8.07;
  _mcpEffi[326] = 7.88;
  _mcpEffi[327] = 7.77;
  _mcpEffi[328] = 7.65;
  _mcpEffi[329] = 7.52;
  _mcpEffi[330] = 7.35;
  _mcpEffi[331] = 7.27;
  _mcpEffi[332] = 7.21;
  _mcpEffi[333] = 7.1;
  _mcpEffi[334] = 6.92;
  _mcpEffi[335] = 6.89;
  _mcpEffi[336] = 6.79;
  _mcpEffi[337] = 6.74;
  _mcpEffi[338] = 6.56;
  _mcpEffi[339] = 6.54;
  _mcpEffi[340] = 6.5;
  _mcpEffi[341] = 6.39;
  _mcpEffi[342] = 6.33;
  _mcpEffi[343] = 6.25;
  _mcpEffi[344] = 6.27;
  _mcpEffi[345] = 6.14;
  _mcpEffi[346] = 6.06;
  _mcpEffi[347] = 6.04;
  _mcpEffi[348] = 6.01;
  _mcpEffi[349] = 5.91;
  _mcpEffi[350] = 5.89;
  _mcpEffi[351] = 5.79;
  _mcpEffi[352] = 5.75;
  _mcpEffi[353] = 5.75;
  _mcpEffi[354] = 5.67;
  _mcpEffi[355] = 5.61;
  _mcpEffi[356] = 5.51;
  _mcpEffi[357] = 5.52;
  _mcpEffi[358] = 5.43;
  _mcpEffi[359] = 5.43;
  _mcpEffi[360] = 5.34;
  _mcpEffi[361] = 5.31;
  _mcpEffi[362] = 5.35;
  _mcpEffi[363] = 5.23;
  _mcpEffi[364] = 5.2;
  _mcpEffi[365] = 5.14;
  _mcpEffi[366] = 5.11;
  _mcpEffi[367] = 5.11;
  _mcpEffi[368] = 5.01;
  _mcpEffi[369] = 4.98;
  _mcpEffi[370] = 4.93;
  _mcpEffi[371] = 4.99;
  _mcpEffi[372] = 4.89;
  _mcpEffi[373] = 4.82;
  _mcpEffi[374] = 4.87;
  _mcpEffi[375] = 4.8;
  _mcpEffi[376] = 4.7;
  _mcpEffi[377] = 4.65;
  _mcpEffi[378] = 4.65;
  _mcpEffi[379] = 4.61;
  _mcpEffi[380] = 4.49;
  _mcpEffi[381] = 4.56;
  _mcpEffi[382] = 4.44;
  _mcpEffi[383] = 4.42;
  _mcpEffi[384] = 4.44;
  _mcpEffi[385] = 4.35;
  _mcpEffi[386] = 4.35;
  _mcpEffi[387] = 4.27;
  _mcpEffi[388] = 4.29;
  _mcpEffi[389] = 4.19;
  _mcpEffi[390] = 4.13;
  _mcpEffi[391] = 4.08;
  _mcpEffi[392] = 4.02;
  _mcpEffi[393] = 4.07;
  _mcpEffi[394] = 3.92;
  _mcpEffi[395] = 3.95;
  _mcpEffi[396] = 3.88;
  _mcpEffi[397] = 3.82;
  _mcpEffi[398] = 3.86;
  _mcpEffi[399] = 3.74;
  _mcpEffi[400] = 3.71;
  _mcpEffi[401] = 3.66;
  _mcpEffi[402] = 3.72;
  _mcpEffi[403] = 3.62;
  _mcpEffi[404] = 3.55;
  _mcpEffi[405] = 3.56;
  _mcpEffi[406] = 3.57;
  _mcpEffi[407] = 3.45;
  _mcpEffi[408] = 3.38;
  _mcpEffi[409] = 3.36;
  _mcpEffi[410] = 3.36;
  _mcpEffi[411] = 3.28;
  _mcpEffi[412] = 3.25;
  _mcpEffi[413] = 3.19;
  _mcpEffi[414] = 3.26;
  _mcpEffi[415] = 3.13;
  _mcpEffi[416] = 3.17;
  _mcpEffi[417] = 3.15;
  _mcpEffi[418] = 3.04;
  _mcpEffi[419] = 2.98;
  _mcpEffi[420] = 2.93;
  _mcpEffi[421] = 2.98;
  _mcpEffi[422] = 2.9;
  _mcpEffi[423] = 2.89;
  _mcpEffi[424] = 2.9;
  _mcpEffi[425] = 2.81;
  _mcpEffi[426] = 2.74;
  _mcpEffi[427] = 2.81;
  _mcpEffi[428] = 2.68;
  _mcpEffi[429] = 2.73;
  _mcpEffi[430] = 2.7;
  _mcpEffi[431] = 2.57;
  _mcpEffi[432] = 2.58;
  _mcpEffi[433] = 2.55;
  _mcpEffi[434] = 2.55;
  _mcpEffi[435] = 2.37;
  _mcpEffi[436] = 2.39;
  _mcpEffi[437] = 2.39;
  _mcpEffi[438] = 2.44;
  _mcpEffi[439] = 2.37;
  _mcpEffi[440] = 2.26;
  _mcpEffi[441] = 2.27;
  _mcpEffi[442] = 2.27;
  _mcpEffi[443] = 2.23;
  _mcpEffi[444] = 2.26;
  _mcpEffi[445] = 2.14;
  _mcpEffi[446] = 2.08;
  _mcpEffi[447] = 2.15;
  _mcpEffi[448] = 2.06;
  _mcpEffi[449] = 2.09;
  _mcpEffi[450] = 2.04;
  _mcpEffi[451] = 2.0;
  _mcpEffi[452] = 1.95;
  _mcpEffi[453] = 2.02;
  _mcpEffi[454] = 1.87;
  _mcpEffi[455] = 1.9;
  _mcpEffi[456] = 1.8;
  _mcpEffi[457] = 1.87;
  _mcpEffi[458] = 1.85;
  _mcpEffi[459] = 1.87;
  _mcpEffi[460] = 1.81;
  _mcpEffi[461] = 1.86;
  _mcpEffi[462] = 1.74;
  _mcpEffi[463] = 1.74;
  _mcpEffi[464] = 1.63;
  _mcpEffi[465] = 1.59;
  _mcpEffi[466] = 1.5;
  _mcpEffi[467] = 1.5;
  _mcpEffi[468] = 1.44;
  _mcpEffi[469] = 1.47;
  _mcpEffi[470] = 1.32;
  _mcpEffi[471] = 1.24;
  _mcpEffi[472] = 1.28;
  _mcpEffi[473] = 1.19;
  _mcpEffi[474] = 1.21;
  _mcpEffi[475] = 1.21;
  _mcpEffi[476] = 1.1;
  _mcpEffi[477] = 1.1;
  _mcpEffi[478] = 1.05;
  _mcpEffi[479] = 1.06;
  _mcpEffi[480] = 0.94;
  _mcpEffi[481] = 0.92;
  _mcpEffi[482] = 0.87;
  _mcpEffi[483] = 0.92;
  _mcpEffi[484] = 0.81;
  _mcpEffi[485] = 0.86;
  _mcpEffi[486] = 0.78;
  _mcpEffi[487] = 0.77;
  _mcpEffi[488] = 0.8;
  _mcpEffi[489] = 0.67;
  _mcpEffi[490] = 0.7;
  _mcpEffi[491] = 0.81;
  _mcpEffi[492] = 0.61;
  _mcpEffi[493] = 0.64;
  _mcpEffi[494] = 0.71;
  _mcpEffi[495] = 0.66;
  _mcpEffi[496] = 0.67;
  _mcpEffi[497] = 0.68;
  _mcpEffi[498] = 0.69;
  _mcpEffi[499] = 0.68;
  _mcpEffi[500] = 0.73;


  for(int i=0; i<501; i++)
  {
    _mcpEffi[i] = _mcpEffi[i]/100. * 4;

    if( i < 25 )
      _mcpEffi[i] = 0;

    if( _mcpEffi[i] > 1 )
    {
      cout << "Effiency greater than 1!" << endl;
      abort();
    }
  }
}


bool DetEffi::Effi( int wavelength )
{
  const int min_wave = 200;

  bool prop = wavelength > 200 && wavelength < 700 && ( rand.Uniform() < _mcpEffi[(int)(wavelength - min_wave)] );
  return prop;
}


bool DetEffi::EffiOLD( int wavelength )
{
  bool prop =  wavelength < 700 && ( rand.Uniform() < _mcpEffiOld[(int)(wavelength / 10 + 0.5)] );
  return prop;
}


bool DetEffi::EffiReal( int wavelength )
{
  const int min_wave = 200;

  bool prop = wavelength > 200 && wavelength < 700 && ( rand.Uniform() < _mcpEffi[(int)(wavelength - min_wave)] / 4 );
  return prop;
}


bool DetEffi::EffiRealOLD( int wavelength )
{
  bool prop =  wavelength < 700 && ( rand.Uniform() < _mcpEffiOld[(int)(wavelength / 10 + 0.5)] / 3 );
  return prop;
}
