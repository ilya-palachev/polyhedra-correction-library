import java.util.Set;
import java.util.Vector;
import java.lang.String;


public class runme {
  static {
    try {
//        System.loadLibrary("libgexf");

        // Linux hack, if you can't get your library
        // path set in your environment
        System.load("/home/sebastien/NetBeansProjects/libgexf/binding/java/libgexf.so");
    } catch (UnsatisfiedLinkError e) {
        System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
        System.exit(1);
    }
  }

  public static void main(String argv[]) {
        // ----- Object creation -----
        System.out.println( "Creating some objects:" );
        org.gephi.libgexf.GEXF gexf = new org.gephi.libgexf.GEXF();
        System.out.println( "    Created gexf " + gexf );
        org.gephi.libgexf.DirectedGraph graph = gexf.getDirectedGraph();
        
        // ----- Generate data -----
        graph.writeLock();
        for (int i=0; i<10; i++) {
            graph.addNode(""+i);
        }
        for (int i=0; i<9; i++) {
            graph.addEdge(""+i, ""+i, ""+(i+1)); // chain
        }
        try {
            graph.readLock();
        } catch (Exception e) {
            System.err.println("Assert readLock exception:\n" + e);
        }
        graph.writeUnlock();
        
        
        // ----- Member data access -----
        System.out.println("node count: " + graph.getNodeCount());
        System.out.println("edge count: " + graph.getEdgeCount());
        
        org.gephi.libgexf.StringVector neighbors = graph.getNeighbors("3");
        System.out.println("Neighbors vector empty: " + neighbors.isEmpty());
        System.out.print("Neighbors content: ");
        for(int i = 0; i < neighbors.size(); i++) {
           System.out.print(neighbors.get(i) + " ");
        }
        System.out.println();
        
        
        // ----- Call some methods -----
        System.out.println("Data integrity (labels missing):");
        gexf.checkIntegrity();
        
        
        // ----- Delete everything -----
    
        // Note: this invokes the virtual destructor
        // You could leave this to the garbage collector
        gexf.delete();
        
        
        // ----- Import a GEXF file -----
        org.gephi.libgexf.FileReader reader = new org.gephi.libgexf.FileReader();
        reader.init( "../../t/attributes.gexf" );
        reader.slurp();
        org.gephi.libgexf.GEXF gexfImport = reader.getGEXFCopy();
        
        
        // ----- Access imported data -----
        System.out.print("Imported nodes: ");
        org.gephi.libgexf.NodeIter it = gexfImport.getDirectedGraph().getNodes();
        while( it.hasNext() ) {
            String id = it.next();
            System.out.print(id + " ");
        }
        System.out.println();
        
        // ----- Export a GEXF file -----
        if( gexfImport.checkIntegrity() ) {
            org.gephi.libgexf.FileWriter writer = new org.gephi.libgexf.FileWriter();
            writer.init("/home/sebastien/NetBeansProjects/libgexf/t/writer_test.gexf", gexfImport); // require an absolute path
            writer.write();
        } else {
            System.out.println("Errors in data, file not written.");
        }
        
        
        // ----- XSD validate a GEXF file -----
        boolean res = org.gephi.libgexf.SchemaValidator.run( "../../t/attributes.gexf", "../../resources/xsd/gexf.xsd");
        System.out.println("XSD valid: " + res);
        
        
        System.out.println( "Goodbye" );
  }
}

