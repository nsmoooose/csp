
#include <sax/HandlerBase.hpp>
#include <framework/XMLFormatter.hpp>
#include <map>
#include <string>

using namespace std;

typedef map<string, string> ssMap;
typedef ssMap::value_type ssValType;
typedef ssMap::iterator ssMapItor;

class AITank_XMLHandler : public HandlerBase, private XMLFormatTarget
{
 public:
  AITank_XMLHandler( const char* const encodingName, const XMLFormatter::UnRepFlags unRepFlags);
  ~AITank_XMLHandler();

  // format target interface
  void writeChars(const XMLByte * const toWrite);
  void writeChars(const XMLByte * const toWrite, 
		  const unsigned int count,
		  XMLFormatter * const formatter);

  // DocumentHandler interface
  void endDocument();
  void endElement(const XMLCh* const name);
  void characters(const XMLCh* const chars, const unsigned int length);
  void ignorableWhitespace(const XMLCh* const chars,
			   const unsigned int length);
  void processingInstruction(const XMLCh* const target, const XMLCh* const data);
  void startDocument();
  void startElement(const XMLCh* const name,  AttributeList& attributes);

  // ErrorHandler interface
  void warning(const SAXParseException& exception);
  void error(const SAXParseException& exception);
  void fatalError(const SAXParseException& exception);

  // DTDHandler interface
  void notationDecl(const XMLCh* const name, 
		    const XMLCh* const publicId,
		    const XMLCh* const systemId);

  void unparsedEntityDecl( const XMLCh* const name,
			   const XMLCh* const publicId,
			   const XMLCh* const systemId,
			   const XMLCh* const notationName);


  void dumpDataMap();
  string getConfigString(string key, string def);
  int getConfigInteger(string key, int def);
  float getConfigFloat(string key, float def);

 private:
  XMLFormatter fFormatter;
  ssMap m_configDataMap;
  string currentTag;

};
