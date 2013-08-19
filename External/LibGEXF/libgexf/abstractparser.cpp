/*! \file abstractparser.cpp
    \author sebastien heymann
    \date 29 octobre 2009, 11:10
    \version 0.1
 */

/*
# Copyright (c) <2009> <Sebastien Heymann>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
*/

#include "abstractparser.h"
#include "conv.h"
#include <string>
using namespace std;

namespace libgexf {


//-----------------------------------------
t_id AbstractParser::getIdAttribute(xmlTextReaderPtr reader, const char* const name) {
//-----------------------------------------
    const xmlChar* const attr = xmlTextReaderGetAttribute(reader, xmlCharStrdup(name));
    if( attr != NULL )
        return Conv::xmlCharToId(attr);
    else {
        stringstream ss;
        ss << "No attribute " << name;
        throw FileReaderException(ss.str());
    }
}

//-----------------------------------------
string AbstractParser::getStringAttribute(xmlTextReaderPtr reader, const char* const name) {
//-----------------------------------------
    const xmlChar* const attr = xmlTextReaderGetAttribute(reader, xmlCharStrdup(name));
    if( attr != NULL )
        return Conv::xmlCharToStr(attr);
    else {
        stringstream ss;
        ss << "No attribute " << name;
        throw FileReaderException(ss.str());
    }
}

//-----------------------------------------
string AbstractParser::getStringAttributeNs(xmlTextReaderPtr reader, const char* const name, const char* const namespaceURI) {
//-----------------------------------------
    const xmlChar* const attr = xmlTextReaderGetAttributeNs(reader, xmlCharStrdup(name), xmlCharStrdup(namespaceURI));
    if( attr != NULL )
        return Conv::xmlCharToStr(attr);
    else {
        stringstream ss;
        ss << "No attribute " << namespaceURI << ":" << name;
        throw FileReaderException(ss.str());
    }
}

//-----------------------------------------
unsigned int AbstractParser::getUnsignedIntAttribute(xmlTextReaderPtr reader, const char* const name) {
//-----------------------------------------
    const xmlChar* const attr = xmlTextReaderGetAttribute(reader, xmlCharStrdup(name));
    if( attr != NULL )
        return Conv::xmlCharToUnsignedInt(attr);
    else {
        stringstream ss;
        ss << "No attribute " << name;
        throw FileReaderException(ss.str());
    }
}

//-----------------------------------------
float AbstractParser::getFloatAttribute(xmlTextReaderPtr reader, const char* const name) {
//-----------------------------------------
    const xmlChar* const attr = xmlTextReaderGetAttribute(reader, xmlCharStrdup(name));
    if( attr != NULL )
        return Conv::xmlCharToFloat(attr);
    else {
        stringstream ss;
        ss << "No attribute " << name;
        throw FileReaderException(ss.str());
    }
}

} /* namespace libgexf */
