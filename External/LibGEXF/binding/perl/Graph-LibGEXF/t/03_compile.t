# see http://perldoc.perl.org/Test/More.html
#     http://search.cpan.org/~adie/Test-Exception-0.27/lib/Test/Exception.pm

use strict;
use Error qw(:try);
use Test::More tests => 1;
use Test::Exception;


BEGIN { use_ok 'Graph::LibGEXF' }

my $gexf = Graph::LibGEXF::GEXF->new();
my $graph = $gexf->getUndirectedGraph();
my $data = $gexf->getData();
$data->addEdgeAttributeColumn( "0", "Notice ID" );

