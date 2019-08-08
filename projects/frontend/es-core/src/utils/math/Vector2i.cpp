#include "Vector2i.h"
#include "Vector2f.h"

Vector2f Vector2i::toFloat() const
{
  return Vector2f((float)mX, (float)mY);
}
