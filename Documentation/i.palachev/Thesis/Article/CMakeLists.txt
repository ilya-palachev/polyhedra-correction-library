cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

ADD_LATEX_DOCUMENT(PalachevSupportMethods.tex
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
)
