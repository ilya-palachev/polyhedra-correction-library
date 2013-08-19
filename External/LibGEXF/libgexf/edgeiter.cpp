/*! \file edgeiter.cpp
    \author sebastien heymann
    \date 9 juillet 2009, 17:38
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


#include "edgeiter.h"

using namespace std;

namespace libgexf {

EdgeIter::EdgeIter(const Graph* g): _graph(g), _cpt(0), _nb_items(g->getEdgeCount()) {
    this->begin();
}

EdgeIter::~EdgeIter() {
}

EdgeIter* EdgeIter::begin() {
    _it = _graph->_edges.begin();
    _it2 = _it->second.begin();
    _cpt = 0;
    _nb_items = _graph->getEdgeCount();
    return this;
}

bool EdgeIter::hasNext() const {
    return _nb_items != 0 && _cpt != _nb_items;
}

t_id EdgeIter::next() {
    _current_edge_id = _it2->second;
    _current_source = _it->first;
    _current_target = _it2->first;

    _cpt++;
    _it2++;
    if(_it2 == _it->second.end()) {
        _it++;
        _it2 = _it->second.begin();
    }

    return _current_edge_id;
}

t_id EdgeIter::currentSource() const {
    return _current_source;
}

t_id EdgeIter::currentTarget() const {
    return _current_target;
}

float EdgeIter::currentProperty(t_edge_property prop) const {
    float r = 0.0;
    t_id edge_id = _current_edge_id;
    map<t_id,std::map<t_edge_property,t_edge_value> >::const_iterator it_properties = _graph->_edges_properties.find(edge_id);
    if( it_properties != _graph->_edges_properties.end() ) {
        map<t_edge_property,t_edge_value>::const_iterator it_prop = (it_properties->second).find(prop);
        if( it_prop != (it_properties->second).end() ) {
            r = it_prop->second;
        }
    }
    return r;
}

}
