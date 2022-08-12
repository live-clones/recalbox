//
// Created by bkg2k on 28/06/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/String.h>

class FormatTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FormatTest, TestNoPlaceHolder)
{
  String result = _F("tagada")();
  ASSERT_TRUE(result == "tagada");

  result = (_F("tagada") / true)();
  ASSERT_TRUE(result == "tagada");
}

TEST_F(FormatTest, TestSimpleBoolean)
{
  String result = (_F("This is {0}") / true)();
  ASSERT_TRUE(result == "This is 1");
}

TEST_F(FormatTest, TestSimpleChars)
{
  String result = (_F("This is a {0}") / 'A')();
  ASSERT_TRUE(result == "This is a A");

  unsigned char b = 'B';
  result = (_F("This is a {0}") / b)();
  ASSERT_TRUE(result == "This is a B");
}

TEST_F(FormatTest, TestSimpleInt)
{
  String result = (_F("This is a {0}") / 0)();
  ASSERT_TRUE(result == "This is a 0");

  result = (_F("This is a {0}, a negative number") / -1)();
  ASSERT_TRUE(result == "This is a -1, a negative number");

  result = (_F("This is a {0}") / 0x10000)();
  ASSERT_TRUE(result == "This is a 65536");

  result = (_F("This is a {0}") / 0x7FFFFFFF)();
  ASSERT_TRUE(result == "This is a 2147483647");

  result = (_F("This is a {0}") / (int)0x80000000)();
  ASSERT_TRUE(result == "This is a -2147483648");
}

TEST_F(FormatTest, TestSimpleUnsignedInt)
{
  String result = (_F("This is a {0}") / 0U)();
  ASSERT_TRUE(result == "This is a 0");

  result = (_F("This is a {0}") / 0x10000U)();
  ASSERT_TRUE(result == "This is a 65536");

  result = (_F("This is a {0}") / 0x7FFFFFFFU)();
  ASSERT_TRUE(result == "This is a 2147483647");

  result = (_F("This is a {0}") / 0x80000000U)();
  ASSERT_TRUE(result == "This is a 2147483648");

  result = (_F("This is a {0}") / 0xFFFFFFFFU)();
  ASSERT_TRUE(result == "This is a 4294967295");
}

TEST_F(FormatTest, TestSimpleLongLong)
{
  String result = (_F("This is a {0}") / 0LL)();
  ASSERT_TRUE(result == "This is a 0");

  result = (_F("This is a {0}, a negative number") / -1LL)();
  ASSERT_TRUE(result == "This is a -1, a negative number");

  result = (_F("This is a {0}") / 0x10000LL)();
  ASSERT_TRUE(result == "This is a 65536");

  result = (_F("This is a {0}") / 0x100000000LL)();
  ASSERT_TRUE(result == "This is a 4294967296");

  result = (_F("This is a {0}") / 0x7FFFFFFFLL)();
  ASSERT_TRUE(result == "This is a 2147483647");

  result = (_F("This is a {0}") / 0x80000000LL)();
  ASSERT_TRUE(result == "This is a 2147483648");

  result = (_F("This is a {0}") / 0x7FFFFFFFFFFFFFFFLL)();
  ASSERT_TRUE(result == "This is a 9223372036854775807");

  result = (_F("This is a {0}") / -0x7FFFFFFFFFFFFFFFLL)();
  ASSERT_TRUE(result == "This is a -9223372036854775807");
}

TEST_F(FormatTest, TestSimpleUnsignedLongLong)
{
  String result = (_F("This is a {0}") / 0LLU)();
  ASSERT_TRUE(result == "This is a 0");

  result = (_F("This is a {0}") / 0x10000LLU)();
  ASSERT_TRUE(result == "This is a 65536");

  result = (_F("This is a {0}") / 0x100000000LLU)();
  ASSERT_TRUE(result == "This is a 4294967296");

  result = (_F("This is a {0}") / 0x7FFFFFFFLLU)();
  ASSERT_TRUE(result == "This is a 2147483647");

  result = (_F("This is a {0}") / 0x80000000LLU)();
  ASSERT_TRUE(result == "This is a 2147483648");

  result = (_F("This is a {0}") / 0x7FFFFFFFFFFFFFFFLLU)();
  ASSERT_TRUE(result == "This is a 9223372036854775807");

  result = (_F("This is a {0}") / 0xFFFFFFFFFFFFFFFFLLU)();
  ASSERT_TRUE(result == "This is a 18446744073709551615");
}

TEST_F(FormatTest, TestSimpleString)
{
  String result = (_F("String value: {0} !") / String("Recalbox"))();
  ASSERT_TRUE(result == "String value: Recalbox !");
}

TEST_F(FormatTest, TestSimpleCharString)
{
  String result = (_F("String value: {0} !") / "Recalbox")();
  ASSERT_TRUE(result == "String value: Recalbox !");
}

TEST_F(FormatTest, TestSimpleFloat)
{
  String result = (_F("Default formatted float value: {0} !") / 123.456f)();
  ASSERT_TRUE(result == "Default formatted float value: 123.45 !");
}

TEST_F(FormatTest, TestFormattedFloat)
{
  String result = (_F("Float value: {0} !") / _FOV(Frac, 5) / 123.456f)();
  ASSERT_TRUE(result == "Float value: 123.45600 !");
}

TEST_F(FormatTest, TestMoreArguments)
{
  String result = (_F("This is a string value: {0}, and an int value: {1}. And there a bool value: {2} !") / "gAmE" / 12345 / true)();
  ASSERT_TRUE(result == "This is a string value: gAmE, and an int value: 12345. And there a bool value: 1 !");
}

TEST_F(FormatTest, TestMoreArgumentsRandomOrder)
{
  String result = (_F("This is a string value: {1}, and an int value: {2}. And there a bool value: {0} !") / true / "gAmE" / 12345)();
  ASSERT_TRUE(result == "This is a string value: gAmE, and an int value: 12345. And there a bool value: 1 !");
}

TEST_F(FormatTest, TestMoreArgumentsRandomOrderDuplicated)
{
  String result = (_F("This is a duplicated string value: {1} {1}, and an int value: {2}. And there a tripple bool value: {0}{0}{0}!") / true / "gAmE" / 12345)();
  ASSERT_TRUE(result == "This is a duplicated string value: gAmE gAmE, and an int value: 12345. And there a tripple bool value: 111!");
}

TEST_F(FormatTest, TestMoreArgumentsFormattedFloat)
{
  String result = (_F("Float value with 5 fracs {0}, then 3 fracs {1} then 0 fracs {2} and finally default fracs {3} !")
    / _FOV(Frac, 5) / 123.456f / _FOV(Frac, 3) / 456.12345f / _FOV(Frac, 0) / 789.456f / _FO(Frac) / 123456.789f)();
  ASSERT_TRUE(result == "Float value with 5 fracs 123.45600, then 3 fracs 456.123 then 0 fracs 789 and finally default fracs 123456.78 !");
}

TEST_F(FormatTest, TestSimpleHexaInt)
{
  String result = (_F("Hexa integer: {0}!") % 0x12345678)();
  ASSERT_TRUE(result == "Hexa integer: 12345678!");
}

TEST_F(FormatTest, TestSimpleHexaLongLong)
{
  String result = (_F("Hexa integer: {0}!") % 0x1234567890123456LL)();
  ASSERT_TRUE(result == "Hexa integer: 1234567890123456!");
}

TEST_F(FormatTest, TestSimpleHexaIntOptions)
{
  String result = (_F("Hexa integers: {0} {1}!") / _FOV(Hexa, 6) % 0x5678 / _FO(Hexa) % 0x1234)();
  ASSERT_TRUE(result == "Hexa integers: 005678 00001234!");
}

TEST_F(FormatTest, TestSimpleHexaLongLongOptions)
{
  String result = (_F("Hexa integers: {0} {1} {2}!") / _FOV(Hexa, 12) % 0x1234567890LL / _FO(Hexa) % 0x1234LL % 0x1234567890LL)();
  ASSERT_TRUE(result == "Hexa integers: 001234567890 00001234 1234567890!");
}
