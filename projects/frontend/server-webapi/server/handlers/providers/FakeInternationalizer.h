//
// Created by bkg2k on 01/05/2020.
//

#include <string>

/*!
 * @brief We dont mind embedding a real Internationalize.
 * Just fake the main method
 */
class Internationalizer
{
  public:
    static std::string GetText(const char* p, int l);
};
