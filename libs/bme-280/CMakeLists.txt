cmake_minimum_required(VERSION 3.15)

project(bme-280)

add_library(${PROJECT_NAME} STATIC
	"src/bme-280.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"include"
)