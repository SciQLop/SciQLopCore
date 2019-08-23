#pragma once
#include "cpp_utils.h"
#include "types_detectors.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

HAS_METHOD(name)
HAS_METHOD(text)
HAS_METHOD(toStdString)

template<typename T> std::string _get_name(const T& item)
{
  if constexpr(has_name<T>) { return _get_name(item.name()); }
  else if constexpr(has_text<T>)
  {
    return _get_name(item.text(0));
  }
  else if constexpr(has_toStdString<T>)
  {
    return _get_name(item.toStdString());
  }
  else
  {
    return item;
  }
}

template<typename T>
void _print_tree(const T& tree, int indent_increment, int indent_lvl)
{
  auto print_lambda = [&indent_lvl, &indent_increment](const auto& node) {
    indent_lvl * [indent_increment](){std::cout <<"│"<< std::string(indent_increment, ' ');};
    std::cout << "├";
    (indent_increment-1) * [](){std::cout <<"─";};
    std::cout << " "<< _get_name(to_value(node)) << std::endl;
    print_tree(to_value(node), indent_increment, indent_lvl+1);
  };
  if constexpr(is_qt_tree_item<T>::value)
  {
    for(int i = 0; i < tree.childCount(); i++)
    {
      print_lambda(tree.child(i));
    }
  }
  else
  {
    std::for_each(std::cbegin(tree), std::cend(tree), print_lambda);
  }
}

template<typename T>
void print_tree(const T& tree, int indent_increment = 3, int indent_lvl = 0)
{
  _print_tree(to_value(tree), indent_increment, indent_lvl);
}
