#ifndef __PROPERTYMAP_H__
#define __PROPERTYMAP_H__



#include <iostream>
#include <stack>
#include <string>
#include <map>


class PropertyMap
{
 public:
  PropertyMap();
  virtual ~PropertyMap();

  map<string, int> tokenMap;
};


#endif
