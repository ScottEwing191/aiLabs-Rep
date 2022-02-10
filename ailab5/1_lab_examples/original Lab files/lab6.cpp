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

};

int main(int argc, char *argv[])
{
  /*Lab6 l6;
  DFS<Lab6> e(&l6);

  while (Lab6 *s = e.next()) {
    s->print(); delete s;
  }*/

  return 0;
}

