#pragma once

#include <SpecificationConcepts.h>

namespace turing {
template<typename T, typename Alphabet>
concept IsTape = requires(T tape) {
  IsAlphabet<Alphabet>;
  requires std::tuple_size<T>::value >= 0;
  requires std::is_same_v<std::array<Alphabet, std::tuple_size<T>::value>, std::decay_t<T>>;
};
}// namespace turing