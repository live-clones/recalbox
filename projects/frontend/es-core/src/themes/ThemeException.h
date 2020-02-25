//
// Created by bkg2k on 10/11/2019.
//
#pragma once

#include <deque>

#include <utils/os/fs/Path.h>

class ThemeException : public std::exception
{
  private:
    std::string msg;

  public:
    const char* what() const noexcept override { return msg.c_str(); }

    ThemeException() = default;
    explicit ThemeException(const std::string& s) { msg = s; }
    ThemeException(const std::string& s, const std::deque<Path>& p) { msg = AddFiles(p) + s; }

    static std::string AddFiles(const std::deque<Path>& deque)
    {
      std::string result;
      result = "from theme \"" + deque.front().ToString() + "\"\n";
      for (auto it = deque.begin() + 1; it != deque.end(); it++)
        result += "  (from included file \"" + (*it).ToString() + "\")\n";
      result += "    ";
      return result;
    }
};

