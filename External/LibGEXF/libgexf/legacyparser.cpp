/*! \file legacyparser.cpp
    \author sebastien heymann
    \date 22 juin 2009, 17:20
    \version 0.1
 */

#include "data.h"


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

#include "legacyparser.h"
#include "filereader.h"
#include "conv.h"
#include <string>
using namespace std;

namespace libgexf {

LegacyParser::LegacyParser(): _last_node_type(), _last_id() {
}

LegacyParser::LegacyParser(const LegacyParser& orig): _last_node_type(orig._last_node_type), _last_id(orig._last_id) {
}

LegacyParser::~LegacyParser() {
    // do not delete _gexf, only Reader instance should do it!
}

//-----------------------------------------
void LegacyParser::bind(GEXF* gexf) {
//-----------------------------------------
    _gexf = gexf;
}

//-----------------------------------------
void LegacyParser::processNode(xmlTextReaderPtr reader, const xmlChar* name) {
//-----------------------------------------
    if( !isProcessableNode(reader) ) return;
    
    if( xmlStrEqual(name, xmlCharStrdup("gexf")) == 1 ) {
        processGEXFNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("meta")) == 1 ) {
        processMetaNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("creator")) == 1 ) {
        processCreatorNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("description")) == 1 ) {
        processDescriptionNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("graph")) == 1 ) {
        processGraphNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("node")) == 1 ) {
        processNodeNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("edges")) == 1 ) {
        processEdgesNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("edge")) == 1 ) {
        processEdgeNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("attributes")) == 1 ) {
        processAttributesNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("attribute")) == 1 ) {
        processAttributeNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("default")) == 1 ) {
        processAttributeDefaultNode(reader);
    }
    else if( xmlStrEqual(name, xmlCharStrdup("attvalue")) == 1 ) {
        processAttvalueNode(reader);
    }
}

//-----------------------------------------
void LegacyParser::processGEXFNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        string version = "1.0";
        try {
            version = this->getStringAttribute(reader, "version");
            //_gexf->getMetaData().setVersion( version );
        } catch (exception &e) {
            cerr << "WARN " << e.what() << endl;
        }
        if( version.compare("1.0") != 0 )
            throw FileReaderException("Wrong GEXF version.");
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for GEXF node.");
    }

}

//-----------------------------------------
void LegacyParser::processMetaNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        string lastmodifieddate= this->getStringAttribute(reader, "lastmodifieddate");
        _gexf->getMetaData().setLastModifiedDate( lastmodifieddate );
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Meta node.");
    }
}

//-----------------------------------------
void LegacyParser::processCreatorNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int ret = xmlTextReaderRead(reader);
    if( ret == 1 ) {
        /* should be the text node */
        const xmlChar* name = xmlTextReaderConstName(reader);
        if ( xmlStrEqual(name, xmlCharStrdup("#text")) == 1 ) {
            int hasVal = xmlTextReaderHasValue(reader);
            if(hasVal == 1) {
                const xmlChar *value = xmlTextReaderConstValue(reader);
                if(value != NULL)
                    _gexf->getMetaData().setCreator( Conv::xmlCharToStr(value) );
            }
            else if(hasVal  == -1) {
                throw FileReaderException("An error occured in xmlTextReaderHasValue() for Creator node.");
            }
        }
    }
}

//-----------------------------------------
void LegacyParser::processDescriptionNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int ret = xmlTextReaderRead(reader);
    if( ret == 1 ) {
        /* should be the text node */
        const xmlChar* name = xmlTextReaderConstName(reader);
        if ( xmlStrEqual(name, xmlCharStrdup("#text")) == 1 ) {
            int hasVal = xmlTextReaderHasValue(reader);
            if(hasVal == 1) {
                const xmlChar *value = xmlTextReaderConstValue(reader);
                if(value != NULL) {
                    _gexf->getMetaData().setDescription( Conv::xmlCharToStr(value) );
                }
            }
            else if(hasVal  == -1) {
                throw FileReaderException("An error occured in xmlTextReaderHasValue() for Description node.");
            }
        }
    }
}

//-----------------------------------------
void LegacyParser::processGraphNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    _gexf->setGraphType(GRAPH_UNDIRECTED); /* default */
    
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        string mode = "static";
        try {
            mode = this->getStringAttribute(reader, "mode");
        } catch(exception &e) {
            cerr << "INFO " << "Unknown mode, static used." << endl;
        }
        /*if( mode.compare("dynamic") == 0 ) {
            // dynamic mode will never be implemented for GEXF 1.0
        }
        else { // static used by default
            // nothing to do
        }*/

        try {
            string defaultedgetype = this->getStringAttribute(reader, "defaultedgetype");
            if( defaultedgetype.compare("dir") == 0 ) {
                _gexf->setGraphType(GRAPH_DIRECTED);
            }
            else {
                cerr << "INFO " << "Unknown default edge type, undirected used." << endl;
            }
        } catch (exception &e) {
            cerr << "INFO " << "Unknown default edge type, undirected used." << endl;
        }
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Graph node.");
    }
}

//-----------------------------------------
void LegacyParser::processNodeNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        t_id node_id = this->getIdAttribute(reader, "id");
        string label= this->getStringAttribute(reader, "label");

        _gexf->getUndirectedGraph().addNode( node_id );
        _gexf->getData().setNodeLabel( node_id, label );
        _last_node_type = NODE;
        _last_id = node_id;
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Graph node.");
    }
}

//-----------------------------------------
void LegacyParser::processEdgesNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    // TODO, do nothing for the moment
}

//-----------------------------------------
void LegacyParser::processEdgeNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        t_id edge_id = this->getIdAttribute(reader, "id");
        t_id source = this->getIdAttribute(reader, "source");
        t_id target = this->getIdAttribute(reader, "target");
        float cardinal = 1.0;
        string tmp_type = "";
        try {
            cardinal = this->getFloatAttribute(reader, "cardinal");
        } catch(exception &e) {
            // nothing to do
        }
        try {
            tmp_type = this->getStringAttribute(reader, "type");
        } catch(exception &e) {
            // nothing to do
        }

        t_edge_type type = EDGE_UNDIRECTED;
        if( tmp_type.compare("dou") == 0 ) {
            type = EDGE_MUTUAL;
        }
        else if( _gexf->getGraphType() == GRAPH_DIRECTED || tmp_type.compare("dir") == 0 ) {
            type = EDGE_DIRECTED;
        }

        if(_gexf->getGraphType() == GRAPH_DIRECTED) {
            _gexf->getDirectedGraph().addEdge( edge_id, source, target, cardinal, type);
        }
        else { /*undirected or mixed, use undirected*/
            _gexf->getUndirectedGraph().addEdge( edge_id, source, target, cardinal, type);
        }
        _last_node_type = EDGE;
        _last_id = edge_id;
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Graph node.");
    }
}

//-----------------------------------------
void LegacyParser::processAttributesNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        string attribute_class = this->getStringAttribute(reader, "class");
        if( attribute_class.compare("node") == 0 ) {
            _last_node_type = ATTR_NODE;
        }
        else if( attribute_class.compare("edge") == 0 ) {
            _last_node_type = ATTR_EDGE;
        }
        else {
            throw FileReaderException("Attributes node: unknown class");
        }
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Attributes node.");
    }
}

//-----------------------------------------
void LegacyParser::processAttributeNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int hasAttr = xmlTextReaderHasAttributes(reader);
    if(hasAttr == 1) {
        t_id attribute_id = this->getIdAttribute(reader, "id");
        string title = this->getStringAttribute(reader, "title");
        string type_str = this->getStringAttribute(reader, "type");
        const t_attr_type type = Conv::strToAttrType(type_str);

        if( _last_node_type == ATTR_NODE ) {
            _gexf->getData().addNodeAttributeColumn(attribute_id, title, Conv::attrTypeToStr(type));
        }
        else if( _last_node_type == ATTR_EDGE ) {
            _gexf->getData().addEdgeAttributeColumn(attribute_id, title, Conv::attrTypeToStr(type));
        }
        _last_id = attribute_id;
    }
    else if(hasAttr  == -1) {
        throw FileReaderException("An error occured in xmlTextReaderHasAttributes() for Graph node.");
    }
}

//-----------------------------------------
void LegacyParser::processAttributeDefaultNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    int ret = xmlTextReaderRead(reader);
    if( ret == 1 ) {
        /* should be the text node */
        const xmlChar* name = xmlTextReaderConstName(reader);
        if ( xmlStrEqual(name, xmlCharStrdup("#text")) == 1 ) {
            int hasVal = xmlTextReaderHasValue(reader);
            if(hasVal == 1) {
                const xmlChar *value = xmlTextReaderConstValue(reader);
                if(value != NULL) {
                    if( _last_node_type == ATTR_NODE ) {
                        _gexf->getData().setNodeAttributeDefault(_last_id, Conv::xmlCharToStr(value));
                    }
                    else if( _last_node_type == ATTR_EDGE ) {
                        _gexf->getData().setEdgeAttributeDefault(_last_id, Conv::xmlCharToStr(value));
                    }
                }
            }
            else if(hasVal  == -1) {
                throw FileReaderException("An error occured in xmlTextReaderHasValue() for Default node.");
            }
        }
    }
}

//-----------------------------------------
void LegacyParser::processAttvalueNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    t_id attribute_id = this->getIdAttribute(reader, "id");
    string value = this->getStringAttribute(reader, "value");

    if( _last_node_type == NODE ) {
        _gexf->getData().setNodeValue(_last_id, attribute_id, value);
    }
    else if( _last_node_type == EDGE ) {
        _gexf->getData().setEdgeValue(_last_id, attribute_id, value);
    }
}

//-----------------------------------------
bool LegacyParser::isProcessableNode(xmlTextReaderPtr reader) {
//-----------------------------------------
    /* for XmlNodeType, see http://www.gnu.org/software/dotgnu/pnetlib-doc/System/Xml/XmlNodeType.html
     */
    int t = xmlTextReaderNodeType(reader);
    return t != 15 && t != 16 && t != 12 && t != 13 && t != 7;
}

} /* namespace libgexf */
