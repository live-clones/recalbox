#pragma once

#include <string>
#include <boost/date_time.hpp>
#include "utils/math/Vectors.h"

Transform4x4f& roundMatrix(Transform4x4f& mat);
Transform4x4f roundMatrix(const Transform4x4f& mat);

Vector3f roundVector(const Vector3f& vec);
Vector2f roundVector(const Vector2f& vec);

boost::posix_time::ptime string_to_ptime(const std::string& str, const std::string& fmt = "%Y%m%dT%H%M%S%F%q");
