function(define_test prefix)
    configure_file(${prefix}.out ${prefix}.out)
    configure_file(${prefix}.lox ${prefix}.lox)
    add_test(NAME ${prefix} COMMAND bash -c "$<TARGET_FILE:cpplox> ${prefix}.lox 2>&1 | tee ${prefix}.run; diff ${prefix}.out ${prefix}.run")
endfunction()


file(GLOB input_files CONFIGURE_DEPENDS *.lox)
foreach(file ${input_files})
    get_filename_component(prefix ${file} NAME_WE)
    message(STATUS "Defining test ${prefix}")
    define_test(${prefix})
endforeach()
