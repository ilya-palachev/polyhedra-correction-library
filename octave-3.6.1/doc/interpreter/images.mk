IMAGES_SRC =
HTMLDIR_IMAGES =
IMAGES_EPS =
IMAGES_PDF =
IMAGES_PNG =
IMAGES_TXT =
IMAGES_SRC += geometryimages.m
GEOMETRYIMAGES_EPS = voronoi.eps triplot.eps griddata.eps convhull.eps delaunay.eps inpolygon.eps
IMAGES_EPS += $(GEOMETRYIMAGES_EPS)
voronoi.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('voronoi', 'eps');"
triplot.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('triplot', 'eps');"
griddata.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('griddata', 'eps');"
convhull.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('convhull', 'eps');"
delaunay.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('delaunay', 'eps');"
inpolygon.eps: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('inpolygon', 'eps');"
GEOMETRYIMAGES_PDF = voronoi.pdf triplot.pdf griddata.pdf convhull.pdf delaunay.pdf inpolygon.pdf
IMAGES_PDF += $(GEOMETRYIMAGES_PDF)
voronoi.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('voronoi', 'pdf');"
triplot.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('triplot', 'pdf');"
griddata.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('griddata', 'pdf');"
convhull.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('convhull', 'pdf');"
delaunay.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('delaunay', 'pdf');"
inpolygon.pdf: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('inpolygon', 'pdf');"
GEOMETRYIMAGES_PNG = voronoi.png triplot.png griddata.png convhull.png delaunay.png inpolygon.png
IMAGES_PNG += $(GEOMETRYIMAGES_PNG)
HTMLDIR_IMAGES +=  octave.html/voronoi.png octave.html/triplot.png octave.html/griddata.png octave.html/convhull.png octave.html/delaunay.png octave.html/inpolygon.png
octave.html/voronoi.png: voronoi.png octave.html/index.html
	cp $< $@
voronoi.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('voronoi', 'png');"
octave.html/triplot.png: triplot.png octave.html/index.html
	cp $< $@
triplot.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('triplot', 'png');"
octave.html/griddata.png: griddata.png octave.html/index.html
	cp $< $@
griddata.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('griddata', 'png');"
octave.html/convhull.png: convhull.png octave.html/index.html
	cp $< $@
convhull.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('convhull', 'png');"
octave.html/delaunay.png: delaunay.png octave.html/index.html
	cp $< $@
delaunay.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('delaunay', 'png');"
octave.html/inpolygon.png: inpolygon.png octave.html/index.html
	cp $< $@
inpolygon.png: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('inpolygon', 'png');"
GEOMETRYIMAGES_TXT = voronoi.txt triplot.txt griddata.txt convhull.txt delaunay.txt inpolygon.txt
IMAGES_TXT += $(GEOMETRYIMAGES_TXT)
voronoi.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('voronoi', 'txt');"
triplot.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('triplot', 'txt');"
griddata.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('griddata', 'txt');"
convhull.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('convhull', 'txt');"
delaunay.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('delaunay', 'txt');"
inpolygon.txt: geometryimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "geometryimages ('inpolygon', 'txt');"
IMAGES_SRC += interpimages.m
INTERPIMAGES_EPS = interpft.eps interpn.eps interpderiv1.eps interpderiv2.eps
IMAGES_EPS += $(INTERPIMAGES_EPS)
interpft.eps: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpft', 'eps');"
interpn.eps: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpn', 'eps');"
interpderiv1.eps: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv1', 'eps');"
interpderiv2.eps: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv2', 'eps');"
INTERPIMAGES_PDF = interpft.pdf interpn.pdf interpderiv1.pdf interpderiv2.pdf
IMAGES_PDF += $(INTERPIMAGES_PDF)
interpft.pdf: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpft', 'pdf');"
interpn.pdf: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpn', 'pdf');"
interpderiv1.pdf: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv1', 'pdf');"
interpderiv2.pdf: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv2', 'pdf');"
INTERPIMAGES_PNG = interpft.png interpn.png interpderiv1.png interpderiv2.png
IMAGES_PNG += $(INTERPIMAGES_PNG)
HTMLDIR_IMAGES +=  octave.html/interpft.png octave.html/interpn.png octave.html/interpderiv1.png octave.html/interpderiv2.png
octave.html/interpft.png: interpft.png octave.html/index.html
	cp $< $@
interpft.png: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpft', 'png');"
octave.html/interpn.png: interpn.png octave.html/index.html
	cp $< $@
interpn.png: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpn', 'png');"
octave.html/interpderiv1.png: interpderiv1.png octave.html/index.html
	cp $< $@
interpderiv1.png: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv1', 'png');"
octave.html/interpderiv2.png: interpderiv2.png octave.html/index.html
	cp $< $@
interpderiv2.png: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv2', 'png');"
INTERPIMAGES_TXT = interpft.txt interpn.txt interpderiv1.txt interpderiv2.txt
IMAGES_TXT += $(INTERPIMAGES_TXT)
interpft.txt: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpft', 'txt');"
interpn.txt: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpn', 'txt');"
interpderiv1.txt: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv1', 'txt');"
interpderiv2.txt: interpimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "interpimages ('interpderiv2', 'txt');"
IMAGES_SRC += plotimages.m
PLOTIMAGES_EPS = plot.eps hist.eps errorbar.eps polar.eps mesh.eps plot3.eps extended.eps
IMAGES_EPS += $(PLOTIMAGES_EPS)
plot.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot', 'eps');"
hist.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('hist', 'eps');"
errorbar.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('errorbar', 'eps');"
polar.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('polar', 'eps');"
mesh.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('mesh', 'eps');"
plot3.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot3', 'eps');"
extended.eps: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('extended', 'eps');"
PLOTIMAGES_PDF = plot.pdf hist.pdf errorbar.pdf polar.pdf mesh.pdf plot3.pdf extended.pdf
IMAGES_PDF += $(PLOTIMAGES_PDF)
plot.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot', 'pdf');"
hist.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('hist', 'pdf');"
errorbar.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('errorbar', 'pdf');"
polar.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('polar', 'pdf');"
mesh.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('mesh', 'pdf');"
plot3.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot3', 'pdf');"
extended.pdf: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('extended', 'pdf');"
PLOTIMAGES_PNG = plot.png hist.png errorbar.png polar.png mesh.png plot3.png extended.png
IMAGES_PNG += $(PLOTIMAGES_PNG)
HTMLDIR_IMAGES +=  octave.html/plot.png octave.html/hist.png octave.html/errorbar.png octave.html/polar.png octave.html/mesh.png octave.html/plot3.png octave.html/extended.png
octave.html/plot.png: plot.png octave.html/index.html
	cp $< $@
plot.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot', 'png');"
octave.html/hist.png: hist.png octave.html/index.html
	cp $< $@
hist.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('hist', 'png');"
octave.html/errorbar.png: errorbar.png octave.html/index.html
	cp $< $@
errorbar.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('errorbar', 'png');"
octave.html/polar.png: polar.png octave.html/index.html
	cp $< $@
polar.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('polar', 'png');"
octave.html/mesh.png: mesh.png octave.html/index.html
	cp $< $@
mesh.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('mesh', 'png');"
octave.html/plot3.png: plot3.png octave.html/index.html
	cp $< $@
plot3.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot3', 'png');"
octave.html/extended.png: extended.png octave.html/index.html
	cp $< $@
extended.png: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('extended', 'png');"
PLOTIMAGES_TXT = plot.txt hist.txt errorbar.txt polar.txt mesh.txt plot3.txt extended.txt
IMAGES_TXT += $(PLOTIMAGES_TXT)
plot.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot', 'txt');"
hist.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('hist', 'txt');"
errorbar.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('errorbar', 'txt');"
polar.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('polar', 'txt');"
mesh.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('mesh', 'txt');"
plot3.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('plot3', 'txt');"
extended.txt: plotimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "plotimages ('extended', 'txt');"
IMAGES_SRC += sparseimages.m
SPARSEIMAGES_EPS = gplot.eps grid.eps spmatrix.eps spchol.eps spcholperm.eps
IMAGES_EPS += $(SPARSEIMAGES_EPS)
gplot.eps: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('gplot', 'eps');"
grid.eps: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('grid', 'eps');"
spmatrix.eps: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spmatrix', 'eps');"
spchol.eps: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spchol', 'eps');"
spcholperm.eps: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spcholperm', 'eps');"
SPARSEIMAGES_PDF = gplot.pdf grid.pdf spmatrix.pdf spchol.pdf spcholperm.pdf
IMAGES_PDF += $(SPARSEIMAGES_PDF)
gplot.pdf: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('gplot', 'pdf');"
grid.pdf: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('grid', 'pdf');"
spmatrix.pdf: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spmatrix', 'pdf');"
spchol.pdf: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spchol', 'pdf');"
spcholperm.pdf: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spcholperm', 'pdf');"
SPARSEIMAGES_PNG = gplot.png grid.png spmatrix.png spchol.png spcholperm.png
IMAGES_PNG += $(SPARSEIMAGES_PNG)
HTMLDIR_IMAGES +=  octave.html/gplot.png octave.html/grid.png octave.html/spmatrix.png octave.html/spchol.png octave.html/spcholperm.png
octave.html/gplot.png: gplot.png octave.html/index.html
	cp $< $@
gplot.png: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('gplot', 'png');"
octave.html/grid.png: grid.png octave.html/index.html
	cp $< $@
grid.png: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('grid', 'png');"
octave.html/spmatrix.png: spmatrix.png octave.html/index.html
	cp $< $@
spmatrix.png: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spmatrix', 'png');"
octave.html/spchol.png: spchol.png octave.html/index.html
	cp $< $@
spchol.png: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spchol', 'png');"
octave.html/spcholperm.png: spcholperm.png octave.html/index.html
	cp $< $@
spcholperm.png: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spcholperm', 'png');"
SPARSEIMAGES_TXT = gplot.txt grid.txt spmatrix.txt spchol.txt spcholperm.txt
IMAGES_TXT += $(SPARSEIMAGES_TXT)
gplot.txt: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('gplot', 'txt');"
grid.txt: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('grid', 'txt');"
spmatrix.txt: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spmatrix', 'txt');"
spchol.txt: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spchol', 'txt');"
spcholperm.txt: sparseimages.m
	$(top_builddir)/run-octave -f -q -H -p $(srcdir) --eval "sparseimages ('spcholperm', 'txt');"
