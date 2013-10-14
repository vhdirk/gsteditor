

set(element_browser_HEADERS browser.h
                   element-tree.h
                   caps-tree.h
                   )
                   

set(element_browser_SOURCES browser.cpp
                    element-tree.cpp
                    caps-tree.cpp
)


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


source_group("src" FILES ${element_browser_SOURCES})
source_group("include" FILES ${element_browser_HEADERS})



add_gsteditor_source(${element_browser_SOURCES})
                   
                   
                   



