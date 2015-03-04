cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

ADD_LATEX_DOCUMENT(PalachevSupportMethods.tex
    IMAGE_DIRS images
    BIBFILES 307167.bib 81035.bib josaa-9-10-1693.bib
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
)
