// Copyright (C) 2001-2002 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// As a special exception to the GNU General Public License, permission is
// granted for additional uses of the text contained in its release
// of Common C++.
//
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
//
// This exception applies only to the code released under the
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
//
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

#include <cc++/config.h>

#ifdef COMMON_XML_PARSING

#include <cc++/export.h>
#include <cc++/xml.h>
#ifndef	WIN32
#include <syslog.h>
#endif

#ifdef HAVE_LIBXML

#include <libxml/xmlversion.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlIO.h>
#include <cstdarg>

// very ugly, but saves a lot of #ifdefs. To understand this, look at
// the private members of XMLRPC.
#ifndef HAVE_SSTREAM
#define strBuf (*oldStrBuf)
#endif

#ifdef	CCXX_NAMESPACES
namespace ost {
#ifdef HAVE_SSTREAM
using std::stringstream;
#else
using std::strstream;
#endif
using std::streambuf;
using std::ofstream;
using std::ostream;
using std::clog;
using std::endl;
using std::ends;
using std::ios;
#endif

extern "C" {

// static int xmlReadCallback(XMLStream *xml, unsigned char *buffer, int len)
// {
//	return xml->read(buffer, len);
// }
//
// static void xmlCloseCallback(XMLStream *xml)
// {
//	xml->close();
// }
//
// static void saxWarning(XMLStream *xml, const char *msg, ...)
// {
//	va_list args;
//
//	va_start(args, msg);
// #ifndef	WIN32
//	if(xml->getLogging() <= Slog::levelWarning)
//		syslog(LOG_WARNING, msg, args);
// #endif
//	va_end(args);
//}
//
static void saxCharacters(XMLStream *xml, const unsigned char *text, int unsigned len)
{
	xml->characters(text, len);
}

static void saxComment(XMLStream *xml, const unsigned char *text)
{
	xml->comment(text);
}

static void saxStartDocument(XMLStream *xml)
{
	xml->startDocument();
}

static void saxEndDocument(XMLStream *xml)
{
	xml->endDocument();
}

static void saxStartElement(XMLStream *xml, const unsigned char *name, const unsigned char **attributes)
{
	xml->startElement(name, attributes);
}

static void saxEndElement(XMLStream *xml, const unsigned char *name)
{
	xml->endElement(name);
}

};

bool XMLStream::parse(const char *resource)
{
	bool ret = false;
	xmlParserCtxtPtr xml;
	xmlSAXHandler sax;
	char buffer[1024];
	int res;

	if(resource)
		if(!open(resource))
			return false;

	memset(&sax, 0, sizeof(sax));
	sax.startDocument = (startDocumentSAXFunc)&saxStartDocument;
	sax.endDocument = (endDocumentSAXFunc)&saxEndDocument;
	sax.startElement = (startElementSAXFunc)&saxStartElement;
	sax.endElement = (endElementSAXFunc)&saxEndElement;
	sax.characters = (charactersSAXFunc)&saxCharacters;
	sax.comment = (commentSAXFunc)&saxComment;

	xml = xmlCreatePushParserCtxt(&sax, this,
		NULL, 0, NULL);

	if(!xml)
		return false;

	while((res = read((unsigned char *) buffer, 1024)))
		xmlParseChunk(xml, buffer, res, 0);
	xmlParseChunk(xml, buffer, 0, 1);

	if(xml->wellFormed)
		ret = true;

	xml->sax = NULL;
	xmlFreeParserCtxt(xml);
	return ret;
}

XMLRPC::XMLRPC(size_t bufferSize) :
XMLStream()
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	buffer = new char[bufferSize];
	oldStrBuf = NULL;
	bufSize = bufferSize;
#endif
}

XMLRPC::~XMLRPC()
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(buffer)
		delete[] buffer;
	if(oldStrBuf)
		delete oldStrBuf;
#endif
	close();
}

void XMLRPC::invoke(const char *member)
{
#ifdef HAVE_SSTREAM
	strBuf.str() = "";
#else
	buffer[0] = 0;
	oldStrBuf = new strstream(buffer,bufSize);
#endif

	structFlag = reply = fault = false;
	array = 0;

	strBuf << "<?xml version=\"1.0\"?>" << endl;
	strBuf << "<methodCall>" << endl;
	strBuf << "<methodName>" << member << "</methodName>" << endl;
	strBuf << "<params>" << endl;
}

void XMLRPC::response(bool f)
{
	reply = true;
	structFlag = false;
	fault = f;
	array = 0;

#ifdef HAVE_SSTREAM
	// nothing
#else
	buffer[0] = 0;
	oldStrBuf = new strstream(buffer,bufSize);
#endif

	strBuf << "<?xml version=\"1.0\"?>" << endl;
	strBuf << "<methodResponse>" << endl;
	if(fault)
		strBuf << "<fault>" << endl;
	else
		strBuf << "<params>" << endl;
}

void XMLRPC::addMember(const char *name, long value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	begStruct();

	strBuf << "<member><name>" << name << "</name>" << endl;
	strBuf << "<value><i4>" << value << "</i4></value></member>" << endl;
}

void XMLRPC::addMember(const char *name, const char *value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	begStruct();

	strBuf << "<member><name>" << name << "</name>" << endl;
	strBuf << "<value><string>" << value << "</string></value></member>" << endl;
}


void XMLRPC::addMember(const char *name, bool value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	begStruct();

	strBuf << "<member><name>" << name << "</name>" << endl;
	strBuf << "<value><boolean>";
	if(value)
		strBuf << "1";
	else
		strBuf << "0";

	strBuf << "</boolean></value></member>" << endl;
}

void XMLRPC::addParam(bool value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	endStruct();

	if(!fault && !array)
		strBuf << "<param>";

	strBuf << "<value><boolean>";
	if(value)
		strBuf << "1";
	else
		strBuf << "0";
	strBuf << "</boolean></value>";

	if(!fault && !array)
		strBuf << "</param>";

	strBuf << endl;
}

void XMLRPC::addParam(long value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	endStruct();

	if(!fault && !array)
		strBuf << "<param>";

	strBuf << "<value><i4>" << value << "</i4></value>";

	if(!fault && !array)
		strBuf << "</param>";

	strBuf << endl;
}

void XMLRPC::addParam(const char *value)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	endStruct();

	if(!fault && !array)
		strBuf << "<param>" << endl;

	strBuf << "<value><string>" << value << "</string></value>";

	if(!fault && !array)
		strBuf << "</param>";

	strBuf << endl;
}

void XMLRPC::begArray(void)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	if(fault) //do not include arrays in fault responses.
		return; 

	if(!array)
		strBuf << "<param>";
	array++;
	strBuf << "<array><data>" << endl;
}

void XMLRPC::endArray(void)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return;
#endif

	if(!array)
		return;

	strBuf << "</data></array>";

	if(!--array)
		strBuf << "</param>";

	strBuf << endl;
}

void XMLRPC::begStruct(void)
{
	if(structFlag)
		return;

	structFlag = true;

	if(!fault && !array)
		strBuf << "<param>";

	strBuf << "<struct><value>" << endl;
}

void XMLRPC::endStruct(void)
{
	if(!structFlag)
		return;

	strBuf << "</struct></value>";

	if(!fault && !array)
		strBuf << "</param>";
	strBuf << endl;
	structFlag = false;
}

bool XMLRPC::send(const char *resource)
{
#ifdef HAVE_SSTREAM
	// nothing
#else
	if(!oldStrBuf)
		return false;
#endif

	endStruct();
	while(array)
	{
		strBuf << "</data></array>" << endl;
		--array;
	}
	if(!fault)
		strBuf << "</params>" << endl;

	if(reply)
		strBuf << "</methodResponse>" << endl << ends;
	else
		strBuf << "</methodCall>" << endl << ends;

	bool result;

#ifdef HAVE_SSTREAM
	result = post(resource, strBuf.str().c_str());
	strBuf.str("");
#else
	delete oldStrBuf;
	oldStrBuf = NULL;
	result = post(resource, (const char *)buffer);
#endif

	return result;
}

#ifdef	CCXX_NAMESPACES
};
#endif

#endif // ifdef HAVE_LIBXML

#else
#pragma warning("Common C++ XML support is not being compiled, are you sure?")
#endif // ifdef COMMON_XML_PARSING

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
