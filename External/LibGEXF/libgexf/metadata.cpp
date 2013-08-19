/*! \file metadata.cpp
    \author sebastien heymann
    \date 30 juin 2009, 14:14
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

#include "metadata.h"
#include <sstream>

using namespace std;

namespace libgexf {

MetaData::MetaData() {
    init();
}

MetaData::MetaData(const MetaData& orig) {
    _version = orig._version;
    _xmlns = orig._xmlns;
    _xsi = orig._xsi;
    _schema = orig._schema;
    _variant = orig._variant;
    _creator = orig._creator;
    _description = orig._description;
    _keywords = orig._keywords;
    _lastmodifieddate = orig._lastmodifieddate;
}

MetaData::~MetaData() {
}

//-----------------------------------------
void MetaData::init() {
//-----------------------------------------
    _version = "1.1";
    _xmlns = "http://www.gexf.net/1.1draft";
    _xsi = "http://www.w3.org/2001/XMLSchema-instance";
    _schema = "http://www.gexf.net/1.1draft http://gexf.net/1.1draft/gexf.xsd";

    _variant = _creator = _description = _keywords = _lastmodifieddate = "";
}

//-----------------------------------------
std::string MetaData::getVersion() const {
//-----------------------------------------
    return _version;
}

//-----------------------------------------
std::string MetaData::getXmlns() const {
//-----------------------------------------
    return _xmlns;
}

//-----------------------------------------
std::string MetaData::getXsi() const {
//-----------------------------------------
    return _xsi;
}

//-----------------------------------------
std::string MetaData::getSchema() const {
//-----------------------------------------
    return _schema;
}

//-----------------------------------------
std::string MetaData::getVariant() const {
//-----------------------------------------
    return _variant;
}

//-----------------------------------------
std::string MetaData::getCreator() const {
//-----------------------------------------
    return _creator;
}

//-----------------------------------------
std::string MetaData::getDescription() const {
//-----------------------------------------
    return _description;
}

//-----------------------------------------
std::string MetaData::getKeywords() const {
//-----------------------------------------
    return _keywords;
}

//-----------------------------------------
std::string MetaData::getLastModifiedDate() const {
//-----------------------------------------
    return _lastmodifieddate;
}

//-----------------------------------------
void MetaData::setVersion(const std::string& version) {
//-----------------------------------------
    _version = version;
}

//-----------------------------------------
void MetaData::setXmlns(const std::string& xmlns) {
//-----------------------------------------
    _xmlns = xmlns;
}

//-----------------------------------------
void MetaData::setXsi(const std::string& xsi) {
//-----------------------------------------
    _xsi = xsi;
}

//-----------------------------------------
void MetaData::setSchema(const std::string& schema) {
//-----------------------------------------
    _schema = schema;
}

//-----------------------------------------
void MetaData::setVariant(const std::string& variant) {
//-----------------------------------------
    _variant = variant;
}

//-----------------------------------------
void MetaData::setCreator(const std::string& creator) {
//-----------------------------------------
    _creator = creator;
}

//-----------------------------------------
void MetaData::setDescription(const std::string& description) {
//-----------------------------------------
    _description = description;
}

//-----------------------------------------
void MetaData::setKeywords(const std::string& keywords) {
//-----------------------------------------
    _keywords = keywords;
}

//-----------------------------------------
void MetaData::setLastModifiedDate(const std::string& lastmodifieddate) {
//-----------------------------------------
    _lastmodifieddate = lastmodifieddate;
}


//-----------------------------------------
std::ostream& operator<<(std::ostream& os, const MetaData& o) {
//-----------------------------------------
    os << "MetaData [" << std::endl;
    os << "\tLastModifiedDate =" << o._lastmodifieddate << std::endl;
    os << "\tKeywords =" << o._keywords << std::endl;
    os << "\tDescription =" << o._description << std::endl;
    os << "\tCreator =" << o._creator << std::endl;
    os << "\tVariant =" << o._variant << std::endl;
    os << "\tSchema =" << o._schema << std::endl;
    os << "\tXSI =" << o._xsi << std::endl;
    os << "\tXMLNS =" << o._xmlns << std::endl;
    os << "\tVersion =" << o._version << std::endl;
    os << "]" << std::endl;
    return os;
}

}
