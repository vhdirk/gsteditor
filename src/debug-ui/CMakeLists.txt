

set(debug_ui_HEADERS debug-ui.h 
                   )
                   

set(debug_ui_SOURCES debug-ui.cpp )


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)


source_group("src" FILES ${debug_ui_SOURCES})
source_group("include" FILES ${debug_ui_HEADERS})



add_source_group(${debug_ui_SOURCES})
                   
                   
                   



