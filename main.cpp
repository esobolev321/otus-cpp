#include "lib.h"

#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <string>
#include <type_traits>

template<typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_ip(T integral_value) {
    auto bytes = reinterpret_cast<unsigned char*>(&integral_value);
    for (int i = sizeof(T) - 1; i >= 0; --i) {
        std::cout << static_cast<int>(bytes[i]);
        if (i > 0) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

void print_ip(const std::string& str) {
	std::cout << str << std::endl;
}

template<typename T>
struct is_container : std::false_type {};

template<typename T, typename A>
struct is_container<std::vector<T, A>> : std::true_type {};

template<typename T, typename A>
struct is_container<std::list<T, A>> : std::true_type {};

template<typename T>
typename std::enable_if<is_container<T>::value, void>::type
print_ip(const T& container) {
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it;
        if (std::next(it) != container.end()) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

template<typename... Ts>
struct are_same;

template<typename T>
struct are_same<T> : std::true_type {};

template<typename T, typename U, typename... Ts>
struct are_same<T, U, Ts...> : std::integral_constant<bool, std::is_same<T, U>::value && are_same<U, Ts...>::value> {};

template<typename T>
struct is_homogeneous_tuple : std::false_type {};

template<typename... Ts>
struct is_homogeneous_tuple<std::tuple<Ts...>> : are_same<Ts...> {};

template<std::size_t I = 0, typename... Ts>
typename std::enable_if<(I < sizeof...(Ts))>::type
print_tuple_impl(const std::tuple<Ts...>& t)
{
    if (I > 0) {
        std::cout << ".";
    }
    std::cout << std::get<I>(t);
    print_tuple_impl<I + 1>(t);
}

template<std::size_t I = 0, typename... Ts>
typename std::enable_if<(I == sizeof...(Ts))>::type
print_tuple_impl(const std::tuple<Ts...>& t)
{
}

template<typename T>
typename std::enable_if<is_homogeneous_tuple<T>::value, void>::type
print_ip(const T& t)
{
    print_tuple_impl(t);
    std::cout << std::endl;
}

int main(int, char **)
{
	print_ip( int8_t{-1} ); // 255
	print_ip( int16_t{0} ); // 0.0
	print_ip( int32_t{2130706433} ); // 127.0.0.1
	print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
	print_ip( std::string{"Hello, World!"} ); // Hello, World!
	print_ip( "Hello, World!" );
	print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
	print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
	print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
	return 0;
}