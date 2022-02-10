#include <gecode/driver.hh>

// Here we specify an integer variable, x, which is constained to
// be greater than 4, and less than 20. How many solutions are there?
//
// Now use the concise linear equation notation we saw on slide 24 of
// the lecture slides (C++ expression templates). Add a new call to
// rel - but first remove the two existing calls to rel.

using namespace Gecode;

struct Lab3 : public Space {

  Gecode::IntVar x;

  Lab3() : x(*this,0,9) {
    rel(*this, x, IRT_GR, 4);
    rel(*this, x, IRT_LE, 20);
  }

  void print() const { std::cout << "Solution: x=" << x << '\n'; }

  Lab3(Lab3 &s) : Space(s) { x.update(*this, s.x); }

  virtual Space *copy(void) { return new Lab3(*this); }
};

int main(int argc, char *argv[])
{
  Lab3 l3;
  DFS<Lab3> e(&l3);

  if (Lab3 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}
