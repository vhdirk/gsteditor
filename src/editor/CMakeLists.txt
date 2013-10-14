

set(editor_HEADERS editor.h
                   gsteditor.h
                   #caps-tree.h
                   #element-tree.h
                   )
                   

set(editor_SOURCES gsteditor.cpp
                    #caps-tree.cpp
                    #element-tree.cpp
)


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)


source_group("src" FILES ${editor_SOURCES})
source_group("include" FILES ${editor_HEADERS})



add_gsteditor_source(${editor_SOURCES})
                   
                   
                   



