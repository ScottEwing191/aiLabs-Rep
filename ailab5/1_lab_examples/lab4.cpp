#include <gecode/driver.hh>

// Here we specify that x + 2y = 0. The IntVarArray xy is given an initial size
// of 2 (i.e. nvars). Branchings determine the shape of the search tree. The
// call to "branch" below provides a variable selection strategy, and a value
// selection strategy. Here we select the variable with the smallest domain
// size first (INT_VAR_SIZE_MIN()) and assign the smallest value of the
// selected variable first (INT_VAL_MIN()). 
//
// Considering the domain provided, why is only one solution displayed?
// Can you change one of the C++ statements to display all of the solutions?

using namespace Gecode;

struct Lab4 : public Space {

  const int nvars = 2;
  Gecode::IntVarArray xy;

  Lab4() : xy(*this, nvars, -5, 4) {
    IntVar x(xy[0]), y(xy[1]);
    rel(*this, x + 2 * y == 0);
    branch(*this, xy, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  void print() const { std::cout << "Solution: xy=" << xy << '\n'; }

  Lab4(Lab4 &s) : Space(s) { xy.update(*this, s.xy); }

  virtual Space *copy(void) { return new Lab4(*this); }
};

int main(int argc, char *argv[])
{
  Lab4 l4;
  DFS<Lab4> e(&l4);

  while (Lab4 *s = e.next()) {          // changed to while
    s->print(); delete s;
  }

  return 0;
}
