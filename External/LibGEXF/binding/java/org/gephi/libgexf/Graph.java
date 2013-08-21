/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.gephi.libgexf;

public class Graph {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected Graph(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Graph obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libgexfJNI.delete_Graph(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public Graph() {
    this(libgexfJNI.new_Graph__SWIG_0(), true);
  }

  public Graph(Graph orig) {
    this(libgexfJNI.new_Graph__SWIG_1(Graph.getCPtr(orig), orig), true);
  }

  public void addNode(String id) {
    libgexfJNI.Graph_addNode(swigCPtr, this, id);
  }

  public void addEdge(String id, String source_id, String target_id, float weight, t_edge_type type) {
    libgexfJNI.Graph_addEdge__SWIG_0(swigCPtr, this, id, source_id, target_id, weight, type.swigValue());
  }

  public void addEdge(String id, String source_id, String target_id, float weight) {
    libgexfJNI.Graph_addEdge__SWIG_1(swigCPtr, this, id, source_id, target_id, weight);
  }

  public void addEdge(String id, String source_id, String target_id) {
    libgexfJNI.Graph_addEdge__SWIG_2(swigCPtr, this, id, source_id, target_id);
  }

  public void removeNode(String id) {
    libgexfJNI.Graph_removeNode(swigCPtr, this, id);
  }

  public void removeEdge(String source_id, String target_id) {
    libgexfJNI.Graph_removeEdge(swigCPtr, this, source_id, target_id);
  }

  public boolean containsNode(String id) {
    return libgexfJNI.Graph_containsNode(swigCPtr, this, id);
  }

  public boolean containsEdge(String source_id, String target_id) {
    return libgexfJNI.Graph_containsEdge(swigCPtr, this, source_id, target_id);
  }

  public String getEdge(String source_id, String target_id) {
    return libgexfJNI.Graph_getEdge(swigCPtr, this, source_id, target_id);
  }

  public NodeIter getNodes() {
    long cPtr = libgexfJNI.Graph_getNodes(swigCPtr, this);
    return (cPtr == 0) ? null : new NodeIter(cPtr, false);
  }

  public EdgeIter getEdges() {
    long cPtr = libgexfJNI.Graph_getEdges(swigCPtr, this);
    return (cPtr == 0) ? null : new EdgeIter(cPtr, false);
  }

  public StringVector getNeighbors(String node_id) {
    return new StringVector(libgexfJNI.Graph_getNeighbors(swigCPtr, this, node_id), true);
  }

  public long getNodeCount() {
    return libgexfJNI.Graph_getNodeCount(swigCPtr, this);
  }

  public long getEdgeCount() {
    return libgexfJNI.Graph_getEdgeCount(swigCPtr, this);
  }

  public long getDegree(String node_id) {
    return libgexfJNI.Graph_getDegree(swigCPtr, this, node_id);
  }

  public void clearEdges(String node_id) {
    libgexfJNI.Graph_clearEdges__SWIG_0(swigCPtr, this, node_id);
  }

  public void clear() {
    libgexfJNI.Graph_clear(swigCPtr, this);
  }

  public void clearEdges() {
    libgexfJNI.Graph_clearEdges__SWIG_1(swigCPtr, this);
  }

  public void readLock() {
    libgexfJNI.Graph_readLock(swigCPtr, this);
  }

  public void readUnlock() {
    libgexfJNI.Graph_readUnlock(swigCPtr, this);
  }

  public void writeLock() {
    libgexfJNI.Graph_writeLock(swigCPtr, this);
  }

  public void writeUnlock() {
    libgexfJNI.Graph_writeUnlock(swigCPtr, this);
  }

  public boolean isReadLock() {
    return libgexfJNI.Graph_isReadLock(swigCPtr, this);
  }

  public boolean isWriteLock() {
    return libgexfJNI.Graph_isWriteLock(swigCPtr, this);
  }

  public boolean isUnlock() {
    return libgexfJNI.Graph_isUnlock(swigCPtr, this);
  }

}