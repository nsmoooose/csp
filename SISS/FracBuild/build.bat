rem Each Call of FracBuild creates one more iteration step of a given heightfield file, rem increasing its size by factor 4.

echo on
rem f4map2csp data\theater.map
fracbuild data\extract.raw data\step1.raw -iterate 0.15

fracbuild data\step1.raw data\step2.raw -iterate 0.15

copy data\step2.raw data\temp.raw
fracbuild data\temp.raw data\step2.raw -gauss
del data\temp.raw

fracbuild data\step2.raw data\step3.raw -iterate 0.3

copy data\step3.raw data\final.raw
fracbuild data\step3.raw data\final.raw -gauss

