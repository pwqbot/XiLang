#pragma once

#include <compiler/parser/utils.h>
#include <variant>

namespace xi
{

// A trait to check that T is one of 'Types...'
template <typename T, typename... Types>
concept is_one_of = std::disjunction_v<std::is_same<std::decay_t<T>, Types>...>;

template <typename T, typename... Types>
concept is_one_of_recursive = std::disjunction_v<
    std::is_same<recursive_wrapper<std::decay_t<T>>, Types>...>;

template <typename T>
struct is_recursive : std::false_type
{
};

template <typename T>
struct is_recursive<recursive_wrapper<T>> : std::true_type
{
};

template <typename... Types, typename T>
auto operator==(const std::variant<Types...> &v, const T &t) noexcept
    requires is_one_of<T, Types...> || is_one_of_recursive<T, Types...>
{
    return std::visit(
        [t]<typename V>(V &&v_)
        {
            using DV = std::decay_t<V>;
            using DT = std::decay_t<T>;
            if constexpr (is_recursive<DV>::value)
            {
                if constexpr (!std::same_as<DV, recursive_wrapper<DT>>)
                {
                    return false;
                }
                else
                {
                    return v_ == t;
                }
            }
            else
            {
                if constexpr (!std::same_as<DV, DT>)
                {
                    return false;
                }
                else
                {
                    return v_ == t;
                }
            }
        },
        v
    );
}

template <typename... Types, typename T>
auto operator!=(const std::variant<Types...> &v, const T &t) noexcept
    requires is_one_of<T, Types...> || is_one_of_recursive<T, Types...>
{
    return !(v == t);
}
} // namespace xi
