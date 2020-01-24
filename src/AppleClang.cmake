# find libomp directory
execute_process(
    COMMAND brew --prefix libomp
    OUTPUT_VARIABLE LIBOMP_DIR
)
string(REGEX REPLACE "\n$" "" LIBOMP_DIR "${LIBOMP_DIR}")

# tell Apple Clang to use openmp
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_DIR}/include")
set(OpenMP_C_LIB_NAMES "omp")

set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_DIR}/include")
set(OpenMP_CXX_LIB_NAMES "omp")

set(OpenMP_omp_LIBRARY "${LIBOMP_DIR}/lib/libomp.a")


