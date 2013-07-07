cmake_minimum_required (VERSION 2.6)
set(VARIANTE_LOGINS btoto atutu)
list(SORT VARIANTE_LOGINS)
string(SHA512 VARIANTE_SHA "${VARIANTE_LOGINS}")
execute_process(COMMAND echo ${VARIANTE_SHA} COMMAND tr "a-f" "A-F" COMMAND xargs printf "16 i 10 o %s 3 %% p" COMMAND tee sauv.txt COMMAND dc OUTPUT_VARIABLE VARIANTE_SUJET)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES toto.h)
configure_file (
  variante.h.in
  variante.h
)