/*! \file undirectedgraph.cpp
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

#include "undirectedgraph.h"
#include <map>
#include <stdexcept>
using namespace std;

namespace libgexf {

UndirectedGraph::UndirectedGraph() : Graph() {
}

UndirectedGraph::UndirectedGraph(const UndirectedGraph& orig) : Graph(orig) {
}

UndirectedGraph::~UndirectedGraph() {
}

/*void UndirectedGraph::mergeSimilarEdges() {
    // 0(n2)
    for(map<t_id,map<t_id,t_id> >::const_iterator it_ref_src = _edges.begin() ; it_ref_src != _edges.end() ; ++it_ref_src) {
        for(map<t_id,t_id>::const_iterator it_ref_target = (it_ref_src->second).begin() ; it_ref_target != (it_ref_src->second).end() ; ++it_ref_target) {
            for(map<t_id,map<t_id,t_id> >::const_iterator it_eval_src = _edges.begin() ; it_eval_src != _edges.end() ; ++it_eval_src) {
                for(map<t_id,t_id>::const_iterator it_eval_target = (it_eval_src->second).begin() ; it_eval_target != (it_eval_src->second).end() ; ++it_eval_target) {

                    // Continue if the edge is a loop
                    if( it_ref_src->first == it_ref_target->first) {
                        continue;
                    }
                    // Continue if the edge is itself
                    if( it_eval_src->first == it_ref_src->first && it_eval_target->first == it_ref_target->first) {
                        continue;
                    }
                    // Continue if the original edge is directed
                    map<t_id,map<t_edge_property,t_edge_value> >::iterator it_ref_data = _edges_properties.find( it_ref_target->second );
                    if(it_ref_data != _edges_properties.end()) {
                        map<t_edge_property,t_edge_value>::iterator it_type = (it_ref_data->second).find(EDGE_TYPE);
                        if(it_type != (it_ref_data->second).end()) {
                            if( it_type->second == EDGE_DIRECTED ) {
                                continue;
                            }
                        }
                    }
                    // Continue if the evaluated edge is directed
                    map<t_id,map<t_edge_property,t_edge_value> >::iterator it_eval_data = _edges_properties.find( it_eval_target->second );
                    if(it_eval_data != _edges_properties.end()) {
                        map<t_edge_property,t_edge_value>::iterator it_type = (it_eval_data->second).find(EDGE_TYPE);
                        if(it_type != (it_eval_data->second).end()) {
                            if( it_type->second == EDGE_DIRECTED ) {
                                continue;
                            }
                        }
                    }
                    if( it_eval_src->first == it_ref_target->first && it_eval_target->first == it_ref_src->first) {
                        // source <> target, the edges are equivalent
                        unsigned int card = 1;
                        if(it_eval_data != _edges_properties.end()) {
                            map<t_edge_property,t_edge_value>::iterator it_count = (it_eval_data->second).find(EDGE_COUNT);
                            if(it_count != (it_eval_data->second).end()) {
                                card = it_count->second;
                            }
                        }
                        // merge
                        if(it_ref_data != _edges_properties.end()) {
                            map<t_edge_property,t_edge_value>::iterator it_prop = (it_ref_data->second).find(EDGE_COUNT);
                            if(it_prop != (it_ref_data->second).end()) {
                                // the count property exists, we do the sum
                                it_prop->second += card;
                            }
                            else {
                                // count property does not exists, we create it
                                pair<t_edge_property,t_edge_value> edge_count = pair<t_edge_property,t_edge_value>(EDGE_COUNT,card+1.0);
                                _edges_properties[it_ref_target->second].insert(edge_count);
                            }
                            if(it_eval_data != _edges_properties.end()) {
                                map<t_edge_property,t_edge_value>::iterator it_type = (it_eval_data->second).find(EDGE_TYPE);
                                if(it_type != (it_eval_data->second).end()) {
                                    it_prop = (it_ref_data->second).find(EDGE_TYPE);
                                    if(it_prop != (it_ref_data->second).end()) {
                                        // the type property exists, we check the values
                                        if( it_type->second != it_prop->second ) {
                                            throw logic_error("Types of edges \"" + it_ref_target->second + "\" and \"" + it_eval_target->second + "\" are different.");
                                        }
                                    }
                                    else {
                                        // type property does not exists, we copy the evaluated edge value
                                        pair<t_edge_property,t_edge_value> edge_type = pair<t_edge_property,t_edge_value>(EDGE_TYPE,it_type->second);
                                        _edges_properties[it_ref_target->second].insert(edge_type);
                                    }
                                }
                            }
                        }
                        else {
                            // no property exists, we create the count property
                            pair<t_edge_property,t_edge_value> edge_count = pair<t_edge_property,t_edge_value>(EDGE_COUNT,card+1.0);
                            _edges_properties[it_ref_target->second].insert(edge_count);
                        }
                       this->removeEdge( it_eval_src->first, it_eval_target->first );
                    }
                }
            }
        }
    }
}*/

}
