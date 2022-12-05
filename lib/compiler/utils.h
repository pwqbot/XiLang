#pragma once

//////////////////////////////////////////////////////////////////////////
// class template recursive_wrapper
//
// See docs and recursive_wrapper_fwd.hpp for more information.
//

#include <compare>
#include <utility>
template <typename T>
class recursive_wrapper {
  public: // typedefs
    using type = T;

  private: // representation
    T *p_;

  public: // structors
    constexpr ~recursive_wrapper();
    constexpr recursive_wrapper();

    constexpr recursive_wrapper(const recursive_wrapper &operand);
    constexpr recursive_wrapper(const T &operand); // NOLINT

    constexpr recursive_wrapper(recursive_wrapper &&operand) noexcept;
    constexpr recursive_wrapper(T &&operand); // NOLINT

  private: // helpers, for modifiers (below)
    void assign(const T &rhs);

  public: // modifiers
    auto operator=(const recursive_wrapper &rhs) -> recursive_wrapper & {
        assign(rhs.get());
        return *this;
    }

    auto operator=(const T &rhs) -> recursive_wrapper & {
        assign(rhs);
        return *this;
    }

    void swap(recursive_wrapper &operand) noexcept {
        T *temp    = operand.p_;
        operand.p_ = p_;
        p_         = temp;
    }

    auto operator=(recursive_wrapper &&rhs) noexcept -> recursive_wrapper & {
        swap(rhs);
        return *this;
    }

    auto operator=(T &&rhs) -> recursive_wrapper & {
        get() = std::move(rhs);
        return *this;
    }

    // queries
    auto get() -> T & { return *get_pointer(); }
    auto get() const -> const T & { return *get_pointer(); }

    auto get_pointer() -> T * { return p_; }
    auto get_pointer() const -> const T * { return p_; }
    auto operator==(const recursive_wrapper &b) const {
        return get() == b.get();
    }

    auto operator<=>(const recursive_wrapper &b) const {
        return get() <=> b.get();
    }
    // auto operator<=>(const T &b) const { return std::strong_ordering::equal;
    // }
};

template <typename T>
constexpr recursive_wrapper<T>::~recursive_wrapper() {
    delete p_;
}

template <typename T>
constexpr recursive_wrapper<T>::recursive_wrapper() : p_(new T) {}

template <typename T>
constexpr recursive_wrapper<T>::recursive_wrapper(
    const recursive_wrapper &operand)
    : p_(new T(operand.get())) {}

template <typename T>
constexpr recursive_wrapper<T>::recursive_wrapper(const T &operand)
    : p_(new T(operand)) {}

template <typename T>
constexpr recursive_wrapper<T>::recursive_wrapper(
    recursive_wrapper &&operand) noexcept
    : p_(new T(std::move(operand.get()))) {}

template <typename T>
constexpr recursive_wrapper<T>::recursive_wrapper(T &&operand)
    : p_(new T(std::move(operand))) {}

template <typename T>
void recursive_wrapper<T>::assign(const T &rhs) {
    this->get() = rhs;
}

// function template swap
//
// Swaps two recursive_wrapper<T> objects of the same type T.
//
template <typename T>
inline void swap(recursive_wrapper<T> &lhs,
                 recursive_wrapper<T> &rhs) noexcept {
    lhs.swap(rhs);
}
