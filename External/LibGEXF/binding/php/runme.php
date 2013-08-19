<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<!-- Hand crafted HTML -->
<html>
<head>
</head>
<body>
<?php

echo "<h2>Loading</h2>";
include("libgexf.php");

$ext_loaded = extension_loaded('libgexf');
$ext_loaded_str = "<span style='color:red ;'><b>not loaded</b></span>";
if($ext_loaded) {
	$ext_loaded_str = "<span style='color:green'>loaded</span>";
}
echo "Libgexf status: $ext_loaded_str<br />";


echo "<h2>Testing</h2>";
try {
	echo "Object creation..<br />";
	$gexf = new GEXF();
	$graph = $gexf->getDirectedGraph();
	$data = $gexf->getData();


	echo "Data generation..<br />";
	$data->addNodeAttributeColumn("0", "title_0", "INTEGER");
	$data->setNodeAttributeDefault("0", "1");
	
	for ($i=0; $i<10; $i++) {
		$graph->addNode("$i");
		//$data->setNodeLabel("$i", "label_$i");
		$value = $i*$i;
		$data->setNodeValue("$i", "0", "$value");
	}

	for ($i=0; $i<9; $i++) {
		$target = $i+1;
		$graph->addEdge("$i", "$i", "$target");
	}


	echo "Member data access..<br />";
	echo "node count: " . $graph->getNodeCount() . "<br />";
	echo "edge count: " . $graph->getEdgeCount() . "<br />";

	$neighbors = $graph->getNeighbors("3");
	echo "Neighbors content: ";
	for ($i=0; $i < $neighbors->size(); $i++) {
		echo $neighbors->get($i) . " ; ";
	}
	echo "<br />";


	echo "Call some methods..<br />";
	echo "Data integrity (should fail):<br />";
	$ok = $gexf->checkIntegrity();
	$ok_str = ($ok) ? "yes" : "no";
	echo "Data ok = $ok_str<br />";
	if(!$ok) {
		echo "<span style='color:red ;'><b>C++ cerr messages aren't handled yet. Integrity report will be available in a future version.</b></span><br />";
	}


	echo "Import a GEXF file..<br />";
	$reader = new FileReader();
	$reader->init( "attributes.gexf" );
	$reader->slurp();
	$gexfImport = $reader->getGEXFCopy();
	echo "done.<br />";

	echo "Access imported data..<br />";
	echo "Imported nodes: ";
	$it = $gexfImport->getDirectedGraph()->getNodes();
	while( $it->hasNext() ) {
		$id = $it->next();
		echo $id . " ; ";
	}
	echo "<br />";


	echo "Export a GEXF file..<br />";
	if( $gexfImport->checkIntegrity() ) {
		$writer = new FileWriter();
		$filepath = dirname(__FILE__)."/test.gexf";
		echo "filepath: $filepath<br />";
		$writer->init($filepath, $gexfImport); // require an absolute path
		$writer->write();
		echo "done.<br />";
	} else {
		echo "<span style='color:red ;'><b>Errors in data, file not written.</b></span><br />";
	}
} catch (Exception $e) {
	echo 'Exception thrown : ',  $e->getMessage(), "<br />";
}

?>
</body>
</html>
