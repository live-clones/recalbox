#pragma once

#include <vector>
#include "FileData.h"

// TODO: typedef the compare function in FolderData.h
namespace FileSorts
{
	struct SortType
	{
		int (*comparisonFunction)(const FileData& a, const FileData& b);
		bool ascending;
		std::string description;

		SortType(int (*sortFunction)(const FileData& a, const FileData& b), bool sortAscending, const std::string & sortDescription)
			: comparisonFunction(sortFunction),
			  ascending(sortAscending),
			  description(sortDescription)
		{
		}
	};

	#define DeclareSortMethodPrototype(x) int x(const FileData& file1, const FileData& file2);
	#define ImplementSortMethod(x) int x(const FileData& file1, const FileData& file2)

	DeclareSortMethodPrototype(compareSystemName)
	DeclareSortMethodPrototype(compareFileName)
	DeclareSortMethodPrototype(compareRating)
	DeclareSortMethodPrototype(compareTimesPlayed)
	DeclareSortMethodPrototype(compareLastPlayed)
	DeclareSortMethodPrototype(compareNumberPlayers)
	DeclareSortMethodPrototype(compareDevelopper)
	DeclareSortMethodPrototype(compareGenre)

	extern std::vector<SortType> SortTypes;
	extern std::vector<SortType> SortTypesForFavorites;
	void init();
}
