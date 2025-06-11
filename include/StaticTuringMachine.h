#pragma once

#include <Configuration.h>
#include <Direction.h>
#include <SpecificationConcepts.h>

#include <vector>

namespace turing {

template<IsAlphabet Alphabet, IsState State, IsTransitionFunction<State, Alphabet> Transition> class StaticTuringMachine
{
  static constexpr Transition transition;

public:
  template<size_t InputSize, Configuration<Alphabet, State, std::array<Alphabet, InputSize>> input>
  static consteval auto compute()
  {

    // Decomposition declaration not allowed for constexpr values.
    constexpr auto transitionResult = transition(input.state, input.tape[input.head]);
    constexpr std::variant<State, SpecialState> nextState = std::get<0>(transitionResult);
    constexpr auto nextSymbol = std::get<1>(transitionResult);
    constexpr auto nextDirection = std::get<2>(transitionResult);

    constexpr size_t NextSize = []() {
      if (nextDirection == Direction::RIGHT && input.head == InputSize - 1) {
        return InputSize + 1;
      } else {
        return InputSize;
      }
    }();

    constexpr auto populate = [&](auto in) {
      std::array<Alphabet, NextSize> out;
      out.back() = Alphabet::END;
      for (size_t i = 0; i < input.tape.size(); i++) { out[i] = input.tape[i]; }
      out[input.head] = nextSymbol;
      return out;
    };
    constexpr std::array<Alphabet, NextSize> out = populate(input);

    constexpr size_t nextHead = [&]() {
      size_t nextHead_;
      switch (nextDirection) {

      case (Direction::LEFT):
        nextHead_ = input.head - 1;
        break;
      case (Direction::RIGHT):
        nextHead_ = input.head + 1;
        break;
      default:
        nextHead_ = input.head;
        break;
      }
      return nextHead_;
    }();
    if constexpr (std::holds_alternative<SpecialState>(nextState)) {
      return out;
    } else {
      constexpr Configuration<Alphabet, State, std::decay_t<decltype(out)>> nextConfiguration{
        out, std::get<State>(nextState), nextHead
      };
      return compute<NextSize, nextConfiguration>();
    }
  }
  static constexpr int x = 4;
};

}// namespace turing