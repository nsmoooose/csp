//#include <Framework.h>

#include "ConfigReader.h"
#include <iostream>

using namespace std;

int main(int argc, char * argv[])
{
  cout << "Starting XMLReader" << endl;

  readXMLConfigurationFile(argv[1]);

  //  ObjectManager::getObjectManager().dumpAllObjects();
	
}


