/* 
 * File:   schemavalidator.cpp
 * Author: sebastien
 * 
 * Created on 31 juillet 2009, 01:19
 */

#include "schemavalidator.h"
#include <iostream>

using namespace std;

namespace libgexf {

SchemaValidator::SchemaValidator() {
}

SchemaValidator::SchemaValidator(const SchemaValidator& orig) {
}

SchemaValidator::~SchemaValidator() {
}

// libxml error handler
//-----------------------------------------
void SchemaValidator::schemaErrorCallback(void*, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator) {
//-----------------------------------------
  cerr << "WARN " << "Schema error: " << msg << endl;
}

// libxml warning handler
//-----------------------------------------
void SchemaValidator::schemaWarningCallback(void* callbackData, const char* const msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator) {
//-----------------------------------------
    cerr << "WARN " << "Schema warning: " << msg << endl;
}

// Returns true if file validated successfully, false otherwise
//-----------------------------------------
bool SchemaValidator::run(const std::string& xml_file_pathname, const std::string& xml_schema_file_pathname) {
//-----------------------------------------
    // TODO handle multiple XSD files (eg viz)
    bool result = false;
    if (xmlSchemaParserCtxt* schemaParser = xmlSchemaNewParserCtxt(xml_schema_file_pathname.c_str())) {
        if (xmlSchema* schema = xmlSchemaParse(schemaParser)) {
          if (xmlSchemaValidCtxt* validityContext = xmlSchemaNewValidCtxt(schema)) {
            xmlSchemaSetValidErrors(validityContext,
                                   (xmlSchemaValidityErrorFunc)SchemaValidator::schemaErrorCallback,
                                   (xmlSchemaValidityWarningFunc)SchemaValidator::schemaWarningCallback,
                                   NULL);

                // Returns 0 if validation succeeded
                result = xmlSchemaValidateFile(validityContext, xml_file_pathname.c_str(), 0) == 0;
                xmlSchemaFreeValidCtxt(validityContext);
            }
            xmlSchemaFree(schema);
        }
        xmlSchemaFreeParserCtxt(schemaParser);
    }

    return result;
}

} /* namespace libgexf */
