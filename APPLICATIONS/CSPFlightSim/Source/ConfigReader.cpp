
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <util/XMLURL.hpp>
#include <internal/XMLScanner.hpp>
#include <validators/DTD/DTDValidator.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <framework/XMLFormatter.hpp>
#include <util/TranscodingException.hpp>
#include <util/XercesDefs.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include <parsers/DOMParser.hpp>
#include <dom/DOM_DOMException.hpp>
#include <dom/DOM.hpp>

#include <stdlib.h>
#include <iostream>

#include "ConfigReader.h"
#include <BaseObject.h>
#include <ObjectFactory.h>

using namespace std;

//#include <AITank_XMLHandler.h>

static char*                    gXmlFile               = 0;
static bool                     gDoNamespaces          = false;
static bool                     gDoSchema              = false;
static bool                     gDoCreate              = false;
static XMLCh*                   gEncodingName          = 0;
static XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;
static DOMParser::ValSchemes    gValScheme             = DOMParser::Val_Auto;
static XMLFormatter*            gFormatter             = 0;

// ---------------------------------------------------------------------------
//  Local const data
//
//  Note: This is the 'safe' way to do these strings. If you compiler supports
//        L"" style strings, and portability is not a concern, you can use
//        those types constants directly.
// ---------------------------------------------------------------------------
static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};

static const XMLCh  gStartCDATA[] =
{
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};


class DOMTreeErrorReporter : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    DOMTreeErrorReporter() :
       fSawErrors(false)
    {
    }

    ~DOMTreeErrorReporter()
    {
    }

    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};

inline bool DOMTreeErrorReporter::getSawErrors() const
{
    return fSawErrors;
}


// Global streaming operator for DOMString is defined in DOMPrint.cpp
extern ostream& operator<<(ostream& target, const DOMString& s);


void DOMTreeErrorReporter::warning(const SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;
    cerr << "Error at file \"" << DOMString(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;
    cerr << "Fatal Error at file \"" << DOMString(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
}

void DOMTreeErrorReporter::resetErrors()
{
    // No-op in this case
}

// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------
ostream& operator<< (ostream& target, const DOMString& s)
{
    char *p = s.transcode();
    target << p;
    delete [] p;
    return target;
}


XMLFormatter& operator<< (XMLFormatter& strm, const DOMString& s)
{
    unsigned int lent = s.length();

	if (lent <= 0)
		return strm;

    XMLCh*  buf = new XMLCh[lent + 1];
    XMLString::copyNString(buf, s.rawBuffer(), lent);
    buf[lent] = 0;
    strm << buf;
    delete [] buf;
    return strm;
}

void printNodeType(int type)
{
  switch (type)
    {
    case DOM_Node::DOCUMENT_NODE:
      cout << "Document Node Type" ;
	break;

    case DOM_Node::ELEMENT_NODE:
      cout << "Element Node Type";
      break;

    case DOM_Node::ATTRIBUTE_NODE:
      cout << "Attribute Node Type";
      break;


    case DOM_Node::TEXT_NODE:
      cout << "Text Node Type";
      break;

    case DOM_Node::ENTITY_NODE:
       cout << "Entity Node Type";
       
    default:
      cout << "Node Type - " << type;
      break;
    }


}


void readXMLConfigurationFile(const char * fileName)
{


  cout << "Reading configuration file " << fileName << endl;
//
// Initialize the XML4C2 system
  try
    {
      XMLPlatformUtils::Initialize();
    }

  catch (const XMLException& toCatch)
    {
      cerr << "Error during XML nitialization!" << endl;
    }


  DOMParser * parser = new DOMParser;
  parser->setValidationScheme(gValScheme);
  parser->setDoNamespaces(gDoNamespaces);
  parser->setDoSchema(gDoSchema);
  DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
  parser->setErrorHandler(errReporter);
  parser->setCreateEntityReferenceNodes(gDoCreate);
  parser->setToCreateXMLDeclTypeNode(true);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    bool errorsOccured = false;
    try
    {
        parser->parse(fileName);
        int errorCount = parser->getErrorCount();
        if (errorCount > 0)
            errorsOccured = true;
    }

    catch (const XMLException& e)
    {
        cerr << "An error occured during parsing\n   Message: "
             << DOMString(e.getMessage()) << endl;
        errorsOccured = true;
    }


    catch (const DOM_DOMException& e)
    {
       cerr << "A DOM error occured during parsing\n   DOMException code: "
             << e.code << endl;
        errorsOccured = true;
    }

    catch (...)
    {
        cerr << "An error occured during parsing\n " << endl;
        errorsOccured = true;
    }

    cout << "Finished Reading XML Config file." << endl;
    DOM_Document doc = parser->getDocument();

    try
      {
	walkDOMNode(doc);
      }
    catch (...)
      {
	cerr << "An error occured during document processing" << endl;
      }

//  AITank_XMLHandler handler(encodingName, unRepFlags);
//  try
//    {
//      parser.setDocumentHandler(&handler);
//      parser.setErrorHandler(&handler);
//      parser.parse(fileName);
//      handler.dumpDataMap();
//
//
//
//    }
//  catch(...)
//    {
//      cerr << "Caught exception during xml parse." << endl;
//    }
//
//  sim_step_time = handler.getConfigInteger("SIM_STEP_TIME", sim_step_time);
//  board_x_min = handler.getConfigFloat("BOARD_X_MIN", board_x_min);
//  board_y_min = handler.getConfigFloat("BOARD_Y_MIN", board_y_min);
//  board_x_max = handler.getConfigFloat("BOARD_X_MAX", board_x_max);
//  board_y_max = handler.getConfigFloat("BOARD_Y_MAX", board_y_max);
//  max_sim_time = handler.getConfigInteger("MAX_TIME", max_sim_time);
//  sim_time = handler.getConfigInteger("BEGIN_TIME", sim_time);
//  object_data = handler.getConfigString("OBJECT_DATA", object_data);
}


void walkDOMNode(DOM_Node nd)
{
  
  cout << endl << "walkDOMNode - entering" << endl;

  cout << "walkDomNode - Node: ";
  printNodeType(nd.getNodeType());
  cout << endl;
  cout << "NodeName: " <<  nd.getNodeName() << endl;
  cout << "NodeType: " << nd.getNodeType() << endl;
  cout << "NodeValue: " << nd.getNodeValue() << endl;  cout << endl;

  if (nd.getNodeType() == DOM_Node::DOCUMENT_NODE)
    {
      cout << "walkDOMNode - processing document" << endl;
      DOM_Node childNode = nd.getFirstChild();
      while (childNode != 0)
	{
	  cout << "walkDOMNode - processing document child" << endl;
	  walkDOMNode(childNode);
	  childNode = childNode.getNextSibling();
	}
    }

  cout << "walkDOMNode - getting attribute list" << endl;
  DOM_NamedNodeMap attrmap;

  if (nd.getNodeType() == DOM_Node::ELEMENT_NODE)
    {
      cout << "walkDOMNode - processing element - TAG: " << nd.getNodeName() << endl;
      attrmap = nd.getAttributes();

      BaseObject * object = ObjectFactory::CreateNamedObject(nd.getNodeName().transcode());

      cout << "walkDOMNode - processing elements attributes - length: " << attrmap.getLength() << endl;
      for (int i=0; i< attrmap.getLength(); i++)
	{
	  DOM_Node attrNode = attrmap.item(i);
	  cout << "walkDOMNode - attribute - Name: " << attrNode.getNodeName() << ", Value: " << attrNode.getNodeValue() << endl;
	  if (object)
	    object->SetNamedParameter(attrNode.getNodeName().transcode(), attrNode.getNodeValue().transcode());
	  walkDOMNode(attrNode);
	}

      cout << "walkDOMNode - processing elements children " << endl;
      DOM_NodeList nl = nd.getChildNodes();
      for (int i=0;i< nl.getLength(); i++)
	{
	  walkDOMNode(nl.item(i));
	}

    }

  if (nd.getNodeType() == DOM_Node::TEXT_NODE)
    {
      cout << "walkDOMNode - Processing text Node: " << nd.getNodeValue() << endl;
    }

  //  DOM_NodeList nl = nd.getChildNodes();

  //  cout << "walkDOMNode - length of child node list: " << nl.getLength() << endl;
  //  for (int i=0; i<nl.getLength(); i++)
  //{
  //  DOM_Node ndCur = nl.item(i);
  //  cout << "walkDomNode - Node: ";
  //  printNodeType(ndCur.getNodeType());
  //  cout << endl;
  //  walkDOMNode(ndCur);
      
  //}
  


}
