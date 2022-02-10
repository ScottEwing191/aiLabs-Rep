#include <gecode/driver.hh>

// The following code finds a solution to the problem:
// If x, y and z are distinct integers from 1 to 3, and
// x - y - z is equal to -4, what are x, y and z?
//
// Is there only one solution?

using namespace Gecode;

struct Lab5 : public Space {

  const int nvars = 3;
  Gecode::IntVarArray xyz;

  Lab5() : xyz(*this, nvars, 1, 3) {
    IntVar x(xyz[0]), y(xyz[1]), z(xyz[2]);
    rel(*this, x - y - z == -4);
    distinct(*this,xyz);
    branch(*this, xyz, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  void print() const { std::cout << "Solution: xyz=" << xyz << '\n'; }

  Lab5(Lab5 &s) : Space(s) {
    xyz.update(*this, s.xyz);
  }

  virtual Space *copy(void) { return new Lab5(*this); }
};

int main(int argc, char *argv[])
{
  Lab5 l5;
  DFS<Lab5> e(&l5);

  if (Lab5 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}

