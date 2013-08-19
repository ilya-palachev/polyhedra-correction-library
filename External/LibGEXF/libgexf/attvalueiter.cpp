/*! \file attvalueiter.cpp
    \author sebastien heymann
    \date 17 juillet 2009, 22:35
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

#include "attvalueiter.h"
#include <stdexcept>

using namespace std;

namespace libgexf {

AttValueIter::AttValueIter(const Data* d, const libgexf::t_id id, const AttValueIter::Type t): _data(d), _t(t), _id(id), _it_row(), _it(), _cpt(0), _nb_items(0) {
    this->begin();
}

AttValueIter::~AttValueIter() {
}

AttValueIter* AttValueIter::begin() {
    if( _t == NODE ) {
        _it_row= _data->_node_values.find(_id);
        if( _it_row == _data->_node_values.end() ) {
            throw logic_error("Undefined reference to the node data "+_id);
        }
        _it = _it_row->second.begin();
        _nb_items = _it_row->second.size();
    }
    else if( _t == EDGE ) {
        _it_row = _data->_edge_values.find(_id);
        if( _it_row == _data->_edge_values.end() ) {
            throw logic_error("Undefined reference to the edge data "+_id);
        }
        _it = _it_row->second.begin();
        _nb_items = _it_row->second.size();
    }
    _cpt = 0;
    return this;
}

bool AttValueIter::hasNext() const {
    return _nb_items != 0 && _cpt != _nb_items;
}

t_id AttValueIter::next() {
    if( _cpt != 0 && _it != _it_row->second.end() ) {
        ++_it;
    }
    ++_cpt;
    return _it->first;
}

string AttValueIter::currentValue() const {
    return _it->second;
}

string AttValueIter::currentName() const {
    if( _t == NODE ) {
        map<const t_id,string >::const_iterator it = _data->_node_attributes.find(_it->first);
        if( it != _data->_node_attributes.end() ) {
            return it->second;
        }
    }
    else if( _t == EDGE ) {
        map<const t_id,string >::const_iterator it = _data->_edge_attributes.find(_it->first);
        if( it != _data->_edge_attributes.end() ) {
            return it->second;
        }
    }
    return "";
}


}
