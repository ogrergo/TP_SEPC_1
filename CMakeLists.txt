cmake_minimum_required(VERSION 2.6)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(Allocphy)
enable_testing()
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -std=gnu99")

#########
# Gestion des variantes
#########
# Vous devez editer la ligne suivante pour y mettre vos logins
# et le numéro de la variante que vous avez choisi
#########
set(VARIANTE_LOGINS login1 login2 login3)
set(VARIANTE_SUJET -1)
###

list(SORT VARIANTE_LOGINS)
if (VARIANTE_LOGINS MATCHES "login[123]")
  message(FATAL_ERROR "** ERREUR **: Vous devez modifier CMakeLists.txt pour y mettre vos logins")
endif()
if (VARIANTE_SUJET EQUAL -1)
  message(FATAL_ERROR "** ERREUR **: Vous devez modifier CMakeLists.txt pour y mettre le numéro de votre variante du sujet")
endif()

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
# sur centOS6, c'est une bibliothèque avec son équivalent de pkg-config
# sinon, on utilise l'autodetection de Cmake
###
if(EXISTS /usr/src/gtest)
    add_subdirectory(/usr/src/gtest ./gtest)
else(EXISTS /usr/src/gtest)
  if(EXISTS /usr/bin/gtest-config)
    execute_process(COMMAND gtest-config --cppflags --cxxflags OUTPUT_VARIABLE CENTOS_GTEST_CFLAGS)
    execute_process(COMMAND gtest-config --ldflags --libs OUTPUT_VARIABLE CENTOS_GTEST_LDFLAGS)
    string(STRIP "${CENTOS_GTEST_CFLAGS}" CENTOS_GTEST_CFLAGS)
    string(STRIP "${CENTOS_GTEST_LDFLAGS}" CENTOS_GTEST_LDFLAGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CENTOS_GTEST_CFLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CENTOS_GTEST_LDFLAGS} -lgtest_main")
  else(EXISTS /usr/bin/gtest-config)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
  endif(EXISTS /usr/bin/gtest-config)
endif(EXISTS /usr/src/gtest)

##
# Si vous utilisé plusieurs fichiers, en plus de mem.c, pour votre
# allocateur il faut les ajouter ici
##
add_library(allocphy SHARED src/mem.c)

##
# Construction du programme de tests unitaires
##
add_executable(alloctest src/alloctest.cc tests/test_bf.cc tests/test_cff.cc  tests/test_buddy.cc tests/test_generic.cc tests/test_run_cpp.cc)
target_link_libraries(alloctest gtest gtest_main allocphy)
add_test(AllTestsAllocator alloctest)

##
# Ajout d'une cible pour lancer les tests de manière verbeuse
##
add_custom_target(check alloctest)

add_executable(memshell src/memshell.c)
target_link_libraries(memshell allocphy)

##
# Construction de l'archive
##
string(REPLACE ";" "_" LOGINS_SANS_POINTVIRGULE "${VARIANTE_LOGINS}")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH ${LOGINS_SANS_POINTVIRGULE})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "\\\\.o$"
  "^${PROJECT_SOURCE_DIR}/build/"
  "^${PROJECT_SOURCE_DIR}/.git/"
)
include(CPack)
