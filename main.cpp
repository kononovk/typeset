#include "typeset.hpp"

#include <cassert>

using empty = my::typeset<>;
using ti = my::typeset<int>;
using tilc = my::typeset<int,long,char>;
using tcli = my::typeset<char,long,int>;
using tcls = my::typeset<char,long,short>;
using tilcs = my::typeset<int,long,char,short>;
using tcl = my::typeset<char,long>;
using tis = my::typeset<int,short>;

////////////
// is_empty
static_assert(my::is_empty<empty>);
static_assert(!my::is_empty<ti>);
static_assert(!my::is_empty<tilc>);

////////
// size
static_assert(my::size<empty> == 0);
static_assert(my::size<ti> == 1);
static_assert(my::size<tilc> == 3);

///////
// get
static_assert(std::is_same_v<my::get<0, ti>, int>);
static_assert(std::is_same_v<my::get<0, tilc>, int>);
static_assert(std::is_same_v<my::get<1, tilc>, long>);
static_assert(std::is_same_v<my::get<2, tilc>, char>);

//////////////
// belongs_to
static_assert(!my::belongs_to<int, empty>);
static_assert(!my::belongs_to<char, ti>);
static_assert(my::belongs_to<int, ti>);
static_assert(!my::belongs_to<short, tilc>);
static_assert(my::belongs_to<int, tilc>);
static_assert(my::belongs_to<long, tilc>);
static_assert(my::belongs_to<char, tilc>);

//////////////
// comparison
template<class TS1, class TS2> struct equal_sets;
template<class... Xs, class... Ys> struct equal_sets<my::typeset<Xs...>, my::typeset<Ys...>> :
    std::bool_constant<
    (my::belongs_to<Xs, my::typeset<Ys...>> && ... && true)
    &&
    (my::belongs_to<Ys, my::typeset<Xs...>> && ... && true)
    > {};
template<class TS1, class TS2> static constexpr bool equal_sets_v = equal_sets<TS1, TS2>::value;

static_assert(equal_sets_v<empty, empty>);
static_assert(equal_sets_v<tilc, tcli>);
static_assert(!equal_sets_v<tilc, tcls>);

////////
// join
static_assert(equal_sets_v<my::join<empty, empty>, empty>);
static_assert(equal_sets_v<my::join<empty, ti>, ti>);
static_assert(equal_sets_v<my::join<ti, empty>, ti>);

static_assert(equal_sets_v<my::join<tilc, ti>, tcli>);
static_assert(equal_sets_v<my::join<tilc, tcls>, tilcs>);

/////////
// cross
static_assert(equal_sets_v<my::cross<empty, empty>, empty>);
static_assert(equal_sets_v<my::cross<tilc, tcli>, tilc>);
static_assert(equal_sets_v<my::cross<tilc, tcls>, tcl>);

////////////
// subtract
static_assert(equal_sets_v<my::subtract<empty, tilcs>, empty>);
static_assert(equal_sets_v<my::subtract<tilc, tcli>, empty>);
static_assert(equal_sets_v<my::subtract<tilcs, tcl>, tis>);

///////////
// variant

void test_variant() {
    using vilc = my::variant_of<tilc>;
    vilc v;
    v = 1;
    assert(std::get<int>(v) == 1);
    v = 2L;
    assert(std::get<long>(v) == 2L);
    v = '3';
    assert(std::get<char>(v) == '3');
}

int main() {
    test_variant();
}