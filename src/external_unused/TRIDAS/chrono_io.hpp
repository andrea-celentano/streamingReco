#ifndef TRIDAS_CHRONO_IO_HPP
#define TRIDAS_CHRONO_IO_HPP

#include <chrono>
#include <ratio>
#include <istream>
#include <ostream>

namespace std {
template<class Rep, std::intmax_t Num, std::intmax_t Den>
ostream& operator <<(ostream& s, chrono::duration<Rep, ratio<Num, Den>> t)
{
  return s << t.count() << " [" << Num << '/' << Den << "] seconds";
}

template<class Rep, std::intmax_t Num, std::intmax_t Den>
istream& operator >>(istream& s, chrono::duration<Rep, ratio<Num, Den>>& t)
{
  Rep x;
  s >> x;
  t = std::chrono::duration<Rep, std::ratio<Num, Den>>(x);
  return s;
}
}

#endif // TRIDAS_CHRONO_IO_HPP
