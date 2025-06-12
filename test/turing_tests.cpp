#include <Configuration.h>
#include <Direction.h>
#include <SpecialState.h>
#include <StaticTuringMachine.h>

#include <array>
#include <cstdint>
#include <tuple>
#include <variant>

using namespace turing;

enum class CopyAlphabet : uint64_t { ZERO = 0, ONE = 1, START = 2, END = 3, COUNT = 4 };

enum class CopyStates : uint64_t { S = 0, R0 = 1, R1 = 2, L = 3, COUNT = 4 };

static constexpr auto CopyTransitionFunction =
  [](CopyStates state,
    CopyAlphabet alphabet) -> std::tuple<std::variant<CopyStates, SpecialState>, CopyAlphabet, Direction> {
  switch (state) {
  case (CopyStates::S):
    switch (alphabet) {
    case (CopyAlphabet::START):
      return { CopyStates::R0, CopyAlphabet::START, Direction::RIGHT };
    default:
      return { SpecialState::HALT, CopyAlphabet::START, Direction::UNCHANGED };
    }
  case (CopyStates::R0):
    switch (alphabet) {
    case (CopyAlphabet::ZERO):
      return { CopyStates::R0, CopyAlphabet::ZERO, Direction::RIGHT };
    case (CopyAlphabet::ONE):
      return { CopyStates::R1, CopyAlphabet::ZERO, Direction::RIGHT };
    case (CopyAlphabet::END):
      return { CopyStates::L, CopyAlphabet::ZERO, Direction::LEFT };
    default:
      return { SpecialState::HALT, CopyAlphabet::START, Direction::UNCHANGED };
    }
  case (CopyStates::R1):
    switch (alphabet) {
    case (CopyAlphabet::ZERO):
      return { CopyStates::R0, CopyAlphabet::ONE, Direction::RIGHT };
    case (CopyAlphabet::ONE):
      return { CopyStates::R1, CopyAlphabet::ONE, Direction::RIGHT };
    case (CopyAlphabet::END):
      return { CopyStates::L, CopyAlphabet::ONE, Direction::LEFT };
    default:
      return { SpecialState::HALT, CopyAlphabet::START, Direction::UNCHANGED };
    }
  case (CopyStates::L):
    switch (alphabet) {
    case (CopyAlphabet::ZERO):
      return { CopyStates::L, CopyAlphabet::ZERO, Direction::LEFT };
    case (CopyAlphabet::ONE):
      return { CopyStates::L, CopyAlphabet::ONE, Direction::LEFT };
    case (CopyAlphabet::START):
      return { SpecialState::HALT, CopyAlphabet::START, Direction::UNCHANGED };
    default:
      return { SpecialState::HALT, CopyAlphabet::START, Direction::LEFT };
    }
  default:
    return { SpecialState::HALT, CopyAlphabet::START, Direction::UNCHANGED };
  }
};


static constexpr std::array testArray{ CopyAlphabet::START, CopyAlphabet::ONE, CopyAlphabet::ONE, CopyAlphabet::END };
static constexpr Configuration<CopyAlphabet, CopyStates, std::array<CopyAlphabet, 4>> config{ testArray,
  CopyStates::S,
  0 };
static constexpr auto outArray =
  StaticTuringMachine<CopyAlphabet, CopyStates, decltype(CopyTransitionFunction)>::compute<4, config>();

static_assert(
  outArray
  == std::array{ CopyAlphabet::START, CopyAlphabet::ZERO, CopyAlphabet::ONE, CopyAlphabet::ONE, CopyAlphabet::END });

int main()
{
  return 0;
}