#include <gecode/driver.hh>

// Here we specify an integer variable, x, with range from 0-4, constrained to
// be equal to 2. Is there a solution?

using namespace Gecode;

struct Lab1 : public Space {

  Gecode::IntVar x;

  Lab1() : x(*this,0,4) { rel(*this, x, IRT_EQ, 2); }

  void print() const { std::cout << "Solution: x=" << x << '\n'; }

  Lab1(Lab1 &s) : Space(s) { x.update(*this, s.x); }

  virtual Space *copy(void) { return new Lab1(*this); }
};

int main(int argc, char *argv[])
{
  Lab1 l1;
  DFS<Lab1> e(&l1);

  if (Lab1 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}
