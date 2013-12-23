cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

ADD_LATEX_DOCUMENT(i.palachev_ReportAboutReconstruction.tex
    IMAGE_DIRS images
    BIBFILES references.bib
    DEFAULT_PDF
)
