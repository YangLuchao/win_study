cmake_minimum_required(VERSION 3.21)
project(win_study)

set(CMAKE_CXX_STANDARD 11)

# 添加 Windows GUI 相关库
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

# 添加源文件
set(SOURCE_FILES win/11/06/PopFile.c win/11/06/PopFind.c win/11/06/PopFont.c win/11/06/PopPad.c win/11/06/PopPrnt0.c win/11/06/POPPAD.RC)
# 生成可执行文件
add_executable(win_study WIN32 ${SOURCE_FILES})
