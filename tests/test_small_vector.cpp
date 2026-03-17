#include <SmallVector/small_vector.h>

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <string>
#include <vector>

using small_vector::SmallVector;

// ==================== Test Fixtures ====================

// Track construction/destruction for RAII tests
struct Counter {
    static int constructed;
    static int destructed;
    static int copied;
    static int moved;

    int value;

    Counter(int v = 0) : value(v) { ++constructed; }
    Counter(const Counter& other) : value(other.value) {
        ++constructed;
        ++copied;
    }
    Counter(Counter&& other) noexcept : value(other.value) {
        ++constructed;
        ++moved;
        other.value = -1;
    }
    Counter& operator=(const Counter& other) {
        value = other.value;
        ++copied;
        return *this;
    }
    Counter& operator=(Counter&& other) noexcept {
        value = other.value;
        ++moved;
        other.value = -1;
        return *this;
    }
    ~Counter() { ++destructed; }

    bool operator==(const Counter& other) const { return value == other.value; }
    bool operator<(const Counter& other) const { return value < other.value; }
};

int Counter::constructed = 0;
int Counter::destructed = 0;
int Counter::copied = 0;
int Counter::moved = 0;

void reset_counters() {
    Counter::constructed = 0;
    Counter::destructed = 0;
    Counter::copied = 0;
    Counter::moved = 0;
}

// ==================== RAII Tests ====================

TEST_CASE("RAII: Default constructor", "[raii]") {
    reset_counters();
    {
        SmallVector<Counter, 5> vec;
        REQUIRE(vec.empty());
        REQUIRE(vec.size() == 0);
        REQUIRE(vec.capacity() == 5);
    }
    REQUIRE(Counter::constructed == Counter::destructed);
}

TEST_CASE("RAII: Destructor cleans up elements", "[raii]") {
    reset_counters();
    {
        SmallVector<Counter, 5> vec;
        vec.push_back(Counter(1));
        vec.push_back(Counter(2));
        vec.push_back(Counter(3));
    }
    REQUIRE(Counter::constructed == Counter::destructed);
}

TEST_CASE("RAII: Copy constructor", "[raii]") {
    reset_counters();
    SmallVector<Counter, 3> vec1;
    vec1.push_back(Counter(1));
    vec1.push_back(Counter(2));

    SmallVector<Counter, 3> vec2(vec1);
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0].value == 1);
    REQUIRE(vec2[1].value == 2);
}

TEST_CASE("RAII: Move constructor", "[raii]") {
    reset_counters();
    SmallVector<std::string, 3> vec1;
    vec1.push_back("hello");
    vec1.push_back("world");

    SmallVector<std::string, 3> vec2(std::move(vec1));
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0] == "hello");
    REQUIRE(vec1.size() == 0);
}

TEST_CASE("RAII: Move constructor with heap", "[raii]") {
    SmallVector<int, 2> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);  // Moves to heap

    SmallVector<int, 2> vec2(std::move(vec1));
    REQUIRE(vec2.size() == 3);
    REQUIRE(vec2[0] == 1);
    REQUIRE(vec2[1] == 2);
    REQUIRE(vec2[2] == 3);
    REQUIRE(vec1.size() == 0);
    REQUIRE(vec1.capacity() == 2);  // Back to stack capacity
}

TEST_CASE("RAII: Copy assignment", "[raii]") {
    SmallVector<int, 3> vec1;
    vec1.push_back(1);
    vec1.push_back(2);

    SmallVector<int, 3> vec2;
    vec2 = vec1;
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0] == 1);
    REQUIRE(vec2[1] == 2);
}

TEST_CASE("RAII: Move assignment", "[raii]") {
    SmallVector<int, 2> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);  // Heap

    SmallVector<int, 2> vec2;
    vec2 = std::move(vec1);
    REQUIRE(vec2.size() == 3);
    REQUIRE(vec1.size() == 0);
}

// ==================== Element Access Tests ====================

TEST_CASE("Element access: operator[]", "[access]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    REQUIRE(vec[0] == 10);
    REQUIRE(vec[1] == 20);
    REQUIRE(vec[2] == 30);

    vec[1] = 25;
    REQUIRE(vec[1] == 25);
}

TEST_CASE("Element access: at()", "[access]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);

    REQUIRE(vec.at(0) == 10);
    REQUIRE(vec.at(1) == 20);

    REQUIRE_THROWS_AS(vec.at(2), std::out_of_range);
    REQUIRE_THROWS_AS(vec.at(100), std::out_of_range);
}

TEST_CASE("Element access: front() and back()", "[access]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    REQUIRE(vec.front() == 10);
    REQUIRE(vec.back() == 30);

    vec.front() = 5;
    vec.back() = 35;
    REQUIRE(vec.front() == 5);
    REQUIRE(vec.back() == 35);
}

TEST_CASE("Element access: data()", "[access]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);

    int* ptr = vec.data();
    REQUIRE(ptr[0] == 10);
    REQUIRE(ptr[1] == 20);
}

// ==================== Capacity Tests ====================

TEST_CASE("Capacity: empty()", "[capacity]") {
    SmallVector<int, 5> vec;
    REQUIRE(vec.empty());

    vec.push_back(1);
    REQUIRE_FALSE(vec.empty());

    vec.pop_back();
    REQUIRE(vec.empty());
}

TEST_CASE("Capacity: size()", "[capacity]") {
    SmallVector<int, 5> vec;
    REQUIRE(vec.size() == 0);

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    REQUIRE(vec.size() == 3);
}

TEST_CASE("Capacity: capacity()", "[capacity]") {
    SmallVector<int, 3> vec;
    REQUIRE(vec.capacity() == 3);

    vec.push_back(1);
    vec.push_back(2);
    REQUIRE(vec.capacity() == 3);  // Still on stack

    vec.push_back(3);
    REQUIRE(vec.capacity() == 3);  // Still on stack, full

    vec.push_back(4);  // Moves to heap
    REQUIRE(vec.capacity() >= 4);
}

TEST_CASE("Capacity: reserve()", "[capacity]") {
    SmallVector<int, 3> vec;
    vec.reserve(10);
    REQUIRE(vec.capacity() >= 10);
    REQUIRE(vec.size() == 0);

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    REQUIRE(vec.size() == 4);
}

TEST_CASE("Capacity: shrink_to_fit()", "[capacity]") {
    SmallVector<int, 3> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    REQUIRE(vec.capacity() >= 5);

    vec.pop_back();
    vec.pop_back();
    vec.shrink_to_fit();

    REQUIRE(vec.size() == 3);
    REQUIRE(vec.capacity() == 3);  // Back to stack
}

TEST_CASE("Capacity: resize()", "[capacity]") {
    SmallVector<int, 5> vec;

    vec.resize(3);
    REQUIRE(vec.size() == 3);

    vec.resize(5, 100);
    REQUIRE(vec.size() == 5);
    REQUIRE(vec[3] == 100);
    REQUIRE(vec[4] == 100);

    vec.resize(2);
    REQUIRE(vec.size() == 2);
}

// ==================== Modifiers Tests ====================

TEST_CASE("Modifiers: push_back()", "[modifiers]") {
    SmallVector<int, 3> vec;

    vec.push_back(1);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == 1);

    vec.push_back(2);
    vec.push_back(3);
    REQUIRE(vec.size() == 3);

    // Push to heap
    vec.push_back(4);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("Modifiers: push_back move", "[modifiers]") {
    SmallVector<std::string, 2> vec;
    std::string s = "hello";
    vec.push_back(std::move(s));

    REQUIRE(vec[0] == "hello");
}

TEST_CASE("Modifiers: emplace_back()", "[modifiers]") {
    SmallVector<std::string, 3> vec;

    vec.emplace_back(5, 'a');
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == "aaaaa");
}

TEST_CASE("Modifiers: pop_back()", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    vec.pop_back();
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.back() == 2);

    vec.pop_back();
    vec.pop_back();
    REQUIRE(vec.empty());
}

TEST_CASE("Modifiers: clear()", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    vec.clear();
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
}

TEST_CASE("Modifiers: insert()", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(3);

    auto it = vec.insert(vec.begin() + 1, 2);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[1] == 2);
    REQUIRE(*it == 2);
}

TEST_CASE("Modifiers: insert multiple", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(5);

    vec.insert(vec.begin() + 1, 3, 0);
    REQUIRE(vec.size() == 5);
    REQUIRE(vec[1] == 0);
    REQUIRE(vec[2] == 0);
    REQUIRE(vec[3] == 0);
}

TEST_CASE("Modifiers: erase()", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    auto it = vec.erase(vec.begin() + 1);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 3);
    REQUIRE(*it == 3);
}

TEST_CASE("Modifiers: erase range", "[modifiers]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    auto it = vec.erase(vec.begin() + 1, vec.begin() + 4);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 5);
}

TEST_CASE("Modifiers: swap()", "[modifiers]") {
    SmallVector<int, 3> vec1;
    vec1.push_back(1);
    vec1.push_back(2);

    SmallVector<int, 3> vec2;
    vec2.push_back(10);
    vec2.push_back(20);
    vec2.push_back(30);

    vec1.swap(vec2);

    REQUIRE(vec1.size() == 3);
    REQUIRE(vec1[0] == 10);

    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0] == 1);
}

// ==================== Iterator Tests ====================

TEST_CASE("Iterators: forward iteration", "[iterators]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    int sum = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        sum += *it;
    }
    REQUIRE(sum == 6);
}

TEST_CASE("Iterators: const iteration", "[iterators]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);

    const SmallVector<int, 5>& cvec = vec;
    int sum = 0;
    for (auto it = cvec.begin(); it != cvec.end(); ++it) {
        sum += *it;
    }
    REQUIRE(sum == 3);
}

TEST_CASE("Iterators: range-based for", "[iterators]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    int sum = 0;
    for (const auto& val : vec) {
        sum += val;
    }
    REQUIRE(sum == 60);
}

TEST_CASE("Iterators: reverse iteration", "[iterators]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    std::vector<int> reversed;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        reversed.push_back(*it);
    }

    REQUIRE(reversed[0] == 3);
    REQUIRE(reversed[1] == 2);
    REQUIRE(reversed[2] == 1);
}

// ==================== STL Algorithm Compatibility Tests ====================

TEST_CASE("STL: std::sort", "[stl]") {
    SmallVector<int, 10> vec;
    vec.push_back(5);
    vec.push_back(2);
    vec.push_back(8);
    vec.push_back(1);
    vec.push_back(9);

    std::sort(vec.begin(), vec.end());

    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 5);
    REQUIRE(vec[3] == 8);
    REQUIRE(vec[4] == 9);
}

TEST_CASE("STL: std::find", "[stl]") {
    SmallVector<int, 5> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    auto it = std::find(vec.begin(), vec.end(), 20);
    REQUIRE(it != vec.end());
    REQUIRE(*it == 20);

    it = std::find(vec.begin(), vec.end(), 999);
    REQUIRE(it == vec.end());
}

TEST_CASE("STL: std::accumulate", "[stl]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    int sum = std::accumulate(vec.begin(), vec.end(), 0);
    REQUIRE(sum == 10);
}

TEST_CASE("STL: std::copy", "[stl]") {
    SmallVector<int, 5> src;
    src.push_back(1);
    src.push_back(2);
    src.push_back(3);

    SmallVector<int, 5> dst;
    dst.resize(3);

    std::copy(src.begin(), src.end(), dst.begin());

    REQUIRE(dst[0] == 1);
    REQUIRE(dst[1] == 2);
    REQUIRE(dst[2] == 3);
}

TEST_CASE("STL: std::transform", "[stl]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    std::transform(vec.begin(), vec.end(), vec.begin(), [](int x) { return x * 2; });

    REQUIRE(vec[0] == 2);
    REQUIRE(vec[1] == 4);
    REQUIRE(vec[2] == 6);
}

TEST_CASE("STL: std::reverse", "[stl]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    std::reverse(vec.begin(), vec.end());

    REQUIRE(vec[0] == 3);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 1);
}

TEST_CASE("STL: std::fill", "[stl]") {
    SmallVector<int, 5> vec;
    vec.resize(5);

    std::fill(vec.begin(), vec.end(), 42);

    for (const auto& val : vec) {
        REQUIRE(val == 42);
    }
}

TEST_CASE("STL: std::count", "[stl]") {
    SmallVector<int, 10> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(1);
    vec.push_back(3);
    vec.push_back(1);

    REQUIRE(std::count(vec.begin(), vec.end(), 1) == 3);
    REQUIRE(std::count(vec.begin(), vec.end(), 2) == 1);
    REQUIRE(std::count(vec.begin(), vec.end(), 99) == 0);
}

// ==================== Stack/Heap Transition Tests ====================

TEST_CASE("Transition: Stack to heap", "[transition]") {
    SmallVector<int, 3> vec;

    // Fill stack
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec.capacity() == 3);

    // Trigger heap allocation
    vec.push_back(4);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec.capacity() >= 4);

    // Verify all elements preserved
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("Transition: Heap to stack via shrink_to_fit", "[transition]") {
    SmallVector<int, 3> vec;

    // Move to heap
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    REQUIRE(vec.capacity() >= 5);

    // Reduce to fit in stack
    vec.pop_back();
    vec.pop_back();

    vec.shrink_to_fit();
    REQUIRE(vec.capacity() == 3);
    REQUIRE(vec.size() == 3);
}

// ==================== Comparison Operators Tests ====================

TEST_CASE("Comparison: equality", "[comparison]") {
    SmallVector<int, 5> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);

    SmallVector<int, 5> b;
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);

    SmallVector<int, 5> c;
    c.push_back(1);
    c.push_back(2);

    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
    REQUIRE(a != c);
}

TEST_CASE("Comparison: ordering", "[comparison]") {
    SmallVector<int, 5> a;
    a.push_back(1);
    a.push_back(2);

    SmallVector<int, 5> b;
    b.push_back(1);
    b.push_back(3);

    SmallVector<int, 5> c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    REQUIRE(a < b);
    REQUIRE(a < c);
    REQUIRE(b > a);
    REQUIRE(a <= b);
    REQUIRE(b >= a);
}

// ==================== Edge Cases ====================

TEST_CASE("Edge: Zero inline size", "[edge]") {
    SmallVector<int, 0> vec;
    REQUIRE(vec.empty());
    REQUIRE(vec.capacity() == 0);

    vec.push_back(1);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == 1);
}

TEST_CASE("Edge: Large inline size", "[edge]") {
    SmallVector<int, 1000> vec;
    REQUIRE(vec.capacity() == 1000);

    for (int i = 0; i < 500; ++i) {
        vec.push_back(i);
    }
    REQUIRE(vec.size() == 500);
    REQUIRE(vec.capacity() == 1000);  // Still on stack
}

TEST_CASE("Edge: Initializer list", "[edge]") {
    SmallVector<int, 5> vec = {1, 2, 3, 4, 5};
    REQUIRE(vec.size() == 5);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[4] == 5);
}

TEST_CASE("Edge: Initializer list exceeds inline size", "[edge]") {
    SmallVector<int, 3> vec = {1, 2, 3, 4, 5};
    REQUIRE(vec.size() == 5);
    REQUIRE(vec.capacity() >= 5);
}

TEST_CASE("Edge: Self-assignment", "[edge]") {
    SmallVector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);

    vec = vec;  // Self-assignment
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
}

TEST_CASE("Edge: Complex type", "[edge]") {
    SmallVector<std::string, 3> vec;
    vec.push_back("hello");
    vec.push_back("world");
    vec.push_back("!");

    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == "hello");

    vec.push_back("extra");  // Moves to heap
    REQUIRE(vec.size() == 4);
    REQUIRE(vec[3] == "extra");
}

// ==================== Memory Leak Detection ====================

TEST_CASE("Memory: No leaks with many operations", "[memory]") {
    reset_counters();
    {
        SmallVector<Counter, 3> vec;

        for (int i = 0; i < 100; ++i) {
            vec.push_back(Counter(i));
        }

        vec.erase(vec.begin(), vec.begin() + 50);

        while (!vec.empty()) {
            vec.pop_back();
        }

        for (int i = 0; i < 10; ++i) {
            vec.push_back(Counter(i));
        }
    }
    REQUIRE(Counter::constructed == Counter::destructed);
}
