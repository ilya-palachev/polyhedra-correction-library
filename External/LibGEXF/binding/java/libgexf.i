/*! \file : libgexf.i
    \brief SWIG interface
 */
%module libgexf

%ignore operator<<;
/*%nodefaultdtor;*/

%{
#define SWIG_FILE_WITH_INIT
/* main */
#include "../../libgexf/typedefs.h"
#include "../../libgexf/exceptions.h"
#include "../../libgexf/gexf.h"
#include "../../libgexf/abstractiter.h"
#include "../../libgexf/memoryvalidator.h"
/* io::input */
#include "../../libgexf/filereader.h"
#include "../../libgexf/abstractparser.h"
#include "../../libgexf/gexfparser.h"
#include "../../libgexf/legacyparser.h"
#include "../../libgexf/rngvalidator.h"
#include "../../libgexf/schemavalidator.h"
/* io::output */
#include "../../libgexf/filewriter.h"
#include "../../libgexf/legacywriter.h"
/* io::utils */
#include "../../libgexf/conv.h"
/* db::topo */
#include "../../libgexf/graph.h"
#include "../../libgexf/directedgraph.h"
#include "../../libgexf/undirectedgraph.h"
#include "../../libgexf/nodeiter.h"
#include "../../libgexf/edgeiter.h"
/* db::data */
#include "../../libgexf/data.h"
#include "../../libgexf/metadata.h"
#include "../../libgexf/attributeiter.h"
#include "../../libgexf/attvalueiter.h"
%}

%include stl.i
%include std_except.i
%include typemaps.i
/*%include various.i*/

/* Exceptions */


/*%typemap(javabase) ReadLockException "java.lang.Exception";
%typemap(javacode) ReadLockException %{
  public String getMessage() {
    return what();
  }
%}*/

/*%typemap(throws, throws="ReadLockException") ReadLockException {
  jclass clazz = jenv->FindClass("java/lang/Exception");
  jenv->ThrowNew(clazz, $1.what().c_str());
  return $null;
}*/



/*%exception {
  try {
    $action
  } catch (const std::exception& e) {
    jclass excep = jenv->FindClass("java/lang/Exception");
    jenv->ThrowNew(excep, e.what());
    return $null;
  } catch(...) {
    jclass excep = jenv->FindClass("java/lang/Exception");
    jenv->ThrowNew(excep, "Unknown error");
    return $null;
  }
}*/

/*%javaexception("java.lang.Exception") Graph::readLock {
  try {
     $action
  } catch (const ReadLockException& e) {
    jclass clazz = jenv->FindClass("java/lang/Exception");
    jenv->ThrowNew(clazz, e.what());
    return $null;
   }
}*/

%typemap(out) unsigned int &INPUT %{ $result = (jlong)$1; %}
%typemap(out) unsigned int *INPUT %{ $result = (jlong)*$1; %}
%typemap(javaout) unsigned int &INPUT { return $jnicall; }
%typemap(javaout) unsigned int *INPUT { return $jnicall; }


namespace std {
  %template(StringVector) std::vector<std::string>;
}


/* Let's just grab the original header file here */
/* main */
%include "../../libgexf/typedefs.h"
%include "../../libgexf/exceptions.h"
%include "../../libgexf/gexf.h"
%include "../../libgexf/abstractiter.h"
%include "../../libgexf/memoryvalidator.h"
/* io::input */
%include "../../libgexf/filereader.h"
%include "../../libgexf/abstractparser.h"
%include "../../libgexf/gexfparser.h"
%include "../../libgexf/legacyparser.h"
%include "../../libgexf/rngvalidator.h"
%include "../../libgexf/schemavalidator.h"
/* io::output */
%include "../../libgexf/filewriter.h"
%include "../../libgexf/legacywriter.h"
/* io::utils */
%include "../../libgexf/conv.h"
/* db::topo */
%include "../../libgexf/graph.h"
%include "../../libgexf/directedgraph.h"
%include "../../libgexf/undirectedgraph.h"
%include "../../libgexf/nodeiter.h"
%include "../../libgexf/edgeiter.h"
/* db::data */
%include "../../libgexf/data.h"
%include "../../libgexf/metadata.h"
%include "../../libgexf/attributeiter.h"
%include "../../libgexf/attvalueiter.h"


