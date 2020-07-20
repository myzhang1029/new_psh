# Distclean.cmake: Clean all CMake-generated files
# Public domain
# Author Zhang Maiyun <myzhang1029@hotmail.com>

foreach(RUNNING_DIR ${subdirs})
    list(APPEND cmake_generated
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/psh-?.?.?-*
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/libpsh.pc
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/config.h
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/select_backend.c

        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/CMakeCache.txt
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/install_manifest.txt
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/compile_commands.json
        ${CMAKE_BINARY_DIR}/uninstall.cmake
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/Makefile
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/rules.ninja
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/build.ninja
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/.ninja_deps
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/.ninja_log
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/Testing
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/CTestTestfile.cmake
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/CPackConfig.cmake
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/CPackSourceConfig.cmake
        ${CMAKE_BINARY_DIR}/${RUNNING_DIR}/_CPack_Packages
    )

    foreach(file ${cmake_generated})
        file(GLOB file ${file})
        foreach(f ${file})
            if (EXISTS ${f})
                message(STATUS "Removing file " ${f})
                file(REMOVE_RECURSE ${f})
            endif()
        endforeach(f)
    endforeach(file)

    # if running dir is not . and is empty, delete it
    if (RUNNING_DIR AND NOT RUNNING_DIR EQUAL "."
        AND NOT RUNNING_DIR EQUAL "./"
        AND NOT RUNNING_DIR EQUAL "")
        string(CONCAT PATT ${RUNNING_DIR} "/*")
        file(GLOB RES ${PATT})
        list(LENGTH RES RES_LEN)
        if(RES_LEN EQUAL 0)
            file(REMOVE_RECURSE ${RUNNING_DIR})
        endif()
    endif()
endforeach(RUNNING_DIR)
