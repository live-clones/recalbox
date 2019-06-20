#include "MetadataDescriptor.h"
#include "MetadataFieldDescriptor.h"
#include "Locale.h"
#include "Util.h"
#include "Log.h"

const std::string MetadataDescriptor::DefaultValueEmulator = "default";
const std::string MetadataDescriptor::DefaultValueCore = "default";
const std::string MetadataDescriptor::DefaultValueRatio = "auto";
const std::string MetadataDescriptor::DefaultValueEmpty = "";
const std::string MetadataDescriptor::DefaultValueRating = "0.0";
const std::string MetadataDescriptor::DefaultValuePlayers = "1";
const std::string MetadataDescriptor::DefaultValuePlaycount = "0";
//const std::string MetadataDescriptor::DefaultValueUnknown = "unknown";
const std::string MetadataDescriptor::DefaultValueFavorite = "false";
const std::string MetadataDescriptor::DefaultValueHidden = "false";

const std::string MetadataDescriptor::GameNodeIdentifier = "game";
const std::string MetadataDescriptor::FolderNodeIdentifier = "folder";

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
      // This static field descriptor array is defined here to allow
      // offsetof accessing private fields
      static const MetadataFieldDescriptor _GameMetadataDescriptors[] =
      {
        MetadataFieldDescriptor("name"       , DefaultValueEmpty    , _("Name")        , _("enter game name")             , (int)offsetof(MetadataDescriptor, _Name)       , MetadataFieldDescriptor::DataType::String , &MetadataDescriptor::IsDefaultName           , &MetadataDescriptor::NameAsString        , &MetadataDescriptor::SetName                , false, true),
        MetadataFieldDescriptor("rating"     , DefaultValueRating   , _("Rating")      , _("enter rating")                , (int)offsetof(MetadataDescriptor, _Rating)     , MetadataFieldDescriptor::DataType::Rating , &MetadataDescriptor::IsDefaultRating         , &MetadataDescriptor::RatingAsString      , &MetadataDescriptor::SetRatingAsString      , false, true),
        MetadataFieldDescriptor("favorite"   , DefaultValueFavorite , _("Favorite")    , _("enter favorite")              , (int)offsetof(MetadataDescriptor, _Favorite)   , MetadataFieldDescriptor::DataType::Bool   , &MetadataDescriptor::IsDefaultFavorite       , &MetadataDescriptor::FavoriteAsString    , &MetadataDescriptor::SetFavoriteAsString    , false, true),
        MetadataFieldDescriptor("hidden"     , DefaultValueHidden   , _("Hidden")      , _("set hidden")                  , (int)offsetof(MetadataDescriptor, _Hidden)     , MetadataFieldDescriptor::DataType::Bool   , &MetadataDescriptor::IsDefaultHidden         , &MetadataDescriptor::HiddenAsString      , &MetadataDescriptor::SetHiddenAsString      , false, true),
        MetadataFieldDescriptor("emulator"   , DefaultValueEmulator , _("Emulator")    , _("enter emulator")              , (int)offsetof(MetadataDescriptor, _Emulator)   , MetadataFieldDescriptor::DataType::PList  , &MetadataDescriptor::IsDefaultEmulator       , &MetadataDescriptor::EmulatorAsString    , &MetadataDescriptor::SetEmulator            , false, true),
        MetadataFieldDescriptor("core"       , DefaultValueCore     , _("Core")        , _("enter core")                  , (int)offsetof(MetadataDescriptor, _Core)       , MetadataFieldDescriptor::DataType::PList  , &MetadataDescriptor::IsDefaultCore           , &MetadataDescriptor::CoreAsString        , &MetadataDescriptor::SetCore                , false, true),
        MetadataFieldDescriptor("ratio"      , DefaultValueRatio    , _("Ratio")       , _("enter ratio")                 , (int)offsetof(MetadataDescriptor, _Ratio)      , MetadataFieldDescriptor::DataType::PList  , &MetadataDescriptor::IsDefaultRatio          , &MetadataDescriptor::RatioAsString       , &MetadataDescriptor::SetRatio               , false, true),
        MetadataFieldDescriptor("desc"       , DefaultValueEmpty    , _("Description") , _("enter description")           , (int)offsetof(MetadataDescriptor, _Description), MetadataFieldDescriptor::DataType::Text   , &MetadataDescriptor::IsDefaultDescription    , &MetadataDescriptor::DescriptionAsString , &MetadataDescriptor::SetDescription         , false, false),
        MetadataFieldDescriptor("image"      , DefaultValueEmpty    , _("Image")       , _("enter path to image")         , (int)offsetof(MetadataDescriptor, _Image)      , MetadataFieldDescriptor::DataType::Path   , &MetadataDescriptor::IsDefaultImage          , &MetadataDescriptor::ImageAsString       , &MetadataDescriptor::SetImagePath           , false, false),
        MetadataFieldDescriptor("thumbnail"  , DefaultValueEmpty    , _("Thumbnail")   , _("enter path to thumbnail")     , (int)offsetof(MetadataDescriptor, _Thumbnail)  , MetadataFieldDescriptor::DataType::PPath  , &MetadataDescriptor::IsDefaultThumbnail      , &MetadataDescriptor::ThumbnailAsString   , &MetadataDescriptor::SetThumbnailPath       , false, false),
        MetadataFieldDescriptor("video"      , DefaultValueEmpty    , _("Video")       , _("enter path to video")         , (int)offsetof(MetadataDescriptor, _Video)      , MetadataFieldDescriptor::DataType::PPath  , &MetadataDescriptor::IsDefaultVideo          , &MetadataDescriptor::VideoAsString       , &MetadataDescriptor::SetVideoPath           , false, false),
        MetadataFieldDescriptor("releasedate", DefaultValueEmpty    , _("Release date"), _("enter release date")          , (int)offsetof(MetadataDescriptor, _ReleaseDate), MetadataFieldDescriptor::DataType::Date   , &MetadataDescriptor::IsDefaultReleaseDateEpoc, &MetadataDescriptor::ReleaseDateAsString , &MetadataDescriptor::SetReleaseDateAsString , false, false),
        MetadataFieldDescriptor("developer"  , DefaultValueEmpty    , _("Developer")   , _("enter game developer")        , (int)offsetof(MetadataDescriptor, _Developer)  , MetadataFieldDescriptor::DataType::String , &MetadataDescriptor::IsDefaultDeveloper      , &MetadataDescriptor::DeveloperAsString   , &MetadataDescriptor::SetDeveloper           , false, false),
        MetadataFieldDescriptor("publisher"  , DefaultValueEmpty    , _("Publisher")   , _("enter game publisher")        , (int)offsetof(MetadataDescriptor, _Publisher)  , MetadataFieldDescriptor::DataType::String , &MetadataDescriptor::IsDefaultPublisher      , &MetadataDescriptor::PublisherAsString   , &MetadataDescriptor::SetPublisher           , false, false),
        MetadataFieldDescriptor("genre"      , DefaultValueEmpty    , _("Genre")       , _("enter game genre")            , (int)offsetof(MetadataDescriptor, _Genre)      , MetadataFieldDescriptor::DataType::String , &MetadataDescriptor::IsDefaultGenre          , &MetadataDescriptor::GenreAsString       , &MetadataDescriptor::SetGenre               , false, false),
        MetadataFieldDescriptor("players"    , DefaultValuePlayers  , _("Players")     , _("enter number of players")     , (int)offsetof(MetadataDescriptor, _Players)    , MetadataFieldDescriptor::DataType::Range  , &MetadataDescriptor::IsDefaultPlayerRange    , &MetadataDescriptor::PlayersAsString     , &MetadataDescriptor::SetPlayersAsString     , false, false),
        MetadataFieldDescriptor("region"     , DefaultValueEmpty    , _("Region")      , _("enter region")                , (int)offsetof(MetadataDescriptor, _Region)     , MetadataFieldDescriptor::DataType::PString, &MetadataDescriptor::IsDefaultRegion         , &MetadataDescriptor::RegionAsString      , &MetadataDescriptor::SetRegion              , false, false),
        MetadataFieldDescriptor("playcount"  , DefaultValuePlaycount, _("Play count")  , _("enter number of times played"), (int)offsetof(MetadataDescriptor, _Playcount)  , MetadataFieldDescriptor::DataType::Int    , &MetadataDescriptor::IsDefaultPlayCount      , &MetadataDescriptor::PlayCountAsString   , &MetadataDescriptor::SetPlayCountAsString   , true , false),
        MetadataFieldDescriptor("lastplayed" , DefaultValueEmpty    , _("Last played") , _("enter last played date")      , (int)offsetof(MetadataDescriptor, _LastPlayed) , MetadataFieldDescriptor::DataType::Date   , &MetadataDescriptor::IsDefaultLastPlayedEpoc , &MetadataDescriptor::LastPlayedAsString  , &MetadataDescriptor::SetLastPlayedAsString  , true , false),
        MetadataFieldDescriptor("hash"       , DefaultValueEmpty    , _("Rom Crc32")   , _("enter rom crc32")             , (int)offsetof(MetadataDescriptor, _RomCrc32)   , MetadataFieldDescriptor::DataType::Crc32  , &MetadataDescriptor::IsDefaultRomCrc32       , &MetadataDescriptor::RomCrc32AsString    , &MetadataDescriptor::SetRomCrc32AsString    , true , false),
      };

      count = sizeof(_GameMetadataDescriptors) / sizeof(MetadataFieldDescriptor);
      return &_GameMetadataDescriptors[0];
    }
    case ItemType::Folder:
    {
      // This static field descriptor array is defined here to allow
      // offsetof accessing private fields
      static const MetadataFieldDescriptor _FolderMetadataDescriptors[] =
      {
        MetadataFieldDescriptor("name"       , ""        , _("Name")        , _("enter game name")             , (int)offsetof(MetadataDescriptor, _Name)       , MetadataFieldDescriptor::DataType::String, &MetadataDescriptor::IsDefaultName           , &MetadataDescriptor::NameAsString        , &MetadataDescriptor::SetName                , false, true),
        MetadataFieldDescriptor("hidden"     , "false"   , _("Hidden")      , _("set hidden")                  , (int)offsetof(MetadataDescriptor, _Hidden)     , MetadataFieldDescriptor::DataType::Bool  , &MetadataDescriptor::IsDefaultHidden         , &MetadataDescriptor::HiddenAsString      , &MetadataDescriptor::SetHiddenAsString      , false, true),
        MetadataFieldDescriptor("desc"       , ""        , _("Description") , _("enter description")           , (int)offsetof(MetadataDescriptor, _Description), MetadataFieldDescriptor::DataType::Text  , &MetadataDescriptor::IsDefaultDescription    , &MetadataDescriptor::DescriptionAsString , &MetadataDescriptor::SetDescription         , false, false),
        MetadataFieldDescriptor("image"      , ""        , _("Image")       , _("enter path to image")         , (int)offsetof(MetadataDescriptor, _Image)      , MetadataFieldDescriptor::DataType::Path  , &MetadataDescriptor::IsDefaultImage          , &MetadataDescriptor::ImageAsString       , &MetadataDescriptor::SetImagePath           , false, false),
        MetadataFieldDescriptor("thumbnail"  , ""        , _("Thumbnail")   , _("enter path to thumbnail")     , (int)offsetof(MetadataDescriptor, _Thumbnail)  , MetadataFieldDescriptor::DataType::PPath , &MetadataDescriptor::IsDefaultThumbnail      , &MetadataDescriptor::ThumbnailAsString   , &MetadataDescriptor::SetThumbnailPath       , false, false),
      };

      count = sizeof(_FolderMetadataDescriptors) / sizeof(MetadataFieldDescriptor);
      return &_FolderMetadataDescriptors[0];
    }
    case ItemType::Empty:
    default: break;
  }
  count = 0;
  return nullptr;
}

MetadataDescriptor MetadataDescriptor::_Default = MetadataDescriptor::BuildDefaultValueMetadataDescriptor();

MetadataDescriptor MetadataDescriptor::BuildDefaultValueMetadataDescriptor()
{
  MetadataDescriptor defaultData("default", ItemType::Game);
  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(ItemType::Game, count);

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Set default value
    (defaultData.*field.SetValueMethod())(field.DefaultValue());
  }

  return defaultData;
}

std::string MetadataDescriptor::FloatToString(float value, int precision)
{
  int ipart = (int)value;
  value -= (float)ipart;

  // convert integer part to string
  std::string result = std::to_string(ipart);

  // check for display option after point
  if (precision > 0)
  {
    result += '.';
    value *= (float)pow(10, precision);
    result += std::to_string((int)value); // TODO: Get XAString::ToInt and get rid of that @#%$*!!! to_string
  }
  return result;
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
  int min; if (!StringToInt(range, min, 0, '-')) return false;
  int max; if (!StringToInt(range, max, p + 1, 0  )) return false;
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
    int v = src[0];
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

bool MetadataDescriptor::Deserialize(const TreeNode& from, const std::string& relativeTo)
{
  #ifdef _METADATA_STATS_
    if (_Type == ItemType::Game) LivingGames--;
    if (_Type == ItemType::Folder) LivingFolders--;
  #endif

  if (from.first == GameNodeIdentifier) _Type = ItemType::Game;
  else if (from.first == FolderNodeIdentifier) _Type = ItemType::Folder;
  else return false; // Unidentified node

  #ifdef _METADATA_STATS_
    if (_Type == ItemType::Game) LivingGames++;
    if (_Type == ItemType::Folder) LivingFolders++;
  #endif

  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(_Type, count);

  // Extract default name
  std::string defaultName = std::move(_Name);

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Get field data as string
    const std::string& defaultStringValue = field.DefaultValue();
    std::string value = from.second.get(field.Key(), defaultStringValue);
    // Ignore default values
    if (value == defaultStringValue) continue;

    // Get neutral target
    void* target = ((unsigned char*)this) + field.Offset();

    // Convert & store
    // We could use field.SetValueMethod() to set fields by string values,
    // but we want to do more control that simple assignations
    switch(field.Type())
    {
      case MetadataFieldDescriptor::DataType::PString:
      case MetadataFieldDescriptor::DataType::PList:
      {
        AssignPString(*((std::string**)target), value);
        break;
      }
      case MetadataFieldDescriptor::DataType::PPath:
      {
        value = resolvePath(value, relativeTo, true).generic_string();
        AssignPString(*((std::string**)target), value);
        break;
      }
      case MetadataFieldDescriptor::DataType::String:
      case MetadataFieldDescriptor::DataType::Text:
      {
        *((std::string*)target) = value;
        break;
      }
      case MetadataFieldDescriptor::DataType::Path:
      {
        value = resolvePath(value, relativeTo, true).generic_string();
        *((std::string*)target) = value;
        break;
      }
      case MetadataFieldDescriptor::DataType::Int:
      {
        int intValue = 0;
        if (!StringToInt(value, intValue))
        {
          LOG(LogWarning) << "Invalid int value " << value;
          StringToInt(field.DefaultValue(), intValue);
        }
        *((int*)target) = intValue;
        break;
      }
      case MetadataFieldDescriptor::DataType::Bool:
      {
        *((bool*)target) = (value == "true");
        break;
      }
      case MetadataFieldDescriptor::DataType::Float:
      case MetadataFieldDescriptor::DataType::Rating:
      {
        float floatValue = 0;
        if (!StringToFloat(value, floatValue))
        {
          LOG(LogWarning) << "Invalid float value " << value;
          StringToFloat(field.DefaultValue(), floatValue);
        }
        *((float*)target) = floatValue;
        break;
      }
      case MetadataFieldDescriptor::DataType::Date:
      {
        int epoch = 0;
        if (value.length() != 0)
        {
          DateTime dt(false); // Unitialized DateTime
          if (DateTime::ParseFromString("%yyyy%MM%ddT%hh%mm%ss", value, dt))
            epoch = (int) dt.ToLocal().ToEpochTime();
          else
            LOG(LogWarning) << "Invalid DateTime value " << value;
        }
        *((int*) target) = epoch;
        break;
      }
      case MetadataFieldDescriptor::DataType::Range:
      {
        int intValue = 0;
        if (!RangeToInt(value, intValue)) { LOG(LogWarning) << "Invalid Range " << value; }
        *((int*)target) = intValue;
        break;
      }
      case MetadataFieldDescriptor::DataType::Crc32:
      {
        int intValue = 0;
        if (value.length() != 0)
          if (!HexToInt(value, intValue))
          {
            LOG(LogWarning) << "Invalid CRC32 " << value;
          }
        *((int*)target) = intValue;
        break;
      }
      default: throw std::logic_error("Unknown field type");
    }
  }

  // Control name
  if (_Name.empty())
  {
    _Name = std::move(defaultName);
    _Dirty = true;
  }
  else _Dirty = false;

  return true;
}

void MetadataDescriptor::Serialize(Tree& parentTree, const std::string& filePath, const std::string& relativeTo) const
{
  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(_Type, count);

  // Add empty node game/folder
  Tree& tree = parentTree.add_child(_Type == ItemType::Game ? GameNodeIdentifier : FolderNodeIdentifier, Tree());

  // Add path
  std::string relative = makeRelativePath(filePath, relativeTo, true).generic_string();
  tree.put("path", relative);

  // Metadata
  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Default value?
    if ((this->*field.IsDefaultValueMethod())()) continue;

    // Get neutral source
    void* source = ((unsigned char*)this) + field.Offset();

    // Convert & store
    // As for Deserialize, we do not use field.GetValues, because we need
    // to do additional tasks regarding each different type
    switch(field.Type())
    {
      case MetadataFieldDescriptor::DataType::String:
      case MetadataFieldDescriptor::DataType::Text:
      {
        tree.put(field.Key(), *((std::string*)source));
        break;
      }
      case MetadataFieldDescriptor::DataType::PString:
      case MetadataFieldDescriptor::DataType::PList:
      {
        tree.put(field.Key(), ReadPString(*((std::string**)source), DefaultValueEmpty));
        break;
      }
      case MetadataFieldDescriptor::DataType::PPath:
      {
        tree.put(field.Key(), makeRelativePath(ReadPString(*((std::string**)source), DefaultValueEmpty), relativeTo, true).generic_string());
        break;
      }
      case MetadataFieldDescriptor::DataType::Path:
      {
        tree.put(field.Key(), makeRelativePath(*((std::string*)source), relativeTo, true).generic_string());
        break;
      }
      case MetadataFieldDescriptor::DataType::Int:
      {
        tree.put(field.Key(), *((int*)source)); // Autoboxing std::string(int)
        break;
      }
      case MetadataFieldDescriptor::DataType::Bool:
      {
        tree.put(field.Key(), *((bool*)source) ? "true" : "false");
        break;
      }
      case MetadataFieldDescriptor::DataType::Float:
      case MetadataFieldDescriptor::DataType::Rating:
      {
        tree.put(field.Key(), *((float*)source)); // Autoboxing std::string(float)
        break;
      }
      case MetadataFieldDescriptor::DataType::Date:
      {
        tree.put(field.Key(), DateTime((long long)*((int*)source)).ToUtc().ToCompactISO8601());
        break;
      }
      case MetadataFieldDescriptor::DataType::Range:
      {
        tree.put(field.Key(), IntToRange(*((int*)source)));
        break;
      }
      case MetadataFieldDescriptor::DataType::Crc32:
      {
        std::string value;
        IntToHex(*((int*)source), value);
        tree.put(field.Key(), value);
        break;
      }
      default: throw std::logic_error("Unknown field type");
    }
  }
}

void MetadataDescriptor::Merge(const MetadataDescriptor& sourceMetadata)
{
  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(_Type, count);

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Default value?
    if ((sourceMetadata.*field.IsDefaultValueMethod())()) continue;

    // Get neutral source
    void* source = ((unsigned char*)&sourceMetadata) + field.Offset();
    void* destination = ((unsigned char*)this) + field.Offset();

    // Convert & store
    switch(field.Type())
    {
      case MetadataFieldDescriptor::DataType::String:
      case MetadataFieldDescriptor::DataType::Text:
      case MetadataFieldDescriptor::DataType::Path:
      {
        *((std::string*)destination) = *((std::string*)source);
        break;
      }
      case MetadataFieldDescriptor::DataType::PList:
      case MetadataFieldDescriptor::DataType::PPath:
      case MetadataFieldDescriptor::DataType::PString:
      {
        *((std::string**)destination) = *((std::string**)source);
        break;
      }
      case MetadataFieldDescriptor::DataType::Date:
      case MetadataFieldDescriptor::DataType::Range:
      case MetadataFieldDescriptor::DataType::Crc32:
      case MetadataFieldDescriptor::DataType::Int:
      {
        *((int*)destination) = *((int*)source);
        break;
      }
      case MetadataFieldDescriptor::DataType::Bool:
      {
        *((bool*)destination) = *((bool*)source);
        break;
      }
      case MetadataFieldDescriptor::DataType::Float:
      case MetadataFieldDescriptor::DataType::Rating:
      {
        *((float*)destination) = *((float*)source);
        break;
      }
      default: throw std::logic_error("Unknown field type");
    }

    // A field has been copied. Set the dirty flag
    _Dirty = true;
  }
}

void MetadataDescriptor::FreeAll()
{
  #ifdef _METADATA_STATS_
  LivingClasses--;
  if (_Type == ItemType::Game) LivingGames--;
  if (_Type == ItemType::Folder) LivingFolders--;
  #endif

  int count = 0;
  const MetadataFieldDescriptor* fields = GetMetadataFieldDescriptors(_Type, count);

  for (; --count >= 0; )
  {
    // Get field descriptor
    const MetadataFieldDescriptor& field = fields[count];

    // Get neutral source
    void* source = ((unsigned char*)this) + field.Offset();

    // Convert & store
    switch(field.Type())
    {
      case MetadataFieldDescriptor::DataType::PList:
      case MetadataFieldDescriptor::DataType::PPath:
      case MetadataFieldDescriptor::DataType::PString:
      {
        FreePString(*((std::string**)source));
        break;
      }
      case MetadataFieldDescriptor::DataType::Rating:
      case MetadataFieldDescriptor::DataType::String:
      case MetadataFieldDescriptor::DataType::Int:
      case MetadataFieldDescriptor::DataType::Bool:
      case MetadataFieldDescriptor::DataType::Float:
      case MetadataFieldDescriptor::DataType::Text:
      case MetadataFieldDescriptor::DataType::Path:
      case MetadataFieldDescriptor::DataType::Date:
      case MetadataFieldDescriptor::DataType::Range:
      case MetadataFieldDescriptor::DataType::Crc32:
      default: break;
    }

    // A field has been copied. Set the dirty flag
    _Dirty = true;
  }
}

MetadataDescriptor::~MetadataDescriptor()
{
  FreeAll();
}
