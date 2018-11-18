#include "FileSorts.h"
#include "Locale.h"

namespace FileSorts
{
  std::vector<SortType> SortTypes;
  
  void init() {
	SortTypes.push_back(SortType(&compareFileName, true, "\uF15d " + _("FILENAME")));
	SortTypes.push_back(SortType(&compareFileName, false, "\uF15e " + _("FILENAME")));
	SortTypes.push_back(SortType(&compareRating, true, "\uF165 " + _("RATING")));
	SortTypes.push_back(SortType(&compareRating, false, "\uF164 " + _("RATING")));
	SortTypes.push_back(SortType(&compareTimesPlayed, true, "\uF160 " + _("TIMES PLAYED")));
	SortTypes.push_back(SortType(&compareTimesPlayed, false, "\uF161 " + _("TIMES PLAYED")));
	SortTypes.push_back(SortType(&compareLastPlayed, true, "\uF160 " + _("LAST PLAYED")));
	SortTypes.push_back(SortType(&compareLastPlayed, false, "\uF161 " + _("LAST PLAYED")));
	SortTypes.push_back(SortType(&compareNumberPlayers, true, "\uF162 " + _("NUMBER OF PLAYERS")));
	SortTypes.push_back(SortType(&compareNumberPlayers, false, "\uF163 " + _("NUMBER OF PLAYERS")));
	SortTypes.push_back(SortType(&compareDevelopper, true, "\uF15d " + _("DEVELOPER")));
	SortTypes.push_back(SortType(&compareDevelopper, false, "\uF15e " + _("DEVELOPER")));
	SortTypes.push_back(SortType(&compareGenre, true, "\uF15d " + _("GENRE")));
	SortTypes.push_back(SortType(&compareGenre, false, "\uF15e " + _("GENRE")));
  }

  static int SimpleCompareUppercase(const std::string& a, const std::string& b)
	{
		for(const char *ap = a.c_str(), *bp = b.c_str();; ap++, bp++)
		{
		  int c1 = std::toupper(*ap);
		  int c2 = std::toupper(*bp);
      if ((c1 | c2) == 0) return 0;
		  int c = c1 - c2;
			if (c != 0) return c;
		}
	}

	//returns if file1 should come before file2
	ImplementSortMethod(compareFileName)
	{
  	return SimpleCompareUppercase(file1->getName(), file2->getName());
	}

	ImplementSortMethod(compareRating)
	{
		//only games have rating metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			float c = file1->Metadata().Rating() - file2->Metadata().Rating();
			if (c < 0) return - 1;
			if (c > 0) return 1;
			return 0;
		}

		return 0;
	}

	ImplementSortMethod(compareTimesPlayed)
	{
		//only games have playcount metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			return (file1)->Metadata().PlayCount() - (file2)->Metadata().PlayCount();
		}

		return 0;
	}

	ImplementSortMethod(compareLastPlayed)
	{
		//only games have lastplayed metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			TimeSpan ts = (file1)->Metadata().LastPlayed() - (file2)->Metadata().LastPlayed();
      if (ts.IsNull()) return 0;
			if (ts.IsNegative()) return -1;
      return 1;
		}

		return 0;
	}

	ImplementSortMethod(compareNumberPlayers)
	{
		//only games have lastplayed metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			return (file1)->Metadata().PlayerRange() - (file2)->Metadata().PlayerRange();
		}

		return 0;
	}

	ImplementSortMethod(compareDevelopper)
	{
		//only games have developper metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			return SimpleCompareUppercase(file1->Metadata().Developer(), file2->Metadata().Developer());
		}

		return 0;
	}

	ImplementSortMethod(compareGenre)
	{
		//only games have genre metadata
		if (MetadataDescriptor::AreGames(file1->Metadata(), file2->Metadata()))
		{
			return SimpleCompareUppercase(file1->Metadata().Genre(), file2->Metadata().Genre());
		}

		return 0;
	}

};
