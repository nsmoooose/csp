#include <PropertyMap.h>
#include <PropertyTokens.h>

#include <iostream>
#include <stack>
#include <string>
#include <map>



using namespace std;

PropertyMap::PropertyMap()
{
  cout << "Building token map" << endl;
  tokenMap["KIND"] = KIND_TOK;
  tokenMap["DOMAIN"] = DOMAIN_TOK;
  tokenMap["CATAGORY"] = CATAGORY_TOK;
  tokenMap["SUBCATAGORY"] = SUBCATAGORY_TOK;
  tokenMap["COUNTRY"] = COUNTRY_TOK;
  tokenMap["COUNTRYID"] = COUNTRYID_TOK;
  tokenMap["SPECIFIC"] = SPECIFIC_TOK;
  tokenMap["POSITION_VECTOR"] = POSITION_VECTOR_TOK;
  tokenMap["VELOCITY_VECTOR"] = VELOCITY_VECTOR_TOK;
  tokenMap["DIRECTION_VECTOR"] = DIRECTION_VECTOR_TOK;

}


PropertyMap::~PropertyMap()
{

}

