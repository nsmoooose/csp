#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>

const short UHRSize = 80;
const short DSISize = 648;
const short ACCSize = 2700;
const short DRFSize = 18;

struct UHR
{
  char sentinel[3];
  char fixed;
  char longorig[8];
  char latiorig[8];
  char longint[4];
  char latiint[4];
  char ava[4];
  char security[3];
  char unique[12];
  char longcount[4];
  char laticount[4];
  char accuracy;
  char reserved[24];
};

struct DSI
{
  char sentinel[3];
  char security;
  char markings[2];
  char shandling[27];
  char reserved[26];
  char DTED[5];
  char URN[15];
  char rserved2[8];
  char DEN[2];
  char match;
  char date[4];
  char matchdate[4];
  char description[4];
  char producer[8];
  char reserved3[16];
  char product[9];
  char PSANCN[2];
  char specdate[4];
  char vdatum[3];
  char hdatum[5];
  char digi[10];
  char compile[4];
  char reserved4[22];
  char latio[9];
  char longo[10];
  char latiSW[7];
  char longSW[8];
  char latiNW[7];
  char longNW[8];
  char latiNE[7];
  char longNE[8];
  char latiSE[7];
  char longSE[8];
  char COA[9];
  char latiint[4];
  char longint[4];
  char latilines[4];
  char longlines[4];
  char PCI[2];
  char reserved5[101];
  char reserved6[100];
  char reserved7[156];
};

struct ACC
{
  char sentinel[3];
  char horiacc[4];
  char vertacc[4];
  char horirel[4];
  char vertrel[4];
  char reserved[4];
  char reserved1;
  char reserved3[31];
  char MAOF[2];
  char ASR[2556];
  char reserved4[18];
  char reserved5[69];
};

struct DRF
{
  char sentinel;
  char dbc[3];
  short longc;
  short latic;
  short *value;
  char checksum[4];
};

struct DTEDDATA
{

  UHR uhr;
  DSI dsi;
  ACC acc;
  DRF *drf;
};


class DTED
{

private:

  DTEDDATA data;

  long width;
  long height;

public:
  DTED();
  ~DTED();

  short ReadFile(char *Filename);
  short GetWidth();
  short GetHeight();
  short GetValue(short x, short z);

};
