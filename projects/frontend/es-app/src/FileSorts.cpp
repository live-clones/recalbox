#include "FileSorts.h"
#include "Locale.h"
#include "SystemData.h"

namespace FileSorts
{
  std::vector<SortType> SortTypes;
  std::vector<SortType> SortTypesForFavorites;

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


      // Keep FILENAME sort at the begining to keep the Jump to next / previous letter working the same way
      SortTypesForFavorites.insert(SortTypesForFavorites.end(), SortTypes.begin(), SortTypes.begin() + 2);
      SortTypesForFavorites.push_back(SortType(&compareSystemName, true, "\uF166 " + _("SYSTEM NAME")));
      SortTypesForFavorites.push_back(SortType(&compareSystemName, false, "\uF167 " + _("SYSTEM NAME")));
      SortTypesForFavorites.insert(SortTypesForFavorites.end(), SortTypes.begin() + 2, SortTypes.end());
  }

  static int simpleCompareUppercase(const std::string& a, const std::string& b)
  {
    for (const char* ap = a.c_str(), * bp = b.c_str();; ap++, bp++)
    {
      int c1 = std::toupper(*ap);
      int c2 = std::toupper(*bp);
      if ((c1 | c2) == 0) { return 0; }
      int c = c1 - c2;
      if (c != 0) { return c; }
    }
  }

  static int compareFoldersAndGames(FileData* const fd1, FileData* const fd2)
  {
    ItemType f1 = fd1->getType();
    ItemType f2 = fd2->getType();
    if (f1 == f2) return 0;                      // Both are games or folders
    if (f1 == ItemType::Folder) return -1; // f1 is a folder, f2 is a game
    return 1;                                    // f2 is a folder
  }

  #define CheckFoldersAndGames(f1, f2) { int folderComparison = compareFoldersAndGames(f1, f2); if (folderComparison != 0) return folderComparison; }

  //returns if file1 should come before file2

  ImplementSortMethod(compareSystemName)
  {
    const SystemData * system1 = file1->getSystem();
    const SystemData * system2 = file2->getSystem();
    const int result = simpleCompareUppercase(system1->getName(), system2->getName());
    if (result) { return result; }
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareFileName)
  {
    CheckFoldersAndGames(file1, file2);
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareRating)
  {
    CheckFoldersAndGames(file1, file2);
    float c = file1->Metadata().Rating() - file2->Metadata().Rating();
    if (c < 0) { return -1; }
    if (c > 0) { return 1; }
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareTimesPlayed)
  {
    CheckFoldersAndGames(file1, file2);
    int playCount = (file1)->Metadata().PlayCount() - (file2)->Metadata().PlayCount();
    if (playCount != 0) return playCount;
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareLastPlayed)
  {
    CheckFoldersAndGames(file1, file2);
    int ep1 = (file1)->Metadata().LastPlayedEpoc();
    int ep2 = (file2)->Metadata().LastPlayedEpoc();
    if (!ep1 || !ep2) {
      if (ep1) return 1;
      if (ep2) return -1;
      return  simpleCompareUppercase(file1->getName(), file2->getName());
    }
    return ep1 < ep2 ? -1 : 1;
  }

  ImplementSortMethod(compareNumberPlayers)
  {
    CheckFoldersAndGames(file1, file2);
    int players = (file1)->Metadata().PlayerRange() - (file2)->Metadata().PlayerRange();
    if (players != 0) return players;
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareDevelopper)
  {
    CheckFoldersAndGames(file1, file2);
    return simpleCompareUppercase(file1->Metadata().Developer(), file2->Metadata().Developer());
  }

  ImplementSortMethod(compareGenre)
  {
    CheckFoldersAndGames(file1, file2);
    return simpleCompareUppercase(file1->Metadata().Genre(), file2->Metadata().Genre());
  }
};
