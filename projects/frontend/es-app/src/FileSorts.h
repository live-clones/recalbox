#pragma once

#include <vector>
#include "FileData.h"

namespace FileSorts
{
	struct SortType
	{
		int (*comparisonFunction)(FileData* const a, FileData* const b);
		bool ascending;
		std::string description;

		SortType(int (*sortFunction)(FileData* const a, FileData* const b), bool sortAscending, const std::string & sortDescription)
			: comparisonFunction(sortFunction), ascending(sortAscending), description(sortDescription) {}
	};

	#define DeclareSortMethodPrototype(x) int x(FileData* const file1, FileData* const file2);
	#define ImplementSortMethod(x) int x(FileData* const file1, FileData* const file2)

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
};
