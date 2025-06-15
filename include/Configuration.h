#pragma once

#include <ConfigurationConcepts.h>
#include <SpecificationConcepts.h>

namespace turing {
template<IsAlphabet Alphabet, IsState State, IsTape<Alphabet> Tape> struct Configuration
{
  static constexpr std::size_t TapeSize = std::tuple_size<Tape>::value;
  Tape tape;
  State state;
  std::size_t head;
};
}// namespace turing