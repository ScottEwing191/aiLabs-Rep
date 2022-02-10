#include <gecode/driver.hh>

// Here we specify another integer variable, x, constrained to
// be equal to 7. Is there a solution? Can you make a change to allow
// a solution to exist?

using namespace Gecode;

struct Lab2 : public Space {

  Gecode::IntVar x;

  Lab2() : x(*this,0,7) { rel(*this, x, IRT_EQ, 7); }           // changed max from four to 8 x(*this,0,4) ==> x(*this,0,7)

  void print() const { std::cout << "Solution: x=" << x << '\n'; }

  Lab2(Lab2 &s) : Space(s) { x.update(*this, s.x); }

  virtual Space *copy(void) { return new Lab2(*this); }
};

int main(int argc, char *argv[])
{
  Lab2 l2;
  DFS<Lab2> e(&l2);

  if (Lab2 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}
