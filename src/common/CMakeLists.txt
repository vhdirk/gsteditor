

set(common_HEADERS utils.h 
                   )
                   

set(common_SOURCES utils.cpp )


## from this project
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)


source_group("src" FILES ${common_SOURCES})
source_group("include" FILES ${common_HEADERS})



add_gsteditor_source(${common_SOURCES})
                   
                   
                   



