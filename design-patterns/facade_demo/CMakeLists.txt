# 最低 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称和语言
project(facade_demo LANGUAGES C)

# 添加可执行文件
add_executable(facade_demo
    main.c
    src/home_theater_facade.c
    src/projector.c
    src/amplifier.c
    src/dvd_player.c
)

# 设置头文件搜索路径
target_include_directories(facade_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include    # 公共 API
    ${CMAKE_SOURCE_DIR}/internal   # 内部头文件
)