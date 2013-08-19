/*! \file data.cpp
    \author sebastien heymann
    \date 30 juin 2009, 13:35
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

#include <map>

#include "data.h"
#include "attributeiter.h"
#include "conv.h"
#include <sstream>
#include <map>
#include <set>

using namespace std;

namespace libgexf {

Data::Data() {
    this->init();
}

Data::Data(const Data& orig): 
        _node_labels(orig._node_labels),
        _edge_labels(orig._edge_labels),
        _node_attributes(orig._node_attributes),
        _node_attributes_types(orig._node_attributes_types),
        _node_default_values(orig._node_default_values),
        _node_options(orig._node_options),
        _node_verbatim_options(orig._node_verbatim_options),
        _node_values(orig._node_values),
        _edge_attributes(orig._edge_attributes),
        _edge_attributes_types(orig._edge_attributes_types),
        _edge_default_values(orig._edge_default_values),
        _edge_options(orig._edge_options),
        _edge_values(orig._edge_values) {
}

Data::~Data() {
}

void Data::init() {
    _node_labels = _edge_labels = map<t_id,string >();
    _node_attributes = _edge_attributes = map<t_id,string >();
    _node_attributes_types = _edge_attributes_types = map<t_id,t_attr_type >();
    _node_values = _edge_values = map<t_id,map<t_id,string > >();
    _node_default_values = _edge_default_values = map<t_id,string >();
    _node_options = _edge_options = map<t_id,std::map<std::string,unsigned short int > >();
    _node_verbatim_options = _edge_verbatim_options = std::map<t_id,std::string >();
}

//-----------------------------------------
string Data::getNodeLabel(const t_id node_id) const {
//-----------------------------------------
    map<t_id,string >::const_iterator it = _node_labels.find(node_id);
    if(it != _node_labels.end()) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
bool Data::hasNodeLabel(const t_id node_id) const {
//-----------------------------------------
    return _node_labels.find(node_id) != _node_labels.end();
}

//-----------------------------------------
void Data::setNodeLabel(const t_id node_id, const std::string& label) {
//-----------------------------------------
    _node_labels[node_id] = label;
}

//-----------------------------------------
string Data::getEdgeLabel(const t_id edge_id) const {
//-----------------------------------------
    map<t_id,string >::const_iterator it = _edge_labels.find(edge_id);
    if(it != _edge_labels.end()) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
bool Data::hasEdgeLabel(const t_id edge_id) const {
//-----------------------------------------
    return _edge_labels.find(edge_id) != _edge_labels.end();
}

//-----------------------------------------
void Data::setEdgeLabel(const t_id edge_id, const std::string& label) {
//-----------------------------------------
    _edge_labels[edge_id] = label;
}

//-----------------------------------------
void Data::addNodeAttributeColumn(const t_id id, const std::string& title, const std::string& type) {
//-----------------------------------------
    const t_attr_type t = Conv::strToAttrType(type);
    _node_attributes.insert(pair<t_id,string>(id,title));
    _node_attributes_types.insert(pair<t_id,t_attr_type>(id,t));
}

//-----------------------------------------
void Data::addEdgeAttributeColumn(const t_id id, const std::string& title, const std::string& type) {
//-----------------------------------------
    const t_attr_type t = Conv::strToAttrType(type);
    _edge_attributes.insert(pair<t_id,string>(id,title));
    _edge_attributes_types.insert(pair<t_id,t_attr_type>(id,t));
}

//-----------------------------------------
void Data::setNodeAttributeDefault(const t_id attr_id, const std::string& default_value) {
//-----------------------------------------
    _node_default_values.insert(pair<t_id,string >(attr_id,default_value));
}

//-----------------------------------------
void Data::setEdgeAttributeDefault(const t_id attr_id, const std::string& default_value) {
//-----------------------------------------
    _edge_default_values.insert(pair<t_id,string >(attr_id,default_value));
}

//-----------------------------------------
string Data::getNodeAttributeDefault(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::string >::const_iterator it = _node_default_values.find(attr_id);
    if( it != _node_default_values.end() ) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
string Data::getEdgeAttributeDefault(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::string >::const_iterator it = _edge_default_values.find(attr_id);
    if( it != _edge_default_values.end() ) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
bool Data::hasNodeAttributeDefault(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::string >::const_iterator it = _node_default_values.find(attr_id);
    return it != _node_default_values.end();
}

//-----------------------------------------
bool Data::hasEdgeAttributeDefault(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::string >::const_iterator it = _edge_default_values.find(attr_id);
    return it != _edge_default_values.end();
}

//-----------------------------------------
bool Data::hasNodeAttributeOptions(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::map<std::string,unsigned short int> >::const_iterator it = _node_options.find(attr_id);
    return it != _node_options.end();
}

//-----------------------------------------
bool Data::hasEdgeAttributeOptions(const t_id attr_id) const {
//-----------------------------------------
    map<t_id,std::map<std::string,unsigned short int> >::const_iterator it = _edge_options.find(attr_id);
    return it != _edge_options.end();
}

//-----------------------------------------
void Data::setNodeAttributeOptions(const t_id attr_id, const std::string& options) {
//-----------------------------------------
    const std::set<std::string> items = Conv::tokenizer("|", options);
    std::set<std::string>::const_iterator it = items.begin();
    std::map<std::string,unsigned short int> tokens;

    unsigned short int idx = 0;
    while( it != items.end() ) {
        tokens.insert(pair<std::string,unsigned short int>(*it++,++idx));
    }
    _node_options.insert(pair<t_id,std::map<std::string,unsigned short int> >(attr_id,tokens));
    _node_verbatim_options.insert(pair<t_id,std::string>(attr_id,options));
}

//-----------------------------------------
void Data::setEdgeAttributeOptions(const t_id attr_id, const std::string& options) {
//-----------------------------------------
    const std::set<std::string> items = Conv::tokenizer("|", options);
    std::set<std::string>::const_iterator it = items.begin();
    std::map<std::string,unsigned short int> tokens;

    unsigned short int idx = 0;
    while( it != items.end() ) {
        tokens.insert(pair<std::string,unsigned short int>(*it++,++idx));
    }
    _edge_options.insert(pair<t_id,std::map<std::string,unsigned short int> >(attr_id,tokens));
    _edge_verbatim_options.insert(pair<t_id,std::string>(attr_id,options));
}

//-----------------------------------------
std::string Data::getNodeAttributeOptions(const t_id attr_id) const {
//-----------------------------------------
    std::map<t_id,std::string >::const_iterator it = _node_verbatim_options.find(attr_id);
    if( it != _node_verbatim_options.end() ) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
std::string Data::getEdgeAttributeOptions(const t_id attr_id) const {
//-----------------------------------------
    std::map<t_id,std::string >::const_iterator it = _edge_verbatim_options.find(attr_id);
    if( it != _edge_verbatim_options.end() ) {
        return it->second;
    }
    return "";
}

//-----------------------------------------
AttributeIter* Data::getNodeAttributeColumn() const {
//-----------------------------------------
    return new AttributeIter(this, AttributeIter::NODE);
}

//-----------------------------------------
AttributeIter* Data::getEdgeAttributeColumn() const {
//-----------------------------------------
    return new AttributeIter(this, AttributeIter::EDGE);
}

//-----------------------------------------
string Data::getNodeAttribute(const t_id node_id, const t_id attr_id) const {
//-----------------------------------------
    map<t_id,map<t_id,string > >::const_iterator it = _node_values.find(node_id);
    if( it != _node_values.end() ) {
        map<t_id,string >::const_iterator it2 = (it->second).find(attr_id);
        if( it2 != (it->second).end() ) {
            return it2->second;
        }
    }
    return "";
}

//-----------------------------------------
string Data::getEdgeAttribute(const t_id edge_id, const t_id attr_id) const {
//-----------------------------------------
    map<t_id,map<t_id,string > >::const_iterator it = _edge_values.find(edge_id);
    if( it != _edge_values.end() ) {
        map<t_id,string >::const_iterator it2 = (it->second).find(attr_id);
        if( it2 != (it->second).end() ) {
            return it2->second;
        }
    }
    return "";
}

//-----------------------------------------
AttValueIter* Data::getNodeAttributeRow(const t_id node_id) const {
//-----------------------------------------
    try {
        return new AttValueIter(this, node_id, AttValueIter::NODE);
    } catch(exception& e) {
        /* case when all attributes has default values and no value is set for this node.
         nothing to do */
    }
    return NULL;
}

//-----------------------------------------
AttValueIter* Data::getEdgeAttributeRow(const t_id edge_id) const {
//-----------------------------------------
    try {
        return new AttValueIter(this, edge_id, AttValueIter::EDGE);
    } catch(exception& e) {
        /* case when all attributes has default values and no value is set for this edge.
         nothing to do */
    }
    return NULL;
}

//-----------------------------------------
bool Data::isNodeAttributeOption(const libgexf::t_id attr_id, const std::string& option) const {
//-----------------------------------------
    std::map<t_id,std::map<std::string,unsigned short int > >::const_iterator it = _node_options.find(attr_id);
    if( it != _node_options.end() ) {
        std::map<std::string,unsigned short int >::const_iterator it2 = it->second.find(option);
        return ( it2 != it->second.end() );
    }
    return false;
}

//-----------------------------------------
bool Data::isEdgeAttributeOption(const libgexf::t_id attr_id, const std::string& option) const {
//-----------------------------------------
    std::map<t_id,std::map<std::string,unsigned short int > >::const_iterator it = _edge_options.find(attr_id);
    if( it != _edge_options.end() ) {
        std::map<std::string,unsigned short int >::const_iterator it2 = it->second.find(option);
        return ( it2 != it->second.end() );
    }
    return false;
}

//-----------------------------------------
void Data::removeNodeAttributeColumn(const libgexf::t_id attr_id) {
//-----------------------------------------
cerr << "WARN " << "Data::removeNodeAttributeColumn() is not coded yet!" << endl;
}

//-----------------------------------------
void Data::removeEdgeAttributeColumn(const libgexf::t_id attr_id) {
//-----------------------------------------
cerr << "WARN " << "Data::removeEdgeAttributeColumn() is not coded yet!" << endl;
}


//-----------------------------------------
void Data::setNodeValue(const t_id node_id, const t_id attr_id, const std::string& value) {
//-----------------------------------------
    _node_values[node_id][attr_id] = value;
}

//-----------------------------------------
void Data::setEdgeValue(const t_id edge_id, const t_id attr_id, const std::string& value) {
//-----------------------------------------
    _edge_values[edge_id][attr_id] = value;
}

//-----------------------------------------
void Data::clearNodeAttributes(const t_id node_id) {
//-----------------------------------------
    _node_values.erase(node_id);
}

//-----------------------------------------
void Data::clearEdgeAttributes(const t_id edge_id) {
//-----------------------------------------
    _edge_values.erase(edge_id);
}

//-----------------------------------------
void Data::clear() {
//-----------------------------------------
    _node_attributes.clear();
    _edge_attributes.clear();
    _node_attributes_types.clear();
    _edge_attributes_types.clear();
    _node_values.clear();
    _edge_values.clear();
    _node_default_values.clear();
    _edge_default_values.clear();
    _node_options.clear();
    _edge_options.clear();
    _node_verbatim_options.clear();
    _edge_verbatim_options.clear();
}

//-----------------------------------------
void Data::clearEdgesAttributes() {
//-----------------------------------------
    _edge_attributes.clear();
    _edge_attributes_types.clear();
    _edge_values.clear();
    _edge_default_values.clear();
    _edge_options.clear();
    _edge_verbatim_options.clear();
}

//-----------------------------------------
unsigned int Data::countNodeAttributeColumn() const {
//-----------------------------------------
    return _node_attributes.size();
}

//-----------------------------------------
unsigned int Data::countEdgeAttributeColumn() const {
//-----------------------------------------
    return _edge_attributes.size();
}


//-----------------------------------------
std::ostream& operator<<(std::ostream& os, const Data& o) {
//-----------------------------------------
    os << "Data [" << std::endl;
    for ( std::map<t_id,std::string>::const_iterator it=o._node_labels.begin() ; it != o._node_labels.end(); it++ ) {
        os << it->first << " => " << it->second << std::endl;
    }
    os << "]" << std::endl;
    return os;
}

} /* namespace libgexf */
