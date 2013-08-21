/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.gephi.libgexf;

class libgexfJNI {
  public final static native long new_StringVector__SWIG_0();
  public final static native long new_StringVector__SWIG_1(long jarg1);
  public final static native long StringVector_size(long jarg1, StringVector jarg1_);
  public final static native long StringVector_capacity(long jarg1, StringVector jarg1_);
  public final static native void StringVector_reserve(long jarg1, StringVector jarg1_, long jarg2);
  public final static native boolean StringVector_isEmpty(long jarg1, StringVector jarg1_);
  public final static native void StringVector_clear(long jarg1, StringVector jarg1_);
  public final static native void StringVector_add(long jarg1, StringVector jarg1_, String jarg2);
  public final static native String StringVector_get(long jarg1, StringVector jarg1_, int jarg2);
  public final static native void StringVector_set(long jarg1, StringVector jarg1_, int jarg2, String jarg3);
  public final static native void delete_StringVector(long jarg1);
  public final static native long new_ReadLockException(String jarg1);
  public final static native void delete_ReadLockException(long jarg1);
  public final static native String ReadLockException_what(long jarg1, ReadLockException jarg1_);
  public final static native long new_WriteLockException(String jarg1);
  public final static native void delete_WriteLockException(long jarg1);
  public final static native String WriteLockException_what(long jarg1, WriteLockException jarg1_);
  public final static native long new_FileWriterException(String jarg1);
  public final static native void delete_FileWriterException(long jarg1);
  public final static native String FileWriterException_what(long jarg1, FileWriterException jarg1_);
  public final static native long new_FileReaderException(String jarg1);
  public final static native void delete_FileReaderException(long jarg1);
  public final static native String FileReaderException_what(long jarg1, FileReaderException jarg1_);
  public final static native long new_GEXF__SWIG_0();
  public final static native long new_GEXF__SWIG_1(long jarg1, GEXF jarg1_);
  public final static native void delete_GEXF(long jarg1);
  public final static native long GEXF_getUndirectedGraph(long jarg1, GEXF jarg1_);
  public final static native long GEXF_getDirectedGraph(long jarg1, GEXF jarg1_);
  public final static native long GEXF_getData(long jarg1, GEXF jarg1_);
  public final static native long GEXF_getMetaData(long jarg1, GEXF jarg1_);
  public final static native void GEXF_setGraphType(long jarg1, GEXF jarg1_, int jarg2);
  public final static native int GEXF_getGraphType(long jarg1, GEXF jarg1_);
  public final static native void GEXF_initGraphMode(long jarg1, GEXF jarg1_, String jarg2);
  public final static native String GEXF_getGraphMode(long jarg1, GEXF jarg1_);
  public final static native boolean GEXF_checkIntegrity(long jarg1, GEXF jarg1_);
  public final static native void GEXF__graph_set(long jarg1, GEXF jarg1_, long jarg2, Graph jarg2_);
  public final static native long GEXF__graph_get(long jarg1, GEXF jarg1_);
  public final static native void GEXF__type_set(long jarg1, GEXF jarg1_, int jarg2);
  public final static native int GEXF__type_get(long jarg1, GEXF jarg1_);
  public final static native void GEXF__mode_set(long jarg1, GEXF jarg1_, String jarg2);
  public final static native String GEXF__mode_get(long jarg1, GEXF jarg1_);
  public final static native void GEXF__data_set(long jarg1, GEXF jarg1_, long jarg2, Data jarg2_);
  public final static native long GEXF__data_get(long jarg1, GEXF jarg1_);
  public final static native void GEXF__meta_set(long jarg1, GEXF jarg1_, long jarg2, MetaData jarg2_);
  public final static native long GEXF__meta_get(long jarg1, GEXF jarg1_);
  public final static native long AbstractIter_begin(long jarg1, AbstractIter jarg1_);
  public final static native boolean AbstractIter_hasNext(long jarg1, AbstractIter jarg1_);
  public final static native String AbstractIter_next(long jarg1, AbstractIter jarg1_);
  public final static native void delete_AbstractIter(long jarg1);
  public final static native void delete_MemoryValidator(long jarg1);
  public final static native boolean MemoryValidator_run(long jarg1, GEXF jarg1_);
  public final static native long new_FileReader__SWIG_0();
  public final static native long new_FileReader__SWIG_1(String jarg1, int jarg2);
  public final static native long new_FileReader__SWIG_2(String jarg1);
  public final static native long new_FileReader__SWIG_3(long jarg1, FileReader jarg1_);
  public final static native void delete_FileReader(long jarg1);
  public final static native long FileReader_getGEXFCopy(long jarg1, FileReader jarg1_);
  public final static native void FileReader_init__SWIG_0(long jarg1, FileReader jarg1_, String jarg2, int jarg3);
  public final static native void FileReader_init__SWIG_1(long jarg1, FileReader jarg1_, String jarg2);
  public final static native void FileReader_slurp(long jarg1, FileReader jarg1_);
  public final static native void AbstractParser_bind(long jarg1, AbstractParser jarg1_, long jarg2, GEXF jarg2_);
  public final static native void AbstractParser_processNode(long jarg1, AbstractParser jarg1_, long jarg2, long jarg3);
  public final static native void delete_AbstractParser(long jarg1);
  public final static native void delete_GexfParser(long jarg1);
  public final static native void GexfParser_processNode(long jarg1, GexfParser jarg1_, long jarg2, long jarg3);
  public final static native long new_LegacyParser__SWIG_0();
  public final static native long new_LegacyParser__SWIG_1(long jarg1, LegacyParser jarg1_);
  public final static native void delete_LegacyParser(long jarg1);
  public final static native void delete_RngValidator(long jarg1);
  public final static native boolean RngValidator_run(String jarg1, String jarg2);
  public final static native void delete_SchemaValidator(long jarg1);
  public final static native boolean SchemaValidator_run(String jarg1, String jarg2);
  public final static native long new_FileWriter__SWIG_0();
  public final static native long new_FileWriter__SWIG_1(String jarg1, long jarg2, GEXF jarg2_);
  public final static native long new_FileWriter__SWIG_2(long jarg1, FileWriter jarg1_);
  public final static native void delete_FileWriter(long jarg1);
  public final static native long FileWriter_getGEXFCopy(long jarg1, FileWriter jarg1_);
  public final static native void FileWriter_init(long jarg1, FileWriter jarg1_, String jarg2, long jarg3, GEXF jarg3_);
  public final static native void FileWriter_write(long jarg1, FileWriter jarg1_);
  public final static native long new_LegacyWriter__SWIG_0();
  public final static native long new_LegacyWriter__SWIG_1(String jarg1, long jarg2, GEXF jarg2_);
  public final static native long new_LegacyWriter__SWIG_2(long jarg1, LegacyWriter jarg1_);
  public final static native void delete_LegacyWriter(long jarg1);
  public final static native long LegacyWriter_getGEXFCopy(long jarg1, LegacyWriter jarg1_);
  public final static native void LegacyWriter_init(long jarg1, LegacyWriter jarg1_, String jarg2, long jarg3, GEXF jarg3_);
  public final static native void LegacyWriter_write(long jarg1, LegacyWriter jarg1_);
  public final static native void delete_Conv(long jarg1);
  public final static native String Conv_xmlCharToId(long jarg1);
  public final static native String Conv_strToId(String jarg1);
  public final static native String Conv_xmlCharToStr(long jarg1);
  public final static native long Conv_xmlCharToUnsignedInt(long jarg1);
  public final static native float Conv_xmlCharToFloat(long jarg1);
  public final static native String Conv_idToStr(String jarg1);
  public final static native String Conv_unsignedIntToStr(long jarg1);
  public final static native String Conv_floatToStr(float jarg1);
  public final static native long Conv_strToUnsignedInt(String jarg1);
  public final static native String Conv_edgeTypeToStr(int jarg1);
  public final static native String Conv_attrTypeToStr(int jarg1);
  public final static native boolean Conv_isBoolean(String jarg1);
  public final static native boolean Conv_isDouble(String jarg1);
  public final static native boolean Conv_isInteger(String jarg1);
  public final static native boolean Conv_isFloat(String jarg1);
  public final static native boolean Conv_isLong(String jarg1);
  public final static native boolean Conv_isAnyURI(String jarg1);
  public final static native long Conv_tokenizer(String jarg1, String jarg2);
  public final static native long new_Graph__SWIG_0();
  public final static native long new_Graph__SWIG_1(long jarg1, Graph jarg1_);
  public final static native void delete_Graph(long jarg1);
  public final static native void Graph_addNode(long jarg1, Graph jarg1_, String jarg2);
  public final static native void Graph_addEdge__SWIG_0(long jarg1, Graph jarg1_, String jarg2, String jarg3, String jarg4, float jarg5, int jarg6);
  public final static native void Graph_addEdge__SWIG_1(long jarg1, Graph jarg1_, String jarg2, String jarg3, String jarg4, float jarg5);
  public final static native void Graph_addEdge__SWIG_2(long jarg1, Graph jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native void Graph_removeNode(long jarg1, Graph jarg1_, String jarg2);
  public final static native void Graph_removeEdge(long jarg1, Graph jarg1_, String jarg2, String jarg3);
  public final static native boolean Graph_containsNode(long jarg1, Graph jarg1_, String jarg2);
  public final static native boolean Graph_containsEdge(long jarg1, Graph jarg1_, String jarg2, String jarg3);
  public final static native String Graph_getEdge(long jarg1, Graph jarg1_, String jarg2, String jarg3);
  public final static native long Graph_getNodes(long jarg1, Graph jarg1_);
  public final static native long Graph_getEdges(long jarg1, Graph jarg1_);
  public final static native long Graph_getNeighbors(long jarg1, Graph jarg1_, String jarg2);
  public final static native long Graph_getNodeCount(long jarg1, Graph jarg1_);
  public final static native long Graph_getEdgeCount(long jarg1, Graph jarg1_);
  public final static native long Graph_getDegree(long jarg1, Graph jarg1_, String jarg2);
  public final static native void Graph_clearEdges__SWIG_0(long jarg1, Graph jarg1_, String jarg2);
  public final static native void Graph_clear(long jarg1, Graph jarg1_);
  public final static native void Graph_clearEdges__SWIG_1(long jarg1, Graph jarg1_);
  public final static native void Graph_readLock(long jarg1, Graph jarg1_);
  public final static native void Graph_readUnlock(long jarg1, Graph jarg1_);
  public final static native void Graph_writeLock(long jarg1, Graph jarg1_);
  public final static native void Graph_writeUnlock(long jarg1, Graph jarg1_);
  public final static native boolean Graph_isReadLock(long jarg1, Graph jarg1_);
  public final static native boolean Graph_isWriteLock(long jarg1, Graph jarg1_);
  public final static native boolean Graph_isUnlock(long jarg1, Graph jarg1_);
  public final static native long new_DirectedGraph__SWIG_0();
  public final static native long new_DirectedGraph__SWIG_1(long jarg1, DirectedGraph jarg1_);
  public final static native void delete_DirectedGraph(long jarg1);
  public final static native void DirectedGraph_removeInEdges(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native void DirectedGraph_removeOutEdges(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getInEdges(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getOutEdges(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getSuccessors(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getPredecessors(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getInDegree(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native long DirectedGraph_getOutDegree(long jarg1, DirectedGraph jarg1_, String jarg2);
  public final static native boolean DirectedGraph_isSuccessor(long jarg1, DirectedGraph jarg1_, String jarg2, String jarg3);
  public final static native boolean DirectedGraph_isPredecessor(long jarg1, DirectedGraph jarg1_, String jarg2, String jarg3);
  public final static native long new_UndirectedGraph__SWIG_0();
  public final static native long new_UndirectedGraph__SWIG_1(long jarg1, UndirectedGraph jarg1_);
  public final static native void delete_UndirectedGraph(long jarg1);
  public final static native long new_NodeIter(long jarg1, Graph jarg1_);
  public final static native void delete_NodeIter(long jarg1);
  public final static native long NodeIter_begin(long jarg1, NodeIter jarg1_);
  public final static native long new_EdgeIter(long jarg1, Graph jarg1_);
  public final static native void delete_EdgeIter(long jarg1);
  public final static native long EdgeIter_begin(long jarg1, EdgeIter jarg1_);
  public final static native String EdgeIter_currentSource(long jarg1, EdgeIter jarg1_);
  public final static native String EdgeIter_currentTarget(long jarg1, EdgeIter jarg1_);
  public final static native float EdgeIter_currentProperty(long jarg1, EdgeIter jarg1_, int jarg2);
  public final static native long new_Data__SWIG_0();
  public final static native long new_Data__SWIG_1(long jarg1, Data jarg1_);
  public final static native void delete_Data(long jarg1);
  public final static native String Data_getNodeLabel(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasNodeLabel(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_setNodeLabel(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native String Data_getEdgeLabel(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasEdgeLabel(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_setEdgeLabel(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_addNodeAttributeColumn__SWIG_0(long jarg1, Data jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native void Data_addNodeAttributeColumn__SWIG_1(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_addEdgeAttributeColumn__SWIG_0(long jarg1, Data jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native void Data_addEdgeAttributeColumn__SWIG_1(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_setNodeAttributeDefault(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_setEdgeAttributeDefault(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_setNodeAttributeOptions(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_setEdgeAttributeOptions(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_setNodeValue(long jarg1, Data jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native void Data_setEdgeValue(long jarg1, Data jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native long Data_getNodeAttributeColumn(long jarg1, Data jarg1_);
  public final static native long Data_getEdgeAttributeColumn(long jarg1, Data jarg1_);
  public final static native String Data_getNodeAttribute(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native String Data_getEdgeAttribute(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native long Data_getNodeAttributeRow(long jarg1, Data jarg1_, String jarg2);
  public final static native long Data_getEdgeAttributeRow(long jarg1, Data jarg1_, String jarg2);
  public final static native String Data_getNodeAttributeDefault(long jarg1, Data jarg1_, String jarg2);
  public final static native String Data_getEdgeAttributeOptions(long jarg1, Data jarg1_, String jarg2);
  public final static native String Data_getNodeAttributeOptions(long jarg1, Data jarg1_, String jarg2);
  public final static native String Data_getEdgeAttributeDefault(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasNodeAttributeDefault(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasEdgeAttributeDefault(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasNodeAttributeOptions(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_hasEdgeAttributeOptions(long jarg1, Data jarg1_, String jarg2);
  public final static native boolean Data_isNodeAttributeOption(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native boolean Data_isEdgeAttributeOption(long jarg1, Data jarg1_, String jarg2, String jarg3);
  public final static native void Data_removeNodeAttributeColumn(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_removeEdgeAttributeColumn(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_clearNodeAttributes(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_clearEdgeAttributes(long jarg1, Data jarg1_, String jarg2);
  public final static native void Data_clear(long jarg1, Data jarg1_);
  public final static native void Data_clearEdgesAttributes(long jarg1, Data jarg1_);
  public final static native long new_MetaData__SWIG_0();
  public final static native long new_MetaData__SWIG_1(long jarg1, MetaData jarg1_);
  public final static native void delete_MetaData(long jarg1);
  public final static native String MetaData_getVersion(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getXmlns(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getXsi(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getSchema(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getVariant(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getCreator(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getDescription(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getKeywords(long jarg1, MetaData jarg1_);
  public final static native String MetaData_getLastModifiedDate(long jarg1, MetaData jarg1_);
  public final static native void MetaData_setVersion(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setXmlns(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setXsi(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setSchema(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setVariant(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setCreator(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setDescription(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setKeywords(long jarg1, MetaData jarg1_, String jarg2);
  public final static native void MetaData_setLastModifiedDate(long jarg1, MetaData jarg1_, String jarg2);
  public final static native long new_AttributeIter(long jarg1, Data jarg1_, int jarg2);
  public final static native void delete_AttributeIter(long jarg1);
  public final static native long AttributeIter_begin(long jarg1, AttributeIter jarg1_);
  public final static native String AttributeIter_currentTitle(long jarg1, AttributeIter jarg1_);
  public final static native int AttributeIter_currentType(long jarg1, AttributeIter jarg1_);
  public final static native long new_AttValueIter(long jarg1, Data jarg1_, String jarg2, int jarg3);
  public final static native void delete_AttValueIter(long jarg1);
  public final static native long AttValueIter_begin(long jarg1, AttValueIter jarg1_);
  public final static native String AttValueIter_currentValue(long jarg1, AttValueIter jarg1_);
  public final static native String AttValueIter_currentName(long jarg1, AttValueIter jarg1_);
  public final static native long SWIGGexfParserUpcast(long jarg1);
  public final static native long SWIGLegacyParserUpcast(long jarg1);
  public final static native long SWIGDirectedGraphUpcast(long jarg1);
  public final static native long SWIGUndirectedGraphUpcast(long jarg1);
  public final static native long SWIGNodeIterUpcast(long jarg1);
  public final static native long SWIGEdgeIterUpcast(long jarg1);
  public final static native long SWIGAttributeIterUpcast(long jarg1);
  public final static native long SWIGAttValueIterUpcast(long jarg1);
}