//
// Created by bkg2k on 01/01/2020.
//

#include <map>
#include "SystemIcons.h"

static const std::map<std::string, const char*>& FavoritesIconsMap()
{
  static const std::map<std::string, const char*> _FavoritesIconsMap =
    {
      {"snes",         "\uF25e "},
      {"3do",          "\uF28a "},
      {"x68000",       "\uF28b "},
      {"amiga600",     "\uF244 "},
      {"amiga1200",    "\uF245 "},
      {"nds",          "\uF267 "},
      {"c64",          "\uF24c "},
      {"nes",          "\uF25c "},
      {"n64",          "\uF260 "},
      {"gba",          "\uF266 "},
      {"gbc",          "\uF265 "},
      {"gb",           "\uF264 "},
      {"fds",          "\uF25d "},
      {"virtualboy",   "\uF25f "},
      {"gw",           "\uF278 "},
      {"dreamcast",    "\uF26e "},
      {"megadrive",    "\uF26b "},
      {"segacd",       "\uF26d "},
      {"sega32x",      "\uF26c "},
      {"mastersystem", "\uF26a "},
      {"gamegear",     "\uF26f "},
      {"sg1000",       "\uF269 "},
      {"psp",          "\uF274 "},
      {"psx",          "\uF275 "},
      {"pcengine",     "\uF271 "},
      {"pcenginecd",   "\uF273 "},
      {"supergrafx",   "\uF272 "},
      {"scummvm",      "\uF27a "},
      {"dos",          "\uF24a "},
      {"fba",          "\uF252 "},
      {"fbneo",        "\uF253 "},
      {"mame",         "\uF255 "},
      {"neogeo",       "\uF257 "},
      {"colecovision", "\uF23f "},
      {"atari2600",    "\uF23c "},
      {"atari7800",    "\uF23e "},
      {"lynx",         "\uF270 "},
      {"ngp",          "\uF258 "},
      {"ngpc",         "\uF259 "},
      {"wswan",        "\uF25a "},
      {"wswanc",       "\uF25b "},
      {"doom",         "\uF277 "},
      {"vectrex",      "\uF240 "},
      {"lutro",        "\uF27d "},
      {"cavestory",    "\uF276 "},
      {"atarist",      "\uF248 "},
      {"amstradcpc",   "\uF246 "},
      {"msx",          "\uF24d "},
      {"msx1",         "\uF24e "},
      {"msx2",         "\uF24f "},
      {"odyssey2",     "\uF241 "},
      {"zx81",         "\uF250 "},
      {"zxspectrum",   "\uF251 "},
      {"moonlight",    "\uF27e "},
      {"apple2",       "\uF247 "},
      {"gamecube",     "\uF262 "},
      {"wii",          "\uF263 "},
      {"imageviewer",  "\uF27b "},
    };
  return _FavoritesIconsMap;
}

const char* SystemIcons::GetIcon(const std::string& systemShortName)
{
  const std::map<std::string, const char*>& map = FavoritesIconsMap();
  auto it = map.find(systemShortName);
  if (it != map.end())
    return it->second;
  return nullptr;
}

