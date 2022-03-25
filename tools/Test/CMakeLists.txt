
if(DEFINED IN_SOURCE_BUILD)
    set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation Target Linker Analysis ScalarOpts Support Svf Cudd)
    add_llvm_tool( test test.cpp )
else()
    add_executable( test test.cpp )

    target_link_libraries( test Svf Cudd ${llvm_libs} ${Boost_LIBRARIES})

    set_target_properties( test PROPERTIES
                           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin 
                           COMPILE_OPTIONS "-frtti")
endif()
