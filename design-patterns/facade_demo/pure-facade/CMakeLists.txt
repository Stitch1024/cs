cmake_minimum_required(VERSION 3.10)
project(pure_facade LANGUAGES C)

add_executable(pure_facade
    main.c
    src/amplifier.c
    src/dvd_player.c
    src/projector.c
    src/home_theater_facade.c    
)

target_include_directories(pure_facade PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_options(pure_facade PRIVATE -Wall -Wextra)
