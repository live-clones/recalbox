#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 10/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestReplacers: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestReplacers, TestReplaceCharChar)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace('*', 'o') == "Recalbox is a cool box, that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(',', ';') == "Recalbox is a cool box; that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace('*', 'y') == "Recalbox is a cool box; that makes you feeling nostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceCharString)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace('*', String("oOo")) == "RecalboOox is a coOooOol boOox, that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace(',', String(" and")) == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace('*', String("o")) == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceCharCString)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace('*', "oOo") == "RecalboOox is a coOooOol boOox, that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace(',', " and") == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace('*', "o") == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceCharCStringSize)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace('*', LEGACY_STRING("oOo")) == "RecalboOox is a coOooOol boOox, that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace(',', LEGACY_STRING(" and")) == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
  ASSERT_TRUE(s.Replace('*', LEGACY_STRING("o")) == "RecalboOox is a coOooOol boOox and that makes yoOou feeling noOostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceStringChar)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace(String("*"), 'o') == "Recalbox is a cool box, that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(String(","), ';') == "Recalbox is a cool box; that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(String("*"), 'o') == "Recalbox is a cool box; that makes you feeling nostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceStringString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("RappelBoite"), String("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("bad"), String("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("angry!"), String("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceStringCString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("RappelBoite"), "Recalbox") == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("bad"), "cool") == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("angry!"), "HAPPY :D") == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceStringCStringSize)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("RappelBoite"), LEGACY_STRING("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("bad"), LEGACY_STRING("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(String("angry!"), LEGACY_STRING("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringChar)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace("*", 'o') == "Recalbox is a cool box, that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(",", ';') == "Recalbox is a cool box; that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace("*", 'o') == "Recalbox is a cool box; that makes you feeling nostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceCStringString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("RappelBoite", String("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("bad", String("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("angry!", String("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringCString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("RappelBoite", "Recalbox") == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("bad", "cool") == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("angry!", "HAPPY :D") == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringCStringSize)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("RappelBoite", LEGACY_STRING("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("bad", LEGACY_STRING("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace("angry!", LEGACY_STRING("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringSizeChar)
{
  String s("Recalb*x is a c**l b*x, that makes y*u feeling n*stalgic!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("*"), 'o') == "Recalbox is a cool box, that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING(","), ';') == "Recalbox is a cool box; that makes you feeling nostalgic!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("*"), 'o') == "Recalbox is a cool box; that makes you feeling nostalgic!");
}

TEST_F(StringTestReplacers, TestReplaceCStringSizeString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("RappelBoite"), String("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("bad"), String("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("angry!"), String("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringSizeCString)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("RappelBoite"), "Recalbox") == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("bad"), "cool") == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("angry!"), "HAPPY :D") == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

TEST_F(StringTestReplacers, TestReplaceCStringSizeCStringSize)
{
  String s("RappelBoite is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("RappelBoite"), LEGACY_STRING("Recalbox")) == "Recalbox is a bad box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("bad"), LEGACY_STRING("cool")) == "Recalbox is a cool box, that makes you feeling angry!");
  ASSERT_TRUE(s.Replace(LEGACY_STRING("angry!"), LEGACY_STRING("HAPPY :D")) == "Recalbox is a cool box, that makes you feeling HAPPY :D");
}

#pragma clang diagnostic pop