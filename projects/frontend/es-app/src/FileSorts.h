#pragma once

#include <vector>
#include "games/FileData.h"

// TODO: typedef the compare function in FolderData.h
class FileSorts
{
  public:
    struct SortType
    {
      FileData::Comparer comparisonFunction;
      bool ascending;
      std::string description;

      SortType(FileData::Comparer sortFunction, bool sortAscending, const std::string & sortDescription)
        : comparisonFunction(sortFunction),
          ascending(sortAscending),
          description(sortDescription)
      {
      }
    };

	#define DeclareSortMethodPrototype(x) static int x(const FileData& file1, const FileData& file2);
	#define ImplementSortMethod(x) int FileSorts::x(const FileData& file1, const FileData& file2)

	DeclareSortMethodPrototype(compareSystemName)
	DeclareSortMethodPrototype(compareFileName)
	DeclareSortMethodPrototype(compareRating)
	DeclareSortMethodPrototype(compareTimesPlayed)
	DeclareSortMethodPrototype(compareLastPlayed)
	DeclareSortMethodPrototype(compareNumberPlayers)
	DeclareSortMethodPrototype(compareDevelopper)
	DeclareSortMethodPrototype(compareGenre)

	static std::vector<SortType> SortTypes;
	static std::vector<SortType> SortTypesForFavorites;

	static bool Initialize();

	static bool mInitialized;
};
