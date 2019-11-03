#include "Util.h"
#include "platform.h"
#include "Locale.h"

::Transform4x4f& roundMatrix(::Transform4x4f& mat)
{
	mat.translation()[0] = round(mat.translation()[0]);
	mat.translation()[1] = round(mat.translation()[1]);
	return mat;
}

::Transform4x4f roundMatrix(const ::Transform4x4f& mat)
{
	::Transform4x4f ret = mat;
	roundMatrix(ret);
	return ret;
}

::Vector3f roundVector(const ::Vector3f& vec)
{
	::Vector3f ret = vec;
	ret[0] = round(ret[0]);
	ret[1] = round(ret[1]);
	ret[2] = round(ret[2]);
	return ret;
}

::Vector2f roundVector(const ::Vector2f& vec)
{
	::Vector2f ret = vec;
	ret[0] = round(ret[0]);
	ret[1] = round(ret[1]);
	return ret;
}

boost::posix_time::ptime string_to_ptime(const std::string& str, const std::string& fmt)
{
	std::istringstream ss(str);
	ss.imbue(std::locale(std::locale::classic(), new boost::posix_time::time_input_facet(fmt))); //std::locale handles deleting the facet
	boost::posix_time::ptime time;
	ss >> time;

	return time;
}
