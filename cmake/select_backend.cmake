# Select backend
if (UNIX)
	add_subdirectory(backends/posix)
else()
	message(FATAL_ERROR "Currently unsupported host system: " ${CMAKE_SYSTEM})
endif()
