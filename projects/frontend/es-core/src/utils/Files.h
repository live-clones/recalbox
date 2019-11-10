#pragma once

#include <utils/os/fs/Path.h>

class Files
{
  public:
    /*!
     * @brief Load the whole content of a file into a string
     * @param path File to load
     * @return File content
     */
    static std::string LoadFile(const Path& path);

    /*!
     * @brief Save the given string into a file
     * @param path File path
     * @param content String ot save
     * @return True if the content has been saved
     */
    static bool SaveFile(const Path& path, const std::string& content);
};
