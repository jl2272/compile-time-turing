#include <Configuration.h>
#include <Direction.h>
#include <StaticTuringMachine.h>

#include <array>
#include <cstdint>
#include <tuple>

static_assert(1 == 1);

namespace turing {

enum class TestAlphabet : uint64_t { A = 0, B = 1, START = 2, END = 3, COUNT = 4 };

enum class TestStates : uint64_t { A = 0, B = 1, COUNT = 2 };

static constexpr auto testTransitionFunction = [](TestStates, TestAlphabet) {
  return std::make_tuple(TestStates::A, TestAlphabet::START, Direction::UNCHANGED);
};

}// namespace turing

using namespace turing;

static constexpr std::array testArray{ TestAlphabet::START, TestAlphabet::END };
static constexpr Configuration<TestAlphabet, TestStates, std::array<TestAlphabet, 2>> config{ testArray,
  TestStates::A,
  0 };
static constexpr auto outArray =
  StaticTuringMachine<TestAlphabet, TestStates, decltype(testTransitionFunction)>::compute<2, config>();

int main()
{
  return 0;
}