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
#include "utils/datetime/DateTime.h"

class StringTestFinders: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestFinders, TestContainsChar)
{
  String s("123");
  ASSERT_TRUE(s.Contains('*') == false);
  ASSERT_TRUE(s.Contains('1') == true);
  ASSERT_TRUE(s.Contains('2') == true);
  ASSERT_TRUE(s.Contains('3') == true);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Contains('B') == false);
  ASSERT_TRUE(s.Contains('R') == true);
  ASSERT_TRUE(s.Contains('l') == true);
  ASSERT_TRUE(s.Contains('x') == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Contains('!') == true);
  ASSERT_TRUE(s.Contains('-') == true);
  ASSERT_TRUE(s.Contains('+') == false);
  ASSERT_TRUE(s.Contains('\0') == false);
}

TEST_F(StringTestFinders, TestContainsCString)
{
  String s("123");
  ASSERT_TRUE(s.Contains("*") == false);
  ASSERT_TRUE(s.Contains("1") == true);
  ASSERT_TRUE(s.Contains("2") == true);
  ASSERT_TRUE(s.Contains("3") == true);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Contains("Box") == false);
  ASSERT_TRUE(s.Contains("Rec") == true);
  ASSERT_TRUE(s.Contains("lb") == true);
  ASSERT_TRUE(s.Contains("ox") == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Contains("console!") == true);
  ASSERT_TRUE(s.Contains("-times") == true);
  ASSERT_TRUE(s.Contains("+times") == false);
}

TEST_F(StringTestFinders, TestContainsCStringLength)
{
  String s("123");
  ASSERT_TRUE(s.Contains(LEGACY_STRING("*")) == false);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("1")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("2")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("3")) == true);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Contains(LEGACY_STRING("Box")) == false);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("Rec")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("lb")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("ox")) == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Contains(LEGACY_STRING("console!")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("-times")) == true);
  ASSERT_TRUE(s.Contains(LEGACY_STRING("+times")) == false);
}

TEST_F(StringTestFinders, TestContainsString)
{
  String s("123");
  ASSERT_TRUE(s.Contains(String("*")) == false);
  ASSERT_TRUE(s.Contains(String("1")) == true);
  ASSERT_TRUE(s.Contains(String("2")) == true);
  ASSERT_TRUE(s.Contains(String("3")) == true);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Contains(String("Box")) == false);
  ASSERT_TRUE(s.Contains(String("Rec")) == true);
  ASSERT_TRUE(s.Contains(String("lb")) == true);
  ASSERT_TRUE(s.Contains(String("ox")) == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Contains(String("console!")) == true);
  ASSERT_TRUE(s.Contains(String("-times")) == true);
  ASSERT_TRUE(s.Contains(String("+times")) == false);
}

TEST_F(StringTestFinders, TestStartsWidthChar)
{
  String s("123");
  ASSERT_TRUE(s.StartsWith('*') == false);
  ASSERT_TRUE(s.StartsWith('1') == true);
  ASSERT_TRUE(s.StartsWith('2') == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.StartsWith('B') == false);
  ASSERT_TRUE(s.StartsWith('R') == true);
  ASSERT_TRUE(s.StartsWith('l') == false);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.StartsWith('R') == true);
  ASSERT_TRUE(s.StartsWith('-') == false);
  ASSERT_TRUE(s.StartsWith('!') == false);
}

TEST_F(StringTestFinders, TestStartsWidthCString)
{
  String s("123");
  ASSERT_TRUE(s.StartsWith("*") == false);
  ASSERT_TRUE(s.StartsWith("1") == true);
  ASSERT_TRUE(s.StartsWith("2") == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.StartsWith("Box") == false);
  ASSERT_TRUE(s.StartsWith("Rec") == true);
  ASSERT_TRUE(s.StartsWith("lb") == false);
  ASSERT_TRUE(s.StartsWith("Recalbox") == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.StartsWith("console!") == false);
  ASSERT_TRUE(s.StartsWith("Recalbox") == true);
  ASSERT_TRUE(s.StartsWith("e") == false);
}

TEST_F(StringTestFinders, TestStartsWidthCStringLength)
{
  String s("123");
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("*")) == false);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("1")) == true);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("2")) == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("Box")) == false);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("Rec")) == true);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("lb")) == false);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("console!")) == false);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("Recalbox")) == true);
  ASSERT_TRUE(s.StartsWith(LEGACY_STRING("e")) == false);
}

TEST_F(StringTestFinders, TestStartsWidthString)
{
  String s("123");
  ASSERT_TRUE(s.StartsWith(String("*")) == false);
  ASSERT_TRUE(s.StartsWith(String("1")) == true);
  ASSERT_TRUE(s.StartsWith(String("2")) == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.StartsWith(String("Box")) == false);
  ASSERT_TRUE(s.StartsWith(String("Rec")) == true);
  ASSERT_TRUE(s.StartsWith(String("lb")) == false);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.StartsWith(String("console!")) == false);
  ASSERT_TRUE(s.StartsWith(String("Recalbox")) == true);
  ASSERT_TRUE(s.StartsWith(String("e")) == false);
}

TEST_F(StringTestFinders, TestEndsWidthChar)
{
  String s("123");
  ASSERT_TRUE(s.EndsWith('*') == false);
  ASSERT_TRUE(s.EndsWith('3') == true);
  ASSERT_TRUE(s.EndsWith('1') == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.EndsWith('R') == false);
  ASSERT_TRUE(s.EndsWith('x') == true);
  ASSERT_TRUE(s.EndsWith('B') == false);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.EndsWith('!') == true);
  ASSERT_TRUE(s.EndsWith('R') == false);
  ASSERT_TRUE(s.EndsWith('-') == false);
}

TEST_F(StringTestFinders, TestEndsWidthCString)
{
  String s("123");
  ASSERT_TRUE(s.EndsWith("*") == false);
  ASSERT_TRUE(s.EndsWith("3") == true);
  ASSERT_TRUE(s.EndsWith("1") == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.EndsWith("Box") == false);
  ASSERT_TRUE(s.EndsWith("box") == true);
  ASSERT_TRUE(s.EndsWith("Recal") == false);
  ASSERT_TRUE(s.EndsWith("Recalbox") == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.EndsWith("console!") == true);
  ASSERT_TRUE(s.EndsWith("Recalbox") == false);
  ASSERT_TRUE(s.EndsWith("e") == false);
}

TEST_F(StringTestFinders, TestEndsWidthCStringLength)
{
  String s("123");
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("*")) == false);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("3")) == true);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("1")) == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("Box")) == false);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("box")) == true);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("Recal")) == false);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("Recalbox")) == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("console!")) == true);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("Recalbox")) == false);
  ASSERT_TRUE(s.EndsWith(LEGACY_STRING("e")) == false);
}

TEST_F(StringTestFinders, TestEndsWidthString)
{
  String s("123");
  ASSERT_TRUE(s.EndsWith(String("*")) == false);
  ASSERT_TRUE(s.EndsWith(String("3")) == true);
  ASSERT_TRUE(s.EndsWith(String("1")) == false);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.EndsWith(String("Box")) == false);
  ASSERT_TRUE(s.EndsWith(String("box")) == true);
  ASSERT_TRUE(s.EndsWith(String("Recal")) == false);
  ASSERT_TRUE(s.EndsWith(String("Recalbox")) == true);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.EndsWith(String("console!")) == true);
  ASSERT_TRUE(s.EndsWith(String("Recalbox")) == false);
  ASSERT_TRUE(s.EndsWith(String("e")) == false);
}

TEST_F(StringTestFinders, TestFindChar)
{
  String s("123");
  ASSERT_TRUE(s.Find('*') < 0);
  ASSERT_TRUE(s.Find('1') == 0);
  ASSERT_TRUE(s.Find('2') == 1);
  ASSERT_TRUE(s.Find('3') == 2);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find('B') < 0);
  ASSERT_TRUE(s.Find('R') == 0);
  ASSERT_TRUE(s.Find('l') == 4);
  ASSERT_TRUE(s.Find('x') == 7);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Find('!') == 50);
  ASSERT_TRUE(s.Find('-') == 19);
  ASSERT_TRUE(s.Find('+') < 0);
  ASSERT_TRUE(s.Find('\0') < 0);
}

TEST_F(StringTestFinders, TestFindCString)
{
  String s("123");
  ASSERT_TRUE(s.Find("13") < 0);
  ASSERT_TRUE(s.Find("12") == 0);
  ASSERT_TRUE(s.Find("23") == 1);
  ASSERT_TRUE(s.Find("123") == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find("Freebox") < 0);
  ASSERT_TRUE(s.Find("Recal") == 0);
  ASSERT_TRUE(s.Find("box") == 5);
  ASSERT_TRUE(s.Find("x") == 7);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Find("Recalbox") == 0);
  ASSERT_TRUE(s.Find("-time") == 19);
  ASSERT_TRUE(s.Find("!") == 50);
}

TEST_F(StringTestFinders, TestFindString)
{
  String s("123");
  ASSERT_TRUE(s.Find(String("13")) < 0);
  ASSERT_TRUE(s.Find(String("12")) == 0);
  ASSERT_TRUE(s.Find(String("23")) == 1);
  ASSERT_TRUE(s.Find(String("123")) == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find(String("Freebox")) < 0);
  ASSERT_TRUE(s.Find(String("Recal")) == 0);
  ASSERT_TRUE(s.Find(String("box")) == 5);
  ASSERT_TRUE(s.Find(String("x")) == 7);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Find(String("Recalbox")) == 0);
  ASSERT_TRUE(s.Find(String("-time")) == 19);
  ASSERT_TRUE(s.Find(String("!")) == 50);
}

TEST_F(StringTestFinders, TestFindCharFrom)
{
  String s("123");
  ASSERT_TRUE(s.Find('*', 0) < 0);
  ASSERT_TRUE(s.Find('1', 2) < 0);
  ASSERT_TRUE(s.Find('2', 0) == 1);
  ASSERT_TRUE(s.Find('3', 1) == 2);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find('B', 0) < 0);
  ASSERT_TRUE(s.Find('R', -1) < 0);
  ASSERT_TRUE(s.Find('l', INT32_MAX) < 0);
  ASSERT_TRUE(s.Find('x', 4) == 7);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Find('!', 25) == 50);
  ASSERT_TRUE(s.Find('-', 40) < 0);
  ASSERT_TRUE(s.Find('+', 0) < 0);
  ASSERT_TRUE(s.Find('\0', 0) < 0);
}

TEST_F(StringTestFinders, TestFindCStringLengthFrom)
{
  String s("123");
  ASSERT_TRUE(s.Find(LEGACY_STRING("13"), 0) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("12"), 2) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("23"), 1) == 1);
  ASSERT_TRUE(s.Find(LEGACY_STRING("123"), 0) == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find(LEGACY_STRING("Rec"), 5) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("Recal"), 0) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("box"), 3) == 5);
  ASSERT_TRUE(s.Find(LEGACY_STRING("x"), 7) == 7);
  s.Assign("Recalbox is the all-times best retrogaming console! all-times, yes!");
  ASSERT_TRUE(s.Find(LEGACY_STRING("Recalbox"), 0) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("-time"), 31) == 55);
  ASSERT_TRUE(s.Find(LEGACY_STRING("!"), 10) == 50);
  ASSERT_TRUE(s.Find(LEGACY_STRING("!"), 51) == 66);
}

TEST_F(StringTestFinders, TestFindStringFrom)
{
  String s("123");
  ASSERT_TRUE(s.Find(String("13"), 0) < 0);
  ASSERT_TRUE(s.Find(String("12"), 2) < 0);
  ASSERT_TRUE(s.Find(String("23"), 1) == 1);
  ASSERT_TRUE(s.Find(String("123"), 0) == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find(String("Rec"), 5) < 0);
  ASSERT_TRUE(s.Find(String("Recal"), 0) == 0);
  ASSERT_TRUE(s.Find(String("box"), 3) == 5);
  ASSERT_TRUE(s.Find(String("x"), 7) == 7);
  s.Assign("Recalbox is the all-times best retrogaming console! all-times, yes!");
  ASSERT_TRUE(s.Find(String("Recalbox"), 0) == 0);
  ASSERT_TRUE(s.Find(String("-time"), 31) == 55);
  ASSERT_TRUE(s.Find(String("!"), 10) == 50);
  ASSERT_TRUE(s.Find(String("!"), 51) == 66);
}

TEST_F(StringTestFinders, TestFindCharFromLength)
{
  String s("123");
  ASSERT_TRUE(s.Find('*', 0, 1) < 0);
  ASSERT_TRUE(s.Find('1', 2, 3) < 0);
  ASSERT_TRUE(s.Find('2', 1, 2) == 1);
  ASSERT_TRUE(s.Find('3', 0, 2) < 0);
  ASSERT_TRUE(s.Find('2', 1, 5) == 1);
  ASSERT_TRUE(s.Find('3', 1, 1) < 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find('B', 0, INT32_MAX) < 0);
  ASSERT_TRUE(s.Find('R', -1, 3) < 0);
  ASSERT_TRUE(s.Find('o', 0, -1) < 0);
  ASSERT_TRUE(s.Find('l', 2, 5) == 4);
  ASSERT_TRUE(s.Find('x', 0, INT32_MAX) == 7);
  ASSERT_TRUE(s.Find('o', INT32_MAX, INT32_MAX) < 0);
  ASSERT_TRUE(s.Find('x', 7, 1) == 7);
  s.Assign("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.Find('!', 25, 26) == 50);
  ASSERT_TRUE(s.Find('-', 40, INT32_MAX) < 0);
  ASSERT_TRUE(s.Find('+', 0, 0) < 0);
  ASSERT_TRUE(s.Find('\0', 0, 0) < 0);
}

TEST_F(StringTestFinders, TestFindCStringLengthFromLength)
{
  String s("123");
  ASSERT_TRUE(s.Find(LEGACY_STRING("13"), 0, 3) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("12"), 0, 0) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("12"), 2, 1) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("12"), 0, 2) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("23"), 1, 2) == 1);
  ASSERT_TRUE(s.Find(LEGACY_STRING("23"), 1, INT32_MAX) == 1);
  ASSERT_TRUE(s.Find(LEGACY_STRING("123"), 0, INT32_MAX) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("123"), 0, 2) == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find(LEGACY_STRING("Recal"), 0, 3) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("Recal"), 1, 5) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("box"), 3, INT32_MAX) == 5);
  ASSERT_TRUE(s.Find(LEGACY_STRING("x"), 7, 1) == 7);
  ASSERT_TRUE(s.Find(LEGACY_STRING("x"), 7, 0) < 0);
  s.Assign("Recalbox is the all-times best retrogaming console! all-times, yes!");
  ASSERT_TRUE(s.Find(LEGACY_STRING("Recalbox"), 0, 10) == 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("all-times"), 0, INT32_MAX) == 16);
  ASSERT_TRUE(s.Find(LEGACY_STRING("all-times"), 50, 3) == 52);
  ASSERT_TRUE(s.Find(LEGACY_STRING("all-times"), 53, INT32_MAX) < 0);
  ASSERT_TRUE(s.Find(LEGACY_STRING("!"), 10, INT32_MAX) == 50);
  ASSERT_TRUE(s.Find(LEGACY_STRING("!"), 51, 20) == 66);
}

TEST_F(StringTestFinders, TestFindStringFromLenth)
{
  String s("123");
  ASSERT_TRUE(s.Find(String("13"), 0, 3) < 0);
  ASSERT_TRUE(s.Find(String("12"), 0, 0) < 0);
  ASSERT_TRUE(s.Find(String("12"), 2, 1) < 0);
  ASSERT_TRUE(s.Find(String("12"), 0, 2) == 0);
  ASSERT_TRUE(s.Find(String("23"), 1, 2) == 1);
  ASSERT_TRUE(s.Find(String("23"), 1, INT32_MAX) == 1);
  ASSERT_TRUE(s.Find(String("123"), 0, INT32_MAX) == 0);
  ASSERT_TRUE(s.Find(String("123"), 0, 2) == 0);
  s.Assign("Recalbox");
  ASSERT_TRUE(s.Find(String("Recal"), 0, 3) == 0);
  ASSERT_TRUE(s.Find(String("Recal"), 1, 5) < 0);
  ASSERT_TRUE(s.Find(String("box"), 3, INT32_MAX) == 5);
  ASSERT_TRUE(s.Find(String("x"), 7, 1) == 7);
  ASSERT_TRUE(s.Find(String("x"), 7, 0) < 0);
  s.Assign("Recalbox is the all-times best retrogaming console! all-times, yes!");
  ASSERT_TRUE(s.Find(String("Recalbox"), 0, 10) == 0);
  ASSERT_TRUE(s.Find(String("all-times"), 0, INT32_MAX) == 16);
  ASSERT_TRUE(s.Find(String("all-times"), 50, 3) == 52);
  ASSERT_TRUE(s.Find(String("all-times"), 53, INT32_MAX) < 0);
  ASSERT_TRUE(s.Find(String("!"), 10, INT32_MAX) == 50);
  ASSERT_TRUE(s.Find(String("!"), 51, 20) == 66);
}

TEST_F(StringTestFinders, TestFindLastChar)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast('!') == 50);
  ASSERT_TRUE(s.FindLast('-') == 19);
  ASSERT_TRUE(s.FindLast('+') < 0);
  ASSERT_TRUE(s.FindLast('\0') < 0);
}

TEST_F(StringTestFinders, TestFindLastCString)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast("ole!") == 47);
  ASSERT_TRUE(s.FindLast("all-times") == 16);
  ASSERT_TRUE(s.FindLast("Recalbox") == 0);
  ASSERT_TRUE(s.FindLast("beast") < 0);
}

TEST_F(StringTestFinders, TestFindLastString)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast(String("ole!")) == 47);
  ASSERT_TRUE(s.FindLast(String("all-times")) == 16);
  ASSERT_TRUE(s.FindLast(String("Recalbox")) == 0);
  ASSERT_TRUE(s.FindLast(String("beast")) < 0);
}

TEST_F(StringTestFinders, TestFindLastCharFrom)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast('!', 25) < 0);
  ASSERT_TRUE(s.FindLast('-', 30) == 19);
  ASSERT_TRUE(s.FindLast('-', 10) < 0);
  ASSERT_TRUE(s.FindLast('R', 0) == 0);
}

TEST_F(StringTestFinders, TestFindLastCStringFrom)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast("ole!", 51) == 47);
  ASSERT_TRUE(s.FindLast("all-times", 30) == 16);
  ASSERT_TRUE(s.FindLast("all-times", 10) < 0);
  ASSERT_TRUE(s.FindLast("Recalbox", 50) == 0);
  ASSERT_TRUE(s.FindLast("beast", 51) < 0);
}

TEST_F(StringTestFinders, TestFindLastCStringLengthFrom)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast(LEGACY_STRING("ole!"), 51) == 47);
  ASSERT_TRUE(s.FindLast(LEGACY_STRING("all-times"), 30) == 16);
  ASSERT_TRUE(s.FindLast(LEGACY_STRING("all-times"), 10) < 0);
  ASSERT_TRUE(s.FindLast(LEGACY_STRING("Recalbox"), 50) == 0);
  ASSERT_TRUE(s.FindLast(LEGACY_STRING("beast"), 51) < 0);
}

TEST_F(StringTestFinders, TestFindLastStringFrom)
{
  String s("Recalbox is the all-times best retrogaming console!");
  ASSERT_TRUE(s.FindLast(String("ole!"), 51) == 47);
  ASSERT_TRUE(s.FindLast(String("all-times"), 30) == 16);
  ASSERT_TRUE(s.FindLast(String("all-times"), 10) < 0);
  ASSERT_TRUE(s.FindLast(String("Recalbox"), 50) == 0);
  ASSERT_TRUE(s.FindLast(String("beast"), 51) < 0);
}

#pragma clang diagnostic pop