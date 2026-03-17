# CMake generated Testfile for 
# Source directory: /home/xcredo/programming/cpp/SmallVector/tests
# Build directory: /home/xcredo/programming/cpp/SmallVector/build_valgrind/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(SmallVectorTests "/home/xcredo/programming/cpp/SmallVector/build_valgrind/tests/run_tests")
set_tests_properties(SmallVectorTests PROPERTIES  _BACKTRACE_TRIPLES "/home/xcredo/programming/cpp/SmallVector/tests/CMakeLists.txt;11;add_test;/home/xcredo/programming/cpp/SmallVector/tests/CMakeLists.txt;0;")
add_test(SmallVectorValgrind "/usr/bin/valgrind" "--leak-check=full" "--error-exitcode=1" "/home/xcredo/programming/cpp/SmallVector/build_valgrind/tests/run_tests")
set_tests_properties(SmallVectorValgrind PROPERTIES  _BACKTRACE_TRIPLES "/home/xcredo/programming/cpp/SmallVector/tests/CMakeLists.txt;17;add_test;/home/xcredo/programming/cpp/SmallVector/tests/CMakeLists.txt;0;")
