/*! \file legacywriter.cpp
    \author sebastien heymann
    \date 8 juillet 2009, 17:58
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
#include "legacywriter.h"
#include "exceptions.h"
#include "typedefs.h"
#include "conv.h"

#include <stdexcept>
#include <map>
#include <libxml/xmlwriter.h>


using namespace std;

namespace libgexf {

const char* LegacyWriter::_ENCODING = "UTF-8";

LegacyWriter::LegacyWriter(): _gexf(0), _filepath(""), _nodes(), _attributes() {
}

LegacyWriter::LegacyWriter(const std::string& filepath, GEXF* gexf): _gexf(gexf),_filepath(filepath), _nodes(), _attributes() {

}

LegacyWriter::LegacyWriter(const LegacyWriter& orig): _gexf(orig._gexf), _filepath(orig._filepath), _nodes(orig._nodes), _attributes(orig._attributes) {
}

LegacyWriter::~LegacyWriter() {
    // do not delete _gexf
}

//-----------------------------------------
GEXF LegacyWriter::getGEXFCopy() {
//-----------------------------------------
    return GEXF(*_gexf);
}

//-----------------------------------------
void LegacyWriter::init(const std::string& filepath, GEXF* gexf) {
//-----------------------------------------
    _filepath = filepath;
    _gexf = gexf;
}


//-----------------------------------------
void LegacyWriter::write() {
//-----------------------------------------
    #ifndef LIBXML_READER_ENABLED
    throw FileWriterException( "LIBXML NOT FOUND" );
    #endif

    cout << "INFO Start writing the file (gexf v1.0).." << endl;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /* Create a new XmlWriter for _filepath, with no compression. */
    xmlTextWriterPtr writer = xmlNewTextWriterFilename(_filepath.c_str(), 0);
    if (writer == NULL) {
        throw FileWriterException( "Error creating the xml LegacyWriter" );
    }

    /* Start the document with the xml default for the version,
     * encoding _ENCODING and the default for the standalone
     * declaration. */
    int rc = xmlTextWriterStartDocument(writer, NULL, _ENCODING, NULL);
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterStartDocument" );
    }

    this->writeGexfNode(writer);
    
    /* Here we could close the elements ORDER and EXAMPLE using the
     * function xmlTextWriterEndElement, but since we do not want to
     * write any other elements, we simply call xmlTextWriterEndDocument,
     * which will do all the work. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterEndDocument" );
    }

    /* Close file and free memory buffers */
    xmlFreeTextWriter(writer);
    xmlCleanupParser();

    cout << "INFO File written." << endl;
}

//-----------------------------------------
void LegacyWriter::writeGexfNode(xmlTextWriterPtr writer) {
//-----------------------------------------
    /* Start an element named "gexf". Since thist is the first
     * element, this will be the root element of the document. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "gexf");
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterStartElement" );
    }

    /* Add an attribute with name "version" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "version", BAD_CAST "1.0");
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    this->writeMetaNode(writer);

    this->writeGraphNode(writer);

    /* Close the element named gexf. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterEndElement" );
    }
}

//-----------------------------------------
void LegacyWriter::writeMetaNode(xmlTextWriterPtr writer) {
//-----------------------------------------
    /* Do we have to write all these nodes? */
    const bool do_lastmodifieddate = _gexf->getMetaData().getLastModifiedDate().compare("") != 0;
    const bool do_creator = _gexf->getMetaData().getCreator().compare("") != 0;
    const bool do_desc = _gexf->getMetaData().getDescription().compare("") != 0;
    const bool do_kw = _gexf->getMetaData().getKeywords().compare("") != 0;
    const bool do_meta = do_creator || do_desc || do_kw || do_lastmodifieddate;

    if( !do_meta ) return;

    /* Start an element named "meta" as child of gexf. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "meta");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Add an attribute with name "lastmodifieddate" */
    if( do_lastmodifieddate ) {
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "lastmodifieddate", BAD_CAST _gexf->getMetaData().getLastModifiedDate().c_str());
        if (rc < 0) {
            throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
        }
    }

    /* Write a text element named "creator" */
    if( do_creator ) {
        rc = xmlTextWriterWriteElement(writer, BAD_CAST "creator", BAD_CAST _gexf->getMetaData().getCreator().c_str());
        if (rc < 0) {
            throw FileWriterException("Error at xmlTextWriterWriteElement");
        }
    }

    /* Write a text element named "description" */
    if( do_desc ) {
        rc = xmlTextWriterWriteElement(writer, BAD_CAST "description", BAD_CAST _gexf->getMetaData().getDescription().c_str());
        if (rc < 0) {
            throw FileWriterException("Error at xmlTextWriterWriteElement");
        }
    }

    /* Write a text element named "keywords" */
    if( do_kw ) {
        rc = xmlTextWriterWriteElement(writer, BAD_CAST "keywords", BAD_CAST _gexf->getMetaData().getKeywords().c_str());
        if (rc < 0) {
            throw FileWriterException("Error at xmlTextWriterWriteElement");
        }
    }

    /* Close the element named meta. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeGraphNode(xmlTextWriterPtr writer) {
//-----------------------------------------
    /* Start an element named "graph" as child of gexf. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "graph");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Add an attribute with name "mode" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "mode", BAD_CAST "static");
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "defaultedgetype" */
    t_graph t = _gexf->getGraphType();
    if (t == GRAPH_DIRECTED) {
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "defaultedgetype", BAD_CAST "directed");
    }
    else if (t == GRAPH_MIXED) {
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "defaultedgetype", BAD_CAST "directed");
    }
    else {
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "defaultedgetype", BAD_CAST "undirected");
    }
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    //_gexf->getData().
    this->writeAttributesNode(writer,"node");
    this->writeAttributesNode(writer,"edge");
    this->writeNodesNode(writer);
    this->writeEdgesNode(writer);

    /* Close the element named graph. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeNodesNode(xmlTextWriterPtr writer) {
//-----------------------------------------
    /* Start an element named "nodes" as child of graph. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "nodes");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Iterate on each node */
    NodeIter* it = _gexf->getUndirectedGraph().getNodes();
    unsigned int cpt = 0;
    while(it->hasNext()) {
        const t_id node_id = it->next();
        const unsigned int new_id = cpt++;
        _nodes.insert( pair<t_id,unsigned int>(node_id,new_id) );
        const string label = _gexf->getData().getNodeLabel(node_id);
        this->writeNodeNode(writer, Conv::idToStr(node_id), Conv::unsignedIntToStr(new_id), label);
    }
    
    /* Close the element named nodes. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeNodeNode(xmlTextWriterPtr writer, const std::string& node_id, const std::string& new_id, const std::string& label) {
//-----------------------------------------
    /* Write an element named "node" as child of nodes. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "node");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterWriteElement");
    }

    /* Add an attribute with name "id" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST new_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "label" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "label", BAD_CAST label.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    AttValueIter* row = _gexf->getData().getNodeAttributeRow(Conv::strToId(node_id));
    if( row != NULL && row->hasNext() ) {
        this->writeAttvaluesNode(writer, NODE, node_id);
    }

    /* Close the element named node. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeEdgesNode(xmlTextWriterPtr writer) {
//-----------------------------------------
    /* Start an element named "edges" as child of graph. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "edges");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Iterate on each edge */
    EdgeIter* it = _gexf->getUndirectedGraph().getEdges();
    unsigned int cpt = 0;
    while(it->hasNext()) {
        const t_id edge_id = it->next();
        const t_id source_id = it->currentSource();
        const t_id target_id = it->currentTarget();

        const unsigned int new_id = cpt++;
        unsigned int new_source = _nodes.find(source_id)->second;
        unsigned int new_target = _nodes.find(target_id)->second;

        const unsigned int card = (unsigned int)it->currentProperty(EDGE_WEIGHT);
        const t_edge_type type = (t_edge_type)it->currentProperty(EDGE_TYPE);
        this->writeEdgeNode(writer, Conv::idToStr(edge_id), Conv::unsignedIntToStr(new_id), Conv::unsignedIntToStr(new_source), Conv::unsignedIntToStr(new_target), Conv::unsignedIntToStr(card), Conv::edgeTypeToStr(type));
    }

    /* Close the element named edges. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeEdgeNode(xmlTextWriterPtr writer, const std::string& edge_id, const std::string& new_id, const std::string& source_id, const std::string& target_id, const std::string& cardinal, const std::string& type) {
//-----------------------------------------
    /* Write an element named "edge" as child of edges. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "edge");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterWriteElement");
    }

    /* Add an attribute with name "id" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST new_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "source" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "source", BAD_CAST source_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "target" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "target", BAD_CAST target_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "cardinal" */
    if(cardinal.compare("1") > 0) {
        /* 1 is a defaultValue and can be omitted */
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "cardinal", BAD_CAST cardinal.c_str());
        if (rc < 0) {
            throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
        }
    }

    /* Add an attribute with name "type" */
    if( type.compare("undef") != 0 ) {
        t_graph t = _gexf->getGraphType();
        if( (t != GRAPH_DIRECTED && type != "undirected") || /* undirected is the default value and can be omitted */
            (t != GRAPH_UNDIRECTED && type != "directed") ) { /* directed can be omitted if it is the default value */

            const string new_type = ( type == "undirected" ) ? "sim" : ( ( type == "directed" ) ? "dir" : "dou" );

            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST new_type.c_str());
            if (rc < 0) {
                throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
            }
        }
    }

    AttValueIter* row = _gexf->getData().getEdgeAttributeRow(Conv::strToId(edge_id));
    if( row != NULL && row->hasNext() ) {
        this->writeAttvaluesNode(writer, EDGE, edge_id);
    }

    /* Close the element named edge. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeAttributesNode(xmlTextWriterPtr writer, const std::string& element_class) {
//-----------------------------------------
AttributeIter* it = 0;

    if( element_class.compare("node") != 0 && element_class.compare("edge") != 0 ) {
        throw invalid_argument("writeAttributesNode: invalid element_class");
    }

    if(element_class.compare("node") == 0) {
        it = _gexf->getData().getNodeAttributeColumn();
    }
    else if(element_class.compare("edge") == 0) {
        it = _gexf->getData().getEdgeAttributeColumn();
    }
    if( !it->hasNext() ) return;

    /* Start an element named "attributes" as child of graph. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "attributes");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Add an attribute with name "class" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "class", BAD_CAST element_class.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    unsigned int cpt = 0;
    while(it->hasNext()) {
        const t_id attr_id = it->next();
        const string title = it->currentTitle();
        const t_attr_type type = it->currentType();
        string default_value = "";

        const unsigned int new_id = cpt++;
        _attributes.insert( pair<t_id,unsigned int>(attr_id,new_id) );

        if(element_class.compare("node") == 0) {
            if( _gexf->getData().hasNodeAttributeDefault(attr_id) ) {
                default_value = _gexf->getData().getNodeAttributeDefault(attr_id);
            }
        }
        else if(element_class.compare("edge") == 0) {
            if( _gexf->getData().hasEdgeAttributeDefault(attr_id) ) {
                default_value = _gexf->getData().getEdgeAttributeDefault(attr_id);
            }
        }
        this->writeAttributeNode(writer, Conv::unsignedIntToStr(new_id), title, Conv::attrTypeToStr(type), default_value);
    }

    /* Close the element named attributes. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeAttributeNode(xmlTextWriterPtr writer, const std::string& new_id, const std::string& title, const std::string& type, const std::string& default_value) {
//-----------------------------------------
    /* Start an element named "attribute" as child of attributes. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "attribute");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Add an attribute with name "id" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST new_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "title" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "title", BAD_CAST title.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "type" */
    const string new_type = ( type == "liststring" ) ? "string" : type;
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST new_type.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Write a text element named "default" */
    if( !default_value.empty() ) {
        rc = xmlTextWriterWriteElement(writer, BAD_CAST "default", BAD_CAST default_value.c_str());
        if (rc < 0) {
            throw FileWriterException("Error at xmlTextWriterWriteElement");
        }
    }

    /* Close the element named attribute. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeAttributeDefaultNode(xmlTextWriterPtr writer, const std::string& default_value) {
//-----------------------------------------
    /* Start an element named "default" as child of attribute. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "default");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Write a text element named "default" */
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "default", BAD_CAST default_value.c_str());
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterWriteElement");
    }

    /* Close the element named default. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeAttvaluesNode(xmlTextWriterPtr writer, const ElemType type, const std::string& id) {
//-----------------------------------------
    /* Start an element named "attvalues" as child of node or edge. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "attvalues");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Write each attribute row */
    AttValueIter* row = 0;
    if( type == NODE ) {
        row = _gexf->getData().getNodeAttributeRow(Conv::strToId(id));
    }
    else if( type == EDGE ) {
        row = _gexf->getData().getEdgeAttributeRow(Conv::strToId(id));
    }
    if(row != NULL) {
        while(row->hasNext()) {
            const t_id attr_id = row->next();
            unsigned int new_id = _attributes.find(attr_id)->second;
            const string v = row->currentValue();
            this->writeAttvalueNode(writer, Conv::unsignedIntToStr(new_id), v);
        }
    }
    
    /* Close the element named default. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

//-----------------------------------------
void LegacyWriter::writeAttvalueNode(xmlTextWriterPtr writer, const std::string& attribute_id, const std::string& value) {
//-----------------------------------------
    /* Start an element named "attvalue" as child of attvalues. */
    int rc = xmlTextWriterStartElement(writer, BAD_CAST "attvalue");
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterStartElement");
    }

    /* Add an attribute with name "id" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST attribute_id.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Add an attribute with name "value" */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "value", BAD_CAST value.c_str());
    if (rc < 0) {
        throw FileWriterException( "Error at xmlTextWriterWriteAttribute");
    }

    /* Close the element named attvalue. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        throw FileWriterException("Error at xmlTextWriterEndElement");
    }
}

} /* namespace libgexf */
