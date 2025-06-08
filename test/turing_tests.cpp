
#include <Direction.h>
#include <StaticTuringMachine.h>

#include <cstdint>
#include <tuple>

static_assert(1 == 1);

namespace turing {

enum class TestAlphabet : uint64_t { A = 0, B = 1 };

enum class TestStates : uint64_t { A = 0, B = 1, HALT = 2, YES = 3, NO = 4 };

static constexpr auto testTransitionFunction = [](TestStates, TestAlphabet) {
  return std::make_tuple(TestStates::A, TestAlphabet::A, Direction::UNCHANGED);
};

}// namespace turing

using namespace turing;

int main() { return StaticTuringMachine<TestAlphabet, TestStates, decltype(testTransitionFunction)>::x; }