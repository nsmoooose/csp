#ifndef __CONFIGREADER_H_
#define __CONFIGREADER_H_

//#include <BaseObject.h>
#include <dom/DOM.hpp>

void readXMLConfigurationFile(const char * fileName);
void processDOMNode(DOM_Node nd /*, BaseObject * pParent*/ );
void processDOMDocument(DOM_Document & nd /*, BaseObject * pParent*/ );
void processDOMElement(DOM_Element & nd /*, BaseObject * pParent*/ );
void processDOMText(DOM_Text & nd /*, BaseObject *pParent*/ );
void processDOMAttr(DOM_Attr & nd /*, BaseObject *pParent*/ );


#endif
