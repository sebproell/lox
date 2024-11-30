## Define a test for a given test input file and arguments of the lox interpreter
function(define_test prefix)
    configure_file(${prefix}.out ${prefix}.out)
    configure_file(${prefix}.lox ${prefix}.lox)
    set(args ${ARGN})
    # concat all args into name, first sort them, strip dahes and join with /
    list(SORT args)
    string(REPLACE "-" "" args_p "${args}")
    string(REPLACE ";" "/" args_p "${args_p}")
    set(test_name "${prefix} [${args_p}]")

    # args as string
    string(REPLACE ";" " " args "${args}")

    # diff ignoring whitespace
    add_test(NAME "${test_name}" COMMAND bash -c "$<TARGET_FILE:clox> ${args} ${prefix}.lox 2>&1 | tee ${prefix}.run; diff -b ${prefix}.out ${prefix}.run")
endfunction()


define_test("basic_arithmetic" --tokens --disassemble --trace_execution)
define_test("boolean_logic" --tokens --disassemble --trace_execution)

