#include "MetadataDescriptor.h"
#include "MetadataFieldDescriptor.h"
#include "utils/locale/LocaleHelper.h"
#include "utils/Log.h"

// TODO: Use const char* instead
const std::string MetadataDescriptor::DefaultValueRatio("auto");
const std::string MetadataDescriptor::DefaultValueEmpty;
const std::string MetadataDescriptor::DefaultValueRating("0.0");
const std::string MetadataDescriptor::DefaultValuePlayers("1");
const std::string MetadataDescriptor::DefaultValuePlayCount("0");
const std::string MetadataDescriptor::DefaultValueFavorite("false");
const std::string MetadataDescriptor::DefaultValueHidden("false");

const std::string MetadataDescriptor::GameNodeIdentifier("game");
const std::string MetadataDescriptor::FolderNodeIdentifier("folder");

MetadataStringHolder MetadataDescriptor::sNameHolder(1 << 20, 128 << 10);
MetadataStringHolder MetadataDescriptor::sDescriptionHolder(1 << 20, 128 << 10);
MetadataStringHolder MetadataDescriptor::sDeveloperHolder(64 << 10, 32 << 10);
MetadataStringHolder MetadataDescriptor::sPublisherHolder(64 << 10, 32 << 10);
MetadataStringHolder MetadataDescriptor::sGenreHolder(64 << 10, 32 << 10);
MetadataStringHolder MetadataDescriptor::sEmulatorHolder(2 << 10, 1 << 10);
MetadataStringHolder MetadataDescriptor::sCoreHolder(2 << 10, 1 << 10);
MetadataStringHolder MetadataDescriptor::sRatioHolder(1 << 10, 1 << 10);
MetadataStringHolder MetadataDescriptor::sPathHolder(64 << 10, 32 << 10);
MetadataStringHolder MetadataDescriptor::sFileHolder(128 << 10, 32 << 10);

#ifdef _METADATA_STATS_
int MetadataDescriptor::LivingClasses = 0;
int MetadataDescriptor::LivingFolders = 0;
int MetadataDescriptor::LivingGames = 0;
#endif

const MetadataFieldDescriptor* MetadataDescriptor::GetMetadataFieldDescriptors(ItemType type, int& count)
{
  switch(type)
  {
    case ItemType::Game:
    {
      static const MetadataFieldDescriptor sGameMetadataDescriptors[] =
      {
        MetadataFieldDescriptor("path"       , DefaultValueEmpty    , _("Path")        , _("enter game path")             , MetadataFieldDescriptor::DataType::Path   , MetadataFieldDescriptor::EditableType::None   , &MetadataDescriptor::IsDefaultRom            , &MetadataDescriptor::RomAsString         , &MetadataDescriptor::SetRomPathAsString      , false, true),
        MetadataFieldDescriptor("name"       , DefaultValueEmpty    , _("Name")        , _("enter game name")             , MetadataFieldDescriptor::DataType::String , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultName           , &MetadataDescriptor::NameAsString        , &MetadataDescriptor::SetName                 , false, true),
        MetadataFieldDescriptor("rating"     , DefaultValueRating   , _("Rating")      , _("enter rating")                , MetadataFieldDescriptor::DataType::Rating , MetadataFieldDescriptor::EditableType::Rating , &MetadataDescriptor::IsDefaultRating         , &MetadataDescriptor::RatingAsString      , &MetadataDescriptor::SetRatingAsString       , false, true),
        MetadataFieldDescriptor("favorite"   , DefaultValueFavorite , _("Favorite")    , _("enter favorite")              , MetadataFieldDescriptor::DataType::Bool   , MetadataFieldDescriptor::EditableType::Switch , &MetadataDescriptor::IsDefaultFavorite       , &MetadataDescriptor::FavoriteAsString    , &MetadataDescriptor::SetFavoriteAsString     , false, true),
        MetadataFieldDescriptor("hidden"     , DefaultValueHidden   , _("Hidden")      , _("set hidden")                  , MetadataFieldDescriptor::DataType::Bool   , MetadataFieldDescriptor::EditableType::Switch , &MetadataDescriptor::IsDefaultHidden         , &MetadataDescriptor::HiddenAsString      , &MetadataDescriptor::SetHiddenAsString       , false, true),
        MetadataFieldDescriptor("emulator"   , DefaultValueEmpty    , _("Emulator")    , _("enter emulator")              , MetadataFieldDescriptor::DataType::List   , MetadataFieldDescriptor::EditableType::List   , &MetadataDescriptor::IsDefaultEmulator       , &MetadataDescriptor::EmulatorAsString    , &MetadataDescriptor::SetEmulator             , false, true),
        MetadataFieldDescriptor("core"       , DefaultValueEmpty    , _("Core")        , _("enter core")                  , MetadataFieldDescriptor::DataType::List   , MetadataFieldDescriptor::EditableType::List   , &MetadataDescriptor::IsDefaultCore           , &MetadataDescriptor::CoreAsString        , &MetadataDescriptor::SetCore                 , false, true),
        MetadataFieldDescriptor("ratio"      , DefaultValueRatio    , _("Ratio")       , _("enter ratio")                 , MetadataFieldDescriptor::DataType::List   , MetadataFieldDescriptor::EditableType::List   , &MetadataDescriptor::IsDefaultRatio          , &MetadataDescriptor::RatioAsString       , &MetadataDescriptor::SetRatio                , false, true),
        MetadataFieldDescriptor("desc"       , DefaultValueEmpty    , _("Description") , _("enter description")           , MetadataFieldDescriptor::DataType::Text   , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultDescription    , &MetadataDescriptor::DescriptionAsString , &MetadataDescriptor::SetDescription          , false, false),
        MetadataFieldDescriptor("image"      , DefaultValueEmpty    , _("Image")       , _("enter path to image")         , MetadataFieldDescriptor::DataType::Path   , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultImage          , &MetadataDescriptor::ImageAsString       , &MetadataDescriptor::SetImagePathAsString    , false, false),
        MetadataFieldDescriptor("thumbnail"  , DefaultValueEmpty    , _("Thumbnail")   , _("enter path to thumbnail")     , MetadataFieldDescriptor::DataType::Path   , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultThumbnail      , &MetadataDescriptor::ThumbnailAsString   , &MetadataDescriptor::SetThumbnailPathAsString, false, false),
        MetadataFieldDescriptor("video"      , DefaultValueEmpty    , _("Video")       , _("enter path to video")         , MetadataFieldDescriptor::DataType::Path   , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultVideo          , &MetadataDescriptor::VideoAsString       , &MetadataDescriptor::SetVideoPathAsString    , false, false),
        MetadataFieldDescriptor("releasedate", DefaultValueEmpty    , _("Release date"), _("enter release date")          , MetadataFieldDescriptor::DataType::Date   , MetadataFieldDescriptor::EditableType::Date   , &MetadataDescriptor::IsDefaultReleaseDateEpoc, &MetadataDescriptor::ReleaseDateAsString , &MetadataDescriptor::SetReleaseDateAsString  , false, false),
        MetadataFieldDescriptor("developer"  , DefaultValueEmpty    , _("Developer")   , _("enter game developer")        , MetadataFieldDescriptor::DataType::String , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultDeveloper      , &MetadataDescriptor::DeveloperAsString   , &MetadataDescriptor::SetDeveloper            , false, false),
        MetadataFieldDescriptor("publisher"  , DefaultValueEmpty    , _("Publisher")   , _("enter game publisher")        , MetadataFieldDescriptor::DataType::String , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultPublisher      , &MetadataDescriptor::PublisherAsString   , &MetadataDescriptor::SetPublisher            , false, false),
        MetadataFieldDescriptor("genre"      , DefaultValueEmpty    , _("Genre")       , _("enter game genre")            , MetadataFieldDescriptor::DataType::Int    , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultGenre          , &MetadataDescriptor::GenreAsString       , &MetadataDescriptor::SetGenre                , false, false),
        MetadataFieldDescriptor("genreid"    , DefaultValueEmpty    , _("Genre ID")    , _("enter game genre id")         , MetadataFieldDescriptor::DataType::Int    , MetadataFieldDescriptor::EditableType::List   , &MetadataDescriptor::IsDefaultGenreId        , &MetadataDescriptor::GenreIdAsString     , &MetadataDescriptor::SetGenreIdAsString      , false, false),
        MetadataFieldDescriptor("adult"      , DefaultValueEmpty    , _("Adult")       , _("enter adult state")           , MetadataFieldDescriptor::DataType::Bool   , MetadataFieldDescriptor::EditableType::Switch , &MetadataDescriptor::IsDefaultAdult          , &MetadataDescriptor::AdultAsString       , &MetadataDescriptor::SetAdultAsString        , false, false),
        MetadataFieldDescriptor("players"    , DefaultValuePlayers  , _("Players")     , _("enter number of players")     , MetadataFieldDescriptor::DataType::Range  , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultPlayerRange    , &MetadataDescriptor::PlayersAsString     , &MetadataDescriptor::SetPlayersAsString      , false, false),
        MetadataFieldDescriptor("region"     , DefaultValueEmpty    , _("Region")      , _("enter region")                , MetadataFieldDescriptor::DataType::Int    , MetadataFieldDescriptor::EditableType::Text   , &MetadataDescriptor::IsDefaultRegion         , &MetadataDescriptor::RegionAsString      , &MetadataDescriptor::SetRegionAsString       , false, false),
        MetadataFieldDescriptor("playcount"  , DefaultValuePlayCount, _("Play count")  , _("enter number of times played"), MetadataFieldDescriptor::DataType::Int    , MetadataFieldDescriptor::EditableType::None   , &MetadataDescriptor::IsDefaultPlayCount      , &MetadataDescriptor::PlayCountAsString   , &MetadataDescriptor::SetPlayCountAsString    , true , false),
        MetadataFieldDescriptor("lastplayed" , DefaultValueEmpty    , _("Last played") , _("enter last played date")      , MetadataFieldDescriptor::DataType::Date   , MetadataFieldDescriptor::EditableType::None   , &MetadataDescriptor::IsDefaultLastPlayedEpoc , &MetadataDescriptor::LastPlayedAsString  , &MetadataDescriptor::SetLastPlayedAsString   , true , false),
        MetadataFieldDescriptor("hash"       , DefaultValueEmpty    , _("Rom Crc32")   , _("enter rom crc32")             , MetadataFieldDescriptor::DataType::Crc32  , MetadataFieldDescriptor::EditableType::None   , &MetadataDescriptor::IsDefaultRomCrc32       , &MetadataDescriptor::RomCrc32AsString    , &MetadataDescriptor::SetRomCrc32AsString     , true , false),
      };

      count = sizeof(sGameMetadataDescriptors) / sizeof(MetadataFieldDescriptor);
      return &sGameMetadataDescriptors[0];
    }
    case ItemType::Folder:
    {
      static const MetadataFieldDescriptor sFolderMetadataDescriptors[] =
      {
        MetadataFieldDescriptor("path"       , ""        , _("Path")        , _("enter game path")             , MetadataFieldDescriptor::DataType::Path  , MetadataFieldDescriptor::EditableType::None  , &MetadataDescriptor::IsDefaultRom            , &MetadataDescriptor::RomAsString         , &MetadataDescriptor::SetRomPathAsString      , false, true),
        MetadataFieldDescriptor("name"       , ""        , _("Name")        , _("enter game name")             , MetadataFieldDescriptor::DataType::String, MetadataFieldDescriptor::EditableType::Text  , &MetadataDescriptor::IsDefaultName           , &MetadataDescriptor::NameAsString        , &MetadataDescriptor::SetName                 , false, true),
        MetadataFieldDescriptor("hidden"     , "false"   , _("Hidden")      , _("set hidden")                  , MetadataFieldDescriptor::DataType::Bool  , MetadataFieldDescriptor::EditableType::Switch, &MetadataDescriptor::IsDefaultHidden         , &MetadataDescriptor::HiddenAsString      , &MetadataDescriptor::SetHiddenAsString       , false, true),
        MetadataFieldDescriptor("desc"       , ""        , _("Description") , _("enter description")           , MetadataFieldDescriptor::DataType::Text  , MetadataFieldDescriptor::EditableType::Text  , &MetadataDescriptor::IsDefaultDescription    , &MetadataDescriptor::DescriptionAsString , &MetadataDescriptor::SetDescription          , false, false),
        MetadataFieldDescriptor("image"      , ""        , _("Image")       , _("enter path to image")         , MetadataFieldDescriptor::DataType::Path  , MetadataFieldDescriptor::EditableType::Text  , &MetadataDescriptor::IsDefaultImage          , &MetadataDescriptor::ImageAsString       , &MetadataDescriptor::SetImagePathAsString    , false, false),
        MetadataFieldDescriptor("thumbnail"  , ""        , _("Thumbnail")   , _("enter path to thumbnail")     , MetadataFieldDescriptor::DataType::Path  , MetadataFieldDescriptor::EditableType::Text  , &MetadataDescriptor::IsDefaultThumbnail      , &MetadataDescriptor::ThumbnailAsString   , &MetadataDescriptor::SetThumbnailPathAsString, false, false),
      };

      count = sizeof(sFolderMetadataDescriptors) / sizeof(MetadataFieldDescriptor);
      return &sFolderMetadataDescriptors[0];
    }
    case ItemType::Root:
    case ItemType::Empty:
    default: break;
  }
  count = 0;
  return nullptr;
}

const MetadataDescriptor& MetadataDescriptor::Default()
{
  static bool initialized = false;
  static MetadataDescriptor defaultData(Path::Empty, "default", ItemType::Game);

  if (!initialized)
  {
    int count = 0;
    const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(ItemType::Game, count);

    if (fields != nullptr)
      for (; --count >= 0;)
      {
        // Get field descriptor
        const MetadataFieldDescriptor& field = fields[count];

        // Set default value
        (defaultData.*field.SetValueMethod())(field.DefaultValue());
      }
    else LOG(LogError) << "[MetadataDescriptor] Error building static object";
  }

  return defaultData;
}

std::string MetadataDescriptor::IntToRange(int range)
{
  int max = range >> 16;
  int min = range & 0xFFFF;

  // min = max, only one number
  if (min == max) return std::to_string(max);

  std::string value = std::to_string(max);

  // min or more range
  if (min == 0xFFFF)
  {
    value += '+';
  }
  else
  {
    // Full range
    value = std::to_string(min) + '-' + value;
  }
  return value;
}

bool MetadataDescriptor::RangeToInt(const std::string& range, int& to)
{
  // max+ (min+)
  int p = (int)range.find('+');
  if (p != (int)std::string::npos)
  {
    if (!StringToInt(range, p, 0, '+')) return false;
    to = (p << 16) + 0xFFFF;
    return true;
  }

  // max-max
  p = (int)range.find('-');
  if (p == (int)std::string::npos)
  {
    if (!StringToInt(range, p)) return false;
    to = (p << 16) + p;
    return true;
  }

  // min-max
  int min = 0; if (!StringToInt(range, min, 0, '-')) return false;
  int max = 0; if (!StringToInt(range, max, p + 1, 0  )) return false;
  if (min > max) { min = min ^ max; max = max ^ min; min = min ^ max; }
  to = (max << 16) + min;
  return true;
}

bool MetadataDescriptor::IntToHex(int from, std::string& to)
{
  static const char* hexa = "0123456789ABCDEF";
  char result[9];
  result[sizeof(result) - 1] = 0;

  for (int i = sizeof(result) - 1, p = 0; -- i >= 0;)
    result[p++] = hexa[(from >> (i << 2)) & 0xF];

  to = result;
  return true;
}

bool MetadataDescriptor::HexToInt(const std::string& from, int& to)
{
  if (from.empty()) return false;
  const char* src = from.c_str();

  int result = 0;
  for (;; src++)
  {
    int v = (unsigned char)src[0];
    if ((unsigned int)(v - 0x30) <= 9) { result <<= 4; result += v - 0x30; }
    else
    {
      v &= 0xDF;
      if ((unsigned int)(v - 0x41) <= 5) { result <<= 4; result += v - 0x37; }
      else break;
    }
  }
  if (src[0] != 0) return false;

  to = result;
  return true;
}

bool MetadataDescriptor::StringToInt(const std::string& from, int& to, int offset, char stop)
{
  const char* src = from.c_str() + offset;

  bool sign = (src[0] == '-');
  if (sign) src++;

  int result = 0;
  while ((unsigned int)(src[0] - 0x30) <= 9) { result *= 10; result += src[0] - 0x30; src++; }
  if (src[0] != stop) return false;

  to = sign ? -result : result;
  return true;
}

bool MetadataDescriptor::StringToInt(const std::string& from, int& to)
{
  const char* src = from.c_str();

  bool sign = (src[0] == '-');
  if (sign) src++;

  int result = 0;
  while ((unsigned int)(src[0] - 0x30) <= 9) { result *= 10; result += src[0] - 0x30; src++; }
  if (src[0] != 0) return false;

  to = sign ? -result : result;
  return true;
}

bool MetadataDescriptor::StringToFloat(const std::string& from, float& to)
{
  const char* src = from.c_str();

  bool sign = (src[0] == '-');
  if (sign) src++;

  int intPart = 0;
  int fractPart = 0;
  int pow10 = 1;

  // Integer part
  while ((unsigned int)(src[0] - 0x30) <= 9) { intPart *= 10; intPart += src[0] - 0x30; src++; }
  if (src[0] == '.')
  {
    src++;
    while ((unsigned int)(src[0] - 0x30) <= 9) { fractPart *= 10; fractPart += src[0] - 0x30; src++; pow10 *= 10; }
  }
  if (src[0] != 0) return false;

  float result = (float)intPart + ((float)fractPart / (float)pow10);

  to = sign ? -result : result;
  return true;
}

bool MetadataDescriptor::Deserialize(const XmlNode from, const Path& relativeTo)
{
  #ifdef _METADATA_STATS_
    if (_Type == ItemType::Game) LivingGames--;
    if (_Type == ItemType::Folder) LivingFolders--;
  #endif

  std::string name = from.name();
  if (name == GameNodeIdentifier) mType = ItemType::Game;
  else if (name == FolderNodeIdentifier) mType = ItemType::Folder;
  else return false; // Unidentified node

  #ifdef _METADATA_STATS_
    if (_Type == ItemType::Game) LivingGames++;
    if (_Type == ItemType::Folder) LivingFolders++;
  #endif

  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(mType, count);
  if (fields == nullptr) return false;

  // Extract default name
  std::string defaultName = sNameHolder.GetString(mName);

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Get field data as string
    const std::string& defaultStringValue = field.DefaultValue();
    std::string value = Xml::AsString(from, field.Key(), defaultStringValue);
    // Ignore default values
    if (value == defaultStringValue) continue;

    // Absolute path
    if (field.Type() == MetadataFieldDescriptor::DataType::Path)
      value = Path(value).ToAbsolute(relativeTo).ToString();

    // Set value!
    (this->*field.SetValueMethod())(value);
  }

  // Control name
  if (mName < 0)
  {
    mName = sNameHolder.AddString32(defaultName);
    mDirty = true;
  }
  else mDirty = false;

  return true;
}

void MetadataDescriptor::Serialize(XmlNode parentNode, const Path& filePath, const Path& relativeTo) const
{
  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(mType, count);
  if (fields == nullptr) return;

  // Add empty node game/folder
  XmlNode node = parentNode.append_child(mType == ItemType::Game ? GameNodeIdentifier.c_str() : FolderNodeIdentifier.c_str());

  // Add path
  bool dummy = false;
  Path relative = filePath.MakeRelative(relativeTo, dummy);
  Xml::AddAsString(node, "path", relative.ToChars());

  // Metadata
  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Default value?
    if ((this->*field.IsDefaultValueMethod())()) continue;

    // Store
    Xml::AddAsString(node, field.Key(), (this->*field.GetValueMethod())());
  }
}

void MetadataDescriptor::Merge(const MetadataDescriptor& sourceMetadata)
{
  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(mType, count);
  if (fields == nullptr) return;

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Default value?
    if ((sourceMetadata.*field.IsDefaultValueMethod())()) continue;

    // Get/Set
    (this->*field.SetValueMethod())((this->*field.GetValueMethod())());

    // A field has been copied. Set the dirty flag
    mDirty = true;
  }
}

void MetadataDescriptor::CleanupHolders()
{
  LOG(LogDebug) << "[MetadataDescriptor] Name storage: "        << sNameHolder.StorageSize()        << " - object count: " << sNameHolder.ObjectCount()       ;
  LOG(LogDebug) << "[MetadataDescriptor] Description storage: " << sDescriptionHolder.StorageSize() << " - object count: " << sDescriptionHolder.ObjectCount();
  LOG(LogDebug) << "[MetadataDescriptor] Publisher storage: "   << sPublisherHolder.StorageSize()   << " - object count: " << sPublisherHolder.ObjectCount()  ;
  LOG(LogDebug) << "[MetadataDescriptor] Developer storage: "   << sDeveloperHolder.StorageSize()   << " - object count: " << sDeveloperHolder.ObjectCount()  ;
  LOG(LogDebug) << "[MetadataDescriptor] Genre storage: "       << sGenreHolder.StorageSize()       << " - object count: " << sGenreHolder.ObjectCount()      ;
  LOG(LogDebug) << "[MetadataDescriptor] Ratio storage: "       << sRatioHolder.StorageSize()       << " - object count: " << sRatioHolder.ObjectCount()      ;
  LOG(LogDebug) << "[MetadataDescriptor] Core storage: "        << sCoreHolder.StorageSize()        << " - object count: " << sCoreHolder.ObjectCount()       ;
  LOG(LogDebug) << "[MetadataDescriptor] Emulator storage: "    << sEmulatorHolder.StorageSize()    << " - object count: " << sEmulatorHolder.ObjectCount()   ;
  LOG(LogDebug) << "[MetadataDescriptor] Path storage: "        << sPathHolder.StorageSize()        << " - object count: " << sPathHolder.ObjectCount()       ;
  LOG(LogDebug) << "[MetadataDescriptor] File storage: "        << sFileHolder.StorageSize()        << " - object count: " << sFileHolder.ObjectCount()       ;
  sNameHolder.Finalize();
  sDescriptionHolder.Finalize();
  sPublisherHolder.Finalize();
  sDeveloperHolder.Finalize();
  sGenreHolder.Finalize();
  sRatioHolder.Finalize();
  sCoreHolder.Finalize();
  sEmulatorHolder.Finalize();
  sPathHolder.Finalize();
  sFileHolder.Finalize();
}
