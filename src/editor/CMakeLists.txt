

set(editor_HEADERS editor.h
                   editorcanvas.h
                   editorproperty.h
                   #element-tree.h
                   )
                   

set(editor_SOURCES editor.cpp
                   editorcanvas.cpp
                   editorproperty.cpp
                    #element-tree.cpp
)


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)


source_group("src" FILES ${editor_SOURCES})
source_group("include" FILES ${editor_HEADERS})



add_source_group(${editor_SOURCES} GROUP "editor")
                   
                   
                   



