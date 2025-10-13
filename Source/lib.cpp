/**
 * @file lib.cpp
 * @brief Реализация функций библиотеки версионирования
 */

#include "lib.h"

#include "version.h"

/**
 * @brief Реализация функции получения версии патча
 * 
 * @return Значение PROJECT_VERSION_PATCH, определенное при сборке
 */
int version() {
	return PROJECT_VERSION_PATCH;
}
