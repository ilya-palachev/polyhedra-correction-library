/*! \file nodeiter.cpp
    \author sebastien heymann
    \date 9 juillet 2009, 15:48
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

#include "nodeiter.h"

using namespace std;

namespace libgexf {

NodeIter::NodeIter(const Graph* g): _graph(g), _cpt(0), _nb_items(g->getNodeCount()) {
    this->begin();
}

NodeIter::~NodeIter() {
}

NodeIter* NodeIter::begin() {
    _it = _graph->_nodes.begin();
    _cpt = 0;
    _nb_items = _graph->getNodeCount();
    return this;
}

bool NodeIter::hasNext() const {
    return _nb_items != 0 && _cpt != _nb_items;
}

t_id NodeIter::next() {
    ++_cpt;
    return *_it++;
}

} /* namespace libgexf */
