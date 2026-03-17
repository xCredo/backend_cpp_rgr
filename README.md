# SmallVector

Шаблонный контейнер `SmallVector<T, N>` — гибридный вектор с оптимизацией для малых размеров.

## Описание

`SmallVector` хранит первые N элементов на стеке (встроенный буфер). При превышении N элементы перемещаются в динамическую память. Это обеспечивает высокую производительность для малых контейнеров.

## Особенности

- **Оптимизация малых размеров**: первые N элементов хранятся на стеке
- **Динамическое расширение**: автоматическое перемещение в кучу при превышении N
- **RAII**: полная поддержка 5 специальных методов
- **STL-совместимые итераторы**: работа с алгоритмами std::sort, std::find и др.
- **Без зависимостей**: не использует STL контейнеры внутри

## Требования

- C++17 или выше
- CMake 3.14+

## Сборка

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Запуск тестов

```bash
cd build
ctest --output-on-failure
```

### С санитайзерами

```bash
cmake -DENABLE_ASAN=ON ..
cmake --build .
ctest --output-on-failure
```

### С Valgrind

```bash
cmake -DENABLE_VALGRIND=ON ..
cmake --build .
ctest --output-on-failure
```

## Использование

```cpp
#include<SmallVector/small_vector.h>

int main() {
    // Вектор с 5 элементами на стеке
    SmallVector<int, 5> vec;
    
    // Добавление элементов (хранятся на стеке)
    for (int i = 0; i< 5; ++i) {
        vec.push_back(i);
    }
    
    // При превышении - переход в кучу
    vec.push_back(100);  // Теперь все в куче
    
    // STL-совместимые итераторы
    std::sort(vec.begin(), vec.end());
    
    return 0;
}
```

## API

### Конструкторы
- `SmallVector()` — по умолчанию
- `SmallVector(size_t count, const T& value)` — заполнение
- `SmallVector(const SmallVector&)` — копирование
- `SmallVector(SmallVector&&) noexcept` — перемещение
- `SmallVector(Iterator first, Iterator last)` — из диапазона
- `SmallVector(std::initializer_list<T>)` — список инициализации

### Доступ к элементам
- `operator[]` — без проверки границ
- `at()` — с проверкой границ
- `front()`, `back()` — первый/последний элемент
- `data()` — указатель на данные

### Размер и ёмкость
- `empty()`, `size()`, `capacity()`
- `reserve()`, `resize()`, `shrink_to_fit()`

### Модификаторы
- `push_back()`, `pop_back()`
- `insert()`, `erase()`
- `clear()`, `swap()`

### Итераторы
- `begin()`, `end()`, `cbegin()`, `cend()`
- `rbegin()`, `rend()`, `crbegin()`, `crend()`

## Лицензия

MIT

## Простой запуск

### Запуск примера использования библиотеки
cd ~/programming/cpp/SmallVector/build
./examples/example

### Перейди в корень проекта
cd ~/programming/cpp/SmallVector

### Вариант 1: Всё автоматически
./run_all_checks.sh

### Вариант 2: Только тесты
mkdir -p build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
