cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

ADD_LATEX_DOCUMENT(PalachevSupportMethods.tex
    IMAGE_DIRS images
    BIBFILES 307167.bib 81035.bib josaa-9-10-1693.bib 234054.bib 10.10022Fima.10007.bib 906585citation.bib 4586384.bib 4333.bib 141749.bib Gardner06convergenceof.bib Guntuboyina2012.bib cgal.bib how_to_cite_cgal.bib 235821.bib glpk.bib clp.bib ipopt.bib ibm-ilog-cplex.bib WachterB06.bib hsl.bib openblas.bib
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
)
