#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 08/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestAsciiCase: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestAsciiCase, TestASCIILower)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! HAHAHA!");
  String sBaseLower(" *<{[(recalbox)]}>*\n\t- the best retrogaming system! try it, you won't be disappointed! hahaha!");
  for(int i = sBase.Count(); --i >= 0; )
  {
    String sub(sBase.substr(0, i).data());
    String compareTo(sBaseLower, 0, i);
    ASSERT_TRUE(sub.LowerCase() == compareTo);
    ASSERT_TRUE(sub == compareTo);
  }
  for(int i = sBase.Count(); --i >= 0; )
  {
    String sub(sBase.substr(0, i).data());
    String subUnmodified(sub);
    String compareTo(sBaseLower, 0, i);
    ASSERT_TRUE(sub.ToLowerCase() == compareTo);
    ASSERT_TRUE(sub == subUnmodified);
  }
}

TEST_F(StringTestAsciiCase, TestASCIILowerFrom)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! HAHAHA!");
  String sBaseLower(" *<{[(recalbox)]}>*\n\t- the best retrogaming system! try it, you won't be disappointed! hahaha!");
  for (int start = sBase.Count(); --start >= 0;)
  {
    String s(sBase);
    String sCopy(s);
    String sCompareTo = sBase.SubString(0, start).Append(sBaseLower, start);
    ASSERT_TRUE(s.LowerCase(start) == sCompareTo);
    ASSERT_TRUE(s == sCompareTo);
    s.Assign(sBase);
    ASSERT_TRUE(s.ToLowerCase(start) == sCompareTo);
    ASSERT_TRUE(s == sCopy);
  }
}

TEST_F(StringTestAsciiCase, TestASCIILowerFromLength)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! HAHAHA!");
  String sBaseLower(" *<{[(recalbox)]}>*\n\t- the best retrogaming system! try it, you won't be disappointed! hahaha!");
  for (int start = sBase.Count(); --start >= 0;)
    for (int length = sBase.Count() - start; --length >= 0; )
    {
      String s(sBase);
      String sCopy(s);
      String sCompareTo = sBase.SubString(0, start).Append(sBaseLower, start, length).Append(sBase, start + length);
      ASSERT_TRUE(s.LowerCase(start, length) == sCompareTo);
      ASSERT_TRUE(s == sCompareTo);
      s.Assign(sBase);
      ASSERT_TRUE(s.ToLowerCase(start, length) == sCompareTo);
      ASSERT_TRUE(s == sCopy);
    }
}

TEST_F(StringTestAsciiCase, TestASCIIUpper)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! Hahaha!");
  String sBaseUpper(" *<{[(RECALBOX)]}>*\n\t- THE BEST RETROGAMING SYSTEM! TRY IT, YOU WON'T BE DISAPPOINTED! HAHAHA!");
  for(int i = sBase.Count(); --i >= 0; )
  {
    String sub(sBase.substr(0, i).data());
    String compareTo(sBaseUpper, 0, i);
    ASSERT_TRUE(sub.UpperCase() == compareTo);
    ASSERT_TRUE(sub == compareTo);
  }
  for(int i = sBase.Count(); --i >= 0; )
  {
    String sub(sBase.substr(0, i).data());
    String subUnmodified(sub);
    String compareTo(sBaseUpper, 0, i);
    ASSERT_TRUE(sub.ToUpperCase() == compareTo);
    ASSERT_TRUE(sub == subUnmodified);
  }
}

TEST_F(StringTestAsciiCase, TestASCIIUpperFrom)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! Hahaha!");
  String sBaseUpper(" *<{[(RECALBOX)]}>*\n\t- THE BEST RETROGAMING SYSTEM! TRY IT, YOU WON'T BE DISAPPOINTED! HAHAHA!");
  for (int start = sBase.Count(); --start >= 0;)
  {
    String s(sBase);
    String sCopy(s);
    String sCompareTo = sBase.SubString(0, start).Append(sBaseUpper, start);
    ASSERT_TRUE(s.UpperCase(start) == sCompareTo);
    ASSERT_TRUE(s == sCompareTo);
    s.Assign(sBase);
    ASSERT_TRUE(s.ToUpperCase(start) == sCompareTo);
    ASSERT_TRUE(s == sCopy);
  }
}

TEST_F(StringTestAsciiCase, TestASCIIUpperFromLength)
{
  String sBase     (" *<{[(RECALBOX)]}>*\n\t- The best Retrogaming system! Try it, you WON'T be disappointed! Hahaha!");
  String sBaseUpper(" *<{[(RECALBOX)]}>*\n\t- THE BEST RETROGAMING SYSTEM! TRY IT, YOU WON'T BE DISAPPOINTED! HAHAHA!");
  for (int start = sBase.Count(); --start >= 0;)
    for (int length = sBase.Count() - start; --length >= 0; )
    {
      String s(sBase);
      String sCopy(s);
      String sCompareTo = sBase.SubString(0, start).Append(sBaseUpper, start, length).Append(sBase, start + length);
      ASSERT_TRUE(s.UpperCase(start, length) == sCompareTo);
      ASSERT_TRUE(s == sCompareTo);
      s.Assign(sBase);
      ASSERT_TRUE(s.ToUpperCase(start, length) == sCompareTo);
      ASSERT_TRUE(s == sCopy);
    }
}

#pragma clang diagnostic pop