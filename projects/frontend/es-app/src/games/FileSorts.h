#pragma once

#include <vector>
#include "games/FileData.h"

class FileSorts
{
  public:
    //! Available sorts
    enum class Sorts
    {
        FileNameAscending,    //!< By filename, A-Z
        FileNameDescending,   //!< By filename, Z-A
        SystemAscending,      //!< By system then by filename, A-Z
        SystemDescending,     //!< By system then by filename, Z-A
        RatingAscending,      //!< By rating then by filename, 0-9
        RatingDescending,     //!< By rating then by filename, 9-0
        TimesPlayedAscending, //!< By times played then by filename, A-Z
        TimesPlayedDescending,//!< By times played then by filename, Z-A
        LastPlayedAscending,  //!< By last-played datetime then by filename, 2000-0
        LastPlayedDescending, //!< By last-played datetime then by filename, 0-2000
        PlayersAscending,     //!< By number of players then by filename, 1-2
        PlayersDescending,    //!< By number of players then by filename, 2-1
        DevelopperAscending,  //!< By developper then by filename, A-Z
        DevelopperDescending, //!< By developper then by filename, Z-A
        PublisherAscending,   //!< By publisher then by filename, A-Z
        PublisherDescending,  //!< By publisher then by filename, Z-A
        GenreAscending,       //!< By normalized genre (genreid) then by filename, EnumFirst-EnumLast
        GenreDescending,      //!< By normalized genre (genreid) then by filename, EnumLast-EnumFirst
    };

  private:
    //! Initialize Sort tables
    static bool Initialize();
    //! Initialization holder for static auto-initialization
    static bool mInitialized;

    struct SortType
    {
      std::string Description;
      FileData::Comparer Comparer;
      Sorts Sort;
      bool Ascending;

      SortType(Sorts sort, FileData::Comparer sortFunction, bool sortAscending, const std::string & sortDescription)
        : Description(sortDescription),
          Comparer(sortFunction),
          Sort(sort),
          Ascending(sortAscending)
      {
      }
    };

    static std::vector<SortType> sAllSorts;

  public:
    #define DeclareSortMethodPrototype(x) static int x(const FileData& file1, const FileData& file2);
    #define ImplementSortMethod(x) int FileSorts::x(const FileData& file1, const FileData& file2)

    DeclareSortMethodPrototype(compareSystemName)
    DeclareSortMethodPrototype(compareFileName)
    DeclareSortMethodPrototype(compareRating)
    DeclareSortMethodPrototype(compareTimesPlayed)
    DeclareSortMethodPrototype(compareLastPlayed)
    DeclareSortMethodPrototype(compareNumberPlayers)
    DeclareSortMethodPrototype(compareDevelopper)
    DeclareSortMethodPrototype(comparePublisher)
    DeclareSortMethodPrototype(compareGenre)

    /*!
     * @brief Get available sorts for a single system
     * @param multisystem or single system?
     * @return Sort list
     */
    static const std::vector<Sorts>& AvailableSorts(bool multisystem);

    /*!
     * @brief Get sort description
     * @param sort Sort to get name from
     * @return Sort name
     */
    static const std::string& Name(Sorts sort);

    /*!
     * @brief Return whether the given sort is an ascending sort or not
     * @param sort Sort to check
     * @return True if the sort is ascending
     */
    static bool IsAscending(Sorts sort);

    /*!
     * @brief Get sort compare method
     * @param sort Sort to get method from
     * @return Sort name
     */
    static FileData::Comparer Comparer(Sorts sort) ;
};
