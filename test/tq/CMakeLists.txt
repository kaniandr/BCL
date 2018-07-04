add_executable(tq-perf tq_perf.cpp)
target_link_libraries(tq-perf Core)
target_compile_options(tq-perf PRIVATE -O3)

include(CTest)

add_executable(tq-empty tq_empty.cpp)
target_link_libraries(tq-empty Core)
add_test(tq-empty tq-empty)

add_executable(tq-eq-one tq_eq_one.cpp)
target_link_libraries(tq-eq-one Core)
add_test(tq-eq-one tq-eq-one)

add_executable(tq-eq-two tq_eq_two.cpp)
target_link_libraries(tq-eq-two Core)
add_test(tq-eq-two tq-eq-two)

add_executable(tq-lt-two-one tq_lt_two_one.cpp)
target_link_libraries(tq-lt-two-one Core)
add_test(tq-lt-two-one tq-lt-two-one)

add_executable(tq-lt-three tq_lt_three.cpp)
target_link_libraries(tq-lt-three Core)
add_test(tq-lt-three tq-lt-three)

install(TARGETS tq-perf tq-empty tq-eq-one tq-eq-two tq-lt-two-one tq-lt-three
  EXPORT BCLExports DESTINATION bin)
