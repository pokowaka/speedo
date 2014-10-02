template <class A>
template <class B>
class Translator {
  public:
  int to(A* in, int cIn, B* out, int cOut) = 0;
  int from(B* in, int cIn, A* out, int cOut) = 0;

  int releaseTo(B* out, int cOut);
  int releaseFrom(A* out, int cOut);
}

