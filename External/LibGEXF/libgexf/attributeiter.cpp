/*! \file attributeiter.cpp
    \author: sebastien heymann
    \date 15 juillet 2009, 14:27
    \version
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

#include "attributeiter.h"

using namespace std;

namespace libgexf {

AttributeIter::AttributeIter(const Data* d, const AttributeIter::Type t): _data(d), _t(t), _cpt(0), _nb_items(0) {
    this->begin();
}

AttributeIter::~AttributeIter() {
}

//-----------------------------------------
AttributeIter* AttributeIter::begin() {
//-----------------------------------------
    if( _t == NODE ) {
        _it = _data->_node_attributes.begin();
        _nb_items = _data->countNodeAttributeColumn();
    }
    else if( _t == EDGE ) {
        _it = _data->_edge_attributes.begin();
        _nb_items = _data->countEdgeAttributeColumn();
    }
    _cpt = 0;
    return this;
}

//-----------------------------------------
bool AttributeIter::hasNext() const {
//-----------------------------------------
    return _nb_items != 0 && _cpt != _nb_items;
}

//-----------------------------------------
t_id AttributeIter::next() {
//-----------------------------------------
    if( _cpt != 0 && (
        (_t == NODE && _it != _data->_node_attributes.end()) ||
        (_t == EDGE && _it != _data->_edge_attributes.end()) ) ) {
        ++_it;
    }
    ++_cpt;
    return _it->first;
}

//-----------------------------------------
string AttributeIter::currentTitle() const {
//-----------------------------------------
    return _it->second;
}

//-----------------------------------------
t_attr_type AttributeIter::currentType() const {
//-----------------------------------------
    if( _t == NODE ) {
        std::map<t_id,t_attr_type >::const_iterator it2 = _data->_node_attributes_types.find(_it->first);
        if( it2 != _data->_node_attributes_types.end() ) {
            return it2->second;
        }
    }
    else if( _t == EDGE ) {
        std::map<t_id,t_attr_type >::const_iterator it2 = _data->_edge_attributes_types.find(_it->first);
        if( it2 != _data->_edge_attributes_types.end() ) {
            return it2->second;
        }
    }

    return STRING;
}

}
