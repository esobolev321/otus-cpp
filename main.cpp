/**
 * @file main.cpp
 * @brief Основной файл программы для печати IP-адресов в различных форматах
 * 
 * Этот файл содержит реализацию шаблонных функций для печати IP-адресов
 * из различных типов данных: целых чисел, строк, контейнеров и кортежей.
 * Используются техники SFINAE (Substitution Failure Is Not An Error) для
 * выбора правильной перегрузки функции в зависимости от типа данных.
 */

#include "lib.h"

#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <string>
#include <type_traits>

/**
 * @brief Печать IP-адреса из целочисленного значения
 * 
 * Функция разбирает целочисленное значение побайтово и выводит
 * каждый байт как число, разделенное точками (формат IP-адреса).
 * Используется SFINAE для активации только для целочисленных типов.
 * 
 * @tparam T Целочисленный тип (int8_t, int16_t, int32_t, int64_t и т.д.)
 * @param integral_value Целочисленное значение для преобразования
 * 
 * @code
 * print_ip(int32_t{2130706433}); // Выведет: 127.0.0.1
 * @endcode
 */
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

/**
 * @brief Печать IP-адреса из строки
 * 
 * Функция просто выводит строку как есть.
 * 
 * @param str Строка для вывода
 * 
 * @code
 * print_ip(std::string{"Hello, World!"}); // Выведет: Hello, World!
 * @endcode
 */
void print_ip(const std::string& str) {
	std::cout << str << std::endl;
}

/**
 * @brief Trait для определения, является ли тип контейнером
 * 
 * Базовый шаблон возвращает false_type для всех типов.
 * 
 * @tparam T Проверяемый тип
 */
template<typename T>
struct is_container : std::false_type {};

/**
 * @brief Специализация trait для std::vector
 * 
 * @tparam T Тип элементов вектора
 * @tparam A Аллокатор вектора
 */
template<typename T, typename A>
struct is_container<std::vector<T, A>> : std::true_type {};

/**
 * @brief Специализация trait для std::list
 * 
 * @tparam T Тип элементов списка
 * @tparam A Аллокатор списка
 */
template<typename T, typename A>
struct is_container<std::list<T, A>> : std::true_type {};

/**
 * @brief Печать IP-адреса из контейнера
 * 
 * Функция выводит элементы контейнера (std::vector или std::list),
 * разделенные точками. Используется SFINAE для активации только
 * для поддерживаемых контейнеров.
 * 
 * @tparam T Тип контейнера (std::vector или std::list)
 * @param container Контейнер с элементами для вывода
 * 
 * @code
 * print_ip(std::vector<int>{100, 200, 300, 400}); // Выведет: 100.200.300.400
 * @endcode
 */
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

/**
 * @brief Trait для проверки, что все типы одинаковые
 * 
 * Используется для проверки гомогенности типов в кортеже.
 * 
 * @tparam Ts Список типов для проверки
 */
template<typename... Ts>
struct are_same;

/**
 * @brief Специализация для одного типа (всегда true)
 * 
 * @tparam T Единственный тип
 */
template<typename T>
struct are_same<T> : std::true_type {};

/**
 * @brief Специализация для проверки равенства нескольких типов
 * 
 * Рекурсивно проверяет, что первый тип равен второму и все остальные равны между собой.
 * 
 * @tparam T Первый тип
 * @tparam U Второй тип
 * @tparam Ts Остальные типы
 */
template<typename T, typename U, typename... Ts>
struct are_same<T, U, Ts...> : std::integral_constant<bool, std::is_same<T, U>::value && are_same<U, Ts...>::value> {};

/**
 * @brief Trait для определения, является ли кортеж гомогенным
 * 
 * Базовый шаблон возвращает false_type.
 * 
 * @tparam T Проверяемый тип
 */
template<typename T>
struct is_homogeneous_tuple : std::false_type {};

/**
 * @brief Специализация для std::tuple с проверкой гомогенности типов
 * 
 * @tparam Ts Типы элементов кортежа
 */
template<typename... Ts>
struct is_homogeneous_tuple<std::tuple<Ts...>> : are_same<Ts...> {};

/**
 * @brief Вспомогательная функция для завершения рекурсии печати кортежа
 * 
 * Активная версия для I == sizeof...(Ts) - завершает печать (базовый случай).
 * Эта функция должна быть определена ПЕРЕД рекурсивной версией.
 * 
 * @tparam I Текущий индекс (равен количеству элементов)
 * @tparam Ts Типы элементов кортежа
 */
template<std::size_t I = 0, typename... Ts>
typename std::enable_if<(I == sizeof...(Ts))>::type
print_tuple_impl(const std::tuple<Ts...>&)
{
}

/**
 * @brief Вспомогательная функция для рекурсивной печати элементов кортежа
 * 
 * Активная версия для I < sizeof...(Ts) - продолжает печать.
 * 
 * @tparam I Текущий индекс элемента
 * @tparam Ts Типы элементов кортежа
 * @param t Кортеж для печати
 */
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

/**
 * @brief Печать IP-адреса из гомогенного кортежа
 * 
 * Функция выводит элементы кортежа, разделенные точками.
 * Используется SFINAE для активации только для кортежей,
 * у которых все элементы одного типа.
 * 
 * @tparam T Тип кортежа
 * @param t Кортеж с элементами одного типа
 * 
 * @code
 * print_ip(std::make_tuple(123, 456, 789, 0)); // Выведет: 123.456.789.0
 * @endcode
 */
template<typename T>
typename std::enable_if<is_homogeneous_tuple<T>::value, void>::type
print_ip(const T& t)
{
    print_tuple_impl(t);
    std::cout << std::endl;
}

/**
 * @brief Главная функция программы
 * 
 * Демонстрирует работу различных перегрузок функции print_ip
 * для разных типов данных:
 * - Целочисленные типы различной разрядности
 * - Строки (std::string и C-строки)
 * - Контейнеры (std::vector, std::list)
 * - Гомогенные кортежи (std::tuple)
 * 
 * @param argc Количество аргументов командной строки (не используется)
 * @param argv Массив аргументов командной строки (не используется)
 * @return 0 при успешном завершении
 * 
 * @see print_ip
 */
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