// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.

#pragma once
#include <type_traits>

#define ZENDO_CONST_ADDRESS 0xFFFF
#define ZENDO_OFFSET_OF(type,member) (size_t)((unsigned char*)(&(((type*)ZENDO_CONST_ADDRESS)->member)) - (unsigned char*)(type*)ZENDO_CONST_ADDRESS)
#define ZENDO_CONTAINER_OF(ptr,type,member) ({const decltype(((type*)ZENDO_CONST_ADDRESS)->member) * mptr = (ptr); (type*)((char*)mptr - ZENDO_OFFSET_OF(type,member));})

#define ZENDO_HAS_MEMBER(member)     template<typename, typename> struct has_member_##member;\
template<typename T, typename Ret, typename... Args>\
struct has_member_##member<T, Ret(Args...)>\
{\
private:\
template<typename> static constexpr auto SFINAE(std::nullptr_t)->typename std::is_same< decltype(std::declval<T>().member(std::declval<Args>()...)), Ret>::type;\
template<typename> static constexpr std::false_type SFINAE(...);\
public:\
using type = decltype(SFINAE<T>(nullptr));\
static constexpr bool value = type::value;\
}; 

/*
 struct test
 {
 void func1(){}
 int func2(int) { return 0; }
 };
 ZENDO_HAS_MEMBER(func1)
 ZENDO_HAS_MEMBER(func2)
 bool ret1 = zendo::has_member_func1<test, void()>::value;
 bool ret2 = zendo::has_member_func1<test, int(int)>::value;
 */

namespace zendo
{
    
    template <typename... Args> void ignore_unused(Args const& ...){}
    
    template<typename, typename> struct has_invoke;
    template<typename T, typename Ret, typename... Args>
    struct has_invoke<T, Ret(Args...)>
    {
    private:
        template<typename> static constexpr auto SFINAE(std::nullptr_t)->typename std::is_same< decltype(std::declval<T>().operator()(std::declval<Args>()...)), Ret>::type;
        template<typename> static constexpr std::false_type SFINAE(...);
    public:
        using type = decltype(SFINAE<T>(nullptr));
        static constexpr bool value = type::value;
    };
    
    
}

