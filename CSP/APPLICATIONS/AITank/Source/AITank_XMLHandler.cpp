#include <util/XMLUniDefs.hpp>
#include <sax/AttributeList.hpp>
#include "AITank_XMLHandler.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;


// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        delete [] fLocalForm;
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline ostream& operator<<(ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}



// ---------------------------------------------------------------------------
//  Local const data
//
//  Note: This is the 'safe' way to do these strings. If you compiler supports
//        L"" style strings, and portability is not a concern, you can use
//        those types constants directly.
// ---------------------------------------------------------------------------
static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull };
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chDigit_1, chPeriod
    ,   chDigit_0, chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};

static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};


AITank_XMLHandler::AITank_XMLHandler(const char* const encodingName, const XMLFormatter::UnRepFlags unRepFlags) :
      fFormatter ( encodingName, this, XMLFormatter::NoEscapes, unRepFlags )
{
  cout << "AITank_XMLHandler::AITank_XMLHandler()" << endl;

  fFormatter << gXMLDecl1 << fFormatter.getEncodingName() << gXMLDecl2;
}

AITank_XMLHandler::~AITank_XMLHandler()
{
  cout << "AITank_XMLHandler::~AITank_XMLHandler()" << endl;
}

// format target interface
void AITank_XMLHandler::writeChars(const XMLByte * const toWrite)
{
  cout << "AITank_XMLHandler::writeChars() - " << toWrite << endl;
}

void AITank_XMLHandler::writeChars(const XMLByte * const toWrite, 
		  const unsigned int count,
		  XMLFormatter * const formatter)
{
  cout << "AITank_XMLHandler::writeChars() - " << toWrite << endl;
  cout.write((char *) toWrite, (int) count);
	cout.flush();
}

// DocumentHandler interface

void AITank_XMLHandler::endDocument()
{
  cout << "AITank_XMLHandler::endDocument()" << endl;
}

void AITank_XMLHandler::endElement(const XMLCh* const name)
{
  StrX str(name);
  cout << "AITank_XMLHandler::endElement() - " << name << "  " << str << endl;
}

void AITank_XMLHandler::characters(const XMLCh* const chars, const unsigned int length)
{
  StrX str(chars);
  cout << "AITank_XMLHandler::characters() - " << chars << " , " << length << "  " << str << endl;
  m_configDataMap.insert(ssValType(currentTag, str.localForm()));
  //  fFormatter.formatBuf(chars, length, XMLFormatter::CharEscapes);

}

void AITank_XMLHandler::ignorableWhitespace(const XMLCh* const chars,
			   const unsigned int length)
{
  StrX str (chars);
  cout << "AITank_XMLHandler::ignorableWhitespace() - " << chars << "  " << str << endl;
}

void AITank_XMLHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data)
{
  cout << "AITank_XMLHandler::processingInstruction() - " << target << endl;
}

void AITank_XMLHandler::startDocument()
{
  cout << "AITank_XMLHandler::startDocument()" << endl;
}

void AITank_XMLHandler::startElement(const XMLCh* const name,  AttributeList& attributes)
{
  StrX str(name);
  cout << "AITank_XMLHandler::startElement() - " << name << "  " << str << endl;
  currentTag = str.localForm();

  if (strcmp(str.localForm() , "SIM_OBJECT") == 0)
      {
	cout << "AITank_XMLHandler::startElement() - Found SIM_OBJECT TAG" << endl;
	unsigned int len = attributes.getLength();

	string typeStr="";

	for (unsigned int index = 0; index < len; index++)
	  {
	    const XMLCh * xmlStr = attributes.getName(index);
	    StrX attrStr(xmlStr);
	    const XMLCh * xmlValueStr = attributes.getValue(index);
	    StrX attrValueStr(xmlValueStr);
	    cout << "Attribute: " << attrStr << "  " << attrValueStr << endl;

	    // get type
	    if (strcmp(attrStr.localForm(), "TYPE") == 0)
	      {
		typeStr = xmlValueStr;
	      }

	  }
	// make new object depending on typeStr
	if (typeStr == "TANK")
	  {
	    
	  }
	else (typeStr == "PLANE")
	  {

	  }

      }

}

// ErrorHandler interface
void AITank_XMLHandler::warning(const SAXParseException& e)
{
  cout << "AITank_XMLHandler::warning()" << endl;
}

void AITank_XMLHandler::error(const SAXParseException& e)
{
  cout << "AITank_XMLHandler::error()" << endl;
}

void AITank_XMLHandler::fatalError(const SAXParseException& e)
{
  cerr << "AITank_XMLHandler::fatalError()" << endl;
  cerr << "\nFatal Error at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << endl;
}

// DTDHandler interface
void AITank_XMLHandler::notationDecl(const XMLCh* const name, 
		    const XMLCh* const publicId,
		    const XMLCh* const systemId)
{
  cout << "AITank_XMLHandler::notationDecl() - " << endl;
}

void AITank_XMLHandler::unparsedEntityDecl( const XMLCh* const name,
			   const XMLCh* const publicId,
			   const XMLCh* const systemId,
			   const XMLCh* const notationName)
{
  cout << "AITank_XMLHandler::unparsedEntityDecl()" << endl;
}


// dump

void AITank_XMLHandler::dumpDataMap()
{
  for (ssMapItor i = m_configDataMap.begin(); i != m_configDataMap.end(); ++i)
    {
      cout << "KEY: " << (*i).first << " VALUE:  " << (*i).second << endl;
    }

}

string AITank_XMLHandler::getConfigString(string key, string def)
{
  ssMapItor i = m_configDataMap.find(key);
  if (i == m_configDataMap.end())
    {
      return def;
    }
  else
    {
      return (*i).second;
    }
}

int AITank_XMLHandler::getConfigInteger(string key, int def)
{
  ssMapItor i = m_configDataMap.find(key);
  if (i == m_configDataMap.end())
    {
      return def;
    }
  else
    {
      const char * str = (*i).second.data();
      return atoi(str);
    }

}

float AITank_XMLHandler::getConfigFloat(string key, float def)
{
  ssMapItor i = m_configDataMap.find(key);
  if (i == m_configDataMap.end())
    {
      return def;
    }
  else
    {
      const char * str = (*i).second.data();
      return atof(str);
    }

}

