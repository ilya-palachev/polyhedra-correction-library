cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

ADD_LATEX_DOCUMENT(PalachevSupportMethods.tex
    IMAGE_DIRS images
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
)
