#pragma once

#include <list>
#include <string>

class FileSystemUtil
{
	public:
		typedef std::list<std::string> stringList;

		enum class EntryTypes
		{
			File = 1,
			Folder = 2,
			All = 3
		};

		static stringList  getDirContent      (const std::string& _path, EntryTypes types, bool _recursive = false);
    static stringList  getPathList        (const std::string& _path);
    static std::string getHomePath        ();
    static std::string getCWDPath         ();
    static std::string getExePath         ();
    static std::string getPreferredPath   (const std::string& _path);
    static std::string getGenericPath     (const std::string& _path);
    static std::string getEscapedPath     (const std::string& _path);
    static std::string getCanonicalPath   (const std::string& _path);
    static std::string getAbsolutePath    (const std::string& _path, const std::string& _base = getCWDPath());
    static std::string getParent          (const std::string& _path);
    static std::string getFileName        (const std::string& _path);
    static std::string getStem            (const std::string& _path);
    static std::string getExtension       (const std::string& _path);
    static std::string resolveRelativePath(const std::string& _path, const std::string& _relativeTo, const bool _allowHome);
    static std::string createRelativePath (const std::string& _path, const std::string& _relativeTo, const bool _allowHome);
    static std::string removeCommonPath   (const std::string& _path, const std::string& _common, bool& _contains);
    static std::string resolveSymlink     (const std::string& _path);
    static bool        removeFile         (const std::string& _path);
    static bool        createDirectories  (const std::string& _path);
    static bool        exists             (const std::string& _path);
    static bool        isAbsolute         (const std::string& _path);
    static bool        isRegularFile      (const std::string& _path);
    static bool        isDirectory        (const std::string& _path);
    static bool        isSymlink          (const std::string& _path);
    static bool        isHidden           (const std::string& _path);
};
