#pragma once

#define	ES_PI (3.1415926535897932384626433832795028841971693993751058209749445923)
#define	ES_RAD_TO_DEG(_x) ((_x) * (180.0 / ES_PI))
#define	ES_DEG_TO_RAD(_x) ((_x) * (ES_PI / 180.0))

class Math
{
  public:
    // added here to avoid including math.h whenever these are used
    static float cosf        (float _num);
    static float sinf        (float _num);
    static float floorf      (float _num);
    static float ceilf       (float _num);

    static int   min         (int _num1, int _num2);
    static int   max         (int _num1, int _num2);
    static float min         (float _num1, float _num2);
    static float max         (float _num1, float _num2);
    static float clamp       (float _num, float _min, float _max);
    static float round       (float _num);
    static float lerp        (float _start, float _end, float _fraction);
    static float smoothStep  (float _left, float _right, float _x);
    static float smootherStep(float _left, float _right, float _x);

    static float bounce      (float _delayTime, float _scrollTime, float _currentTime, float _scrollLength);
    static float loop       ( float _delayTime, float _scrollTime, float _currentTime, float _scrollLength);
};
