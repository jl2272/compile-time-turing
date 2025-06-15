#include <Configuration.h>
#include <Direction.h>
#include <SpecialState.h>
#include <StaticTuringMachine.h>

#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <variant>

using namespace turing;

// This tests a basic turing machine that prepends a single zero to the given input.
namespace PrependTest {

// Define alphabet, states and transition function for the test.
enum class Alphabet : uint64_t { ZERO = 0, ONE = 1, START = 2, END = 3, COUNT = 4 };

enum class States : uint64_t { S = 0, R0 = 1, R1 = 2, L = 3, COUNT = 4 };

static constexpr auto TransitionFunction =
  [](States state, Alphabet alphabet) -> std::tuple<std::variant<States, SpecialState>, Alphabet, Direction> {
  switch (state) {
  case (States::S):
    switch (alphabet) {
    case (Alphabet::START):
      return { States::R0, Alphabet::START, Direction::RIGHT };
    default:
      return { SpecialState::HALT, Alphabet::START, Direction::UNCHANGED };
    }
  case (States::R0):
    switch (alphabet) {
    case (Alphabet::ZERO):
      return { States::R0, Alphabet::ZERO, Direction::RIGHT };
    case (Alphabet::ONE):
      return { States::R1, Alphabet::ZERO, Direction::RIGHT };
    case (Alphabet::END):
      return { States::L, Alphabet::ZERO, Direction::LEFT };
    default:
      return { SpecialState::HALT, Alphabet::START, Direction::UNCHANGED };
    }
  case (States::R1):
    switch (alphabet) {
    case (Alphabet::ZERO):
      return { States::R0, Alphabet::ONE, Direction::RIGHT };
    case (Alphabet::ONE):
      return { States::R1, Alphabet::ONE, Direction::RIGHT };
    case (Alphabet::END):
      return { States::L, Alphabet::ONE, Direction::LEFT };
    default:
      return { SpecialState::HALT, Alphabet::START, Direction::UNCHANGED };
    }
  case (States::L):
    switch (alphabet) {
    case (Alphabet::ZERO):
      return { States::L, Alphabet::ZERO, Direction::LEFT };
    case (Alphabet::ONE):
      return { States::L, Alphabet::ONE, Direction::LEFT };
    case (Alphabet::START):
      return { SpecialState::HALT, Alphabet::START, Direction::UNCHANGED };
    default:
      return { SpecialState::HALT, Alphabet::START, Direction::LEFT };
    }
  default:
    return { SpecialState::HALT, Alphabet::START, Direction::UNCHANGED };
  }
};

// Test 1
namespace test1 {
  // Define initial state.
  static constexpr std::array initialArray{ Alphabet::START, Alphabet::ONE, Alphabet::ONE, Alphabet::END };
  static constexpr Configuration<Alphabet, States, std::decay_t<decltype(initialArray)>> initialConfiguration{
    initialArray,
    States::S,
    0
  };

  // Compute final tape.
  static constexpr auto outputArray =
    StaticTuringMachine<Alphabet, States, decltype(TransitionFunction)>::compute<initialConfiguration>();

  // Assert this matches what we expect.
  static_assert(
    outputArray == std::array{ Alphabet::START, Alphabet::ZERO, Alphabet::ONE, Alphabet::ONE, Alphabet::END });
}// namespace test1

// Test 2
namespace test2 {
  // Define initial state.
  static constexpr std::array initialArray{ Alphabet::START,
    Alphabet::ONE,
    Alphabet::ONE,
    Alphabet::ZERO,
    Alphabet::ONE,
    Alphabet::ZERO,
    Alphabet::END };
  static constexpr Configuration<Alphabet, States, std::decay_t<decltype(initialArray)>> initialConfiguration{
    initialArray,
    States::S,
    0
  };

  // Compute final tape.
  static constexpr auto outputArray =
    StaticTuringMachine<Alphabet, States, decltype(TransitionFunction)>::compute<initialConfiguration>();

  // Assert this matches what we expect.
  static_assert(outputArray
                == std::array{ Alphabet::START,
                  Alphabet::ZERO,
                  Alphabet::ONE,
                  Alphabet::ONE,
                  Alphabet::ZERO,
                  Alphabet::ONE,
                  Alphabet::ZERO,
                  Alphabet::END });

}// namespace test2

}// namespace PrependTest

int main() { return 0; }