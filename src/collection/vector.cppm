module;

#include <doctest/doctest.h>

#include <cstddef>
#include <memory>
#include <utility>
#include <concepts>

export module sde.collection.vector;

using namespace std;

namespace sde {

export
template<typename T, typename Allocator = allocator<T>>
class Vector {
public:
    using allocator_type = Allocator;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = pointer;
    using const_iterator = const_pointer;

    Vector() = default;

    explicit Vector(size_type init_size)
      : start_of_memory_{ allocator_.allocate(init_size) },
        end_of_memory_{ start_of_memory_ + init_size },
        free_of_memory_{ start_of_memory_ + init_size }
    {
        uninitialized_value_construct(start_of_memory_, end_of_memory_);
    }

    Vector(const Vector& other)
      : start_of_memory_{ allocator_.allocate(other.get_capacity()) },
        end_of_memory_{ start_of_memory_ + other.get_capacity() },
        free_of_memory_{ start_of_memory_ }
    {
        uninitialized_copy(other.start_of_memory_, other.end_of_memory_, start_of_memory_);
        free_of_memory_ += other.get_size();
    }

    Vector& operator=(const Vector& other)
    {
        if (this == &other)
            return *this;

        reset();

        start_of_memory_ = allocator_.allocate(other.get_capacity());
        end_of_memory_ = start_of_memory_ + other.get_capacity();
        free_of_memory_ = start_of_memory_;

        uninitialized_copy(other.start_of_memory_, other.end_of_memory_, start_of_memory_);
        free_of_memory_ += other.get_size();

        return *this;
    }

    Vector(Vector&& other) noexcept
      : start_of_memory_{ other.start_of_memory_ },
        end_of_memory_{ other.end_of_memory_ },
        free_of_memory_{ other.free_of_memory_ }
    {
        other.start_of_memory_ = other.end_of_memory_ = other.free_of_memory_ = nullptr;
    }

    Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
            return *this;

        reset();

        start_of_memory_ = other.start_of_memory_;
        end_of_memory_ = other.end_of_memory_;
        free_of_memory_ = other.free_of_memory_;
        other.start_of_memory_ = other.end_of_memory_ = other.free_of_memory_ = nullptr;
        return *this;
    }

    ~Vector()
    {
        if (start_of_memory_)
            reset();
    }

    constexpr bool is_empty() const noexcept
    {
        return free_of_memory_ == start_of_memory_;
    }

    iterator begin() noexcept
    {
        return start_of_memory_;
    }

    iterator begin() const noexcept
    {
        return start_of_memory_;
    }

    iterator end() noexcept
    {
        return free_of_memory_;
    }

    iterator end() const noexcept
    {
        return free_of_memory_;
    }

    const_iterator cbegin() noexcept
    {
        return start_of_memory_;
    }

    const_iterator cbegin() const noexcept
    {
        return start_of_memory_;
    }

    const_iterator cend() noexcept
    {
        return free_of_memory_;
    }

    const_iterator cend() const noexcept
    {
        return free_of_memory_;
    }

    reference operator[](size_type index) noexcept
    {
        return *(start_of_memory_ + index);
    }

    const_reference operator[](size_type index) const noexcept
    {
        return *(start_of_memory_ + index);
    }

    void clear() noexcept
    {
        destroy(start_of_memory_, free_of_memory_);
        free_of_memory_ = start_of_memory_;
    }

    void push_back(const T& value)
    {
        if (full())
            expand_capacity();

        *free_of_memory_ = value;
        ++free_of_memory_;
    }

    void push_back(T&& value)
    {
        if (full())
            expand_capacity();

        *free_of_memory_ = value;
        ++free_of_memory_;
    }

    template<constructible_from<T>... Args>
    void emplace_back(Args&&... args)
    {
        if (full())
            expand_capacity();

        construct_at(free_of_memory_, std::forward<Args>(args)...);
        ++free_of_memory_;
    }

    void shrink_to_fit()
    {
        if (free_of_memory_ == end_of_memory_)
            return;

        auto current_size = get_size();
        auto new_start_of_memory = allocator_.allocate(current_size);
        uninitialized_move(start_of_memory_, free_of_memory_, new_start_of_memory);
        allocator_.deallocate(start_of_memory_, get_capacity());

        start_of_memory_ = new_start_of_memory;
        end_of_memory_ = start_of_memory_ + current_size;
        free_of_memory_ = start_of_memory_ + current_size;
    }

    constexpr size_type get_size() const noexcept
    {
        return free_of_memory_ - start_of_memory_;
    }

    void set_size(size_type new_size)
    {
        if (new_size <= get_size()) {
            destroy(start_of_memory_ + new_size, free_of_memory_);
            free_of_memory_ = start_of_memory_ + new_size;
        } else {
            if (new_size > get_capacity())
                set_capacity(new_size);

            uninitialized_value_construct(free_of_memory_, end_of_memory_);
            free_of_memory_ += (new_size - get_size());
        }
    }

    constexpr size_type get_capacity() const noexcept
    {
        return end_of_memory_ - start_of_memory_;
    }

    void set_capacity(size_type new_capacity)
    {
        auto current_size = get_size();

        if (new_capacity < get_size()) {
            destroy(start_of_memory_ + new_capacity, free_of_memory_);
            current_size = new_capacity;
        }

        auto new_start_of_memory = allocator_.allocate(new_capacity);
        uninitialized_move(start_of_memory_, start_of_memory_ + current_size, new_start_of_memory);
        allocator_.deallocate(start_of_memory_, get_capacity());

        start_of_memory_ = new_start_of_memory;
        end_of_memory_ = start_of_memory_ + new_capacity;
        free_of_memory_ = start_of_memory_ + current_size;
    }

private:
    static constexpr size_type EXPAND_FACTOR = 2;

    Allocator allocator_{};
    pointer start_of_memory_ = nullptr;
    pointer end_of_memory_ = nullptr;
    pointer free_of_memory_ = nullptr;

    constexpr bool full() const noexcept
    {
        return free_of_memory_ == end_of_memory_;
    }

    void reset()
    {
        clear();
        allocator_.deallocate(start_of_memory_, get_capacity());
        start_of_memory_ = end_of_memory_ = free_of_memory_ = nullptr;
    }

    void expand_capacity()
    {
        if (!start_of_memory_)
            set_capacity(1);
        else
            set_capacity(get_capacity() * EXPAND_FACTOR);
    }
};

} // namespace sde


using namespace sde;

TEST_CASE("constructor")
{
    SUBCASE("default constructor")
    {
        Vector<int> arr{};
        REQUIRE(arr.get_size() == 0);
        REQUIRE(arr.get_capacity() == 0);
        REQUIRE(arr.is_empty());
    }

    SUBCASE("init size")
    {
        Vector<int> arr( 3 );
        REQUIRE(arr.get_size() == 3);
        REQUIRE(arr.get_capacity() == 3);
        REQUIRE_FALSE(arr.is_empty());

        REQUIRE(arr[0] == int{});
        REQUIRE(arr[1] == int{});
        REQUIRE(arr[2] == int{});
    }
}

TEST_CASE("copy semantic")
{
    Vector<int> arr1( 10 );
    arr1[0] = 1;
    arr1[1] = 2;
    arr1[2] = 3;

    SUBCASE("copy constructor")
    {
        Vector<int> arr2{ arr1 };

        REQUIRE(arr2.get_size() == 10);
        REQUIRE(arr2.get_capacity() == 10);
        REQUIRE(arr2[0] == 1);
        REQUIRE(arr2[1] == 2);
        REQUIRE(arr2[2] == 3);
    }

    SUBCASE("copy assignment")
    {
        Vector<int> arr2;
        arr2 = arr1;

        REQUIRE(arr2.get_size() == 10);
        REQUIRE(arr2.get_capacity() == 10);
        REQUIRE(arr2[0] == 1);
        REQUIRE(arr2[1] == 2);
        REQUIRE(arr2[2] == 3);
    }
}

TEST_CASE("move semantic")
{
    SUBCASE("move constructor")
    {
        Vector<int> arr1( 10 );
        arr1[0] = 1;
        arr1[1] = 2;
        arr1[2] = 3;
        Vector<int> arr2{ std::move(arr1) };

        REQUIRE(arr2.get_size() == 10);
        REQUIRE(arr2.get_capacity() == 10);
        REQUIRE(arr2[0] == 1);
        REQUIRE(arr2[1] == 2);
        REQUIRE(arr2[2] == 3);
    }

    SUBCASE("move assignment")
    {
        Vector<int> arr( 10 );
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;

        Vector<int> arr2;
        arr2 = std::move(arr);

        REQUIRE(arr2.get_size() == 10);
        REQUIRE(arr2.get_capacity() == 10);
        REQUIRE(arr2[0] == 1);
        REQUIRE(arr2[1] == 2);
        REQUIRE(arr2[2] == 3);
    }
}

TEST_CASE("modify size")
{
    SUBCASE("expand size")
    {
        Vector<int> arr( 10 );
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;

        arr.set_size(20);

        REQUIRE(arr.get_size() == 20);
        REQUIRE(arr.get_capacity() == 20);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SUBCASE("shrink size")
    {
        Vector<int> arr( 10 );
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;

        arr.set_size(2);

        REQUIRE(arr.get_size() == 2);
        REQUIRE(arr.get_capacity() == 10);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
    }
}

TEST_CASE("modify capacity")
{
    SUBCASE("expand capacity")
    {
        Vector<int> arr( 10 );
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;

        arr.set_capacity(20);

        REQUIRE(arr.get_size() == 10);
        REQUIRE(arr.get_capacity() == 20);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SUBCASE("shrink capacity")
    {
        Vector<int> arr( 10 );
        arr[0] = 1;
        arr[1] = 2;
        arr[2] = 3;

        arr.set_capacity(5);

        REQUIRE(arr.get_size() == 5);
        REQUIRE(arr.get_capacity() == 5);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SUBCASE("shrink to fit")
    {
        Vector<int> arr{};
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);

        CHECK(arr.get_size() == 3);
        CHECK(arr.get_capacity() == 4);

        arr.shrink_to_fit();

        REQUIRE(arr.get_size() == 3);
        REQUIRE(arr.get_capacity() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }
}

TEST_CASE("auto expand memory")
{
    Vector<int> arr{};

    CHECK(arr.get_size() == 0);
    CHECK(arr.get_capacity() == 0);

    SUBCASE("push_back")
    {
        arr.push_back(1);

        REQUIRE(arr.get_size() == 1);
        REQUIRE(arr.get_capacity() == 1);
        REQUIRE(arr[0] == 1);

        arr.push_back(2);
        REQUIRE(arr.get_size() == 2);
        REQUIRE(arr.get_capacity() == 2);
        REQUIRE(arr[1] == 2);

        arr.push_back(3);
        REQUIRE(arr.get_size() == 3);
        REQUIRE(arr.get_capacity() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SUBCASE("push_back rvalue")
    {
        arr.push_back(std::move(1));

        REQUIRE(arr.get_size() == 1);
        REQUIRE(arr.get_capacity() == 1);
        REQUIRE(arr[0] == 1);

        arr.push_back(std::move(2));
        REQUIRE(arr.get_size() == 2);
        REQUIRE(arr.get_capacity() == 2);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);

        arr.push_back(std::move(3));
        REQUIRE(arr.get_size() == 3);
        REQUIRE(arr.get_capacity() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SUBCASE("emplace_back")
    {
        arr.emplace_back(1);

        REQUIRE(arr.get_size() == 1);
        REQUIRE(arr.get_capacity() == 1);
        REQUIRE(arr[0] == 1);

        arr.emplace_back(2);
        REQUIRE(arr.get_size() == 2);
        REQUIRE(arr.get_capacity() == 2);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);

        arr.emplace_back(3);
        REQUIRE(arr.get_size() == 3);
        REQUIRE(arr.get_capacity() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }
}

TEST_CASE("clear")
{
    Vector<int> arr{ 10 };

    CHECK(arr.get_size() == 10);
    CHECK(arr.get_capacity() == 10);
    CHECK_FALSE(arr.is_empty());

    arr.clear();

    REQUIRE(arr.get_size() == 0);
    REQUIRE(arr.get_capacity() == 10);
    REQUIRE(arr.is_empty());
}