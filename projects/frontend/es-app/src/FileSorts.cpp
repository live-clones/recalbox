#include "FileSorts.h"
#include "Locale.h"

namespace FileSorts
{
  std::vector<SortType> SortTypes;

  void init()
  {
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
    FileData::FileType f1 = fd1->getType();
    FileData::FileType f2 = fd2->getType();
    if (f1 == f2) return 0;                      // Both are games or folders
    if (f1 == FileData::FileType::Folder) return -1; // f1 is a folder, f2 is a game
    return 1;                                    // f2 is a folder
  }

  static int compareFavorites(FileData* const fd1, FileData* const fd2)
  {
    bool f1 = fd1->Metadata().Favorite();
    bool f2 = fd2->Metadata().Favorite();
    if (f1 == f2) return 0; // Both favorite or not
    if (f1) return -1;      // f1 is favorite, not f2
    return 1;               // f2 is favorite
  }

  #define CheckFavorites(f1, f2) { int favoriteComparison = compareFavorites(f1, f2); if (favoriteComparison != 0) return favoriteComparison; }
  #define CheckFoldersAndGames(f1, f2) { int folderComparison = compareFoldersAndGames(f1, f2); if (folderComparison != 0) return folderComparison; }

  //returns if file1 should come before file2
  ImplementSortMethod(compareFileName)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareRating)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    float c = file1->Metadata().Rating() - file2->Metadata().Rating();
    if (c < 0) { return -1; }
    if (c > 0) { return 1; }
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareTimesPlayed)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    int playCount = (file1)->Metadata().PlayCount() - (file2)->Metadata().PlayCount();
    if (playCount != 0) return playCount;
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareLastPlayed)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    TimeSpan ts = (file1)->Metadata().LastPlayed() - (file2)->Metadata().LastPlayed();
    if (ts.IsNull()) { simpleCompareUppercase(file1->getName(), file2->getName()); }
    if (ts.IsNegative()) { return -1; }
    return 1;
  }

  ImplementSortMethod(compareNumberPlayers)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    int players = (file1)->Metadata().PlayerRange() - (file2)->Metadata().PlayerRange();
    if (players != 0) return players;
    return simpleCompareUppercase(file1->getName(), file2->getName());
  }

  ImplementSortMethod(compareDevelopper)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    return simpleCompareUppercase(file1->Metadata().Developer(), file2->Metadata().Developer());
  }

  ImplementSortMethod(compareGenre)
  {
    CheckFavorites(file1, file2);
    CheckFoldersAndGames(file1, file2);

    return simpleCompareUppercase(file1->Metadata().Genre(), file2->Metadata().Genre());
  }
};
