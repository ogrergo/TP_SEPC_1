cmake_minimum_required (VERSION 2.6)

# remplacer login1 login2 login3 par les logins des binomes
# l'ordre est sans importance
set(VARIANTE_LOGINS login1 login2 login3)


list(SORT VARIANTE_LOGINS)
string(SHA512 VARIANTE_SHA "${VARIANTE_LOGINS}")
if (${VARIANTE_SHA} STREQUAL "740444f437149a43d9f494c09ceec14f974517a6886da494619e4ed1c6530518b36f4b36615346ee845743ab072be382b3d4308b8a864b3bdd17406c69fc9a43" )
  message(FATAL_ERROR "** ERREUR **: Vous devez modifier CMakeLists.txt pour y mettre vos logins")
endif()
execute_process(COMMAND echo ${VARIANTE_SHA} COMMAND tr "a-f" "A-F" COMMAND xargs printf "16 i 10 o %s 3 %% p" COMMAND tee sauv.txt COMMAND dc OUTPUT_VARIABLE VARIANTE_SUJET)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES toto.h)
configure_file (
  variante.h.in
  variante.h
)