#pragma once

#include <Direction.h>

#include <concepts>
#include <tuple>
#include <type_traits>

namespace turing {

template<typename T>
concept IsAlphabet = requires(T) { std::is_enum_v<T>; };

template<typename T>
concept IsState = requires(T) {
  std::is_enum_v<T>;
  T::HALT;
  T::YES;
  T::NO;
};

template<typename T, typename State, typename Alphabet>
concept IsTransitionFunction = requires(T func) {
  IsAlphabet<Alphabet>;
  IsState<State>;
  {
    func(std::declval<State>(), std::declval<Alphabet>())
  } -> std::same_as<std::tuple<State, Alphabet, Direction>>;
};
}// namespace turing