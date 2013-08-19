/*! \file rngvalidator.cpp
    \author sebastien heymann
    \date 30 juillet 2009, 23:12
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

#include "rngvalidator.h"
#include <iostream>

using namespace std;

namespace libgexf {

RngValidator::RngValidator() {
}

RngValidator::RngValidator(const RngValidator& orig) {
}

RngValidator::~RngValidator() {
}


//-----------------------------------------
void RngValidator::readerErr(void* arg, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
//-----------------------------------------
{
    const int line = xmlTextReaderLocatorLineNumber(locator);
    cerr << "WARN " << "Some kinda error! " << msg << ", severity: " << severity << ", line: " << line << endl;
}

//-----------------------------------------
void RngValidator::structErr(void* userData, xmlErrorPtr error)
//-----------------------------------------
{
    const char *msg = error->message;
    const int line = error->line;
    const int level = error->level;

    cerr << "WARN " << "Instance doc well-formedness error or validity error, level: " << level << endl;
    cerr << "WARN " << "message: " << msg;
    cerr << "WARN " << "line: " << line << endl;
}

//-----------------------------------------
void RngValidator::rngWarn(void* ctx, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
//-----------------------------------------
{
    cerr << "WARN " << "Relax NG warn: " << msg << endl;
}

//-----------------------------------------
void RngValidator::rngErr(void* ctx, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
//-----------------------------------------
{
    cerr << "WARN " << "Relax NG err: " << msg << endl;
}

//-----------------------------------------
bool RngValidator::run(const std::string& xml_file_pathname, const std::string& rng_file_pathname)
//-----------------------------------------
{
    // TODO handle multiple RNG files (eg viz)
    
    // RELAX NG Parser Context
    xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(rng_file_pathname.c_str());
    xmlRelaxNGSetParserErrors(ctxt,
                              (xmlRelaxNGValidityErrorFunc)RngValidator::rngErr,
                              (xmlRelaxNGValidityWarningFunc)RngValidator::rngWarn,
                              NULL);

    xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);
    xmlRelaxNGFreeParserCtxt(ctxt);

    xmlTextReaderPtr reader = xmlNewTextReaderFilename(xml_file_pathname.c_str());

    xmlTextReaderRelaxNGSetSchema(reader, schema);

    xmlTextReaderSetErrorHandler(reader, (xmlTextReaderErrorFunc)RngValidator::readerErr, NULL);
    xmlTextReaderSetStructuredErrorHandler(reader, (xmlStructuredErrorFunc)RngValidator::structErr, NULL);

    while (xmlTextReaderRead(reader));

    const bool valid = xmlTextReaderIsValid(reader) == 1;

    xmlFreeTextReader(reader);
    xmlRelaxNGFree(schema);

    return valid;
}



} /* namespace libgexf */
