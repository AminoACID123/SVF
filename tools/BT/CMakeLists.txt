set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(DEFINED IN_SOURCE_BUILD)
    set(LLVM_LINK_COMPONENTS BitWriter Core IPO IrReader InstCombine Instrumentation Target Linker Analysis ScalarOpts Support Svf)
    add_llvm_tool( bt bt.cpp )
else()
    add_executable( bt bt.cpp )

    target_link_libraries( bt Svf ${llvm_libs} Threads::Threads )

    set_target_properties( bt PROPERTIES
                           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
endif()


