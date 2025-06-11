#pragma once

#include <ConfigurationConcepts.h>
#include <SpecificationConcepts.h>

namespace turing {
template<IsAlphabet Alphabet, IsState State, IsTape<Alphabet> Tape> struct Configuration
{
  Tape tape;
  State state;
  size_t head;
};
}// namespace turing