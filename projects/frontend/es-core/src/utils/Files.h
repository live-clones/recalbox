#pragma once

#include <utils/os/fs/Path.h>

class Files
{
  private:
    static constexpr long sBufferSizeBits = 20;
    static constexpr long sBufferSize = 1 << sBufferSizeBits;
    static constexpr long sBufferSizeMask = (sBufferSize - 1);

  public:
    /*!
     * @brief Load the whole content of a file into a string
     * @param path File to load
     * @return File content
     */
    static std::string LoadFile(const Path& path);

    /*!
     * @brief Load partial content of a file into a string
     * @param path File to load
     * @param from Offset to read from
     * @param size Size to read
     * @return Partial file content
     */
    static std::string LoadFile(const Path& path, long long from, long long size);

    /*!
     * @brief Save the given string into a file
     * @param path File path
     * @param content String ot save
     * @return True if the content has been saved
     */
    static bool SaveFile(const Path& path, const std::string& content);

    /*!
     * @brief Append the given string at the end of the given file or create it if it does not exist
     * @param path File path
     * @param data Data buffer to append
     * @param size Data size to append
     * @return True if the content has been saved
     */
    static bool AppendToFile(const Path& path, const void* data, int size);

    /*!
     * @brief Append the given string at the end of the given file or create it if it does not exist
     * @param path File path
     * @param content String ot append
     * @return True if the content has been saved
     */
    static bool AppendToFile(const Path& path, const std::string& content)
    {
      return AppendToFile(path, content.c_str(), content.length());
    }

    /*!
     * @brief Copy a single file
     * @param from Source file
     * @param to Destination file (fullpath)
     * @return True if the copy is successful, false otherwise
     */
    static bool CopyFile(const Path& from, const Path& to);

    /*!
     * @brief Copy a whole folder, including all files and folders
     * @param from Source folder
     * @param to Destination folder (fullpath)
     * @param recurse Set to true to copy all files & folders recusively
     * @return True if the copy is successful, false otherwise
     */
    static bool CopyFolder(const Path& from, const Path& to, bool recurse);
};
