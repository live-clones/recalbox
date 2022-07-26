#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 12/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestSplitAndJoin: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestSplitAndJoin, TestSplitChar)
{
  String s("Recalbox|Is|The|Best!");
  String::List l = s.Split('|');
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCharMax)
{
  String s("Recalbox|Is|The|Best!");
  String::List l = s.Split('|', 2);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is|The|Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCharMerge)
{
  String s("Recalbox||Is||||The||||||||||Best!");
  String::List l = s.Split('|', true);
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCharMaxMerge)
{
  String s("Recalbox||Is||||The||||||||||Best!");
  String::List l = s.Split('|', 2, true);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is||||The||||||||||Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCString)
{
  String s("Recalbox{#}Is{#}The{#}Best!");
  String::List l = s.Split("{#}");
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCStringMerge)
{
  String s("Recalbox{#}{#}Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
  String::List l = s.Split("{#}", true);
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCStringMaxMerge)
{
  String s("Recalbox{#}{#}Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
  String::List l = s.Split("{#}", 2, true);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCStringLength)
{
  String s("Recalbox{#}Is{#}The{#}Best!");
  String::List l = s.Split(LEGACY_STRING("{#}"));
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCStringLengthMax)
{
  String s("Recalbox{#}Is{#}The{#}Best!");
  String::List l = s.Split(LEGACY_STRING("{#}"), 2);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is{#}The{#}Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitCStringLengthMaxMerge)
{
  String s("Recalbox{#}{#}Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
  String::List l = s.Split(LEGACY_STRING("{#}"), 2, true);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitString)
{
  String s("Recalbox{#}Is{#}The{#}Best!");
  String p("{#}");
  String::List l = s.Split(p);
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitStringMax)
{
  String s("Recalbox{#}Is{#}The{#}Best!");
  String p("{#}");
  String::List l = s.Split(p, 2);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is{#}The{#}Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitStringMerge)
{
  String s("Recalbox{#}{#}Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
  String p("{#}");
  String::List l = s.Split(p, true);
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitStringMaxMerge)
{
  String s("Recalbox{#}{#}Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
  String p("{#}");
  String::List l = s.Split(p, 2, true);
  ASSERT_TRUE(l.size() == 2);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is{#}{#}{#}{#}The{#}{#}{#}{#}{#}{#}{#}{#}{#}{#}Best!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedChar)
{
  String s("Recalbox Is The \"Best Retrogaming System\"!");
  String::List l = s.SplitQuoted(' ');
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best Retrogaming System\"!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCharMax)
{
  String s("Recalbox Is The \"Best Retrogaming System\" and it's easy to setup!");
  String::List l = s.SplitQuoted(' ', 6);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best Retrogaming System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's easy to setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCharMerge)
{
  String s("Recalbox  Is   The     \"Best Retrogaming System\"!");
  String::List l = s.SplitQuoted(' ', true);
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best Retrogaming System\"!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCharMaxMerge)
{
  String s("Recalbox  Is    The        \"Best Retrogaming System\"         and      it's easy to setup!");
  String::List l = s.SplitQuoted(' ', 6, true);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best Retrogaming System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's easy to setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCString)
{
  String s("Recalbox--Is--The--\"Best--Retrogaming--System\"--and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted("--");
  ASSERT_TRUE(l.size() == 9);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's");
  ASSERT_TRUE(l[6] == "easy");
  ASSERT_TRUE(l[7] == "to");
  ASSERT_TRUE(l[8] == "setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCStringMerged)
{
  String s("Recalbox--Is------The----\"Best--Retrogaming--System\"----and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted("--", true);
  ASSERT_TRUE(l.size() == 9);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's");
  ASSERT_TRUE(l[6] == "easy");
  ASSERT_TRUE(l[7] == "to");
  ASSERT_TRUE(l[8] == "setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCStringMaxMerged)
{
  String s("Recalbox--Is------The----\"Best--Retrogaming--System\"----and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted("--", 6, true);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's--easy--to--setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCStringLength)
{
  String s("Recalbox--Is--The--\"Best--Retrogaming--System\"--and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(LEGACY_STRING("--"));
  ASSERT_TRUE(l.size() == 9);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's");
  ASSERT_TRUE(l[6] == "easy");
  ASSERT_TRUE(l[7] == "to");
  ASSERT_TRUE(l[8] == "setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCStringLengthMax)
{
  String s("Recalbox--Is--The--\"Best--Retrogaming--System\"--and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(LEGACY_STRING("--"), 6);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's--easy--to--setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedCStringLengthMaxMerged)
{
  String s("Recalbox--Is------The----\"Best--Retrogaming--System\"----and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(LEGACY_STRING("--"), 6, true);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's--easy--to--setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedStringLength)
{
  String s("Recalbox--Is--The--\"Best--Retrogaming--System\"--and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(String("--"));
  ASSERT_TRUE(l.size() == 9);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's");
  ASSERT_TRUE(l[6] == "easy");
  ASSERT_TRUE(l[7] == "to");
  ASSERT_TRUE(l[8] == "setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedStringLengthMax)
{
  String s("Recalbox--Is--The--\"Best--Retrogaming--System\"--and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(String("--"), 6);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's--easy--to--setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedStringLengthMerged)
{
  String s("Recalbox--Is------The----\"Best--Retrogaming--System\"----and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(String("--"), true);
  ASSERT_TRUE(l.size() == 9);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's");
  ASSERT_TRUE(l[6] == "easy");
  ASSERT_TRUE(l[7] == "to");
  ASSERT_TRUE(l[8] == "setup!");
}

TEST_F(StringTestSplitAndJoin, TestSplitQuotedStringLengthMaxMerged)
{
  String s("Recalbox--Is------The----\"Best--Retrogaming--System\"----and--it's--easy--to--setup!");
  String::List l = s.SplitQuoted(String("--"), 6, true);
  ASSERT_TRUE(l.size() == 6);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "\"Best--Retrogaming--System\"");
  ASSERT_TRUE(l[4] == "and");
  ASSERT_TRUE(l[5] == "it's--easy--to--setup!");
}

#pragma clang diagnostic pop