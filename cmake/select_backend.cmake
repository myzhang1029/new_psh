# Select backend
if (UNIX)
	add_subdirectory(backends/posix2)
else()
	message(FATAL_ERROR "Currently unsupported host system: " ${CMAKE_SYSTEM})
endif()
