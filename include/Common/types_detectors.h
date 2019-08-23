#pragma once
#include <type_traits>

template<typename T, typename = void> struct is_smart_ptr : std::false_type
{};
template<typename T>
struct is_smart_ptr<T, decltype(std::declval<T>().get(),
                                std::declval<T>().reset())> : std::true_type
{};

template<typename T, typename = void> struct is_qt_tree_item : std::false_type
{};
template<typename T>
struct is_qt_tree_item<T, decltype(std::declval<T>().takeChildren(),
                                   std::declval<T>().parent(),
                                   std::declval<T>().addChild(nullptr))>
    : std::true_type
{};

template<typename T, typename = void> struct has_name_method : std::false_type
{};
template<typename T>
struct has_name_method<T, decltype(std::declval<T>().name(),
                                   std::declval<void>())> : std::true_type
{};

template<typename T, typename = void> struct has_text_method : std::false_type
{};
template<typename T>
struct has_text_method<T, decltype(std::declval<T>().text(),
                                   std::declval<void>())> : std::true_type
{};


template<typename T, typename = void> struct has_tostdstring_method : std::false_type
{};
template<typename T>
struct has_tostdstring_method<T, decltype(std::declval<T>().toStdString(),
                                   std::declval<void>())> : std::true_type
{};
