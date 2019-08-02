#pragma once
#include <cstddef>

#if __cplusplus > 201703L
#include <span>
#else // c++17
#include <array>
#include <iterator>
#include <limits>
#include <type_traits>

namespace std {
    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    template<
        class T,
        std::size_t Extent = std::dynamic_extent
    > class span;

    namespace detail {
        template <typename>
        struct is_span : std::false_type {};

        template <typename T, std::size_t S>
        struct is_span<std::span<T, S>> : std::true_type {};


        template<typename>
        struct is_std_array : std::false_type {};

        template<typename T, std::size_t N>
        struct is_std_array<std::array<T, N>> : std::true_type {};


        template<typename, typename = void>
        struct has_size_and_data : std::false_type {};

        template<typename T>
        struct has_size_and_data<T, std::void_t<decltype(std::size(std::declval<T>())),
            decltype(std::data(std::declval<T>()))>>
            : std::true_type {};


        template <typename C, typename U = std::remove_cv_t<typename std::remove_reference_t<C>>>
        struct is_container {
            static constexpr bool value =
                !is_span<U>::value && !is_std_array<U>::value &&
                !std::is_array<U>::value && has_size_and_data<C>::value;
        };

        template<typename C>
        constexpr bool is_container_v = is_container<C>::value;


        template<typename, typename = size_t>
        struct is_complete : std::false_type {};

        template<typename T>
        struct is_complete<T, decltype(sizeof(T))> : std::true_type {};

        template<typename T>
        constexpr bool is_complete_v = is_complete<T>::value;


        template <typename, typename, typename = void>
        struct is_convertible_to_element_type : std::false_type {};

        template <typename T, typename E>
        struct is_convertible_to_element_type<
            T, E, void_t<decltype(std::data(std::declval<T>()))>>
            : std::is_convertible<
                std::remove_pointer_t<decltype(std::data(std::declval<T>()))>(*)[], 
                E(*)[]> 
        {};

        template<typename T, typename E>
        constexpr bool is_convertible_to_element_type_v = is_convertible_to_element_type<T, E>::value;


        template<std::size_t Extent, std::size_t Offset, std::size_t Count = dynamic_extent>
        constexpr std::size_t E = Count != dynamic_extent ? Count
            : (Extent != dynamic_extent ? Extent - Offset
            : dynamic_extent);

    } // detail


    template <typename T, std::size_t Extent>
    class span {

        static_assert(std::is_object<T>::value, "T must be an object type!");
        static_assert(detail::is_complete<T>::value, "T must be a complete type!");
        static_assert(!std::is_abstract<T>::value, "T must not be an abstract type!");
     
        struct span_storage {
            constexpr span_storage() noexcept : size(Extent == dynamic_extent ? 0 : Extent) {}
            constexpr span_storage(T* ptr, std::ptrdiff_t size) noexcept
                : ptr(ptr), size(size)
            {}

            T* ptr = nullptr;
            std::size_t size = 0;
        };

    public:
        using element_type = T;
        using value_type = typename std::remove_cv_t<T>;
        using index_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        static constexpr std::size_t extent = Extent;

        constexpr span() noexcept = default;
        constexpr span(const span& other) noexcept = default;
        constexpr span& operator=(const span& other) noexcept = default;

        constexpr span(pointer ptr, index_type count)
            : storage_(ptr, count)
        {}

        constexpr span(pointer first_elem, pointer last_elem)
            : storage_(first_elem, last_elem - first_elem)
        {}

        template<
            std::size_t N, 
            typename = std::enable_if_t<
                (Extent == dynamic_extent || N == Extent) &&
                detail::is_convertible_to_element_type_v<
                    element_type(&)[N], T
        >>>
        constexpr span(element_type (&arr)[N]) noexcept :
            storage_(std::data(arr), N)
        {}

        template<
            std::size_t N,
            typename = std::enable_if_t<
                (Extent == dynamic_extent || N == Extent) &&
                detail::is_convertible_to_element_type_v<
                    std::array<value_type, N>&, T
        >>>
        constexpr span(std::array<value_type, N>& arr) noexcept
            : storage_(std::data(arr), N)
        {}

        template<
            std::size_t N,
            typename = std::enable_if_t<
            (Extent == dynamic_extent || N == Extent) &&
            detail::is_convertible_to_element_type_v<
            std::array<value_type, N>&, T
            >>>
        constexpr span(const std::array<value_type, N>& arr) noexcept
            : storage_(std::data(arr), N)
        {}

        template<
            typename Container,
            typename = std::enable_if_t<
                detail::is_container_v<Container> &&
                detail::is_convertible_to_element_type_v<
                    Container&, T
        >>>
        constexpr span(Container& cont)
            : storage_(std::data(cont), std::size(cont))
        {}

        template<
            typename Container,
            typename = std::enable_if_t<
            detail::is_container_v<Container> &&
            detail::is_convertible_to_element_type_v<
            Container&, T
        >>>
        constexpr span(const Container& cont)
            : storage_(std::data(cont), std::size(cont))
        {}

        template<typename U, std::size_t N,
            typename = std::enable_if_t<
                (Extent == dynamic_extent || N == Extent) &&
                std::is_convertible_v<U(*)[], T(*)[]
        >>>
        constexpr span(const span<U, N> & other) noexcept
            : storage_(std::data(other), std::size(other))
        {}

        ~span() noexcept = default;


        template <std::size_t Count>
        constexpr span<element_type, Count> first() const
        {
            static_assert(Count <= size());
            return { data(), Count };
        }

        constexpr std::span<element_type, std::dynamic_extent> first(std::size_t Count) const
        {
            return { data(), Count };
        }

        template <std::size_t Count>
        constexpr span<element_type, Count> last() const
        {
            static_assert(Count <= size());
            return { data() + (size() - Count), Count };
        }

        constexpr std::span<element_type, std::dynamic_extent> last(std::size_t Count) const
        {
            return { data() + (size() - Count), Count };
        }

        template <std::size_t Offset, std::size_t Count = dynamic_extent>
        constexpr span<element_type, detail::E<Extent, Offset, Count>> subspan() const
        {
            return { data() + Offset, detail::E<Offset, Count> };

        }

        constexpr span<element_type, dynamic_extent>
            subspan(index_type offset, index_type count = dynamic_extent) const
        {
            return { data() + offset,
                count == dynamic_extent ? size() - offset : count
            };
        }

        constexpr index_type size() const noexcept 
        {
            return storage_.size;
        }

        constexpr index_type size_bytes() const noexcept
        {
            return size() * sizeof(element_type);
        }

        constexpr bool empty() const noexcept
        {
            return size() == 0; 
        }

        constexpr reference operator[](index_type idx) const
        {
            return *(data() + idx);
        }

        constexpr reference front() const
        {
            return *data();
        }

        constexpr reference back() const
        {
            return *(data() + (size() - 1));
        }

        constexpr pointer data() const noexcept 
        {
            return storage_.ptr; 
        }

        constexpr iterator begin() const noexcept 
        { 
            return data();
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return begin();
        }

        constexpr iterator end() const noexcept
        { 
            return data() + size();
        }

        constexpr const_iterator cend() const noexcept 
        {
            return end();
        }

        constexpr reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(end());
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(cend());
        }

        constexpr reverse_iterator rend() const noexcept
        {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

    private:
        span_storage storage_{};
    };

    // Deduction guides
    template<class T, size_t N>
    span(T(&)[N]) -> span<T, N>;

    template<class T, size_t N>
    span(std::array<T, N>&) -> span<T, N>;

    template<class T, size_t N>
    span(const std::array<T, N>&) -> span<const T, N>;

    template<class Container>
    span(Container&) -> span<typename Container::value_type>;

    template<class Container>
    span(const Container&) -> span<const typename Container::value_type>;
}
#endif

namespace syscrypto {

    struct bytes_span : std::span<std::byte> {
        using std::span<std::byte>::span;
    };

    struct bytes_view : std::span<const std::byte> {
        using std::span<const std::byte>::span;
    };

    struct pd_bytes_span : std::span<std::byte> {
        using std::span<std::byte>::span;
    };

    struct pd_bytes_view : std::span<const std::byte> {
        using std::span<const std::byte>::span;
    };
}