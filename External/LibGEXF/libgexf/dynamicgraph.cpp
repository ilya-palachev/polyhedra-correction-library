/*! \file dynamicgraph.cpp
    \author sebastien heymann
    \date 2 novembre 2009, 18:52
    \version 0.1
 */

#include "dynamicgraph.h"
#include <set>

using namespace std;

namespace libgexf {

DynamicGraph::DynamicGraph(): _graph(), _nodes_dates(), _edges_dates() {
}

DynamicGraph::DynamicGraph(const DynamicGraph& orig): _graph(orig._graph), _nodes_dates(orig._nodes_dates), _edges_dates(orig._edges_dates) {
}

DynamicGraph::~DynamicGraph() {
}

//-----------------------------------------
bool DynamicGraph::hasNodeStart(const t_id node_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        return ((it->second).first) != "";
    }
    return false;
}

//-----------------------------------------
bool DynamicGraph::hasEdgeStart(const t_id edge_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        return ((it->second).first) != "";
    }
    return false;
}

//-----------------------------------------
bool DynamicGraph::hasNodeEnd(const t_id node_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        return ((it->second).second) != "";
    }
    return false;
}

//-----------------------------------------
bool DynamicGraph::hasEdgeEnd(const t_id edge_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        return ((it->second).second) != "";
    }
    return false;
}

//-----------------------------------------
string DynamicGraph::getNodeStart(const t_id node_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        return (it->second).first;
    }
    return "";
}

//-----------------------------------------
string DynamicGraph::getEdgeStart(const t_id edge_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        return (it->second).first;
    }
    return "";
}

//-----------------------------------------
string DynamicGraph::getNodeEnd(const t_id node_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        return (it->second).second;
    }
    return "";
}

//-----------------------------------------
string DynamicGraph::getEdgeEnd(const t_id edge_id) const {
//-----------------------------------------
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        return (it->second).second;
    }
    return "";
}

//-----------------------------------------
void DynamicGraph::setNodeStart(const t_id node_id, const string& date) {
//-----------------------------------------
    // dates exists ?
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        // yes
        _nodes_dates[node_id] = pair<string,string>(date,it->second.second);
    }
    else {
        // no
        _nodes_dates[node_id] = pair<string,string>(date,"");
    }
}

//-----------------------------------------
void DynamicGraph::setEdgeStart(const t_id edge_id, const string& date) {
//-----------------------------------------
    // dates exists ?
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        // yes
        _edges_dates[edge_id] = pair<string,string>(date,it->second.second);
    }
    else {
        // no
        _edges_dates[edge_id] = pair<string,string>(date,"");
    }
}

//-----------------------------------------
void DynamicGraph::setNodeEnd(const t_id node_id, const string& date) {
//-----------------------------------------
    // dates exists ?
    map<t_id,pair<string,string> >::const_iterator it = _nodes_dates.find(node_id);
    if(it != _nodes_dates.end()) {
        // yes
        _nodes_dates[node_id] = pair<string,string>(it->second.first,date);
    }
    else {
        // no
        _nodes_dates[node_id] = pair<string,string>("",date);
    }
}

//-----------------------------------------
void DynamicGraph::setEdgeEnd(const t_id edge_id, const string& date) {
//-----------------------------------------
    // dates exists ?
    map<t_id,pair<string,string> >::const_iterator it = _edges_dates.find(edge_id);
    if(it != _edges_dates.end()) {
        // yes
        _edges_dates[edge_id] = pair<string,string>(it->second.first,date);
    }
    else {
        // no
        _edges_dates[edge_id] = pair<string,string>("",date);
    }
}

//-----------------------------------------
unsigned int DynamicGraph::getDegree(const libgexf::t_id node_id, const std::string& date) const {
//-----------------------------------------
cerr << "WARN " << "DynamicGraph::getDegree() is not coded yet! Returns 0." << endl;
    return 0;
}

//-----------------------------------------
void DynamicGraph::removeNode(const libgexf::t_id id) {
//-----------------------------------------
    _nodes_dates.erase(id);

    //find edges to delete
    vector<t_id> neighbors = _graph.getNeighbors(id);
    for(unsigned int i=0;i < neighbors.size(); i++) {
        const t_id neighbor = neighbors.at(i);
        // source-target
        t_id edge_id = _graph.getEdge(id, neighbor);
        if(edge_id != "") {
            _edges_dates.erase(edge_id);
        }
        // target-source
        edge_id = _graph.getEdge(neighbor, id);
        if(edge_id != "") {
            _edges_dates.erase(edge_id);
        }
    }

    _graph.removeNode(id);
}

//-----------------------------------------
void DynamicGraph::removeEdge(const libgexf::t_id source_id, const libgexf::t_id target_id) {
//-----------------------------------------
    const t_id edge_id = _graph.getEdge(source_id, target_id);
    _edges_dates.erase(edge_id);
    _graph.removeEdge(source_id, target_id);
}

//-----------------------------------------
void DynamicGraph::clearEdges(const libgexf::t_id node_id) {
//-----------------------------------------
    //find edges to delete
    vector<t_id> neighbors = _graph.getNeighbors(node_id);
    for(unsigned int i=0;i < neighbors.size(); i++) {
        const t_id neighbor = neighbors.at(i);
        // source-target
        t_id edge_id = _graph.getEdge(node_id, neighbor);
        if(edge_id != "") {
            _edges_dates.erase(edge_id);
        }
        // target-source
        edge_id = _graph.getEdge(neighbor, node_id);
        if(edge_id != "") {
            _edges_dates.erase(edge_id);
        }
    }
    
    _graph.clearEdges(node_id);
}

//-----------------------------------------
void DynamicGraph::clearEdges() {
//-----------------------------------------
    _edges_dates.clear();
    _graph.clearEdges();
}

//-----------------------------------------
void DynamicGraph::clear() {
//-----------------------------------------
    _nodes_dates.clear();
    _edges_dates.clear();
    _graph.clear();
}

} /* namespace libgexf */
