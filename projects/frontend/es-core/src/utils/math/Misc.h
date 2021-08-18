#pragma once

#include <cmath>

class Math
  {
    public:
      static constexpr double PiD = 3.1415926535897932384626433832795028841971693993751058209749445923;
      static constexpr float  PiF = 3.1415926535897932384626433832795028841971693993751058209749445923f;

      static float  Radian2Degree(float r)  { return (float)((r) * (180.0f / PiF)); }
      static double Radian2Degree(double r) { return ((r) * (180.0 / PiD)); }
      static float  Degree2Radian(float d)  { return (float)((d) * (PiF / 180.0f)); }
      static double Degree2Radian(double d) { return ((d) * (PiD /180.0)); }

      static float  cos         (float _num) { return ::cosf(_num); }
      static float  sin         (float _num) { return ::sinf(_num); }
      static float  floor       (float _num) { return ::floorf(_num); }
      static float  ceil        (float _num) { return ::ceilf(_num); }
      static int    floori      (float _num) { return (int)::floorf(_num); }
      static int    ceili       (float _num) { return (int)::ceilf(_num); }

      static int    min         (int _num1, int _num2) { return (_num1 < _num2) ? _num1 : _num2; }
      static int    max         (int _num1, int _num2) { return (_num1 > _num2) ? _num1 : _num2; }
      static float  min         (float _num1, float _num2) { return (_num1 < _num2) ? _num1 : _num2; }
      static float  max         (float _num1, float _num2) { return (_num1 > _num2) ? _num1 : _num2; }
      static double min         (double _num1, double _num2) { return (_num1 < _num2) ? _num1 : _num2; }
      static double max         (double _num1, double _num2) { return (_num1 > _num2) ? _num1 : _num2; }
      static float  clamp       (float _num, float _min, float _max) { return max(min(_num, _max), _min); }
      static double clampf      (double _num, double _min, double _max) { return max(min(_num, _max), _min); }
      static int    clampi      (int _num, int _min, int _max) { return max(min(_num, _max), _min); }
      static float  round       (float _num) { _num += 0.5f; int num = (int)_num; return (float)num; }
      static double round       (double _num) { _num += 0.5; int num = (int)_num; return (double)num; }
      static float  integer     (float _num) { int num = (int)_num; return (float)num; }
      static double integer     (double _num) { int num = (int)_num; return (double)num; }
      static int    roundi      (float _num) { _num += 0.5f; return  (int)_num; }

      static float  lerp        (float _start, float _end, float _fraction);
      static float  smoothStep  (float _left, float _right, float _x);
      static float  smootherStep(float _left, float _right, float _x);

      static float  bounce      (float _delayTime, float _scrollTime, float _currentTime, float _scrollLength);
      static float  loop        (float _delayTime, float _scrollTime, float _currentTime, float _scrollLength);

      static int    absi        (int value) { int m = value >> (sizeof(int) * 8 - 1); return (value + m) ^ m; }
      static float  abs         (float value) { return (value < 0.f) ? -value : value; }
  };
