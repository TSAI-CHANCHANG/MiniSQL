cmake_minimum_required(VERSION 3.7)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        recordManager/testRecordMgr.cpp
        recordManager/RecordManager.cpp
        recordManager/RecordManager.h

#        record+catalog+buffer/main.cpp
        index+catalog+buffer/table.h
        index+catalog+buffer/catalogmanager.h
        index+catalog+buffer/catalogmanager.cpp
        index+catalog+buffer/buffermanager.h
        index+catalog+buffer/buffermanager.cpp
        index+catalog+buffer/defination.h
        index+catalog+buffer/IndexManager.cpp
        index+catalog+buffer/IndexManager.h
        )
add_executable(MiniSQL ${SOURCE_FILES})