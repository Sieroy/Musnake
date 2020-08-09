cmake_minimum_required(VERSION 3.0.0)
project(Snake VERSION 0.1.0)
include(CTest)
enable_testing()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(SDL2_INCLUDE_DIR 
    "D:/lib/SDL2-2.0.10/include" 
    "D:/lib/SDL2_image-2.0.5/include" 
    "D:/lib/SDL2_ttf-2.0.15/include" 
    "D:/lib/SDL2_mixer-2.0.4/include"
    "./include")

set(SDL2_LIB 
    "D:/lib/SDL2-2.0.10/lib/x86/SDL2.lib" 
    "D:/lib/SDL2-2.0.10/lib/x86/SDL2main.lib"
    "D:/lib/SDL2_image-2.0.5/lib/x86/SDL2_image.lib"
    "D:/lib/SDL2_ttf-2.0.15/lib/x86/SDL2_ttf.lib"
    "D:/lib/SDL2_mixer-2.0.4/lib/x86/SDL2_mixer.lib"
    )

include_directories(${SDL2_INCLUDE_DIR})

add_executable(main 
    main.cpp )
target_link_libraries(main ${SDL2_LIB})