#pragma once

#include <Direction.h>
#include <SpecialState.h>

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <variant>

namespace turing {

template<typename T>
concept IsAlphabet = requires(T) {
  std::is_enum_v<T>;
  T::COUNT;
  T::START;
  T::END;
};

template<typename T>
concept IsState = requires(T) {
  std::is_enum_v<T>;
  T::COUNT;
};

template<typename T, typename State, typename Alphabet>
concept IsTransitionFunction = requires(T func) {
  IsAlphabet<Alphabet>;
  IsState<State>;
  // Check it produces the correct outputs.
  {
    func(std::declval<State>(), std::declval<Alphabet>())
  } -> std::convertible_to<std::tuple<std::variant<State, SpecialState>, Alphabet, Direction>>;

  // Check we cannot overwrite the start symbol, or go left of start.
    requires []() {
        for (uint64_t state = 0; state < static_cast<uint64_t>(State::COUNT); state++) {
          if (auto [_, nextSymbol, nextDir] = T()(static_cast<State>(state), Alphabet::START);
                nextDir == Direction::LEFT || nextSymbol != Alphabet::START)
                return false;
        }
    return true;
    }();
  };
}// namespace turing