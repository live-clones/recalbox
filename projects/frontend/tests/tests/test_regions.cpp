//
// Created by bkg2k on 27/05/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <games/classifications/Regions.h>
#include <string>

class RegionTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

#define F(t) ((int)Regions::GameRegions::t)
#define Quad(r1,r2,r3,r4) ((F(r1) << 24) | (F(r2) << 16) | (F(r3) << 8) | (F(r4) << 0))

TEST_F(RegionTest, TestRegionReset)
{
  Regions::RegionPack regions;
  ASSERT_TRUE(regions.Pack == 0);
  regions.Pack = 1;
  regions.Reset();
  ASSERT_TRUE(regions.Pack == 0);
}

TEST_F(RegionTest, TestRegionPackPush)
{
  Regions::RegionPack regions;
  regions.Push(Regions::GameRegions::DE);
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, DE));
  regions.Pack = 0; // Reset
  regions.Push(Regions::GameRegions::FR);
  regions.Push(Regions::GameRegions::JP);
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, JP, FR));
  regions.Pack = 0; // Reset
  regions.Push(Regions::GameRegions::US);
  regions.Push(Regions::GameRegions::FR);
  regions.Push(Regions::GameRegions::JP);
  ASSERT_EQ(regions.Pack, Quad(Unknown, JP, FR, US));
  regions.Pack = 0; // Reset
  regions.Push(Regions::GameRegions::US);
  regions.Push(Regions::GameRegions::FR);
  regions.Push(Regions::GameRegions::IT);
  regions.Push(Regions::GameRegions::JP);
  ASSERT_EQ(regions.Pack, Quad(JP, IT, FR, US));

  // Ignore push if full
  regions.Pack = 0; // Reset
  regions.Push(Regions::GameRegions::US);
  regions.Push(Regions::GameRegions::FR);
  regions.Push(Regions::GameRegions::IT);
  regions.Push(Regions::GameRegions::JP);
  regions.Push(Regions::GameRegions::DE);
  regions.Push(Regions::GameRegions::ES);
  ASSERT_EQ(regions.Pack, Quad(JP, IT, FR, US));
}

TEST_F(RegionTest, TestRegionFirst)
{
  Regions::RegionPack regions;
  regions.Pack =  Quad(Unknown, Unknown, Unknown, Unknown);
  ASSERT_EQ(regions.First(), Regions::GameRegions::Unknown);
  regions.Pack =  Quad(Unknown, Unknown, Unknown, US);
  ASSERT_EQ(regions.First(), Regions::GameRegions::US);
  regions.Pack =  Quad(Unknown, IT, Unknown, Unknown);
  ASSERT_EQ(regions.First(), Regions::GameRegions::IT);
  regions.Pack =  Quad(FR, JP, DE, Unknown);
  ASSERT_EQ(regions.First(), Regions::GameRegions::DE);
}

TEST_F(RegionTest, TestRegionHasRegion1)
{
  Regions::RegionPack regions;
  ASSERT_FALSE(regions.HasRegion());
  regions.Push(Regions::GameRegions::FR);
  ASSERT_TRUE(regions.HasRegion());
}

TEST_F(RegionTest, TestRegionHasRegion2)
{
  Regions::RegionPack regions;
  regions.Push(Regions::GameRegions::US);
  regions.Push(Regions::GameRegions::FR);
  regions.Push(Regions::GameRegions::IT);
  regions.Push(Regions::GameRegions::JP);
  ASSERT_TRUE(regions.HasRegion(Regions::GameRegions::FR));
  ASSERT_TRUE(regions.HasRegion(Regions::GameRegions::US));
  ASSERT_TRUE(regions.HasRegion(Regions::GameRegions::IT));
  ASSERT_TRUE(regions.HasRegion(Regions::GameRegions::JP));
}

TEST_F(RegionTest, TestDeserializeRegions1)
{
  // Deserialize lowercases
  Regions::RegionPack regions = Regions::Deserialize4Regions("eu");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, EU));
  regions = Regions::Deserialize4Regions("eu,us");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, US, EU));
  regions = Regions::Deserialize4Regions("eu,us,jp");
  ASSERT_EQ(regions.Pack, Quad(Unknown, JP, US, EU));
  regions = Regions::Deserialize4Regions("eu,us,jp,fr");
  ASSERT_EQ(regions.Pack, Quad(FR, JP, US, EU));

  // Do not deserialize uppercase
  regions = Regions::Deserialize4Regions("EU,US,JP,FR");
  ASSERT_EQ(regions.Pack, 0);
}

TEST_F(RegionTest, TestDeserializeRegions2)
{
  // Deserialize natural names
  Regions::RegionPack regions = Regions::Deserialize4Regions("Europe");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, EU));
  regions = Regions::Deserialize4Regions("Europe,USA");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, US, EU));
  regions = Regions::Deserialize4Regions("Europe,USA,Japan");
  ASSERT_EQ(regions.Pack, Quad(Unknown, JP, US, EU));
  regions = Regions::Deserialize4Regions("Europe,USA,Japan,France");
  ASSERT_EQ(regions.Pack, Quad(FR, JP, US, EU));

  // Check non case sensitivity
  regions = Regions::Deserialize4Regions("eUrOpE,UsA,jApAn,FrAnCe");
  ASSERT_EQ(regions.Pack, Quad(FR, JP, US, EU));
}

TEST_F(RegionTest, TestSerializeRegions)
{
  Regions::RegionPack regions;
  regions.Pack = Quad(Unknown, Unknown, Unknown, EU);
  std::string s = Regions::Serialize4Regions(regions);
  ASSERT_EQ(s, "eu");
  regions.Pack = Quad(Unknown, Unknown, US, EU);
  s = Regions::Serialize4Regions(regions);
  ASSERT_EQ(s, "eu,us");
  regions.Pack = Quad(Unknown, JP, US, EU);
  s = Regions::Serialize4Regions(regions);
  ASSERT_EQ(s, "eu,us,jp");
  regions.Pack = Quad(IT, JP, US, EU);
  s = Regions::Serialize4Regions(regions);
  ASSERT_EQ(s, "eu,us,jp,it");
}

TEST_F(RegionTest, TestIsIn4Regions)
{
  Regions::RegionPack regions;
  regions.Pack = Quad(Unknown, Unknown, Unknown, EU);
  ASSERT_TRUE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::EU));
  ASSERT_FALSE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::US));
  regions.Pack = Quad(JP, US, FR, EU);
  ASSERT_TRUE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::EU));
  ASSERT_TRUE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::JP));
  ASSERT_TRUE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::US));
  ASSERT_TRUE(Regions::IsIn4Regions(regions.Pack, Regions::GameRegions::FR));
}

TEST_F(RegionTest, TestExtractNoIntro)
{
  // Real cases
  Regions::RegionPack regions = Regions::ExtractRegionsFromNoIntroName("[BIOS] LaserActive (Japan) (v1.02).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, JP));
  regions = Regions::ExtractRegionsFromNoIntroName("[BIOS] LaserActive (USA) (v1.02).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, US));
  regions = Regions::ExtractRegionsFromNoIntroName("[BIOS] Mega-CD 2 (Europe) (v2.00W).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, EU));
  regions = Regions::ExtractRegionsFromNoIntroName("[BIOS] Mega-CD (Asia) (v1.00S).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, ASI));
  regions = Regions::ExtractRegionsFromNoIntroName("Barver Battle Saga - Tai Kong Zhan Shi (China) (Unl).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, CN));
  regions = Regions::ExtractRegionsFromNoIntroName("[BIOS] Sega TMSS (World).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, WOR));
  regions = Regions::ExtractRegionsFromNoIntroName("Arnold Palmer Tournament Golf (USA, Europe).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, EU, US));
  regions = Regions::ExtractRegionsFromNoIntroName("Ayrton Senna's Super Monaco GP II (Japan, Europe) (En,Ja).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, EU, JP));
  regions = Regions::ExtractRegionsFromNoIntroName("Battle Mania Daiginjou (Japan, Korea).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, KR, JP));

  // No regions
  regions = Regions::ExtractRegionsFromNoIntroName("Recalbox ultimate game (Gloubi, Boulga).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, Unknown));
}

TEST_F(RegionTest, TestExtractTOSEC)
{
  // Real cases
  Regions::RegionPack regions = Regions::ExtractRegionsFromTosecName("Accele Brid (1993)(Takara-Tomy)(JP)[tr en]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, JP));
  regions = Regions::ExtractRegionsFromTosecName("3 Ninjas Kick Back (1994)(Sony)(US)[a].zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, US));
  regions = Regions::ExtractRegionsFromTosecName("Act Raiser 2 (1994)(Enix - Quintet)(EU)[tr es].zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, EU));
  regions = Regions::ExtractRegionsFromTosecName("Act Raiser (1992)(Enix - Quintet)(DE).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, DE));
  regions = Regions::ExtractRegionsFromTosecName("Araiguma Rascal (1994)(NCS)(JP-US).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, US, JP));
  regions = Regions::ExtractRegionsFromTosecName("Ardy Lightfoot (1994)(Ascii)(EU-JP).zip");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, JP, EU));
  regions = Regions::ExtractRegionsFromTosecName("Battle Pinball (1995)(Banpresto)(FR-DE-IT-ES).zip");
  ASSERT_EQ(regions.Pack, Quad(ES, IT, DE, FR));
}

TEST_F(RegionTest, TestExtractFromName)
{
  // Real cases
  Regions::RegionPack regions = Regions::ExtractRegionsFromName("Yamamoto Kakapote [JP]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, JP));
  regions = Regions::ExtractRegionsFromName("We love guns way too much [US]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, US));
  regions = Regions::ExtractRegionsFromName("Tales of the old Continent [EU]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, EU));
  regions = Regions::ExtractRegionsFromName("Ja! Arbeit! Schnell! [DE]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, Unknown, DE));
  regions = Regions::ExtractRegionsFromName("The Pacific War [JP,US]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, US, JP));
  regions = Regions::ExtractRegionsFromName("A Samouraï in Europe [EU,JP]");
  ASSERT_EQ(regions.Pack, Quad(Unknown, Unknown, JP, EU));
  regions = Regions::ExtractRegionsFromName("Almosty Friends [FR,DE,IT,ES]");
  ASSERT_EQ(regions.Pack, Quad(ES, IT, DE, FR));

  // Check non case sensitivity
  regions = Regions::ExtractRegionsFromName("Almosty Friends [Fr,dE,It,eS]");
  ASSERT_EQ(regions.Pack, Quad(ES, IT, DE, FR));
}
