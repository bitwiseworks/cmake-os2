set(var "CMAKE_Fortran_COMPILE_OPTIONS_MSVC_DEBUG_INFORMATION_FORMAT_Embedded")
string(REPLACE "-Z7" "-Z7;-DTEST_Z7" "${var}" "${${var}}")
set(var "CMAKE_Fortran_COMPILE_OPTIONS_MSVC_DEBUG_INFORMATION_FORMAT_ProgramDatabase")
string(REPLACE "-Zi" "-Zi;-DTEST_Zi" "${var}" "${${var}}")
