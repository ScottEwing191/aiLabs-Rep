#include <gecode/driver.hh>

// Generates pythogorean triples. Includes scaled versions of smaller
// ones; i.e. some triples have common divisors.
//
// Try using the Gist GUI. e.g. Gist::dfs(&l7);
//
// The Qt DLLs are in ../../Gecode/bin/platforms and should be found
//   aka. QT_QPA_PLATFORM_PLUGIN_PATH

using namespace Gecode;

struct Lab7 : public Space {

  const int nvars = 3;
  Gecode::IntVarArray xyz;

  Lab7() : xyz(*this, nvars, 1, 100) {
    IntVar x(xyz[0]), y(xyz[1]), z(xyz[2]);
    rel(*this, x*x + y*y == z*z);
    rel(*this, z > y);
    rel(*this, y > x);
    branch(*this, xyz, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  void print() const { std::cout << "Solution: xyz=" << xyz << '\n'; }

  Lab7(Lab7 &s) : Space(s) { xyz.update(*this, s.xyz); }

  virtual Space *copy(void) { return new Lab7(*this); }
};

int main(int argc, char *argv[])
{
  Lab7 l7;
  DFS<Lab7> e(&l7);

  while (Lab7 *s = e.next()) {
    s->print(); delete s;
  }

  return 0;
}
