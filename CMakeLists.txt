cmake_minimum_required(VERSION 3.19)
project(DSL)

set(CMAKE_CXX_STANDARD 14)
aux_source_directory(./ main_srcs)
aux_source_directory(./DSL dsl_srcs)
aux_source_directory(./DSL/InterpretorImplements dsl_interpretor_srcs)
aux_source_directory(./DSL/FrontEnd dsl_frontend_srcs)

include_directories(${CMAKE_SOURCE_DIR}/.)

add_executable(DSL ${main_srcs} ${dsl_srcs} ${dsl_interpretor_srcs} ${dsl_frontend_srcs})