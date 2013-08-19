/*! \file directedgraph.cpp
    \author sebastien heymann
    \date 8 juin 2009, 10:21
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

#include "directedgraph.h"
#include "exceptions.h"
#include <stdexcept>
#include <map>
using namespace std;

namespace libgexf {

DirectedGraph::DirectedGraph() : Graph() {
}

DirectedGraph::DirectedGraph(const DirectedGraph& orig) : Graph(orig) {
}

DirectedGraph::~DirectedGraph() {
}

//-----------------------------------------
void DirectedGraph::removeInEdges(const t_id target_id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    // O(n + 4*log-n)
    set<t_id>& sources = _reverse_edges[target_id];
    for( set<t_id>::iterator it=sources.begin() ; it != sources.end(); ++it ) {
        for( map<t_id,t_id>::const_iterator it2=_edges[*it].begin() ; it2 != _edges[*it].end(); ++it2 ) {
            _bloom_edges.erase(it2->second);
        }
        _edges[*it].erase(target_id);
        if(_edges[*it].empty()) {
            _edges.erase(*it);
        }
    }
    _reverse_edges.erase(target_id);
}

//-----------------------------------------
void DirectedGraph::removeOutEdges(const t_id source_id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    // O(n + 4*log-n)
    map<t_id,t_id>& links = _edges[source_id];
    for( map<t_id,t_id>::iterator it=links.begin() ; it != links.end(); ++it ) {
        _bloom_edges.erase(it->second);
        _reverse_edges[it->first].erase(source_id);
        if(_reverse_edges[it->first].empty()) {
            _reverse_edges.erase(it->first);
        }
    }
    _edges.erase(source_id);
}

//-----------------------------------------
std::vector<t_id> DirectedGraph::getInEdges(const t_id node_id) const {
//-----------------------------------------
set<t_id> s = set<t_id>();

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(2n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        for(set<t_id>::const_iterator it = (it_re->second).begin(); it != (it_re->second).end(); ++it) {
            map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(*it); // source_id = source_id
            if(it_e != _edges.end()) {
                for(map<t_id,t_id>::const_iterator it2 = (it_e->second).begin(); it2 != (it_e->second).end(); ++it2) {
                    s.insert(it2->second); // edge_id
                }
            }
        }
    }
    std::vector<t_id> v( s.begin(),s.end() );
    s.clear();

    return v;
}

//-----------------------------------------
std::vector<t_id> DirectedGraph::getOutEdges(const t_id node_id) const {
//-----------------------------------------
set<t_id> s = set<t_id>();

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(2n)
    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        for(map<t_id,t_id>::const_iterator it2 = (it_e->second).begin(); it2 != (it_e->second).end(); ++it2) {
            s.insert(it2->second); // edge_id
        }
    }
    std::vector<t_id> v( s.begin(),s.end() );
    s.clear();

    return v;
}

//-----------------------------------------
std::vector<t_id> DirectedGraph::getSuccessors(const t_id node_id) const {
//-----------------------------------------
set<t_id> s = set<t_id>();

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        // 0(n)
        for(map<t_id,t_id>::const_iterator it = (it_e->second).begin(); it != (it_e->second).end(); ++it) {
            s.insert(it->first); // succ_id
        }
    }
    std::vector<t_id> v( s.begin(),s.end() );
    s.clear();

    return v;
}

//-----------------------------------------
std::vector<t_id> DirectedGraph::getPredecessors(const t_id node_id) const {
//-----------------------------------------
set<t_id> s = set<t_id>();

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        for(set<t_id>::const_iterator it = (it_re->second).begin(); it != (it_re->second).end(); ++it) {
            s.insert(*it); // pred_id
        }
    }
    std::vector<t_id> v( s.begin(),s.end() );
    s.clear();

    return v;
}

//-----------------------------------------
unsigned int DirectedGraph::getInDegree(const t_id node_id) const {
//-----------------------------------------
unsigned int count = 0;

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(2*log-n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        count += (it_re->second).size();

        set<t_id>::const_iterator it_s;
        for ( it_s=(it_re->second).begin() ; it_s != (it_re->second).end(); ++it_s ) {
            map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(*it_s);
            map<t_id,t_id>::const_iterator it_t = (it_e->second).find(node_id);
            map<t_id,map<t_edge_property,t_edge_value> >::const_iterator it_properties = _edges_properties.find(it_t->second); // it_t->second = edge_id
            /*if(it_properties != _edges_properties.end()) {
                map<t_edge_property,t_edge_value>::const_iterator it_count = (it_properties->second).find(EDGE_COUNT);
                if(it_count != (it_properties->second).end()) {
                    count += (unsigned int)it_count->second - 1;
                }
            }*/
        }
    }


    return count;
}

//-----------------------------------------
unsigned int DirectedGraph::getOutDegree(const t_id node_id) const {
//-----------------------------------------
unsigned int count = 0;

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(2*log-n)
    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        count += (it_e->second).size();

        /* add cardinals */
        map<t_id,t_id>::const_iterator it_t;
        for ( it_t=(it_e->second).begin() ; it_t != (it_e->second).end(); ++it_t ) {
            map<t_id,map<t_edge_property,t_edge_value> >::const_iterator it_properties = _edges_properties.find(it_t->second);
            /*if(it_properties != _edges_properties.end()) {
                map<t_edge_property,t_edge_value>::const_iterator it_count = (it_properties->second).find(EDGE_COUNT);
                if(it_count != (it_properties->second).end()) {
                    count += (unsigned int)it_count->second - 1;
                }
            }*/
        }
    }

    return count;
}

//-----------------------------------------
bool DirectedGraph::isSuccessor(const t_id node_id, const t_id successor_id) const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(log-n)
    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        return ( (it_e->second).find(successor_id) != (it_e->second).end() );
    }

    return false;
}

//-----------------------------------------
bool DirectedGraph::isPredecessor(const t_id node_id, const t_id predecessor_id) const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(log-n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        return ( (it_re->second).find(predecessor_id) != (it_re->second).end() );
    }

    return false;
}


}
