#version minimum de cmake
cmake_minimum_required (VERSION 2.6)

#Déclaration du projet
project(ProGenWCMake)

#declaration de la bibliothèque pthread
set(CMAKE_C_FLAGS "-std=gnu99")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#Déclaration de l'executable
add_executable(
	ProGenWCMake
	main.c
	trains/train.c
	file/file.c
)

#ajout de la bibliothèque pthread à l'executable
target_link_libraries (ProGenWCMake ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (ProGenWCMake rt)
