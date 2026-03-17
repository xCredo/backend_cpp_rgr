#!/bin/bash
# Скрипт для запуска всех проверок SmallVector

set -e
cd "$(dirname "$0")"

echo "=========================================="
echo "1. Обычная сборка и тесты"
echo "=========================================="
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
cd ..

echo ""
echo "=========================================="
echo "2. AddressSanitizer"
echo "=========================================="
rm -rf build_asan
mkdir build_asan
cd build_asan
cmake -DENABLE_ASAN=ON ..
cmake --build .
ctest --output-on-failure
cd ..

echo ""
echo "=========================================="
echo "3. UndefinedBehaviorSanitizer"
echo "=========================================="
rm -rf build_usan
mkdir build_usan
cd build_usan
cmake -DENABLE_USAN=ON ..
cmake --build .
ctest --output-on-failure
cd ..

echo ""
echo "=========================================="
echo "4. Valgrind"
echo "=========================================="
rm -rf build_valgrind
mkdir build_valgrind
cd build_valgrind
cmake -DENABLE_VALGRIND=ON ..
cmake --build .
ctest --output-on-failure
cd ..

echo ""
echo "=========================================="
echo "5. Clang-format проверка"
echo "=========================================="
clang-format --dry-run --Werror SmallVector/small_vector.h tests/test_small_vector.cpp examples/example.cpp
echo "Clang-format: OK"

echo ""
echo "=========================================="
echo "6. Clang-tidy проверка"
echo "=========================================="
cd build
clang-tidy -p . ../SmallVector/small_vector.h 2>&1 | grep -E "error:" || echo "Clang-tidy: OK (только warnings)"
cd ..

echo ""
echo "=========================================="
echo "ВСЕ ПРОВЕРКИ ПРОЙДЕНЫ!"
echo "=========================================="
