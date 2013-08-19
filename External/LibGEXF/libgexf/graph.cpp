/*! \file graph.cpp
    \author sebastien heymann
    \date 17 avril 2009, 17:27
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

#include "graph.h"
#include "exceptions.h"
#include <stdexcept>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

namespace libgexf {

Graph::Graph() : _nodes(), _edges(), _reverse_edges(), _bloom_edges(), _edges_properties(), _rlock_count(0), _lock_flag('0') {
}

Graph::Graph(const Graph& orig): _nodes(orig._nodes), _edges(orig._edges), _reverse_edges(orig._reverse_edges),
        _edges_properties(orig._edges_properties), _rlock_count(orig._rlock_count), _lock_flag(orig._lock_flag) {
}

Graph::~Graph() {
    _edges.clear();
    _reverse_edges.clear();
    _bloom_edges.clear();
    _edges_properties.clear();
    _nodes.clear();
}

//-----------------------------------------
void Graph::addNode(const t_id id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");
    
    _nodes.insert(id);
}

//-----------------------------------------
void Graph::addEdge(const t_id id, const t_id source_id, const t_id target_id, const float weight, const t_edge_type type) {
//-----------------------------------------
const float e_type = (float)type;

    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    if(_bloom_edges.find(id) != _bloom_edges.end()) {
        throw invalid_argument("Edge id " + id + " is already in bloom filter.");
    }

    // 2 * O(log-n)
    if( _nodes.find(source_id) == _nodes.end() ) {
        throw invalid_argument("Invalid source node " + source_id + " for the edge "+id);
    }
    if( _nodes.find(target_id) == _nodes.end() ) {
        
        throw invalid_argument("Invalid target node " + target_id + " for the edge "+id);
    }

    // 4*O(log-n)
    map<t_id,map<t_id,t_id> >::iterator it = _edges.find(source_id);
    if(it != _edges.end()) {
        /* the source exists */
        map<t_id,t_id>::iterator it_target = (it->second).find(target_id);
        if(it_target != (it->second).end()) {
            /* an edge already exists between the two nodes */
            const t_id real_edge_id = it_target->second;

            map<t_id,map<t_edge_property,t_edge_value> >::iterator it_data = _edges_properties.find(real_edge_id);
            if(it_data != _edges_properties.end()) {
                /* at least one property exists */
                map<t_edge_property,t_edge_value>::iterator it_prop = (it_data->second).find(EDGE_WEIGHT);
                if(it_prop != (it_data->second).end()) {
                    /* the weight property exists, we increment it */
                    (it_prop->second) += weight;
                }
                else {
                    /* the weight property doesn't exists, we create it */
                    pair<t_edge_property,t_edge_value> edge_count = pair<t_edge_property,t_edge_value>(EDGE_WEIGHT,weight+1.0);
                    (it_data->second).insert(edge_count);
                }

                if( type != EDGE_UNDEF ) {
                    it_prop = (it_data->second).find(EDGE_TYPE);
                    if(it_prop != (it_data->second).end()) {
                        /* the type property exists, we change it */
                        it_prop->second = e_type;
                    }
                    else {
                        /* the type property doesn't exists, we create it */
                        pair<t_edge_property,t_edge_value> edge_type = pair<t_edge_property,t_edge_value>(EDGE_TYPE,e_type);
                        (it_data->second).insert(edge_type);
                    }
                }
            }
            else {
                /* no property exists, we create the entry and the properties */
                pair<t_edge_property,t_edge_value> edge_weight = pair<t_edge_property,t_edge_value>(EDGE_WEIGHT,weight+1.0);
                _edges_properties[real_edge_id].insert(edge_weight);

                if( type != EDGE_UNDEF ) {
                    pair<t_edge_property,t_edge_value> edge_type = pair<t_edge_property,t_edge_value>(EDGE_TYPE,e_type);
                    _edges_properties[real_edge_id].insert(edge_type);
                }
            }
        }
        else {
            /* no edge exists between the two nodes, we create the link and update the weight if needed */
            pair<t_id,t_id> link = pair<t_id,t_id>(target_id,id);
            (it->second).insert(link);
            
            if(weight > 1.0) {
                pair<t_edge_property,t_edge_value> edge_weight = pair<t_edge_property,t_edge_value>(EDGE_WEIGHT,weight);
                _edges_properties[id].insert(edge_weight);
            }
        }
    }
    else {
        /* the source doesn't exists, so we create the link */
        map<t_id,t_id> edges;
        edges.insert(pair<t_id,t_id>(target_id,id));
        _edges.insert(pair<t_id,map<t_id,t_id> >(source_id, edges));

        /* the weight property doesn't exist, we create it */
        if(weight > 1.0) {
            pair<t_edge_property,t_edge_value> edge_weight = pair<t_edge_property,t_edge_value>(EDGE_WEIGHT,weight);
            _edges_properties[id].insert(edge_weight);
        }

        if( type != EDGE_UNDEF ) {
            /* the type property doesn't exists, we create it */
            pair<t_edge_property,t_edge_value> edge_type = pair<t_edge_property,t_edge_value>(EDGE_TYPE,e_type);
            _edges_properties[id].insert(edge_type);
        }
    }

    // 2*O(log-n)
    map<t_id,set<t_id> >::iterator it2 = _reverse_edges.find(target_id);
    if(it2 != _reverse_edges.end()) {
        /* the target exists */
        ((*it2).second).insert(source_id);
    }
    else {
        /* the target doesn't exists */
        set<t_id> edges;
        edges.insert(source_id);
        _reverse_edges.insert(pair<t_id,set<t_id> >(target_id,edges));
    }

    _bloom_edges.insert(id);
}

//-----------------------------------------
void Graph::removeNode(const t_id id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    // 0(2n)
    this->clearEdges(id);

    // O(log-n)
    _nodes.erase(id);
}

//-----------------------------------------
void Graph::removeEdge(const t_id source_id, const t_id target_id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    // O(log-n)
    map<t_id,t_id>& links = _edges[source_id];
    std::map<t_id,t_id>::iterator it_t = links.find(target_id);
    if(it_t != links.end()) {
        const t_id edge_id = it_t->second;
        _bloom_edges.erase(edge_id);
        _edges_properties.erase(edge_id);
    }
    else {
        throw logic_error("Target id not found: " + target_id);
    }

    // O(log-n)
    links.erase(target_id);
    if(links.empty()) {
        _edges.erase(source_id);
    }

    // O(log-n)
    set<t_id>& sources = _reverse_edges[target_id];
    sources.erase(source_id);
    if(sources.empty()) {
        _reverse_edges.erase(target_id);
    }
}

//-----------------------------------------
NodeIter* Graph::getNodes() const {
//-----------------------------------------
    return new NodeIter(this);
}

//-----------------------------------------
EdgeIter* Graph::getEdges() const {
//-----------------------------------------
    return new EdgeIter(this);
}

//-----------------------------------------
bool Graph::containsNode(const t_id id) const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(log-n)
    return (_nodes.find(id) != _nodes.end());
}

//-----------------------------------------
bool Graph::containsEdge(const t_id source_id, const t_id target_id) const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 2*0(log-n)
    map<t_id,map<t_id,t_id> >::const_iterator it = _edges.find(source_id);
    if(it != _edges.end()) {
        return ((it->second).find(target_id) != (it->second).end());
    }
    return false;
}

//-----------------------------------------
t_id Graph::getEdge(const t_id source_id, const t_id target_id) const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 2*0(log-n)
    map<t_id,map<t_id,t_id> >::const_iterator it = _edges.find(source_id);
    if(it != _edges.end()) {
        map<t_id,t_id>::const_iterator it2 = (it->second).find(target_id);
        if(it2 != (it->second).end()) {
            return it2->second;
        }
    }
    return "";
}

//-----------------------------------------
unsigned int Graph::getNodeCount() const {
//-----------------------------------------
    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(log-n)
    return _nodes.size();
}

//-----------------------------------------
unsigned int Graph::getEdgeCount() const {
//-----------------------------------------
unsigned int count = 0;

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(n)
    for(map<t_id,map<t_id,t_id> >::const_iterator it = _edges.begin() ; it != _edges.end() ; ++it) {
        count += (it->second).size();
    }

    return count;
}

//-----------------------------------------
unsigned int Graph::getDegree(const t_id node_id) const {
//-----------------------------------------
unsigned int count = 0;

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(2*log-n)
    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        count += (it_e->second).size();

        /* add weights */
        for ( map<t_id,t_id>::const_iterator it_t=(it_e->second).begin() ; it_t != (it_e->second).end(); ++it_t ) {
            map<t_id,map<t_edge_property,t_edge_value> >::const_iterator it_data = _edges_properties.find(it_t->second);
            /*if(it_data != _edges_properties.end()) {
                map<t_edge_property,t_edge_value>::const_iterator it_count = (it_data->second).find(EDGE_COUNT);
                if(it_count != (it_data->second).end()) {
                    count += (unsigned int)it_count->second - 1;
                }
            }*/
        }
    }

    // 0(log-n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        count += (it_re->second).size();
    }

    return count;
}

//-----------------------------------------
vector<t_id> Graph::getNeighbors(const t_id node_id) const {
//-----------------------------------------
set<t_id> s = set<t_id>();

    if(_lock_flag == '2') throw WriteLockException("Read not allowed");

    // 0(n)
    map<t_id,map<t_id,t_id> >::const_iterator it_e = _edges.find(node_id);
    if(it_e != _edges.end()) {
        for(map<t_id,t_id>::const_iterator it = (it_e->second).begin(); it != (it_e->second).end(); ++it) {
            //v.push_back(it->first); // succ_id
            s.insert(it->first); // succ_id
        }
    }
    // 0(n)
    map<t_id,set<t_id> >::const_iterator it_re = _reverse_edges.find(node_id);
    if(it_re != _reverse_edges.end()) {
        for(set<t_id>::const_iterator it = (it_re->second).begin(); it != (it_re->second).end(); ++it) {
            //v.push_back(*it); // pred_id
            s.insert(*it); // pred_id
        }
    }
    
    vector<t_id> v(s.begin(), s.end());
    s.clear();

    return v;
}

//-----------------------------------------
void Graph::clearEdges(const t_id node_id) {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");

    /* removeInEdges */
    // O(n + 4*log-n)
    set<t_id>& sources = _reverse_edges[node_id];
    for( set<t_id>::iterator it=sources.begin() ; it != sources.end(); ++it ) {
        for( map<t_id,t_id>::const_iterator it2=_edges[*it].begin() ; it2 != _edges[*it].end(); ++it2 ) {
            _bloom_edges.erase(it2->second);
        }
        _edges[*it].erase(node_id);
        if(_edges[*it].empty()) {
            _edges.erase(*it);
        }
    }
    _reverse_edges.erase(node_id);

    /* removeOutEdges */
    // O(n + 4*log-n)
    map<t_id,t_id>& links = _edges[node_id];
    for( map<t_id,t_id>::iterator it=links.begin() ; it != links.end(); ++it ) {
        _bloom_edges.erase(it->second);
        _reverse_edges[it->first].erase(node_id);
        if(_reverse_edges[it->first].empty()) {
            _reverse_edges.erase(it->first);
        }
    }
    _edges.erase(node_id);
}

//-----------------------------------------
void Graph::clear() {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");
    _edges_properties.clear();
    _edges.clear();
    _reverse_edges.clear();
    _bloom_edges.clear();
    _nodes.clear();
    _lock_flag = '0';
}

//-----------------------------------------
void Graph::clearEdges() {
//-----------------------------------------
    if(_lock_flag == '1') throw ReadLockException("Write not allowed");
    _edges_properties.clear();
    _edges.clear();
    _reverse_edges.clear();
    _bloom_edges.clear();
}

//-----------------------------------------
void Graph::readLock() throw(ReadLockException) {
//-----------------------------------------
    if(_lock_flag != '2') {
        _lock_flag = '1';
        _rlock_count +=1;
    }
    else throw ReadLockException("Can't set a read-lock: a write-lock exists");
}

//-----------------------------------------
void Graph::readUnlock() {
//-----------------------------------------
    if(_rlock_count >= 1) {
        _rlock_count -=1;
        if(_rlock_count == 0) _lock_flag = '0';
    }
}

//-----------------------------------------
void Graph::writeLock() throw(WriteLockException) {
//-----------------------------------------
    if(_lock_flag == '0') _lock_flag = '2';
    else throw WriteLockException("Can't set a write-lock: another lock exists");
}

//-----------------------------------------
void Graph::writeUnlock() {
//-----------------------------------------
    _lock_flag = '0';
}

//-----------------------------------------
bool Graph::isReadLock() {
//-----------------------------------------
    return _lock_flag == '1';
}

//-----------------------------------------
bool Graph::isWriteLock() {
//-----------------------------------------
    return _lock_flag == '2';
}

//-----------------------------------------
bool Graph::isUnlock() {
//-----------------------------------------
    return _lock_flag == '0';
}


//-----------------------------------------
ostream& operator<<(ostream& os, const Graph& o) {
//-----------------------------------------
    os << "Graph [" << endl;
    os << "_nodes [" << endl;
    for ( set<t_id>::const_iterator it = o._nodes.begin() ; it != o._nodes.end(); ++it ) {
        os << " " << *it;
    }
    os << endl << "]" << endl;
    os << "_edges [" << endl;
    for ( map<t_id,map<t_id,t_id> >::const_iterator it=o._edges.begin() ; it != o._edges.end(); ++it ) {
        os << it->first << " => ";
        for ( map<t_id,t_id>::const_iterator its = it->second.begin() ; its != it->second.end(); ++its ) {
            os << its->first << " => " << its->second << std::endl;
        }
        os << std::endl;
    }
    os << endl << "]" << endl;
    os << "_reverse_edges [" << endl;
    for ( map<t_id,set<t_id> >::const_iterator it=o._reverse_edges.begin() ; it != o._reverse_edges.end(); ++it ) {
        os << it->first << " => ";
        for ( set<t_id>::const_iterator its = it->second.begin() ; its != it->second.end(); ++its ) {
            os << " " << *its;
        }
        os << std::endl;
    }
    os << endl << "]" << endl;
    os << "_edges_properties [" << endl;
    std::map<t_id,std::map<t_edge_property,t_edge_value> >::const_iterator it;
    for ( it=o._edges_properties.begin() ; it != o._edges_properties.end(); ++it ) {
        os << (*it).first << "-> ";
        std::map<t_edge_property,t_edge_value>::const_iterator it2;
        for ( it2=((*it).second).begin() ; it2 != ((*it).second).end(); ++it2 ) {
            os << "{" << (*it2).first << "; " << (*it2).second << "}" << endl;
        }
    }
    os << endl << "]" << endl;
    os << "]" << endl << endl;

    return os;
}

} /* namespace libgexf */
