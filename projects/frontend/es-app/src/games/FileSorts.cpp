#include "utils/locale/LocaleHelper.h"
#include "FileSorts.h"
#include "systems/SystemData.h"

std::vector<FileSorts::SortType> FileSorts::sAllSorts;
bool FileSorts::mInitialized = false;

bool FileSorts::Initialize()
{
  if (!mInitialized)
  {
    sAllSorts.push_back(SortType(Sorts::FileNameAscending, &compareFileName, true, "\uF15d " + _("FILENAME")));
    sAllSorts.push_back(SortType(Sorts::FileNameDescending, &compareFileName, false, "\uF15e " + _("FILENAME")));
    sAllSorts.push_back(SortType(Sorts::RatingAscending, &compareRating, true, "\uF165 " + _("RATING")));
    sAllSorts.push_back(SortType(Sorts::RatingDescending, &compareRating, false, "\uF164 " + _("RATING")));
    sAllSorts.push_back(SortType(Sorts::TimesPlayedAscending, &compareTimesPlayed, true, "\uF160 " + _("TIMES PLAYED")));
    sAllSorts.push_back(SortType(Sorts::TimesPlayedDescending, &compareTimesPlayed, false, "\uF161 " + _("TIMES PLAYED")));
    sAllSorts.push_back(SortType(Sorts::LastPlayedAscending, &compareLastPlayed, true, "\uF160 " + _("LAST PLAYED")));
    sAllSorts.push_back(SortType(Sorts::LastPlayedDescending, &compareLastPlayed, false, "\uF161 " + _("LAST PLAYED")));
    sAllSorts.push_back(SortType(Sorts::PlayersAscending, &compareNumberPlayers, true, "\uF162 " + _("NUMBER OF PLAYERS")));
    sAllSorts.push_back(SortType(Sorts::PlayersDescending, &compareNumberPlayers, false, "\uF163 " + _("NUMBER OF PLAYERS")));
    sAllSorts.push_back(SortType(Sorts::DevelopperAscending, &compareDevelopper, true, "\uF15d " + _("DEVELOPER")));
    sAllSorts.push_back(SortType(Sorts::DevelopperDescending, &compareDevelopper, false, "\uF15e " + _("DEVELOPER")));
    sAllSorts.push_back(SortType(Sorts::PublisherAscending, &comparePublisher, true, "\uF15d " + _("PUBLISHER")));
    sAllSorts.push_back(SortType(Sorts::PublisherDescending, &comparePublisher, false, "\uF15e " + _("PUBLISHER")));
    sAllSorts.push_back(SortType(Sorts::GenreAscending, &compareGenre, true, "\uF15d " + _("GENRE")));
    sAllSorts.push_back(SortType(Sorts::GenreDescending, &compareGenre, false, "\uF15e " + _("GENRE")));
    sAllSorts.push_back(SortType(Sorts::SystemAscending, &compareSystemName, true, "\uF166 " + _("SYSTEM NAME")));
    sAllSorts.push_back(SortType(Sorts::SystemDescending, &compareSystemName, false, "\uF167 " + _("SYSTEM NAME")));
    mInitialized = true;
  }
  return mInitialized;
}

static int unicodeCompareUppercase(const std::string& a, const std::string& b)
{
  for (int apos = 0, bpos = 0;; )
  {
    int c1 = (int)Strings::UpperChar(Strings::chars2Unicode(a, apos));
    int c2 = (int)Strings::UpperChar(Strings::chars2Unicode(b, bpos));
    if ((c1 | c2) == 0) { return 0; }
    int c = c1 - c2;
    if (c != 0) { return c; }
  }
}

static int compareFoldersAndGames(const FileData& fd1, const FileData& fd2)
{
  ItemType f1 = fd1.Type();
  ItemType f2 = fd2.Type();
  if (f1 == f2) return 0;                // Both are games or folders
  if (f1 == ItemType::Folder) return -1; // f1 is a folder, f2 is a game
  return 1;                              // f2 is a folder
}

#define CheckFoldersAndGames(f1, f2) { int folderComparison = compareFoldersAndGames(f1, f2); if (folderComparison != 0) return folderComparison; }

ImplementSortMethod(compareSystemName)
{
  const SystemData& system1 = file1.System();
  const SystemData& system2 = file2.System();
  const int result = unicodeCompareUppercase(system1.Name(), system2.Name());
  if (result != 0) { return result; }
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

ImplementSortMethod(compareFileName)
{
  CheckFoldersAndGames(file1, file2)
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

ImplementSortMethod(compareRating)
{
  CheckFoldersAndGames(file1, file2)
  float c = file1.Metadata().Rating() - file2.Metadata().Rating();
  if (c < 0) { return -1; }
  if (c > 0) { return 1; }
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

ImplementSortMethod(compareTimesPlayed)
{
  CheckFoldersAndGames(file1, file2)
  int playCount = (file1).Metadata().PlayCount() - (file2).Metadata().PlayCount();
  if (playCount != 0) return playCount;
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

ImplementSortMethod(compareLastPlayed)
{
  CheckFoldersAndGames(file1, file2)
  unsigned int ep1 = (file1).Metadata().LastPlayedEpoc();
  unsigned int ep2 = (file2).Metadata().LastPlayedEpoc();
  if (ep1 == 0) ep1 = 0xFFFFFFFF;
  if (ep2 == 0) ep2 = 0xFFFFFFFF;
  if (ep1 == ep2)
    return unicodeCompareUppercase(file1.Name(), file2.Name());
  return ep1 < ep2 ? -1 : 1;
}

ImplementSortMethod(compareNumberPlayers)
{
  CheckFoldersAndGames(file1, file2)
  int players = (file1).Metadata().PlayerRange() - (file2).Metadata().PlayerRange();
  if (players != 0) return players;
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

ImplementSortMethod(compareDevelopper)
{
  CheckFoldersAndGames(file1, file2)
  return unicodeCompareUppercase(file1.Metadata().Developer(), file2.Metadata().Developer());
}

ImplementSortMethod(comparePublisher)
{
  CheckFoldersAndGames(file1, file2)
  return unicodeCompareUppercase(file1.Metadata().Publisher(), file2.Metadata().Publisher());
}

ImplementSortMethod(compareGenre)
{
  CheckFoldersAndGames(file1, file2)
  int genre = (int)(file1).Metadata().GenreId() - (int)(file2).Metadata().GenreId();
  if (genre != 0) return genre;
  return unicodeCompareUppercase(file1.Name(), file2.Name());
}

const std::vector<FileSorts::Sorts>& FileSorts::AvailableSorts(bool multisystem)
{
  //! Ordered mono-system sorts
  static std::vector<FileSorts::Sorts> sSingle =
  {
    Sorts::FileNameAscending,
    Sorts::FileNameDescending,
    Sorts::GenreAscending,
    Sorts::GenreDescending,
    Sorts::RatingAscending,
    Sorts::RatingDescending,
    Sorts::TimesPlayedAscending,
    Sorts::TimesPlayedDescending,
    Sorts::LastPlayedAscending,
    Sorts::LastPlayedDescending,
    Sorts::PlayersAscending,
    Sorts::PlayersDescending,
    Sorts::DevelopperAscending,
    Sorts::DevelopperDescending,
    Sorts::PublisherAscending,
    Sorts::PublisherDescending,
  };

  //! Ordered multi-system sorts
  static std::vector<FileSorts::Sorts> sMulti =
  {
    Sorts::FileNameAscending,
    Sorts::FileNameDescending,
    Sorts::SystemAscending,
    Sorts::SystemDescending,
    Sorts::GenreAscending,
    Sorts::GenreDescending,
    Sorts::RatingAscending,
    Sorts::RatingDescending,
    Sorts::TimesPlayedAscending,
    Sorts::TimesPlayedDescending,
    Sorts::LastPlayedAscending,
    Sorts::LastPlayedDescending,
    Sorts::PlayersAscending,
    Sorts::PlayersDescending,
    Sorts::DevelopperAscending,
    Sorts::DevelopperDescending,
    Sorts::PublisherAscending,
    Sorts::PublisherDescending,
  };

  return multisystem ? sMulti : sSingle;
}

const std::string& FileSorts::Name(FileSorts::Sorts sort)
{
  // Lazy initialization
  Initialize();

  for(const FileSorts::SortType& sortType : sAllSorts)
    if (sortType.Sort == sort)
      return sortType.Description;

  static std::string unknown("Unknown sort");
  return unknown;
}

bool FileSorts::IsAscending(FileSorts::Sorts sort)
{
  // Lazy initialization
  Initialize();

  for(const FileSorts::SortType& sortType : sAllSorts)
    if (sortType.Sort == sort)
      return sortType.Ascending;

  return false;
}


FileData::Comparer FileSorts::Comparer(FileSorts::Sorts sort)
{
  // Lazy initialization
  Initialize();

  for(const FileSorts::SortType& sortType : sAllSorts)
    if (sortType.Sort == sort)
      return sortType.Comparer;

  return nullptr;
}

