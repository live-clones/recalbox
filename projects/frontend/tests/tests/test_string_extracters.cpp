#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 15/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestExtracters: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestExtracters, TestExtractCharLeftRight)
{
  String s("Recalbox=Super cool!");
  String left, right;
  ASSERT_TRUE(s.Extract('=', left, right, false));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");

  s.Assign("\tRecalbox : Super cool!\r\n");
  ASSERT_FALSE(s.Extract('=', left, right, false));

  left.clear();
  right.clear();
  ASSERT_TRUE(s.Extract(':', left, right, true));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");
}

TEST_F(StringTestExtracters, TestExtractCStringLeftRight)
{
  String s("Recalbox==Super cool!");
  String left, right;
  ASSERT_TRUE(s.Extract("==", left, right, false));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");

  s.Assign("\tRecalbox :: Super cool!\r\n");
  ASSERT_FALSE(s.Extract("==", left, right, false));

  left.clear();
  right.clear();
  ASSERT_TRUE(s.Extract("::", left, right, true));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");
}

TEST_F(StringTestExtracters, TestExtractCStringLengthLeftRight)
{
  String s("Recalbox==Super cool!");
  String left, right;
  ASSERT_TRUE(s.Extract(LEGACY_STRING("=="), left, right, false));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");

  s.Assign("\tRecalbox :: Super cool!\r\n");
  ASSERT_FALSE(s.Extract(LEGACY_STRING("=="), left, right, false));

  left.clear();
  right.clear();
  ASSERT_TRUE(s.Extract(LEGACY_STRING("::"), left, right, true));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");
}

TEST_F(StringTestExtracters, TestExtractStringLeftRight)
{
  String s("Recalbox==Super cool!");
  String left, right;
  ASSERT_TRUE(s.Extract(String("=="), left, right, false));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");

  s.Assign("\tRecalbox :: Super cool!\r\n");
  ASSERT_FALSE(s.Extract(String("=="), left, right, false));

  left.clear();
  right.clear();
  ASSERT_TRUE(s.Extract(String("::"), left, right, true));
  ASSERT_TRUE(left == "Recalbox");
  ASSERT_TRUE(right == "Super cool!");
}

TEST_F(StringTestExtracters, TestExtractMiddleChar)
{
  String s("Do you know what si (Recalbox)? It's a cool retrogaming system!");
  String result;
  ASSERT_FALSE(s.Extract('[', ']', result, false));
  ASSERT_FALSE(s.Extract('(', '(', result, false));
  ASSERT_FALSE(s.Extract(')', ')', result, false));
  ASSERT_TRUE(s.Extract('(', ')', result, true));
  ASSERT_TRUE(result == "Recalbox");
}

TEST_F(StringTestExtracters, TestExtractMiddleCString)
{
  String s("Do you know what si <{(Recalbox)}>? It's a cool retrogaming system!");
  String result;
  ASSERT_FALSE(s.Extract("[[[", "]]]", result, false));
  ASSERT_FALSE(s.Extract("<{(", "<{(", result, false));
  ASSERT_FALSE(s.Extract(")}>", ")}>", result, false));
  ASSERT_TRUE(s.Extract("<{(", ")}>", result, true));
  ASSERT_TRUE(result == "Recalbox");
}

TEST_F(StringTestExtracters, TestExtractMiddleCStringLength)
{
  String s("Do you know what si <{(Recalbox)}>? It's a cool retrogaming system!");
  String result;
  ASSERT_FALSE(s.Extract(LEGACY_STRING("[[["), LEGACY_STRING("]]]"), result, false));
  ASSERT_FALSE(s.Extract(LEGACY_STRING("<{("), LEGACY_STRING("<{("), result, false));
  ASSERT_FALSE(s.Extract(LEGACY_STRING(")}>"), LEGACY_STRING(")}>"), result, false));
  ASSERT_TRUE(s.Extract(LEGACY_STRING("<{("), LEGACY_STRING(")}>"), result, true));
  ASSERT_TRUE(result == "Recalbox");
}

TEST_F(StringTestExtracters, TestExtractMiddleString)
{
  String s("Do you know what si <{(Recalbox)}>? It's a cool retrogaming system!");
  String result;
  ASSERT_FALSE(s.Extract(String("[[["), String("]]]"), result, false));
  ASSERT_FALSE(s.Extract(String("<{("), String("<{("), result, false));
  ASSERT_FALSE(s.Extract(String(")}>"), String(")}>"), result, false));
  ASSERT_TRUE(s.Extract(String("<{("), String(")}>"), result, true));
  ASSERT_TRUE(result == "Recalbox");
}

#pragma clang diagnostic pop