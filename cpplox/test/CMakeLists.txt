## Define a test for a given test input file and an argument of the lox interpreter
function(define_test group prefix args)
    configure_file(${prefix}.out ${prefix}.out)
    configure_file(${prefix}.lox ${prefix}.lox)
    add_test(NAME "${group}/${prefix}" COMMAND bash -c "$<TARGET_FILE:cpplox> ${args} ${prefix}.lox 2>&1 | tee ${prefix}.run; diff ${prefix}.out ${prefix}.run")
endfunction()

add_subdirectory(ast)
add_subdirectory(interpret)
add_subdirectory(tokens)
