#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 07/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestTrimming: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestTrimming, TestTrim)
{
  ASSERT_TRUE(String().Trim().empty());
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.Trim() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.Trim() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.Trim() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.Trim() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
}

TEST_F(StringTestTrimming, TestTrimLeft)
{
  ASSERT_TRUE(String().TrimLeft().empty());
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.TrimLeft() == "Recalbox\t ");
  ASSERT_TRUE(s == "Recalbox\t ");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.TrimLeft() == "Recalbox\t\t  ");
  ASSERT_TRUE(s == "Recalbox\t\t  ");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.TrimLeft() == "Recalbox\t\t  ");
  ASSERT_TRUE(s == "Recalbox\t\t  ");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.TrimLeft() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
}

TEST_F(StringTestTrimming, TestTrimRight)
{
  ASSERT_TRUE(String().TrimRight().empty());
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.TrimRight() == " \tRecalbox");
  ASSERT_TRUE(s == " \tRecalbox");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.TrimRight() == "\r\n\n\rRecalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.TrimRight() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.TrimRight() == "\r\n\n\rRecalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox");
}

TEST_F(StringTestTrimming, TestToTrim)
{
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.ToTrim() == "Recalbox");
  ASSERT_TRUE(s == " \tRecalbox\t ");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.ToTrim() == "Recalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox\t\t  ");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.ToTrim() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox\t\t  ");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.ToTrim() == "Recalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox");
}

TEST_F(StringTestTrimming, TestToTrimLeft)
{
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.ToTrimLeft() == "Recalbox\t ");
  ASSERT_TRUE(s == " \tRecalbox\t ");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.ToTrimLeft() == "Recalbox\t\t  ");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox\t\t  ");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.ToTrimLeft() == "Recalbox\t\t  ");
  ASSERT_TRUE(s == "Recalbox\t\t  ");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.ToTrimLeft() == "Recalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox");
}

TEST_F(StringTestTrimming, TestToTrimRight)
{
  String s(" \tRecalbox\t ");
  ASSERT_TRUE(s.ToTrimRight() == " \tRecalbox");
  ASSERT_TRUE(s == " \tRecalbox\t ");
  s.Assign("\r\n\n\rRecalbox\t\t  ");
  ASSERT_TRUE(s.ToTrimRight() == "\r\n\n\rRecalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox\t\t  ");
  s.Assign("Recalbox\t\t  ");
  ASSERT_TRUE(s.ToTrimRight() == "Recalbox");
  ASSERT_TRUE(s == "Recalbox\t\t  ");
  s.Assign("\r\n\n\rRecalbox");
  ASSERT_TRUE(s.ToTrimRight() == "\r\n\n\rRecalbox");
  ASSERT_TRUE(s == "\r\n\n\rRecalbox");
}

TEST_F(StringTestTrimming, TestTrimConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimLeftConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]]");
  ASSERT_TRUE(s == "Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimRightConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.TrimRight(trimmed) == "[[Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.TrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.TrimRight(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.TrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimLeftConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]]");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimRightConstChar)
{
  const char* trimmed = "{[<()>]}";
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "[[Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimLeftString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]]");
  ASSERT_TRUE(s == "Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimRightString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.TrimRight(trimmed) == "[[Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.TrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.TrimRight(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.TrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimLeftString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]]");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox]}>");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestToTrimRightString)
{
  String trimmed("{[<()>]}");
  String s("[[Recal[)ox]]");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "[[Recal[)ox");
  ASSERT_TRUE(s == "[[Recal[)ox]]");
  s.Assign("<{[Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox]}>");
  s.Assign("Recal[)ox]}>");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "Recal[)ox");
  ASSERT_TRUE(s == "Recal[)ox]}>");
  s.Assign("<{[Recal[)ox");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "<{[Recal[)ox");
  ASSERT_TRUE(s == "<{[Recal[)ox");
}

TEST_F(StringTestTrimming, TestTrimChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.Trim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
}

TEST_F(StringTestTrimming, TestTrimLeftChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal*Box**");
  ASSERT_TRUE(s == "Recal*Box**");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal*Box***");
  ASSERT_TRUE(s == "Recal*Box***");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal*Box***");
  ASSERT_TRUE(s == "Recal*Box***");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.TrimLeft(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
}

TEST_F(StringTestTrimming, TestTrimRightChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.TrimRight(trimmed) == "**Recal*Box");
  ASSERT_TRUE(s == "**Recal*Box");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.TrimRight(trimmed) == "***Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.TrimRight(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.TrimRight(trimmed) == "***Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box");
}

TEST_F(StringTestTrimming, TestToTrimChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "**Recal*Box**");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box***");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box***");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.ToTrim(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box");
}

TEST_F(StringTestTrimming, TestToTrimLeftChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal*Box**");
  ASSERT_TRUE(s == "**Recal*Box**");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal*Box***");
  ASSERT_TRUE(s == "***Recal*Box***");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal*Box***");
  ASSERT_TRUE(s == "Recal*Box***");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.ToTrimLeft(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box");
}

TEST_F(StringTestTrimming, TestToTrimRightChar)
{
  char trimmed = '*';
  String s("**Recal*Box**");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "**Recal*Box");
  ASSERT_TRUE(s == "**Recal*Box**");
  s.Assign("***Recal*Box***");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "***Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box***");
  s.Assign("Recal*Box***");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "Recal*Box");
  ASSERT_TRUE(s == "Recal*Box***");
  s.Assign("***Recal*Box");
  ASSERT_TRUE(s.ToTrimRight(trimmed) == "***Recal*Box");
  ASSERT_TRUE(s == "***Recal*Box");
}

#pragma clang diagnostic pop