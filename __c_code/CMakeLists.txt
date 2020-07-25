cmake_minimum_required(VERSION 3.16)
project(nkm C)

set(CMAKE_C_STANDARD 99)

add_executable(nkm src/main.c src/options.h src/version.h src/parser.c src/parser.h src/model.h src/vector.c src/vector.h src/runner.h src/runner.c src/debug.h src/stringbuilder.c src/stringbuilder.h)