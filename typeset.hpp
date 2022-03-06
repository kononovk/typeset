#pragma once

#include <cstddef>
#include <type_traits>
#include <variant>

namespace my {

/* main class Typeset */
template<class ...>
struct typeset;


template<class Head, class... Types>
struct typeset<Head, Types...> {
    static_assert(!(... || std::is_same_v<Head, Types>), "typeset with same types");
    using head = Head;
    using tail = typeset<Types...>;
};

template<>
struct typeset<>;

/*
 * Details Namespace with meta funcs implementation
 * */
namespace details {

template<class ... Types>
struct GetVariant;

template<class ... Types>
struct GetVariant<typeset<Types...>> {
    using type = std::variant<Types...>;
};

template<class Typeset>
struct SizeImpl {
    static constexpr std::size_t size =
        SizeImpl<typename Typeset::tail>::size + 1;
};

template<>
struct SizeImpl<typeset<>> {
    static constexpr std::size_t size = 0;
};


template<size_t N, class Typeset>
struct GetImpl {
    using type = typename GetImpl<N - 1, typename Typeset::tail>::type;
};

template<class Typeset>
struct GetImpl<0, Typeset> {
    using type = typename Typeset::head;
};

template<class...Args>
struct BelongsImpl;

template<class T, class ...Args>
struct BelongsImpl<T, typeset<Args...>> {
    constexpr static bool value = (... || std::is_same_v<T, Args>);
};

template<class ...>
struct JoinImpl;

template<class ... Args1, class ... Args2>
struct JoinImpl<typeset<Args1...>, typeset<Args2...>> {
    using type = typeset<Args1..., Args2...>;
};

template<class ...>
struct SubstractImpl;

template<class Type, class...Args>
struct not_get_type {
    using type = std::conditional_t<(... && !std::is_same_v<Args, Type>),
        typeset<Type>, typeset<>>;
};

template<class ... Args, class ... Types>
struct SubstractImpl<typeset<Args...>, typeset<Types...>> {
    using type = typename details::JoinImpl<
        typename not_get_type<typename typeset<Args...>::head, Types...>::type,
        typename SubstractImpl<typename typeset<Args...>::tail, typeset<Types...>>::type
    >::type;
};

template<class ... Types>
struct SubstractImpl<typeset<>, typeset<Types...>> {
    using type = typeset<>;
};


template<class Type, class...Args>
struct GetType {
    using type = std::conditional_t<BelongsImpl<Type, typeset<Args...>>::value,
        typeset<Type>, typeset<>>;
};

template<class ...>
struct CrossImpl;

template<class ... Args, class ... Types>
struct CrossImpl<typeset<Args...>, typeset<Types...>> {
    using type = typename details::JoinImpl<
        typename GetType<typename typeset<Args...>::head, Types...>::type,
        typename CrossImpl<typename typeset<Args...>::tail, typeset<Types...>>::type
    >::type;
};

template<class ... Types>
struct CrossImpl<typeset<>, typeset<Types...>> {
    using type = typeset<>;
};

} // namespace details;

/**
 * Implemented functions:
 **/
template<class Typeset>
constexpr bool is_empty = std::is_same_v<Typeset, typeset<>>;

template<class Typeset>
constexpr size_t size = details::SizeImpl<Typeset>::size;

template<size_t N, class Typeset>
using get = typename details::GetImpl<N, Typeset>::type;

template<class T, class Typeset>
constexpr bool belongs_to = details::BelongsImpl<T, Typeset>::value;

template<class Ts1, class Ts2>
using join = typename details::JoinImpl<typename details::SubstractImpl<Ts1, Ts2>::type, Ts2>::type;

template<class Ts1, class Ts2>
using cross = typename details::CrossImpl<Ts1, Ts2>::type;

template<class Ts1, class Ts2>
using subtract = typename details::SubstractImpl<join<Ts1, Ts2>, Ts2>::type;

template<class Typeset>
using variant_of = typename details::GetVariant<Typeset>::type;

} // namespace cls02;
