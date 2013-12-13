cmake_minimum_required(VERSION 2.8)

include(UseLATEX)

file(GLOB_RECURSE BIBFILES_report "*.bib")

set(BIBFILES_NAMES_report "")
foreach(bib_file_path ${BIBFILES_report})
	set(bib_file_name "")
	get_filename_component(bib_file_name ${bib_file_path} NAME)
	list(APPEND BIBFILES_NAMES_report ${bib_file_name})
endforeach()

ADD_LATEX_DOCUMENT(i.palachev_ReportAboutReconstruction.tex
    BIBFILES ${BIBFILES_NAMES_report}
    DEFAULT_PDF
)
