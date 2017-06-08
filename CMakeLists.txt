cmake_minimum_required(VERSION 3.7)
project(MiniSQL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        recordManager/testRecordMgr.cpp
        recordManager/RecordManager.cpp
        recordManager/RecordManager.h

#        record+catalog+buffer/main.cpp
        record+catalog+buffer/table.h
        record+catalog+buffer/catalogmanager.h
        record+catalog+buffer/catalogmanager.cpp
        record+catalog+buffer/buffermanager.h
        record+catalog+buffer/buffermanager.cpp
        record+catalog+buffer/defination.h
        record+catalog+buffer/IndexManager.cpp
        record+catalog+buffer/IndexManager.h
        )
add_executable(MiniSQL ${SOURCE_FILES})