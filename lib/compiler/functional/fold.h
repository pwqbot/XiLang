#include <functional>
#include <numeric>

namespace xi
{

template <typename F, typename Init, typename T>
auto foldr(F func, Init init, T t)
{

    return std::accumulate(
        t.rbegin(),
        t.rend(),
        init,
        [func]<typename G, typename E>(G &&lhs, E &&acc) {
            return std::invoke(
                func, std::forward<E>(acc), std::forward<G>(lhs)
            );
        }
    );
}

template <typename F, typename Init, typename T>
auto foldl(F func, Init init, T t)
{
    return std::accumulate(t.rbegin(), t.rend(), init, func);
}
} // namespace xi
