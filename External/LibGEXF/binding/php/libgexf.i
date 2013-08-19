/*! \file : libgexf.i
    \brief SWIG interface
 */
%module(directors="1") libgexf

%ignore operator<<;
/*%nodefaultdtor;*/

/*%feature("director"); bad for performances*/

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

namespace std {
  %template(StringVector) std::vector<std::string>;
}

/* Exceptions */
%feature("director:except") {
    if ($error == FAILURE) {
        throw Swig::DirectorMethodException();
    }
}
%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
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


