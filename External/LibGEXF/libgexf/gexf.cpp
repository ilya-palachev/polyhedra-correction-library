/*! \file gexf.cpp
    \author sebastien heymann
    \date 17 avril 2009, 17:28
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

#include "gexf.h"
#include "data.h"

#include <string>

namespace libgexf {

GEXF::GEXF(): _graph(), _type(GRAPH_UNDEF), _data(), _meta() {
}

GEXF::GEXF(const GEXF& orig) : _graph(orig._graph), _type(orig._type), _data(orig._data), _meta(orig._meta) {
}

GEXF::~GEXF() {
}

//-----------------------------------------
UndirectedGraph& GEXF::getUndirectedGraph() {
//-----------------------------------------
    // a graph is instanciable if the current one is empty
    if(_type == GRAPH_UNDEF) {
        UndirectedGraph* g = new UndirectedGraph();
        _graph = *g;
        _type = GRAPH_UNDIRECTED;
    }

    return (UndirectedGraph&)_graph;
}

//-----------------------------------------
DirectedGraph& GEXF::getDirectedGraph() {
//-----------------------------------------
    // a graph is instanciable if the current one is empty
    if(_type == GRAPH_UNDEF) {
        DirectedGraph* g = new DirectedGraph();
        _graph = *g;
        _type = GRAPH_DIRECTED;
    }

    return (DirectedGraph&)_graph;
}

//-----------------------------------------
t_graph GEXF::getGraphType() {
//-----------------------------------------
    return _type;
}

//-----------------------------------------
void GEXF::setGraphType(t_graph t) {
//-----------------------------------------
    _type = t;
}

//-----------------------------------------
std::string GEXF::getGraphMode() {
//-----------------------------------------
    return _mode;
}

//-----------------------------------------
void GEXF::initGraphMode(const std::string& mode) {
//-----------------------------------------
    if(mode.compare("static") == 0 || mode.compare("dynamic") == 0) {
        _mode = mode;
    }
    else {
        _mode = "static";
    }
}

//-----------------------------------------
Data& GEXF::getData() {
//-----------------------------------------
    return _data;
}

//-----------------------------------------
MetaData& GEXF::getMetaData() {
//-----------------------------------------
    return _meta;
}

//-----------------------------------------
bool GEXF::checkIntegrity() {
//-----------------------------------------
    return MemoryValidator::run(*this);
}

//-----------------------------------------
std::ostream& operator<<(std::ostream& os, const GEXF& o) {
//-----------------------------------------
    os << "GEXF [" << std::endl;
    os << o._graph << std::endl;
    os << o._meta << std::endl;
    os << o._data << "]" << std::endl;
    return os;
}


} /* namespace libgexf */
