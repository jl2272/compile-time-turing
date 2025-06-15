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
  template<auto input>
    requires std::same_as<decltype(input),
      Configuration<Alphabet, State, std::array<Alphabet, decltype(input)::TapeSize>>>
  static consteval auto compute()
  {
    static_assert(input.tape.front() == Alphabet::START);
    static_assert(input.tape.back() == Alphabet::END);
    constexpr std::size_t InputSize = decltype(input)::TapeSize;

    constexpr auto transitionResult = transition(input.state, input.tape[input.head]);
    constexpr std::variant<State, SpecialState> nextState = std::get<0>(transitionResult);
    constexpr auto nextSymbol = std::get<1>(transitionResult);
    constexpr auto nextDirection = std::get<2>(transitionResult);

    constexpr std::size_t NextSize = []() {
      if (nextDirection == Direction::RIGHT && input.head == InputSize - 2) {
        return InputSize + 1;
      } else {
        return InputSize;
      }
    }();

    constexpr auto populate = [&]() {
      std::array<Alphabet, NextSize> out;
      out.back() = Alphabet::END;
      for (std::size_t i = 0; i < input.tape.size(); i++) { out[i] = input.tape[i]; }
      out[input.head] = nextSymbol;
      return out;
    };
    constexpr std::array<Alphabet, NextSize> out = populate();

    constexpr std::size_t nextHead = [&]() {
      std::size_t nextHead_ = input.head;
      switch (nextDirection) {

      case (Direction::LEFT):
        nextHead_ -= 1;
        break;
      case (Direction::RIGHT):
        nextHead_ += 1;
        break;
      default:
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
      return compute<nextConfiguration>();
    }
  }
};

}// namespace turing