#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <boost/filesystem.hpp>
#include "MetaData.h"

class SystemData;

enum FileType
{
	GAME = 1,   // Cannot have children.
	FOLDER = 2
};

// todo: should be moved somewhere else because, it does no longer deals with file list
enum FileChangeType
{
	FILE_ADDED,
	FILE_RUN,
	FILE_METADATA_CHANGED,
	FILE_REMOVED,
	FILE_SORTED,
	FILE_DISPLAY_UPDATED
};

// Used for loading/saving gamelist.xml.
const char* fileTypeToString(FileType type);
FileType stringToFileType(const char* str);

// Remove (.*) and [.*] from str
std::string removeParenthesis(const std::string& str);

// A tree node that holds information for a file.
class FileData
{
public:
	FileData(FileType type, const boost::filesystem::path& path, SystemData* system);
	virtual ~FileData();

	inline const std::string& getName() const { return metadata.get("name"); }
	inline const std::string& getHash() const { return metadata.get("hash"); }
	inline FileType getType() const { return mType; }
	inline const boost::filesystem::path& getPath() const { return mPath; }
	inline FileData* getParent() const { return mParent; }
	inline const std::unordered_map<std::string, FileData*>& getChildrenByFilename() const { return mChildrenByFilename; }
	inline const std::vector<FileData*>& getChildren() const { return mChildren; }
	inline SystemData* getSystem() const { return mSystem; }
	
	virtual const std::string& getThumbnailPath() const;

	std::vector<FileData*> getFilesRecursive(unsigned int typeMask) const;
	std::vector<FileData*> getFavoritesRecursive(unsigned int typeMask) const;
	std::vector<FileData*> getHiddenRecursive(unsigned int typeMask) const;
	std::vector<FileData*> getDisplayableRecursive(unsigned int typeMask) const;
	void addChild(FileData* file); // Error if mType != FOLDER
	void removeChild(FileData* file); //Error if mType != FOLDER

	void addAlreadyExistingChild(FileData *file);
	void removeAlreadyExistingChild(FileData *file);

	void clear();

	// Returns our best guess at the "real" name for this file (will strip parenthesis and attempt to perform MAME name translation)
	std::string getCleanName() const;

	bool isSingleGameFolder() const;

	typedef bool ComparisonFunction(const FileData* a, const FileData* b);
	struct SortType
	{
		ComparisonFunction* comparisonFunction;
		bool ascending;
		std::string description;

		SortType(ComparisonFunction* sortFunction, bool sortAscending, const std::string & sortDescription) 
			: comparisonFunction(sortFunction), ascending(sortAscending), description(sortDescription) {}
	};

	static void populateRecursiveFolder(FileData* folder, const std::vector<std::string>& searchExtensions = std::vector<std::string>(), SystemData* systemData = nullptr);
	MetaDataList metadata;

private:
	FileType mType;
	boost::filesystem::path mPath;
	SystemData* mSystem;
	FileData* mParent;
	std::unordered_map<std::string,FileData*> mChildrenByFilename;
	std::vector<FileData*> mChildren;
};
