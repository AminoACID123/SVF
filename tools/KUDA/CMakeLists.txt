
if(DEFINED IN_SOURCE_BUILD)
    set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation Target Linker Analysis ScalarOpts Support Svf Cudd)
    add_llvm_tool( kuda kuda.cpp )
else()
    add_executable( kuda kuda.cpp )

    target_link_libraries( kuda Svf Cudd ${llvm_libs} )

    set_target_properties( kuda PROPERTIES
                           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif()


