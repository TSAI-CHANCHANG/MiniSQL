cmake_minimum_required(VERSION 3.7)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES table.h runtimeexception.h buffermanager.cpp buffermanager.h catalogmanager.cpp catalogmanager.h Interpret.cpp Interpreter.h RecordManager.cpp RecordManager.h)
add_executable(SampleMiniSQL ${SOURCE_FILES})