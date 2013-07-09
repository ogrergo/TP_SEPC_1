cmake_minimum_required(VERSION 2.4)
project(Allocphy)
enable_testing()
add_definitions(-Wall -Werror)

#########
# Gestion des variantes
#########
# Vous devez editer la ligne suivante pour y mettre vos logins
#########
set(VARIANTE_LOGINS login1 login2 login)

###

list(SORT VARIANTE_LOGINS)
string(SHA512 VARIANTE_SHA "${VARIANTE_LOGINS}")
if (${VARIANTE_SHA} STREQUAL "740444f437149a43d9f494c09ceec14f974517a6886da494619e4ed1c6530518b36f4b36615346ee845743ab072be382b3d4308b8a864b3bdd17406c69fc9a43" )
  message(FATAL_ERROR "** ERREUR **: Vous devez modifier CMakeLists.txt pour y mettre vos logins")
endif()
execute_process(COMMAND echo ${VARIANTE_SHA} COMMAND tr "a-f" "A-F" COMMAND xargs printf "16 i 10 o %s 3 %% p" COMMAND tee sauv.txt COMMAND dc OUTPUT_VARIABLE VARIANTE_SUJET)

configure_file (
  src/variante.h.in
  ${CMAKE_SOURCE_DIR}/src/variante.h
)

#########
# Fin de gestion des variantes
#########

#######
# Compilation standard avec googletest
#######


###
# sur Debian et al, les googletests sont à recompiler
# sinon, on utilise l'autodetection de Cmake
###
if(EXISTS /usr/src/gtest)
    add_subdirectory(/usr/src/gtest ./gtest)
else(EXISTS /usr/src/gtest)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
endif(EXISTS /usr/src/gtest)

##
# Si vous utilisé plusieurs fichiers, en plus de mem.c, il faut les
# ajouter ici
##
add_library(liballocphy STATIC src/mem.c)


add_executable(alloctest src/alloctest.cc tests/test_bf.cc tests/test_cff.cc  tests/test_buddy.cc tests/test_generic.cc tests/test_run_cpp.cc)
target_link_libraries(alloctest gtest gtest_main liballocphy)
add_test(AllTestsAllocator alloctest)

