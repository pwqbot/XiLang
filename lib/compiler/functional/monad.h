#pragma once

#include <compiler/functional/fold.h>
#include <range/v3/action/reverse.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <vector>

namespace xi
{

template <typename M>
struct unit_
{
};

template <
    template <typename T, typename...>
    typename M,
    typename... Args,
    typename T>
auto Unit(T t) -> M<T, Args...>
{
    return unit_<M<T, Args...>>::unit(t);
}

// sequence a list of monad
template <
    template <typename T, typename...>
    typename M,
    typename T,
    typename... Args>
auto sequence(std::vector<M<T, Args...>> monads)
{
    return foldr(
               [](M<T, Args...>              monad,
                  M<std::vector<T>, Args...> acc) -> M<std::vector<T>, Args...>
               {
                   return monad >>= [acc](auto &&value)
                   {
                       return acc >>= [value](std::vector<T> acc_)
                       {
                           acc_.push_back(value);
                           return Unit<M, Args...>(acc_);
                       };
                   };
               },
               Unit<M, Args...>(std::vector<T>()),
               monads
           ) >>= [](auto &&ret) -> M<std::vector<T>, Args...>
    {
        return std::move(ret) | ranges::actions::reverse;
    };
}

template <typename T, typename Func>
auto flatmap(std::vector<T> v, Func &&f)
{
    const auto mapped = v | ranges::views::transform(f) | ranges::to_vector;
    return sequence(mapped);
}

template <typename T, typename Func>
auto flatmap_(std::vector<T>& v, Func &&f)
{
    const auto mapped = v | ranges::views::transform(f) | ranges::to_vector;
    return sequence(mapped);
}
} // namespace xi
