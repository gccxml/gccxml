PROJECT(GAT C)

SET(GAT_AS as)

FOREACH(test weak)
  ADD_CUSTOM_TARGET(test_${test} echo DEPENDS ${GAT_BINARY_DIR}/${test}.o)
  ADD_CUSTOM_COMMAND(OUTPUT ${GAT_BINARY_DIR}/${test}.o
    COMMAND ${GAT_AS}
    ARGS -o
    "${GAT_BINARY_DIR}/${test}.o"
    "${GAT_SOURCE_DIR}/${test}.s"
    DEPENDS ${GAT_SOURCE_DIR}/${test}.s
    )
ENDFOREACH(test)
