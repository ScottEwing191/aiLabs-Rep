#include <gecode/driver.hh>

// Find a, b, c, d, and e given that each are distinct integers from 1 to 5,
// such that the following equation holds true:
//
//                     a + b - c x d / e = 6
//
// An expression template will form the main constraint, but you may also want
// to restrict the (integer) division result to ensure there is no remainer.

using namespace Gecode;

struct Lab6 : public Space {

  const int nvars = 5;
  Gecode::IntVarArray sol;

  Lab6() : sol(*this, nvars, 1, 5) {
    IntVar a(sol[0]), b(sol[1]), c(sol[2]), d(sol[3]), e(sol[4]);
    rel(*this, a + b - c * d / e == 6);
    rel(*this, c * d % e == 0); // n.b. avoids zero integer division results
                                // e.g. 2, 4, 1, 3, 5 (2 + 4 - 1 * 3 / 5)
                                // i.e. 3/5 is 0 with integer division

    // rel(*this, c * d / e != 0); // This looks like a simpler version of
                                   // above, ...but are you happy with 4/3==1
                                   // and so a solution like: 2, 5, 1, 4, 3 ?
    distinct(*this, sol);
    branch(*this, sol, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  void print() const { std::cout << "Solution: sol=" << sol << '\n'; }

  Lab6(Lab6& s) : Space(s) {
    sol.update(*this, s.sol);
  }

  virtual Space* copy() { return new Lab6(*this); }

};

int main(int argc, char *argv[])
{
  Lab6 l6;
  DFS<Lab6> e(&l6);

  while (Lab6 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}

