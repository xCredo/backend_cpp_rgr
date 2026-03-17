#ifndef SMALL_VECTOR_H
#define SMALL_VECTOR_H

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace small_vector {

template <typename T, size_t N>
class SmallVector {
   public:
    // Iterator types
    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

   private:
    // Internal buffer for small storage
    alignas(T) unsigned char stack_buffer_[N * sizeof(T)];

    // Pointer to current data (stack or heap)
    pointer data_;

    // Current number of elements
    size_type size_;

    // Current capacity (N for stack, or heap capacity)
    size_type capacity_;

    // Flag: true if using heap
    bool is_heap_;

    // Helper methods
    void allocate_heap(size_type new_capacity) {
        data_ = static_cast<pointer>(::operator new(new_capacity * sizeof(T)));
        capacity_ = new_capacity;
        is_heap_ = true;
    }

    void deallocate_heap() {
        if (is_heap_ && data_) {
            ::operator delete(data_);
            data_ = nullptr;
        }
    }

    void destroy_elements() {
        for (size_type i = 0; i < size_; ++i) {
            data_[i].~T();
        }
    }

    void move_to_heap(size_type new_capacity) {
        pointer new_data = static_cast<pointer>(::operator new(new_capacity * sizeof(T)));

        // Move existing elements
        for (size_type i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }

        data_ = new_data;
        capacity_ = new_capacity;
        is_heap_ = true;
    }

    pointer stack_data() noexcept { return reinterpret_cast<pointer>(stack_buffer_); }

    const_pointer stack_data() const noexcept {
        return reinterpret_cast<const_pointer>(stack_buffer_);
    }

   public:
    // ==================== Constructors ====================

    // Default constructor
    SmallVector() noexcept : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {}

    // Fill constructor
    explicit SmallVector(size_type count, const T& value = T())
        : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {
        if (count > N) {
            allocate_heap(count);
        }
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    // Range constructor
    template <typename InputIt, typename = std::enable_if_t<!std::is_integral_v<InputIt>>>
    SmallVector(InputIt first, InputIt last)
        : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {
        for (auto it = first; it != last; ++it) {
            push_back(*it);
        }
    }

    // Initializer list constructor
    SmallVector(std::initializer_list<T> init)
        : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {
        if (init.size() > N) {
            allocate_heap(init.size());
        }
        for (const auto& elem : init) {
            push_back(elem);
        }
    }

    // Copy constructor
    SmallVector(const SmallVector& other)
        : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {
        if (other.size_ > N) {
            allocate_heap(other.size_);
        }
        for (size_type i = 0; i < other.size_; ++i) {
            push_back(other[i]);
        }
    }

    // Move constructor
    SmallVector(SmallVector&& other) noexcept
        : data_(stack_data()), size_(0), capacity_(N), is_heap_(false) {
        if (other.is_heap_) {
            // Steal heap resources
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            is_heap_ = true;

            // Reset other
            other.data_ = other.stack_data();
            other.size_ = 0;
            other.capacity_ = N;
            other.is_heap_ = false;
        } else {
            // Move elements from stack to stack
            for (size_type i = 0; i < other.size_; ++i) {
                push_back(std::move(other[i]));
                other[i].~T();  // Явно вызываем деструктор
            }
            other.size_ = 0;  // Очищаем размер other
        }
    }

    // Destructor
    ~SmallVector() {
        destroy_elements();
        deallocate_heap();
    }

    // ==================== Assignment operators ====================

    SmallVector& operator=(const SmallVector& other) {
        if (this != &other) {
            clear();
            if (other.size_ > capacity_) {
                deallocate_heap();
                allocate_heap(other.size_);
                data_ = stack_data();
                is_heap_ = false;
                if (other.size_ > N) {
                    allocate_heap(other.size_);
                }
            }
            for (size_type i = 0; i < other.size_; ++i) {
                push_back(other[i]);
            }
        }
        return *this;
    }

    SmallVector& operator=(SmallVector&& other) noexcept {
        if (this != &other) {
            clear();
            deallocate_heap();

            if (other.is_heap_) {
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                is_heap_ = true;

                other.data_ = other.stack_data();
                other.size_ = 0;
                other.capacity_ = N;
                other.is_heap_ = false;
            } else {
                data_ = stack_data();
                size_ = 0;
                capacity_ = N;
                is_heap_ = false;

                for (size_type i = 0; i < other.size_; ++i) {
                    push_back(std::move(other[i]));
                }
                other.clear();
            }
        }
        return *this;
    }

    // ==================== Element access ====================

    reference operator[](size_type pos) noexcept { return data_[pos]; }

    const_reference operator[](size_type pos) const noexcept { return data_[pos]; }

    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("SmallVector::at index out of range");
        }
        return data_[pos];
    }

    const_reference at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("SmallVector::at index out of range");
        }
        return data_[pos];
    }

    reference front() noexcept { return data_[0]; }

    const_reference front() const noexcept { return data_[0]; }

    reference back() noexcept { return data_[size_ - 1]; }

    const_reference back() const noexcept { return data_[size_ - 1]; }

    pointer data() noexcept { return data_; }

    const_pointer data() const noexcept { return data_; }

    // ==================== Iterators ====================

    pointer begin() noexcept { return data_; }
    const_pointer begin() const noexcept { return data_; }
    const_pointer cbegin() const noexcept { return data_; }

    pointer end() noexcept { return data_ + size_; }
    const_pointer end() const noexcept { return data_ + size_; }
    const_pointer cend() const noexcept { return data_ + size_; }

    std::reverse_iterator<pointer> rbegin() noexcept {
        return std::reverse_iterator<pointer>(end());
    }

    std::reverse_iterator<const_pointer> rbegin() const noexcept {
        return std::reverse_iterator<const_pointer>(end());
    }

    std::reverse_iterator<const_pointer> crbegin() const noexcept {
        return std::reverse_iterator<const_pointer>(cend());
    }

    std::reverse_iterator<pointer> rend() noexcept {
        return std::reverse_iterator<pointer>(begin());
    }

    std::reverse_iterator<const_pointer> rend() const noexcept {
        return std::reverse_iterator<const_pointer>(begin());
    }

    std::reverse_iterator<const_pointer> crend() const noexcept {
        return std::reverse_iterator<const_pointer>(cbegin());
    }

    // ==================== Capacity ====================

    bool empty() const noexcept { return size_ == 0; }

    size_type size() const noexcept { return size_; }

    size_type capacity() const noexcept { return capacity_; }

    void reserve(size_type new_cap) {
        if (new_cap > capacity_) {
            if (!is_heap_) {
                move_to_heap(new_cap);
            } else {
                pointer new_data = static_cast<pointer>(::operator new(new_cap * sizeof(T)));
                for (size_type i = 0; i < size_; ++i) {
                    new (new_data + i) T(std::move(data_[i]));
                    data_[i].~T();
                }
                ::operator delete(data_);
                data_ = new_data;
                capacity_ = new_cap;
            }
        }
    }

    void shrink_to_fit() {
        if (is_heap_ && size_ <= N) {
            // Move back to stack
            pointer src = data_;
            pointer dst = stack_data();

            for (size_type i = 0; i < size_; ++i) {
                new (dst + i) T(std::move(src[i]));
                src[i].~T();
            }

            ::operator delete(data_);
            data_ = dst;
            capacity_ = N;
            is_heap_ = false;
        } else if (is_heap_ && size_ < capacity_) {
            pointer new_data = static_cast<pointer>(::operator new(size_ * sizeof(T)));
            for (size_type i = 0; i < size_; ++i) {
                new (new_data + i) T(std::move(data_[i]));
                data_[i].~T();
            }
            ::operator delete(data_);
            data_ = new_data;
            capacity_ = size_;
        }
    }

    // ==================== Modifiers ====================

    void clear() noexcept {
        destroy_elements();
        size_ = 0;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (data_ + size_) T(value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (data_ + size_) T(std::forward<Args>(args)...);
        return data_[size_++];
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    pointer insert(const_pointer pos, const T& value) {
        size_type index = pos - data_;
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        // Shift elements
        for (size_type i = size_; i > index; --i) {
            new (data_ + i) T(std::move(data_[i - 1]));
            data_[i - 1].~T();
        }

        new (data_ + index) T(value);
        ++size_;
        return data_ + index;
    }

    pointer insert(const_pointer pos, T&& value) {
        size_type index = pos - data_;
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        for (size_type i = size_; i > index; --i) {
            new (data_ + i) T(std::move(data_[i - 1]));
            data_[i - 1].~T();
        }

        new (data_ + index) T(std::move(value));
        ++size_;
        return data_ + index;
    }

    pointer insert(const_pointer pos, size_type count, const T& value) {
        size_type index = pos - data_;
        if (size_ + count > capacity_) {
            reserve(size_ + count);
        }

        // Shift elements
        for (size_type i = size_; i > index; --i) {
            new (data_ + i + count - 1) T(std::move(data_[i - 1]));
            data_[i - 1].~T();
        }

        // Insert new elements
        for (size_type i = 0; i < count; ++i) {
            new (data_ + index + i) T(value);
        }

        size_ += count;
        return data_ + index;
    }

    pointer erase(const_pointer pos) {
        size_type index = pos - data_;
        data_[index].~T();

        for (size_type i = index; i < size_ - 1; ++i) {
            new (data_ + i) T(std::move(data_[i + 1]));
            data_[i + 1].~T();
        }

        --size_;
        return data_ + index;
    }

    pointer erase(const_pointer first, const_pointer last) {
        size_type first_index = first - data_;
        size_type count = last - first;

        // Destroy erased elements
        for (size_type i = 0; i < count; ++i) {
            data_[first_index + i].~T();
        }

        // Shift remaining elements
        size_type remaining = size_ - first_index - count;
        for (size_type i = 0; i < remaining; ++i) {
            new (data_ + first_index + i) T(std::move(data_[first_index + count + i]));
            data_[first_index + count + i].~T();
        }

        size_ -= count;
        return data_ + first_index;
    }

    void resize(size_type count) {
        if (count < size_) {
            for (size_type i = count; i < size_; ++i) {
                data_[i].~T();
            }
            size_ = count;
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                new (data_ + i) T();
            }
            size_ = count;
        }
    }

    void resize(size_type count, const T& value) {
        if (count < size_) {
            for (size_type i = count; i < size_; ++i) {
                data_[i].~T();
            }
            size_ = count;
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                new (data_ + i) T(value);
            }
            size_ = count;
        }
    }

    void swap(SmallVector& other) noexcept {
        if (this == &other)
            return;

        if (is_heap_ && other.is_heap_) {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        } else if (!is_heap_ && !other.is_heap_) {
            // Оба на стеке - просто меняем элементы местами
            size_type max_size = std::max(size_, other.size_);
            for (size_type i = 0; i < max_size; ++i) {
                if (i < size_ && i < other.size_) {
                    std::swap(data_[i], other.data_[i]);
                } else if (i < size_) {
                    // Элемент есть только в this
                    new (&other.data_[i]) T(std::move(data_[i]));
                    data_[i].~T();
                } else {
                    // Элемент есть только в other
                    new (&data_[i]) T(std::move(other.data_[i]));
                    other.data_[i].~T();
                }
            }
            // Меняем размеры
            std::swap(size_, other.size_);
            // capacity_ не меняется (оба = N)
        } else {
            // One on stack, one on heap
            SmallVector tmp(std::move(*this));
            *this = std::move(other);
            other = std::move(tmp);
        }
    }
};

// Non-member swap
template <typename T, size_t N>
void swap(SmallVector<T, N>& lhs, SmallVector<T, N>& rhs) noexcept {
    lhs.swap(rhs);
}

// Comparison operators
template <typename T, size_t N>
bool operator==(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (!(lhs[i] == rhs[i]))
            return false;
    }
    return true;
}

template <typename T, size_t N>
bool operator!=(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    return !(lhs == rhs);
}

template <typename T, size_t N>
bool operator<(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    size_t min_size = std::min(lhs.size(), rhs.size());
    for (size_t i = 0; i < min_size; ++i) {
        if (lhs[i] < rhs[i])
            return true;
        if (rhs[i] < lhs[i])
            return false;
    }
    return lhs.size() < rhs.size();
}

template <typename T, size_t N>
bool operator<=(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    return !(rhs < lhs);
}

template <typename T, size_t N>
bool operator>(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    return rhs < lhs;
}

template <typename T, size_t N>
bool operator>=(const SmallVector<T, N>& lhs, const SmallVector<T, N>& rhs) {
    return !(lhs < rhs);
}

}  // namespace small_vector

#endif  // SMALL_VECTOR_H
