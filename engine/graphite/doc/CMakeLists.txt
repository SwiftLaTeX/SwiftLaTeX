project(docs)

set(GRAPHITE2_DOXYGEN_CONFIG public CACHE STRING "Specifies the base configuration file for doxygen. Current values: public, all")

find_program(A2X a2x)
find_program(DBLATEX dblatex)
find_program(DOXYGEN doxygen)
find_program(LATEX pdflatex)

set(DOC_DEPENDS)
if(A2X)
    if(DBLATEX)
        add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/manual.pdf
                           DEPENDS ${PROJECT_SOURCE_DIR}/[a-z]*.asc
                           COMMAND ${A2X} --icons -D ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/manual.asc)
        add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/GTF.pdf
                           DEPENDS ${PROJECT_SOURCE_DIR}/GTF.asc
                                   ${PROJECT_SOURCE_DIR}/OpCodes.asc
                           COMMAND ${A2X} --icons -D ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/GTF.asc)
        set(DOC_DEPENDS ${PROJECT_BINARY_DIR}/manual.pdf ${PROJECT_BINARY_DIR}/GTF.pdf)
    endif()
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/manual.html
                       DEPENDS ${PROJECT_SOURCE_DIR}/[a-z]*.asc ${PROJECT_SOURCE_DIR}/graphite.css
                       COMMAND ${A2X} -f xhtml --stylesheet=graphite.css -D ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/manual.asc)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/GTF.html
                       DEPENDS ${PROJECT_SOURCE_DIR}/GTF.asc
                               ${PROJECT_SOURCE_DIR}/OpCodes.asc
                               ${PROJECT_SOURCE_DIR}/graphite.css
                       COMMAND ${A2X} -f xhtml --stylesheet=graphite.css -D ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/GTF.asc)
    set(DOC_DEPENDS ${DOC_DEPENDS} ${PROJECT_BINARY_DIR}/manual.html ${PROJECT_BINARY_DIR}/GTF.html)
endif()

if(DOXYGEN AND LATEX)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doxygen/latex/refman.pdf
                        DEPENDS ${PROJECT_SOURCE_DIR}/../include/graphite2/*.h
                        MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/${GRAPHITE2_DOXYGEN_CONFIG}.cfg
                        COMMAND ${DOXYGEN} ${PROJECT_SOURCE_DIR}/${GRAPHITE2_DOXYGEN_CONFIG}.cfg
                        COMMAND make -C ${PROJECT_BINARY_DIR}/doxygen/latex)
    set(DOC_DEPENDS ${DOC_DEPENDS} ${PROJECT_BINARY_DIR}/doxygen/latex/refman.pdf)
endif()

add_custom_target(docs DEPENDS ${DOC_DEPENDS})
