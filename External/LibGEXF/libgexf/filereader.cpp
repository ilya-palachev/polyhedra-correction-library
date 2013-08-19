/*! \file filereader.cpp
    \author sebastien heymann
    \date 19 juin 2009, 12:37
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

#include "filereader.h"
#include "gexfparser.h"
#include "legacyparser.h"
#include "exceptions.h"

#include <cstdio>
#include <iostream>
#include <libxml/xmlreader.h>


using namespace std;

namespace libgexf {

FileReader::FileReader(): _gexf(0), _parser(), _filepath(""), _v(_1_1) {
}

FileReader::FileReader(const std::string& filepath, const Version v) {
    this->init(filepath, v);
}

FileReader::FileReader(const FileReader& orig): _gexf(orig._gexf), _filepath(orig._filepath), _v(orig._v) {
    this->createParser();
}

FileReader::~FileReader() {
    delete _parser;
    delete _gexf;
}

//-----------------------------------------
GEXF FileReader::getGEXFCopy() {
//-----------------------------------------
    return *_gexf; // object passed by value, of course :)
}

//-----------------------------------------
void FileReader::init(const std::string& filepath, const Version v) {
//-----------------------------------------
    _filepath = filepath;
    _v = v;
    _gexf = new GEXF();
    this->createParser();
}

//-----------------------------------------
void FileReader::createParser() {
//-----------------------------------------
    if( _v == _1_1 ) {
        AbstractParser* parser = new GexfParser();
        _parser = parser;
        cout << "INFO Parser GEXF 1.1" << endl;
    } else {
        AbstractParser* parser = new LegacyParser();
        _parser = parser;
        cout << "INFO Parser GEXF 1.0 (legacy)" << endl;
    }
    _parser->bind(_gexf);
}

//-----------------------------------------
void FileReader::slurp() {
//-----------------------------------------
    #ifndef LIBXML_READER_ENABLED
    throw "LIBXML NOT FOUND";
    #endif

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    streamFile();
    xmlCleanupParser();
    cout << "INFO File parsed." << endl;
}

//-----------------------------------------
void FileReader::streamFile() {
//-----------------------------------------
    xmlTextReaderPtr reader = xmlReaderForFile(_filepath.data(), NULL, 0);
    if (reader != NULL) {
        int ret = xmlTextReaderRead(reader);
        const xmlChar* name;
        while (ret == 1) {
            name = xmlTextReaderConstName(reader);
            if (name != NULL) {
                _parser->processNode(reader, name);
                ret = xmlTextReaderRead(reader);
            }
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            throw "Failed to parse " + _filepath;
        }
    } else {
        throw "Unable to open " + _filepath;
    }
}


} /* namespace libgexf */
