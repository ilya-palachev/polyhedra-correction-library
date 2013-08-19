/*! \file memoryvalidator.cpp
    \author sebastien heymann
    \date 2 septembre 2009, 14:53
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

#include "memoryvalidator.h"
#include "conv.h"
#include "nodeiter.h"

#include <cstdio>
#include <string>

namespace libgexf {

MemoryValidator::MemoryValidator() {
}

MemoryValidator::MemoryValidator(const MemoryValidator& orig) {
}

MemoryValidator::~MemoryValidator() {
}

//-----------------------------------------
bool MemoryValidator::run(const GEXF& gexf) {
//-----------------------------------------
    return ( checkDefaultOptions(gexf) && checkNodeLabels(gexf) && checkAttValues(gexf) );
}

//-----------------------------------------
bool MemoryValidator::checkNodeLabels(const GEXF& gexf) {
//-----------------------------------------
bool r = true;

    /* check if each node has a label */
    NodeIter* it = gexf._graph.getNodes();
    while(it->hasNext()) {
        const t_id node_id = it->next();
        if( !gexf._data.hasNodeLabel(node_id) ) {
            std::cerr << "No label for the node " << (std::string)node_id << std::endl;
            r = false;
        }
    }

    return r;
}

//-----------------------------------------
bool MemoryValidator::checkDefaultOptions(const GEXF& gexf) {
//-----------------------------------------
bool r = true;

    /* check if default value of liststrings are part of the options  */
    AttributeIter* it = gexf._data.getNodeAttributeColumn();
    while( it->hasNext() ) {
        const t_id attr_id = it->next();
        const t_attr_type type = it->currentType();
        if( type == LISTSTRING && gexf._data.hasNodeAttributeDefault(attr_id) && gexf._data.hasNodeAttributeOptions(attr_id) ) {
            const std::string default_v = gexf._data.getNodeAttributeDefault(attr_id);
            const bool ok = gexf._data.isNodeAttributeOption(attr_id, default_v );
            if( !ok ) {
                std::cerr << "Incorrect default value \"" << default_v << "\" for the node attribute " << (std::string)attr_id << std::endl;
                r = false;
            }
        }
    }
    /* same for edges */
    it = gexf._data.getEdgeAttributeColumn();
    while( it->hasNext() ) {
        const t_id attr_id = it->next();
        const t_attr_type type = it->currentType();
        if(type == LISTSTRING &&  gexf._data.hasEdgeAttributeDefault(attr_id) && gexf._data.hasEdgeAttributeOptions(attr_id) ) {
            const std::string default_v = gexf._data.getEdgeAttributeDefault(attr_id);
            const bool ok = gexf._data.isEdgeAttributeOption(attr_id, default_v );
            if( !ok ) {
                std::cerr << "Incorrect default value \"" << default_v << "\" for the edge attribute " << (std::string)attr_id << std::endl;
                r = false;
            }
        }
    }

    return r;
}

//-----------------------------------------
bool MemoryValidator::checkAttValues(const GEXF& gexf) {
//-----------------------------------------
bool r = true;

    /* check if each attvalue has a value or a defaultvalue, and the type of each value */
    AttributeIter* it_attr = gexf._data.getNodeAttributeColumn();
    while(it_attr->hasNext()) {
        const t_id attr_id = it_attr->next();
        const std::string title = it_attr->currentTitle();
        const t_attr_type type = it_attr->currentType();
        const bool has_default = gexf._data.hasNodeAttributeDefault(attr_id);
        NodeIter* it_node = gexf._graph.getNodes();
        while(it_node->hasNext()) {
            const t_id node_id = it_node->next();
            const std::string value = gexf._data.getNodeAttribute(node_id, attr_id);
            if( value.empty() ) {
                if( !has_default ) {
                    r = false;
                    std::cerr << "A value is required for the attribute \"" << title << "\"(id=" << (std::string)attr_id << ") of the node " << (std::string)node_id << std::endl;
                }
            }
            else {
                r = r && checkAttValueType(gexf, value, type, node_id, attr_id, true);
            }
        }
    }
    /* same for edges */
    it_attr = gexf._data.getEdgeAttributeColumn();
    while(it_attr->hasNext()) {
        const t_id attr_id = it_attr->next();
        const std::string title = it_attr->currentTitle();
        const t_attr_type type = it_attr->currentType();
        const bool has_default = gexf._data.hasEdgeAttributeDefault(attr_id);
        EdgeIter* it_edge = gexf._graph.getEdges();
        while(it_edge->hasNext()) {
            const t_id edge_id = it_edge->next();
            const std::string value = gexf._data.getEdgeAttribute(edge_id, attr_id);
            if( value.empty() ) {
                if( !has_default ) {
                    r = false;
                    std::cerr << "A value is required for the attribute " << title << "(id=" << (std::string)attr_id << ") of the edge " << (std::string)edge_id << std::endl;
                }
            }
            else {
                r = r && checkAttValueType(gexf, value, type, edge_id, attr_id, false);
            }
        }
    }

    return r;
}

//-----------------------------------------
bool MemoryValidator::checkAttValueType(const GEXF& gexf, const std::string& value, const libgexf::t_attr_type type, const t_id elem_id, const t_id attr_id, const bool isNode) {
//-----------------------------------------
bool r = false;

    switch(type) {
        case INTEGER:
            r = Conv::isInteger(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be an integer" << std::endl;
            }
            break;
        case DOUBLE:
            r = Conv::isDouble(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be a double" << std::endl;
            }
            break;
        case FLOAT:
            r = Conv::isFloat(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be a float" << std::endl;
            }
            break;
        case LONG:
            r = Conv::isLong(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be a long" << std::endl;
            }
            break;
        case BOOLEAN:
            r = Conv::isBoolean(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be a boolean (true|false)" << std::endl;
            }
            break;
        case ANYURI:
            r = Conv::isAnyURI(value);
            if( !r ) {
                const std::string elem = (isNode) ? "node" : "edge";
                std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" should be an uri" << std::endl;
            }
            break;
        default: // =STRING | LISTSTRING
            r = true;
            break;
    }

    if(r) {
        if( isNode && gexf._data.hasNodeAttributeOptions(attr_id) ) {
            r = gexf._data.isNodeAttributeOption(attr_id, value);
        }
        else if(!isNode && gexf._data.hasEdgeAttributeOptions(attr_id)) {
            r = gexf._data.isEdgeAttributeOption(attr_id, value);
        }
        if( !r ) {
            const std::string elem = (isNode) ? "node" : "edge";
            std::cerr << "Attribute type error for the " << elem << " \"" << elem_id << "\": \"" << value << "\" does not exist in option \"" << attr_id << "\"" << std::endl;
        }
    }

    return r;
}


}
