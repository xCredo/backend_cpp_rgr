#include "../SmallVector/small_vector.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

using small_vector::SmallVector;

void print_separator(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    print_separator("1. Basic Usage");

    // Create SmallVector with 5 elements on stack
    SmallVector<int, 5> vec;
    std::cout << "Initial: size=" << vec.size() << ", capacity=" << vec.capacity() << "\n";

    // Add elements (stored on stack)
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i * 10);
    }
    std::cout << "After 5 push_backs: size=" << vec.size() << ", capacity=" << vec.capacity()
              << "\n";
    std::cout << "Elements: ";
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    print_separator("2. Stack to Heap Transition");

    // Adding 6th element triggers heap allocation
    vec.push_back(60);
    std::cout << "After 6th push_back: size=" << vec.size() << ", capacity=" << vec.capacity()
              << "\n";
    std::cout << "Elements: ";
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // Add more elements
    vec.push_back(70);
    vec.push_back(80);
    std::cout << "After more push_backs: size=" << vec.size() << ", capacity=" << vec.capacity()
              << "\n";

    print_separator("3. Element Access");

    std::cout << "vec[0] = " << vec[0] << "\n";
    std::cout << "vec.at(2) = " << vec.at(2) << "\n";
    std::cout << "front() = " << vec.front() << "\n";
    std::cout << "back() = " << vec.back() << "\n";

    print_separator("4. STL Algorithm Compatibility");

    // Sort
    SmallVector<int, 10> unsorted = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    std::cout << "Before sort: ";
    for (const auto& val : unsorted) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    std::sort(unsorted.begin(), unsorted.end());
    std::cout << "After sort: ";
    for (const auto& val : unsorted) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // Find
    auto it = std::find(unsorted.begin(), unsorted.end(), 5);
    if (it != unsorted.end()) {
        std::cout << "Found 5 at position " << (it - unsorted.begin()) << "\n";
    }

    // Accumulate
    int sum = std::accumulate(unsorted.begin(), unsorted.end(), 0);
    std::cout << "Sum of elements: " << sum << "\n";

    print_separator("5. Reverse Iterators");

    std::cout << "Reverse iteration: ";
    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit) {
        std::cout << *rit << " ";
    }
    std::cout << "\n";

    print_separator("6. Insert and Erase");

    SmallVector<int, 5> nums = {1, 2, 4, 5};
    std::cout << "Before insert: ";
    for (const auto& val : nums) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    nums.insert(nums.begin() + 2, 3);
    std::cout << "After insert 3 at position 2: ";
    for (const auto& val : nums) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    nums.erase(nums.begin() + 1);
    std::cout << "After erase at position 1: ";
    for (const auto& val : nums) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    print_separator("7. Resize and Reserve");

    SmallVector<int, 3> resizable;
    resizable.resize(5, 100);
    std::cout << "After resize(5, 100): ";
    for (const auto& val : resizable) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    resizable.reserve(20);
    std::cout << "After reserve(20): capacity=" << resizable.capacity() << "\n";

    print_separator("8. Copy and Move");

    SmallVector<std::string, 3> original;
    original.push_back("hello");
    original.push_back("world");

    // Copy
    SmallVector<std::string, 3> copied = original;
    std::cout << "Copied vector: ";
    for (const auto& val : copied) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // Move
    SmallVector<std::string, 3> moved = std::move(original);
    std::cout << "Moved vector: ";
    for (const auto& val : moved) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    std::cout << "Original after move: size=" << original.size() << "\n";

    print_separator("9. Shrink to Fit");

    SmallVector<int, 3> shrinkable;
    for (int i = 0; i < 10; ++i) {
        shrinkable.push_back(i);
    }
    std::cout << "Before shrink: size=" << shrinkable.size()
              << ", capacity=" << shrinkable.capacity() << "\n";

    // Remove elements to fit back in stack
    while (shrinkable.size() > 3) {
        shrinkable.pop_back();
    }
    shrinkable.shrink_to_fit();
    std::cout << "After shrink: size=" << shrinkable.size()
              << ", capacity=" << shrinkable.capacity() << "\n";

    print_separator("10. Complex Types");

    SmallVector<std::string, 4> strings;
    strings.emplace_back("C++");
    strings.emplace_back("is");
    strings.emplace_back("awesome");
    strings.push_back("!");

    std::cout << "String vector: ";
    for (const auto& s : strings) {
        std::cout << s << " ";
    }
    std::cout << "\n";

    print_separator("Summary");

    std::cout << "SmallVector demonstration complete!\n";
    std::cout << "- Stack allocation for first N elements\n";
    std::cout << "- Automatic heap transition when needed\n";
    std::cout << "- STL-compatible iterators\n";
    std::cout << "- Full RAII support\n";

    return 0;
}
