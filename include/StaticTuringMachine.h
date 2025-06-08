#pragma once
#include <Concepts.h>
#include <Direction.h>

namespace turing {

template<IsAlphabet Alphabet, IsState State, IsTransitionFunction<State,Alphabet> Transition>
class StaticTuringMachine
{
public:
  static constexpr int x = 4;
};

}// namespace turing