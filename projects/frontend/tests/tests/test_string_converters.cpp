#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 06/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestConverters: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestConverters, TestBooleanConverter)
{
  String s("0");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("-");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("1+");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("1");
  ASSERT_TRUE(s.AsBool() == true);
  s.Assign("false");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("FALSE");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("fAlSe");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("true");
  ASSERT_TRUE(s.AsBool() == true);
  s.Assign("TRUE");
  ASSERT_TRUE(s.AsBool() == true);
  s.Assign("tRuE");
  ASSERT_TRUE(s.AsBool() == true);
  s.Assign("TRU");
  ASSERT_TRUE(s.AsBool() == false);
  s.Assign("fals");
  ASSERT_TRUE(s.AsBool() == false);
}

TEST_F(StringTestConverters, TestBooleanConverterFrom)
{
  String s(" 0");
  ASSERT_TRUE(s.AsBool(0) == false);
  ASSERT_TRUE(s.AsBool(1) == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsBool(0) == false);
  ASSERT_TRUE(s.AsBool(1) == true);
  ASSERT_TRUE(s.AsBool(2) == false);
  ASSERT_TRUE(s.AsBool(-1) == false);
  s.Assign(" 1+");
  ASSERT_TRUE(s.AsBool(1) == false);
  s.Assign("&false");
  ASSERT_TRUE(s.AsBool(1) == false);
  ASSERT_TRUE(s.AsBool(2) == false);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.AsBool(1) == false);
  ASSERT_TRUE(s.AsBool(2) == false);
  s.Assign("#fAlSe");
  ASSERT_TRUE(s.AsBool(1) == false);
  ASSERT_TRUE(s.AsBool(2) == false);
  s.Assign("ttrue");
  ASSERT_TRUE(s.AsBool(1) == true);
  ASSERT_TRUE(s.AsBool(2) == false);
  s.Assign("TTRUE");
  ASSERT_TRUE(s.AsBool(1) == true);
  ASSERT_TRUE(s.AsBool(2) == false);
  s.Assign("itRuE");
  ASSERT_TRUE(s.AsBool(1) == true);
  ASSERT_TRUE(s.AsBool(2) == false);
}

TEST_F(StringTestConverters, TestBooleanConverterStop)
{
  String s("0");
  ASSERT_TRUE(s.AsBool('\0') == false);
  s.Assign("1|");
  ASSERT_TRUE(s.AsBool('\0') == false);
  ASSERT_TRUE(s.AsBool('|') == true);
  s.Assign("0|");
  ASSERT_TRUE(s.AsBool('\0') == false);
  ASSERT_TRUE(s.AsBool('|') == false);
}

TEST_F(StringTestConverters, TestBooleanConverterFromStop)
{
  String s(" 0");
  ASSERT_TRUE(s.AsBool(0, '\0') == false);
  ASSERT_TRUE(s.AsBool(1, '\0') == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsBool(0, '\0') == false);
  ASSERT_TRUE(s.AsBool(1, '\0') == true);
  ASSERT_TRUE(s.AsBool(2, '\0') == false);
  ASSERT_TRUE(s.AsBool(-1, '\0') == false);
  s.Assign(" 1+");
  ASSERT_TRUE(s.AsBool(1, '+') == true);
  s.Assign("&false");
  ASSERT_TRUE(s.AsBool(1, '\0') == false);
  ASSERT_TRUE(s.AsBool(2, '\0') == false);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.AsBool(1, '\0') == false);
  ASSERT_TRUE(s.AsBool(2, '\0') == false);
  s.Assign("&true&");
  ASSERT_TRUE(s.AsBool(1, '&') == true);
  ASSERT_TRUE(s.AsBool(2, '&') == false);
  s.Assign("+TRUE&");
  ASSERT_TRUE(s.AsBool(1, '&') == true);
  ASSERT_TRUE(s.AsBool(2, '&') == false);
  s.Assign("&true+");
  ASSERT_TRUE(s.AsBool(1, '&') == false);
  s.Assign("+TRUE+");
  ASSERT_TRUE(s.AsBool(1, '&') == false);
}

TEST_F(StringTestConverters, TestBooleanConverterDefault)
{
  String s("0");
  ASSERT_TRUE(s.AsBool(true) == false);
  s.Assign("-");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("1+");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("1");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("false");
  ASSERT_TRUE(s.AsBool(true) == false);
  s.Assign("FALSE");
  ASSERT_TRUE(s.AsBool(true) == false);
  s.Assign("fAlSe");
  ASSERT_TRUE(s.AsBool(true) == false);
  s.Assign("true");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("TRUE");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("tRuE");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("TRU");
  ASSERT_TRUE(s.AsBool(true) == true);
  s.Assign("fals");
  ASSERT_TRUE(s.AsBool(true) == true);
}

TEST_F(StringTestConverters, TestBooleanConverterFromDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsBool(0, true) == true);
  ASSERT_TRUE(s.AsBool(1, true) == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsBool(0, true) == true);
  ASSERT_TRUE(s.AsBool(1, true) == true);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  ASSERT_TRUE(s.AsBool(-1, true) == true);
  s.Assign(" 1+");
  ASSERT_TRUE(s.AsBool(1, true) == true);
  s.Assign("&false");
  ASSERT_TRUE(s.AsBool(1, true) == false);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.AsBool(1, true) == false);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  s.Assign(")fAlSe");
  ASSERT_TRUE(s.AsBool(1, true) == false);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  s.Assign("ttrue");
  ASSERT_TRUE(s.AsBool(1, true) == true);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  s.Assign("TTRUE");
  ASSERT_TRUE(s.AsBool(1, true) == true);
  ASSERT_TRUE(s.AsBool(2, true) == true);
  s.Assign("itRuE");
  ASSERT_TRUE(s.AsBool(1, true) == true);
  ASSERT_TRUE(s.AsBool(2, true) == true);
}

TEST_F(StringTestConverters, TestBooleanConverterStopDefault)
{
  String s("0");
  ASSERT_TRUE(s.AsBool('\0', true) == false);
  s.Assign("1|");
  ASSERT_TRUE(s.AsBool('\0', true) == true);
  ASSERT_TRUE(s.AsBool('|', true) == true);
  s.Assign("0|");
  ASSERT_TRUE(s.AsBool('\0', true) == true);
  ASSERT_TRUE(s.AsBool('|', true) == false);
}

TEST_F(StringTestConverters, TestBooleanConverterFromStopDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsBool(0, '\0', true) == true);
  ASSERT_TRUE(s.AsBool(1, '\0', true) == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsBool(0, '\0', true) == true);
  ASSERT_TRUE(s.AsBool(1, '\0', true) == true);
  ASSERT_TRUE(s.AsBool(2, '\0', true) == true);
  ASSERT_TRUE(s.AsBool(-1, '\0', true) == true);
  s.Assign(" 1+");
  ASSERT_TRUE(s.AsBool(1, '+', true) == true);
  s.Assign("&false");
  ASSERT_TRUE(s.AsBool(1, '\0', true) == false);
  ASSERT_TRUE(s.AsBool(2, '\0', true) == true);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.AsBool(1, '\0', true) == false);
  ASSERT_TRUE(s.AsBool(2, '\0', true) == true);
  s.Assign("&true&");
  ASSERT_TRUE(s.AsBool(1, '&', true) == true);
  ASSERT_TRUE(s.AsBool(2, '&', true) == true);
  s.Assign("+TRUE&");
  ASSERT_TRUE(s.AsBool(1, '&', true) == true);
  ASSERT_TRUE(s.AsBool(2, '&', true) == true);
  s.Assign("&true+");
  ASSERT_TRUE(s.AsBool(1, '&', true) == true);
  s.Assign("+TRUE+");
  ASSERT_TRUE(s.AsBool(1, '&', true) == true);
}

TEST_F(StringTestConverters, TestBooleanTryConverter)
{
  bool r = false;
  String s("0");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == false);
  s.Assign("-");
  ASSERT_TRUE(s.TryAsBool(r) == false);
  s.Assign("1+");
  ASSERT_TRUE(s.TryAsBool(r) == false);
  s.Assign("1");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == true);
  s.Assign("false");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == false);
  s.Assign("FALSE");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == false);
  s.Assign("fAlSe");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == false);
  s.Assign("true");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == true);
  s.Assign("TRUE");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == true);
  s.Assign("tRuE");
  ASSERT_TRUE(s.TryAsBool(r) == true); ASSERT_TRUE(r == true);
  s.Assign("TRU");
  ASSERT_TRUE(s.TryAsBool(r) == false);
  s.Assign("fals");
  ASSERT_TRUE(s.TryAsBool(r) == false);
}

TEST_F(StringTestConverters, TestBooleanTryConverterFrom)
{
  bool r = false;
  String s(" 0");
  ASSERT_TRUE(s.TryAsBool(0, r) == false);
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsBool(0, r) == false);
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  ASSERT_TRUE(s.TryAsBool(-1, r) == false);
  s.Assign(" 1+");
  ASSERT_TRUE(s.TryAsBool(1, r) == false);
  s.Assign("&false");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == false);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == false);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  s.Assign("(fAlSe");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == false);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  s.Assign("ttrue");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  s.Assign("TTRUE");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
  s.Assign("itRuE");
  ASSERT_TRUE(s.TryAsBool(1, r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, r) == false);
}

TEST_F(StringTestConverters, TestBooleanTryConverterStop)
{
  bool r = false;
  String s("0");
  ASSERT_TRUE(s.TryAsBool('\0', r) == true); ASSERT_TRUE(r == false);
  s.Assign("1|");
  ASSERT_TRUE(s.TryAsBool('\0', r) == false);
  ASSERT_TRUE(s.TryAsBool('|', r) == true); ASSERT_TRUE(r == true);
  s.Assign("0|");
  ASSERT_TRUE(s.TryAsBool('\0', r) == false);
  ASSERT_TRUE(s.TryAsBool('|', r) == true); ASSERT_TRUE(r == false);
}

TEST_F(StringTestConverters, TestBooleanTryConverterFromStop)
{
  bool r = false;
  String s(" 0");
  ASSERT_TRUE(s.TryAsBool(0, '\0', r) == false);
  ASSERT_TRUE(s.TryAsBool(1, '\0', r) == true); ASSERT_TRUE(r == false);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsBool(0, '\0', r) == false);
  ASSERT_TRUE(s.TryAsBool(1, '\0', r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, '\0', r) == false);
  ASSERT_TRUE(s.TryAsBool(-1, '\0', r) == false);
  s.Assign(" 1+");
  ASSERT_TRUE(s.TryAsBool(1, '+', r) == true); ASSERT_TRUE(r == true);
  s.Assign("&false");
  ASSERT_TRUE(s.TryAsBool(1, '\0', r) == true); ASSERT_TRUE(r == false);
  ASSERT_TRUE(s.TryAsBool(2, '\0', r) == false);
  s.Assign("+FALSE");
  ASSERT_TRUE(s.TryAsBool(1, '\0', r) == true); ASSERT_TRUE(r == false);
  ASSERT_TRUE(s.TryAsBool(2, '\0', r) == false);
  s.Assign("&true&");
  ASSERT_TRUE(s.TryAsBool(1, '&', r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, '&', r) == false);
  s.Assign("+TRUE&");
  ASSERT_TRUE(s.TryAsBool(1, '&', r) == true); ASSERT_TRUE(r == true);
  ASSERT_TRUE(s.TryAsBool(2, '&', r) == false);
  s.Assign("&false++");
  ASSERT_TRUE(s.TryAsBool(1, '+', r) == true); ASSERT_TRUE(r == false);
  s.Assign("+FALSE+++");
  ASSERT_TRUE(s.TryAsBool(1, '+', r) == true); ASSERT_TRUE(r == false);
}

TEST_F(StringTestConverters, TestIntConverter)
{
  String s("0");
  ASSERT_TRUE(s.AsInt() == 0);
  s.Assign("1");
  ASSERT_TRUE(s.AsInt() == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt() == -1);
  s.Assign("1234567");
  ASSERT_TRUE(s.AsInt() == 1234567);
  s.Assign("2147483647");
  ASSERT_TRUE(s.AsInt() == 2147483647);
  s.Assign("-2147483648");
  ASSERT_TRUE(s.AsInt() == -2147483648);
  s.Assign("$123ABC");
  ASSERT_TRUE(s.AsInt() == 0x123ABC);
  s.Assign("0x456DEF");
  ASSERT_TRUE(s.AsInt() == 0x456DEF);
  s.Assign("0x7FFFFFFF");
  ASSERT_TRUE(s.AsInt() == INT32_MAX);
  s.Assign("-0x7FFFFFFF");
  ASSERT_TRUE(s.AsInt() == -INT32_MAX);
  s.Assign("0x80000000");
  ASSERT_TRUE(s.AsInt() == INT32_MIN);
}

TEST_F(StringTestConverters, TestIntConverterFrom)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt(1) == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt(1) == 1);
  s.Assign(" -1");
  ASSERT_TRUE(s.AsInt(0) == 0);
  ASSERT_TRUE(s.AsInt(1) == -1);
  ASSERT_TRUE(s.AsInt(2) == 1);
  s.Assign(" 0x1234567");
  ASSERT_TRUE(s.AsInt(1) == 0x1234567);
  ASSERT_TRUE(s.AsInt(3) == 1234567);
  ASSERT_TRUE(s.AsInt(7) == 567);
  s.Assign("///-2147483648");
  ASSERT_TRUE(s.AsInt(3) == -2147483648);
  ASSERT_TRUE(s.AsInt(5) == 147483648);
  ASSERT_TRUE(s.AsInt(INT32_MAX) == 0);
}

TEST_F(StringTestConverters, TestIntConverterStop)
{
  String s("1");
  ASSERT_TRUE(s.AsInt('\0') == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt('\0') == -1);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsInt('|') == -1);
  s.Assign("0x1234567/");
  ASSERT_TRUE(s.AsInt('/') == 0x1234567);
  ASSERT_TRUE(s.AsInt('5') == 0);
}

TEST_F(StringTestConverters, TestIntConverterFromStop)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt(1, '\0') == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt(1, '\0') == 1);
  s.Assign(" -1|");
  ASSERT_TRUE(s.AsInt(0, '|') == 0);
  ASSERT_TRUE(s.AsInt(1, '|') == -1);
  ASSERT_TRUE(s.AsInt(2, '|') == 1);
  ASSERT_TRUE(s.AsInt(2, '\0') == 0);
  s.Assign("(0x1234567)");
  ASSERT_TRUE(s.AsInt(1, ')') == 0x1234567);
  ASSERT_TRUE(s.AsInt(3, '4') == 0);
  s.Assign("///-2147483648///");
  ASSERT_TRUE(s.AsInt(3, '/') == -2147483648);
  ASSERT_TRUE(s.AsInt(5, '/') == 147483648);
  ASSERT_TRUE(s.AsInt(INT32_MAX, '\0') == 0);
}

TEST_F(StringTestConverters, TestIntConverterDefault)
{
  String s("0");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 0);
  s.Assign("1");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt(0, 0x55) == -1);
  s.Assign("*");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 0x55);
  s.Assign("2147483647");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 2147483647);
  s.Assign("-2147483648");
  ASSERT_TRUE(s.AsInt(0, 0x55) == -2147483648);
  s.Assign("$123ABC");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 0x123ABC);
  s.Assign("0x456DEF");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 0x456DEF);
  s.Assign("7FFFFFFF");
  ASSERT_TRUE(s.AsInt(0, 0x55) == 0x55);
  s.Assign("0x7FFFFFFF");
  ASSERT_TRUE(s.AsInt(0, 0x55) == INT32_MAX);
  s.Assign("-0x7FFFFFFF");
  ASSERT_TRUE(s.AsInt(0, 0x55) == -INT32_MAX);
  s.Assign("0x80000000");
  ASSERT_TRUE(s.AsInt(0, 0x55) == INT32_MIN);
}

TEST_F(StringTestConverters, TestIntConverterFromDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt(1, 0x5555) == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt(1, 0x5555) == 1);
  s.Assign(" -1");
  ASSERT_TRUE(s.AsInt(0, 0x5555) == 0x5555);
  ASSERT_TRUE(s.AsInt(1, 0x5555) == -1);
  ASSERT_TRUE(s.AsInt(2, 0x5555) == 1);
  s.Assign(" 0x1234567");
  ASSERT_TRUE(s.AsInt(1, 0x5555) == 0x1234567);
  ASSERT_TRUE(s.AsInt(2, 0x5555) == 0x5555);
  ASSERT_TRUE(s.AsInt(3, 0x5555) == 1234567);
  ASSERT_TRUE(s.AsInt(7, 0x5555) == 567);
  s.Assign("///-2147483648");
  ASSERT_TRUE(s.AsInt(0, 0x5555) == 0x5555);
  ASSERT_TRUE(s.AsInt(3, 0x5555) == -2147483648);
  ASSERT_TRUE(s.AsInt(5, 0x5555) == 147483648);
  ASSERT_TRUE(s.AsInt(INT32_MAX, 0x5555) == 0x5555);
}

TEST_F(StringTestConverters, TestIntConverterStopDefault)
{
  String s("1");
  ASSERT_TRUE(s.AsInt('\0', 0x5555) == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt('\0', 0x5555) == -1);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsInt('|', 0x5555) == -1);
  ASSERT_TRUE(s.AsInt('1', 0x5555) == 0x5555);
  ASSERT_TRUE(s.AsInt('\0', 0x5555) == 0x5555);
  s.Assign("0x1234567/");
  ASSERT_TRUE(s.AsInt('/', 0x5555) == 0x1234567);
  ASSERT_TRUE(s.AsInt('5', 0x5555) == 0x5555);
  ASSERT_TRUE(s.AsInt('\0', 0x5555) == 0x5555);
}

TEST_F(StringTestConverters, TestIntConverterFromStopDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt(1, '\0', 0xAAAA) == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt(1, '\0', 0xAAAA) == 1);
  s.Assign(" -1|");
  ASSERT_TRUE(s.AsInt(0, '|', 0xAAAA) == 0xAAAA);
  ASSERT_TRUE(s.AsInt(1, '|', 0xAAAA) == -1);
  ASSERT_TRUE(s.AsInt(2, '|', 0xAAAA) == 1);
  ASSERT_TRUE(s.AsInt(3, '|', 0xAAAA) == 0);
  ASSERT_TRUE(s.AsInt(2, '\0', 0xAAAA) == 0xAAAA);
  s.Assign("(0x1234567)");
  ASSERT_TRUE(s.AsInt(1, ')', 0xAAAA) == 0x1234567);
  ASSERT_TRUE(s.AsInt(3, ')', 0xAAAA) == 1234567);
  s.Assign("///-2147483648///");
  ASSERT_TRUE(s.AsInt(3, '/', 0xAAAA) == -2147483648);
  ASSERT_TRUE(s.AsInt(5, '/', 0xAAAA) == 147483648);
  ASSERT_TRUE(s.AsInt(6, '4', 0xAAAA) == 0xAAAA);
  ASSERT_TRUE(s.AsInt(INT32_MAX, '\0', 0xAAAA) == 0xAAAA);
}

TEST_F(StringTestConverters, TestIntTryConverter)
{
  int r = 0;
  String s("0");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 0);
  s.clear();
  ASSERT_TRUE(s.TryAsInt(r) == false);
  s.Assign("-");
  ASSERT_TRUE(s.TryAsInt(r) == false);
  s.Assign("$");
  ASSERT_TRUE(s.TryAsInt(r) == false);
  s.Assign("0x");
  ASSERT_TRUE(s.TryAsInt(r) == false);
  s.Assign("T");
  ASSERT_TRUE(s.TryAsInt(r) == false);
  s.Assign("1");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == -1);
  s.Assign("1234567");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 1234567);
  s.Assign("2147483647");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 2147483647);
  s.Assign("-2147483648");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == -2147483648);
  s.Assign("$123ABC");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 0x123ABC);
  s.Assign("0x456DEF");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == 0x456DEF);
  s.Assign("0x7FFFFFFF");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == INT32_MAX);
  s.Assign("-0x7FFFFFFF");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == -INT32_MAX);
  s.Assign("0x80000000");
  ASSERT_TRUE(s.TryAsInt(r) == true); ASSERT_TRUE(r == INT32_MIN);
}

TEST_F(StringTestConverters, TestIntTryConverterFrom)
{
  int r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsInt(1, r) == true); ASSERT_TRUE(r == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsInt(1, r) == true); ASSERT_TRUE(r == 1);
  s.Assign(" -1");
  ASSERT_TRUE(s.TryAsInt(0, r) == false);
  ASSERT_TRUE(s.TryAsInt(1, r) == true); ASSERT_TRUE(r == -1);
  ASSERT_TRUE(s.TryAsInt(2, r) == true); ASSERT_TRUE(r == 1);
  s.Assign(" 0x1234567");
  ASSERT_TRUE(s.TryAsInt(1, r) == true); ASSERT_TRUE(r == 0x1234567);
  ASSERT_TRUE(s.TryAsInt(3, r) == true); ASSERT_TRUE(r == 1234567);
  ASSERT_TRUE(s.TryAsInt(7, r) == true); ASSERT_TRUE(r == 567);
  s.Assign("///-2147483648");
  ASSERT_TRUE(s.TryAsInt(3, r) == true); ASSERT_TRUE(r == -2147483648);
  ASSERT_TRUE(s.TryAsInt(5, r) == true); ASSERT_TRUE(r == 147483648);
  ASSERT_TRUE(s.TryAsInt(INT32_MAX, r) == false);
}

TEST_F(StringTestConverters, TestIntTryConverterStop)
{
  int r = 0;
  String s("1");
  ASSERT_TRUE(s.TryAsInt('\0', r) == true); ASSERT_TRUE(r == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsInt('\0', r) == true); ASSERT_TRUE(r == -1);
  s.Assign("-1|");
  ASSERT_TRUE(s.TryAsInt('|', r) == true); ASSERT_TRUE(r == -1);
  s.Assign("0x1234567/");
  ASSERT_TRUE(s.TryAsInt('/', r) == true); ASSERT_TRUE(r == 0x1234567);
  ASSERT_TRUE(s.TryAsInt('5', r) == false);
}

TEST_F(StringTestConverters, TestIntTryConverterFromStop)
{
  int r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsInt(1, '\0', r) == true); ASSERT_TRUE(r == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsInt(1, '\0', r) == true); ASSERT_TRUE(r == 1);
  s.Assign(" -1|");
  ASSERT_TRUE(s.TryAsInt(0, '|', r) == false);
  ASSERT_TRUE(s.TryAsInt(1, '|', r) == true); ASSERT_TRUE(r == -1);
  ASSERT_TRUE(s.TryAsInt(2, '|', r) == true); ASSERT_TRUE(r == 1);
  ASSERT_TRUE(s.TryAsInt(2, '\0', r) == false);
  s.Assign("(0x1234567)");
  ASSERT_TRUE(s.TryAsInt(1, ')', r) == true); ASSERT_TRUE(r == 0x1234567);
  ASSERT_TRUE(s.TryAsInt(4, ')', r) == true); ASSERT_TRUE(r == 234567);
  s.Assign("///-2147483648///");
  ASSERT_TRUE(s.TryAsInt(3, '/', r) == true); ASSERT_TRUE(r == -2147483648);
  ASSERT_TRUE(s.TryAsInt(5, '/', r) == true); ASSERT_TRUE(r == 147483648);
  ASSERT_TRUE(s.TryAsInt(6, '8', r) == false);
  ASSERT_TRUE(s.TryAsInt(INT32_MAX, '\0', r) == false);
}

TEST_F(StringTestConverters, TestInt64Converter)
{
  String s("0");
  ASSERT_TRUE(s.AsInt64() == 0);
  s.clear();
  ASSERT_TRUE(s.AsInt64() == 0);
  s.Assign("-");
  ASSERT_TRUE(s.AsInt64() == 0);
  s.Assign("1");
  ASSERT_TRUE(s.AsInt64() == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt64() == -1LL);
  s.Assign("0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(s.AsInt64() == 0x7FFFFFFFFFFFFFFFLL);
  s.Assign("0x8000000000000000");
  ASSERT_TRUE(s.AsInt64() == -0x7FFFFFFFFFFFFFFF-1LL);
  s.Assign("-9223372036854775808");
  ASSERT_TRUE(s.AsInt64() == -9223372036854775807LL-1LL);
  s.Assign("9223372036854775807");
  ASSERT_TRUE(s.AsInt64() == 9223372036854775807LL);
}

TEST_F(StringTestConverters, TestInt64ConverterFrom)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt64(1) == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt64(1) == 1);
  s.Assign(" -1");
  ASSERT_TRUE(s.AsInt64(0) == 0);
  ASSERT_TRUE(s.AsInt64(1) == -1LL);
  ASSERT_TRUE(s.AsInt64(2) == 1);
  s.Assign("///-9223372036854775808");
  ASSERT_TRUE(s.AsInt64(3) == -9223372036854775807LL-1LL);
  ASSERT_TRUE(s.AsInt64(5) == 223372036854775808LL);
  ASSERT_TRUE(s.AsInt64(INT32_MAX) == 0);
}

TEST_F(StringTestConverters, TestInt64ConverterStop)
{
  String s("1");
  ASSERT_TRUE(s.AsInt64('\0') == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt64('\0') == -1);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsInt64('|') == -1LL);
  s.Assign("-9223372036854775808/");
  ASSERT_TRUE(s.AsInt64('/') == -9223372036854775807LL-1LL);
}

TEST_F(StringTestConverters, TestInt64ConverterFromStop)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt64(1, '\0') == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsInt64(1, '\0') == 1);
  s.Assign(" -1|");
  ASSERT_TRUE(s.AsInt64(0, '|') == 0);
  ASSERT_TRUE(s.AsInt64(1, '|') == -1LL);
  ASSERT_TRUE(s.AsInt64(2, '|') == 1LL);
  ASSERT_TRUE(s.AsInt64(2, '\0') == 0);
  s.Assign("///-9223372036854775808///");
  ASSERT_TRUE(s.AsInt64(3, '/') == -9223372036854775807LL-1LL);
  ASSERT_TRUE(s.AsInt64(5, '/') == 223372036854775808LL);
  ASSERT_TRUE(s.AsInt64(INT32_MAX, '\0') == 0);
}

TEST_F(StringTestConverters, TestInt64ConverterDefault)
{
  String s("0");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == -1LL);
  s.Assign("*");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("2-3");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("123.");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("7FFFFFFF");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0x1122334455667788LL);
}

TEST_F(StringTestConverters, TestInt64ConverterFromDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt64(1, 0x1122334455667788LL) == 0);
  s.Assign(" -1");
  ASSERT_TRUE(s.AsInt64(1, 0x1122334455667788LL) == -1LL);
  s.Assign(" 0x1234567890AB");
  ASSERT_TRUE(s.AsInt64(1, 0x1122334455667788LL) == 0x1234567890ABLL);
  ASSERT_TRUE(s.AsInt64(2, 0x1122334455667788LL) == 0x1122334455667788LL);
  ASSERT_TRUE(s.AsInt64(3, 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("///-2147483648000");
  ASSERT_TRUE(s.AsInt64(0, 0x1122334455667788LL) == 0x1122334455667788LL);
  ASSERT_TRUE(s.AsInt64(3, 0x1122334455667788LL) == -2147483648000);
}

TEST_F(StringTestConverters, TestInt64ConverterStopDefault)
{
  String s("1");
  ASSERT_TRUE(s.AsInt64('\0', 0x1122334455667788LL) == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.AsInt64('\0', 0x1122334455667788LL) == -1LL);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsInt64('|', 0x1122334455667788LL) == -1LL);
  ASSERT_TRUE(s.AsInt64('\0', 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("0x1234567ABCD/");
  ASSERT_TRUE(s.AsInt64('/', 0x1122334455667788LL) == 0x1234567ABCDLL);
  ASSERT_TRUE(s.AsInt64('\0', 0x1122334455667788LL) == 0x1122334455667788LL);
}

TEST_F(StringTestConverters, TestInt64ConverterFromStopDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsInt64(1, '\0', 0x1122334455667788LL) == 0);
  s.Assign(" -1|");
  ASSERT_TRUE(s.AsInt64(0, '|', 0x1122334455667788LL) == 0x1122334455667788LL);
  ASSERT_TRUE(s.AsInt64(1, '|', 0x1122334455667788LL) == -1LL);
  ASSERT_TRUE(s.AsInt64(2, '|', 0x1122334455667788LL) == 1LL);
  ASSERT_TRUE(s.AsInt64(3, '|', 0x1122334455667788LL) == 0);
  ASSERT_TRUE(s.AsInt64(2, '\0', 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("(0x1234567ABCD)");
  ASSERT_TRUE(s.AsInt64(1, ')', 0x1122334455667788LL) == 0x1234567ABCD);
  ASSERT_TRUE(s.AsInt64(3, '\0', 0x1122334455667788LL) == 0x1122334455667788LL);
  s.Assign("///-2147483648000///");
  ASSERT_TRUE(s.AsInt64(3, '/', 0x1122334455667788LL) == -2147483648000);
  ASSERT_TRUE(s.AsInt64(3, '+', 0x1122334455667788LL) == 0x1122334455667788LL);
  ASSERT_TRUE(s.AsInt64(INT32_MAX, '/', 0x1122334455667788LL) == 0x1122334455667788LL);
}

TEST_F(StringTestConverters, TestInt64TryConverter)
{
  long long r = 0;
  String s("0");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == 0);
  s.clear();
  ASSERT_TRUE(s.TryAsInt64(r) == false);
  s.Assign("-");
  ASSERT_TRUE(s.TryAsInt64(r) == false);
  s.Assign("$");
  ASSERT_TRUE(s.TryAsInt64(r) == false);
  s.Assign("0x");
  ASSERT_TRUE(s.TryAsInt64(r) == false);
  s.Assign("T");
  ASSERT_TRUE(s.TryAsInt64(r) == false);
  s.Assign("1");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == -1);
  s.Assign("0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == 0x7FFFFFFFFFFFFFFFLL);
  s.Assign("0x8000000000000000");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == -0x7FFFFFFFFFFFFFFFLL-1LL);
  s.Assign("-9223372036854775808");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == -9223372036854775807LL-1LL);
  s.Assign("9223372036854775807");
  ASSERT_TRUE(s.TryAsInt64(r) == true); ASSERT_TRUE(r == 9223372036854775807LL);
}

TEST_F(StringTestConverters, TestInt64TryConverterFrom)
{
  long long r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsInt64(1, r) == true); ASSERT_TRUE(r == 0);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsInt64(1, r) == true); ASSERT_TRUE(r == 1);
  s.Assign(" -1");
  ASSERT_TRUE(s.TryAsInt64(0, r) == false);
  s.Assign("///0x1234567890ABCDEF");
  ASSERT_TRUE(s.TryAsInt64(3, r) == true); ASSERT_TRUE(r == 0x1234567890ABCDEFLL);
}

TEST_F(StringTestConverters, TestInt64TryConverterStop)
{
  long long r = 0;
  String s("1");
  ASSERT_TRUE(s.TryAsInt64('\0', r) == true); ASSERT_TRUE(r == 1);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsInt64('\0', r) == true); ASSERT_TRUE(r == -1);
  s.Assign("-1|");
  ASSERT_TRUE(s.TryAsInt64('|', r) == true); ASSERT_TRUE(r == -1);
  s.Assign("0x1234567890ABCDEF/");
  ASSERT_TRUE(s.TryAsInt64('/', r) == true); ASSERT_TRUE(r == 0x1234567890ABCDEFLL);
}

TEST_F(StringTestConverters, TestInt64TryConverterFromStop)
{
  long long r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsInt64(1, '\0', r) == true); ASSERT_TRUE(r == 0);
  s.Assign(" -1|");
  ASSERT_TRUE(s.TryAsInt64(1, '|', r) == true); ASSERT_TRUE(r == -1);
  s.Assign("(0x7FFFFFFFFFFFFFFF)");
  ASSERT_TRUE(s.TryAsInt64(1, ')', r) == true); ASSERT_TRUE(r == 0x7FFFFFFFFFFFFFFFLL);
  s.Assign("(0x8000000000000000)");
  ASSERT_TRUE(s.TryAsInt64(1, ')', r) == true); ASSERT_TRUE(r == -0x7FFFFFFFFFFFFFFFLL-1LL);
  s.Assign("///-9223372036854775808///");
  ASSERT_TRUE(s.TryAsInt64(3, '/', r) == true); ASSERT_TRUE(r == -9223372036854775807LL-1LL);
  s.Assign("///9223372036854775807///");
  ASSERT_TRUE(s.TryAsInt64(3, '/', r) == true); ASSERT_TRUE(r == 9223372036854775807LL);
}

TEST_F(StringTestConverters, TestFloatConverter)
{
  String s("0"); ASSERT_TRUE(s.AsFloat() == 0.f);
  s.Assign("1"); ASSERT_TRUE(s.AsFloat() == 1.f);
  s.Assign("-1"); ASSERT_TRUE(s.AsFloat() == -1.f);
  s.Assign("0."); ASSERT_TRUE(s.AsFloat() == 0.f);
  s.Assign("1."); ASSERT_TRUE(s.AsFloat() == 1.f);
  s.Assign("-1."); ASSERT_TRUE(s.AsFloat() == -1.f);
  s.Assign("123.456"); ASSERT_TRUE(s.AsFloat() == 123.456f);
  s.Assign(".21474"); ASSERT_TRUE(s.AsFloat() == .21474f);
  s.Assign("0.21474"); ASSERT_TRUE(s.AsFloat() == .21474f);
  s.Assign("-.214"); ASSERT_TRUE(s.AsFloat() == -.214f);
  s.Assign(".123.4"); ASSERT_TRUE(s.AsFloat() == 0.f);
  s.Assign("1.123.4"); ASSERT_TRUE(s.AsFloat() == 0.f);
  s.Assign("0.23a"); ASSERT_TRUE(s.AsFloat() == 0.f);
  s.Assign(".-23"); ASSERT_TRUE(s.AsFloat() == 0.f);
}

TEST_F(StringTestConverters, TestFloatConverterFrom)
{
  String s("0"); ASSERT_TRUE(s.AsFloat(0) == 0.f);
  s.Assign(" 1"); ASSERT_TRUE(s.AsFloat(1) == 1.f);
  s.Assign("  -1"); ASSERT_TRUE(s.AsFloat(2) == -1.f);
  s.Assign("   0."); ASSERT_TRUE(s.AsFloat(3) == 0.f);
  s.Assign("    1."); ASSERT_TRUE(s.AsFloat(4) == 1.f);
  s.Assign("     -1."); ASSERT_TRUE(s.AsFloat(5) == -1.f);
  s.Assign("      123.456"); ASSERT_TRUE(s.AsFloat(6) == 123.456f);
  s.Assign("       .21474"); ASSERT_TRUE(s.AsFloat(7) == .21474f);
  s.Assign("      0.21474"); ASSERT_TRUE(s.AsFloat(6) == .21474f);
  s.Assign("     -.214"); ASSERT_TRUE(s.AsFloat(5) == -.214f);
  s.Assign("    .123.4"); ASSERT_TRUE(s.AsFloat(4) == 0.f);
  s.Assign("   1.123.4"); ASSERT_TRUE(s.AsFloat(3) == 0.f);
  s.Assign("  0.23a"); ASSERT_TRUE(s.AsFloat(2) == 0.f);
  s.Assign(" .-23"); ASSERT_TRUE(s.AsFloat(1) == 0.f);
}

TEST_F(StringTestConverters, TestFloatConverterStop)
{
  String s("0"); ASSERT_TRUE(s.AsFloat('\0') == 0.f);
  s.Assign("1|"); ASSERT_TRUE(s.AsFloat('|') == 1.f);
  s.Assign("-1."); ASSERT_TRUE(s.AsFloat('.') == 0.f);
  s.Assign(".21474."); ASSERT_TRUE(s.AsFloat('.') == .21474f);
  s.Assign("0.23a"); ASSERT_TRUE(s.AsFloat('a') == 0.23f);
}

TEST_F(StringTestConverters, TestFloatConverterFromStop)
{
  String s("0"); ASSERT_TRUE(s.AsFloat(0, '\0') == 0.f);
  s.Assign(" 1|"); ASSERT_TRUE(s.AsFloat(1, '|') == 1.f);
  s.Assign("  -1-"); ASSERT_TRUE(s.AsFloat(2, '-') == -1.f);
  s.Assign("   0.0"); ASSERT_TRUE(s.AsFloat(3, '0') == 0.f);
  s.Assign("    1."); ASSERT_TRUE(s.AsFloat(4, '.') == 0.f);
  s.Assign("     -1.a"); ASSERT_TRUE(s.AsFloat(5, 'a') == -1.f);
  s.Assign("      123.456"); ASSERT_TRUE(s.AsFloat(6, '6') == 0.f);
  s.Assign("       .21474"); ASSERT_TRUE(s.AsFloat(7, '4') == 0.f);
  s.Assign("      0.21474"); ASSERT_TRUE(s.AsFloat(6, '1') == 0.f);
  s.Assign("     -.214."); ASSERT_TRUE(s.AsFloat(5, '.') == -.214f);
  s.Assign("    .123.4"); ASSERT_TRUE(s.AsFloat(4, '.') == 0.123f);
  s.Assign("   1.123.4"); ASSERT_TRUE(s.AsFloat(3, '4') == 0.f);
  s.Assign("  0.23a"); ASSERT_TRUE(s.AsFloat(2, '\0') == 0.f);
  s.Assign(" .-23"); ASSERT_TRUE(s.AsFloat(1, '\0') == 0.f);
}

TEST_F(StringTestConverters, TestFloatConverterDefault)
{
  String s("0"); ASSERT_TRUE(s.AsFloat(0, 0.99f) == 0.f);
  s.Assign("1."); ASSERT_TRUE(s.AsFloat(0, 0.99f) == 1.f);
  s.Assign("-1.0000"); ASSERT_TRUE(s.AsFloat(0, 0.99f) == -1.f);
  s.Assign("+2.3"); ASSERT_TRUE(s.AsFloat(0, 0.99f) == 0.99f);
  s.Assign("214748.123"); ASSERT_TRUE(s.AsFloat(0, 0.99f) == 214748.123f);
  s.Assign("-214748.123"); ASSERT_TRUE(s.AsFloat(0, 0.99f) == -214748.123f);
}

TEST_F(StringTestConverters, TestFloatConverterFromDefault)
{
  String s(" .0");
  ASSERT_TRUE(s.AsFloat(1, 0.997f) == 0.f);
  s.Assign(" 1.");
  ASSERT_TRUE(s.AsFloat(1, 0.997f) == 1.f);
  s.Assign(" -1.");
  ASSERT_TRUE(s.AsFloat(0, 0.997f) == 0.997f);
  ASSERT_TRUE(s.AsFloat(1, 0.997f) == -1.f);
  ASSERT_TRUE(s.AsFloat(2, 0.997f) == 1.f);
  s.Assign("///-21474.836");
  ASSERT_TRUE(s.AsFloat(0, 0.997f) == 0.997f);
  ASSERT_TRUE(s.AsFloat(3, 0.997f) == -21474.836f);
  ASSERT_TRUE(s.AsFloat(5, 0.997f) == 1474.836f);
  ASSERT_TRUE(s.AsFloat(INT32_MAX, 0.997f) == 0.997f);
}

TEST_F(StringTestConverters, TestFloatConverterStopDefault)
{
  String s("1");
  ASSERT_TRUE(s.AsFloat('\0', 0.789f) == 1.f);
  s.Assign("-1");
  ASSERT_TRUE(s.AsFloat('\0', 0.789f) == -1.f);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsFloat('|', 0.789f) == -1.f);
  ASSERT_TRUE(s.AsFloat('1', 0.789f) == 0.789f);
  ASSERT_TRUE(s.AsFloat('\0', 0.789f) == 0.789f);
  s.Assign("1234.567/");
  ASSERT_TRUE(s.AsFloat('/', 0.789f) == 1234.567f);
  ASSERT_TRUE(s.AsFloat('7', 0.789f) == 0.789f);
  ASSERT_TRUE(s.AsFloat('\0', 0.789f) == 0.789f);
}

TEST_F(StringTestConverters, TestFloatConverterFromStopDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsFloat(1, '\0', 0.345f) == 0.f);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsFloat(1, '\0', 0.345f) == 1.f);
  s.Assign(" -1.|");
  ASSERT_TRUE(s.AsFloat(0, '|', 0.345f) == 0.345f);
  ASSERT_TRUE(s.AsFloat(1, '|', 0.345f) == -1.f);
  ASSERT_TRUE(s.AsFloat(2, '|', 0.345f) == 1.f);
  ASSERT_TRUE(s.AsFloat(3, '|', 0.345f) == 0.f);
  ASSERT_TRUE(s.AsFloat(2, '\0', 0.345f) == 0.345f);
  s.Assign("///-214748.364///");
  ASSERT_TRUE(s.AsFloat(3, '/', 0.345f) == -214748.364f);
  ASSERT_TRUE(s.AsFloat(5, '/', 0.345f) == 14748.364f);
  ASSERT_TRUE(s.AsFloat(6, '8', 0.345f) == 0.345f);
  ASSERT_TRUE(s.AsFloat(INT32_MAX, '\0', 0.345f) == 0.345f);
}

TEST_F(StringTestConverters, TestFloatTryConverter)
{
  float r = 0;
  String s("0"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == 0.f);
  s.clear(); ASSERT_TRUE(s.TryAsFloat(r) == false);
  s.Assign("-"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == 0.f);
  s.Assign("$"); ASSERT_TRUE(s.TryAsFloat(r) == false);
  s.Assign("0x"); ASSERT_TRUE(s.TryAsFloat(r) == false);
  s.Assign("T"); ASSERT_TRUE(s.TryAsFloat(r) == false);
  s.Assign("1."); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == 1.f);
  s.Assign("-1"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == -1.f);
  s.Assign("-.1"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == -.1f);
  s.Assign("123.4567"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == 123.4567f);
  s.Assign("214748.3647"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == 214748.3647f);
  s.Assign("-21474.83648"); ASSERT_TRUE(s.TryAsFloat(r) == true); ASSERT_TRUE(r == -21474.83648f);
}

TEST_F(StringTestConverters, TestFloatTryConverterFrom)
{
  float r = 0;
  String s(" 0.");
  ASSERT_TRUE(s.TryAsFloat(1, r) == true); ASSERT_TRUE(r == 0.f);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsFloat(1, r) == true); ASSERT_TRUE(r == 1.f);
  s.Assign(" -1.");
  ASSERT_TRUE(s.TryAsFloat(0, r) == false);
  ASSERT_TRUE(s.TryAsFloat(1, r) == true); ASSERT_TRUE(r == -1.f);
  ASSERT_TRUE(s.TryAsFloat(2, r) == true); ASSERT_TRUE(r == 1.f);
  s.Assign(" 123.4567");
  ASSERT_TRUE(s.TryAsFloat(1, r) == true); ASSERT_TRUE(r == 123.4567f);
  ASSERT_TRUE(s.TryAsFloat(3, r) == true); ASSERT_TRUE(r == 3.4567f);
  ASSERT_TRUE(s.TryAsFloat(7, r) == true); ASSERT_TRUE(r == 67.f);
  s.Assign("///-214748.3648");
  ASSERT_TRUE(s.TryAsFloat(3, r) == true); ASSERT_TRUE(r == -214748.3648f);
  ASSERT_TRUE(s.TryAsFloat(5, r) == true); ASSERT_TRUE(r == 14748.3648f);
  ASSERT_TRUE(s.TryAsFloat(INT32_MAX, r) == false);
}

TEST_F(StringTestConverters, TestFloatTryConverterStop)
{
  float r = 0;
  String s("1.");
  ASSERT_TRUE(s.TryAsFloat('\0', r) == true); ASSERT_TRUE(r == 1.f);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsFloat('\0', r) == true); ASSERT_TRUE(r == -1.f);
  s.Assign("-1.|");
  ASSERT_TRUE(s.TryAsFloat('|', r) == true); ASSERT_TRUE(r == -1.f);
  s.Assign("1234.567/");
  ASSERT_TRUE(s.TryAsFloat('/', r) == true); ASSERT_TRUE(r == 1234.567f);
  ASSERT_TRUE(s.TryAsFloat('1', r) == false);
}

TEST_F(StringTestConverters, TestFloatTryConverterFromStop)
{
  float r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsFloat(1, '\0', r) == true); ASSERT_TRUE(r == 0.f);
  s.Assign(" 1.");
  ASSERT_TRUE(s.TryAsFloat(1, '\0', r) == true); ASSERT_TRUE(r == 1.f);
  s.Assign(" -1.0|");
  ASSERT_TRUE(s.TryAsFloat(0, '|', r) == false);
  ASSERT_TRUE(s.TryAsFloat(1, '|', r) == true); ASSERT_TRUE(r == -1.f);
  ASSERT_TRUE(s.TryAsFloat(2, '|', r) == true); ASSERT_TRUE(r == 1.f);
  ASSERT_TRUE(s.TryAsFloat(2, '\0', r) == false);
  s.Assign("(12345.67)");
  ASSERT_TRUE(s.TryAsFloat(1, ')', r) == true); ASSERT_TRUE(r == 12345.67f);
  ASSERT_TRUE(s.TryAsFloat(7, ')', r) == true); ASSERT_TRUE(r == 67.f);
  s.Assign("///-21474.83648///");
  ASSERT_TRUE(s.TryAsFloat(3, '/', r) == true); ASSERT_TRUE(r == -21474.83648f);
  ASSERT_TRUE(s.TryAsFloat(5, '/', r) == true); ASSERT_TRUE(r == 1474.83648f);
  ASSERT_TRUE(s.TryAsFloat(6, '4', r) == false);
  ASSERT_TRUE(s.TryAsFloat(INT32_MAX, '\0', r) == false);
}

TEST_F(StringTestConverters, TestDoubleConverter)
{
  String s("0"); ASSERT_TRUE(s.AsDouble() == 0.);
  s.Assign("1"); ASSERT_TRUE(s.AsDouble() == 1.);
  s.Assign("-1"); ASSERT_TRUE(s.AsDouble() == -1.);
  s.Assign("0."); ASSERT_TRUE(s.AsDouble() == 0.);
  s.Assign("1."); ASSERT_TRUE(s.AsDouble() == 1.);
  s.Assign("-1."); ASSERT_TRUE(s.AsDouble() == -1.);
  s.Assign("123.456"); ASSERT_TRUE(s.AsDouble() == 123.456);
  s.Assign(".21474"); ASSERT_TRUE(s.AsDouble() == .21474);
  s.Assign("0.21474"); ASSERT_TRUE(s.AsDouble() == .21474);
  s.Assign("-.214"); ASSERT_TRUE(s.AsDouble() == -.214);
  s.Assign(".123.4"); ASSERT_TRUE(s.AsDouble() == 0.);
  s.Assign("1.123.4"); ASSERT_TRUE(s.AsDouble() == 0.);
  s.Assign("0.23a"); ASSERT_TRUE(s.AsDouble() == 0.);
  s.Assign(".-23"); ASSERT_TRUE(s.AsDouble() == 0.);
}

TEST_F(StringTestConverters, TestDoubleConverterFrom)
{
  String s("0"); ASSERT_TRUE(s.AsDouble(0) == 0.);
  s.Assign(" 1"); ASSERT_TRUE(s.AsDouble(1) == 1.);
  s.Assign("  -1"); ASSERT_TRUE(s.AsDouble(2) == -1.);
  s.Assign("   0."); ASSERT_TRUE(s.AsDouble(3) == 0.);
  s.Assign("    1."); ASSERT_TRUE(s.AsDouble(4) == 1.);
  s.Assign("     -1."); ASSERT_TRUE(s.AsDouble(5) == -1.);
  s.Assign("      123.456"); ASSERT_TRUE(s.AsDouble(6) == 123.456);
  s.Assign("       .21474"); ASSERT_TRUE(s.AsDouble(7) == .21474);
  s.Assign("      0.21474"); ASSERT_TRUE(s.AsDouble(6) == .21474);
  s.Assign("     -.214"); ASSERT_TRUE(s.AsDouble(5) == -.214);
  s.Assign("    .123.4"); ASSERT_TRUE(s.AsDouble(4) == 0.);
  s.Assign("   1.123.4"); ASSERT_TRUE(s.AsDouble(3) == 0.);
  s.Assign("  0.23a"); ASSERT_TRUE(s.AsDouble(2) == 0.);
  s.Assign(" .-23"); ASSERT_TRUE(s.AsDouble(1) == 0.);
}

TEST_F(StringTestConverters, TestDoubleConverterStop)
{
  String s("0"); ASSERT_TRUE(s.AsDouble('\0') == 0.);
  s.Assign("1|"); ASSERT_TRUE(s.AsDouble('|') == 1.);
  s.Assign("-1."); ASSERT_TRUE(s.AsDouble('.') == 0.);
  s.Assign(".21474."); ASSERT_TRUE(s.AsDouble('.') == .21474);
  s.Assign("0.23a"); ASSERT_TRUE(s.AsDouble('a') == 0.23);
}

TEST_F(StringTestConverters, TestDoubleConverterFromStop)
{
  String s("0"); ASSERT_TRUE(s.AsDouble(0, '\0') == 0.);
  s.Assign(" 1|"); ASSERT_TRUE(s.AsDouble(1, '|') == 1.);
  s.Assign("  -1-"); ASSERT_TRUE(s.AsDouble(2, '-') == -1.);
  s.Assign("   0.0"); ASSERT_TRUE(s.AsDouble(3, '0') == 0.);
  s.Assign("    1."); ASSERT_TRUE(s.AsDouble(4, '.') == 0.);
  s.Assign("     -1.a"); ASSERT_TRUE(s.AsDouble(5, 'a') == -1.);
  s.Assign("      123.456"); ASSERT_TRUE(s.AsDouble(6, '6') == 0.);
  s.Assign("       .21474"); ASSERT_TRUE(s.AsDouble(7, '4') == 0.);
  s.Assign("      0.21474"); ASSERT_TRUE(s.AsDouble(6, '1') == 0.);
  s.Assign("     -.214."); ASSERT_TRUE(s.AsDouble(5, '.') == -.214);
  s.Assign("    .123.4"); ASSERT_TRUE(s.AsDouble(4, '.') == 0.123);
  s.Assign("   1.123.4"); ASSERT_TRUE(s.AsDouble(3, '4') == 0.);
  s.Assign("  0.23a"); ASSERT_TRUE(s.AsDouble(2, '\0') == 0.);
  s.Assign(" .-23"); ASSERT_TRUE(s.AsDouble(1, '\0') == 0.);
}

TEST_F(StringTestConverters, TestDoubleConverterDefault)
{
  String s("0"); ASSERT_TRUE(s.AsDouble(0, 0.99) == 0.);
  s.Assign("1."); ASSERT_TRUE(s.AsDouble(0, 0.99) == 1.);
  s.Assign("-1.0000"); ASSERT_TRUE(s.AsDouble(0, 0.99) == -1.);
  s.Assign("+2.3"); ASSERT_TRUE(s.AsDouble(0, 0.99) == 0.99);
  s.Assign("214748.123"); ASSERT_TRUE(s.AsDouble(0, 0.99) == 214748.123);
  s.Assign("-214748.123"); ASSERT_TRUE(s.AsDouble(0, 0.99) == -214748.123);
}

TEST_F(StringTestConverters, TestDoubleConverterFromDefault)
{
  String s(" .0");
  ASSERT_TRUE(s.AsDouble(1, 0.997) == 0.);
  s.Assign(" 1.");
  ASSERT_TRUE(s.AsDouble(1, 0.997) == 1.);
  s.Assign(" -1.");
  ASSERT_TRUE(s.AsDouble(0, 0.997) == 0.997);
  ASSERT_TRUE(s.AsDouble(1, 0.997) == -1.);
  ASSERT_TRUE(s.AsDouble(2, 0.997) == 1.);
  s.Assign("///-21474.836");
  ASSERT_TRUE(s.AsDouble(0, 0.997) == 0.997);
  ASSERT_TRUE(s.AsDouble(3, 0.997) == -21474.836);
  ASSERT_TRUE(s.AsDouble(5, 0.997) == 1474.836);
  ASSERT_TRUE(s.AsDouble(INT32_MAX, 0.997) == 0.997);
}

TEST_F(StringTestConverters, TestDoubleConverterStopDefault)
{
  String s("1");
  ASSERT_TRUE(s.AsDouble('\0', 0.789) == 1.);
  s.Assign("-1");
  ASSERT_TRUE(s.AsDouble('\0', 0.789) == -1.);
  s.Assign("-1|");
  ASSERT_TRUE(s.AsDouble('|', 0.789) == -1.);
  ASSERT_TRUE(s.AsDouble('1', 0.789) == 0.789);
  ASSERT_TRUE(s.AsDouble('\0', 0.789) == 0.789);
  s.Assign("1234.567/");
  ASSERT_TRUE(s.AsDouble('/', 0.789) == 1234.567);
  ASSERT_TRUE(s.AsDouble('7', 0.789) == 0.789);
  ASSERT_TRUE(s.AsDouble('\0', 0.789) == 0.789);
}

TEST_F(StringTestConverters, TestDoubleConverterFromStopDefault)
{
  String s(" 0");
  ASSERT_TRUE(s.AsDouble(1, '\0', 0.345) == 0.);
  s.Assign(" 1");
  ASSERT_TRUE(s.AsDouble(1, '\0', 0.345) == 1.);
  s.Assign(" -1.|");
  ASSERT_TRUE(s.AsDouble(0, '|', 0.345) == 0.345);
  ASSERT_TRUE(s.AsDouble(1, '|', 0.345) == -1.);
  ASSERT_TRUE(s.AsDouble(2, '|', 0.345) == 1.);
  ASSERT_TRUE(s.AsDouble(3, '|', 0.345) == 0.);
  ASSERT_TRUE(s.AsDouble(2, '\0', 0.345) == 0.345);
  s.Assign("///-214748.364///");
  ASSERT_TRUE(s.AsDouble(3, '/', 0.345) == -214748.364);
  ASSERT_TRUE(s.AsDouble(5, '/', 0.345) == 14748.364);
  ASSERT_TRUE(s.AsDouble(6, '8', 0.345) == 0.345);
  ASSERT_TRUE(s.AsDouble(INT32_MAX, '\0', 0.345) == 0.345);
}

TEST_F(StringTestConverters, TestDoubleTryConverter)
{
  double r = 0;
  String s("0"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == 0.);
  s.clear(); ASSERT_TRUE(s.TryAsDouble(r) == false);
  s.Assign("-"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == 0.);
  s.Assign("$"); ASSERT_TRUE(s.TryAsDouble(r) == false);
  s.Assign("0x"); ASSERT_TRUE(s.TryAsDouble(r) == false);
  s.Assign("T"); ASSERT_TRUE(s.TryAsDouble(r) == false);
  s.Assign("1."); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == 1.);
  s.Assign("-1"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == -1.);
  s.Assign("-.1"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == -.1);
  s.Assign("123.4567"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == 123.4567);
  s.Assign("214748.3647"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == 214748.3647);
  s.Assign("-21474.83648"); ASSERT_TRUE(s.TryAsDouble(r) == true); ASSERT_TRUE(r == -21474.83648);
}

TEST_F(StringTestConverters, TestDoubleTryConverterFrom)
{
  double r = 0;
  String s(" 0.");
  ASSERT_TRUE(s.TryAsDouble(1, r) == true); ASSERT_TRUE(r == 0.);
  s.Assign(" 1");
  ASSERT_TRUE(s.TryAsDouble(1, r) == true); ASSERT_TRUE(r == 1.);
  s.Assign(" -1.");
  ASSERT_TRUE(s.TryAsDouble(0, r) == false);
  ASSERT_TRUE(s.TryAsDouble(1, r) == true); ASSERT_TRUE(r == -1.);
  ASSERT_TRUE(s.TryAsDouble(2, r) == true); ASSERT_TRUE(r == 1.);
  s.Assign(" 123.4567");
  ASSERT_TRUE(s.TryAsDouble(1, r) == true); ASSERT_TRUE(r == 123.4567);
  ASSERT_TRUE(s.TryAsDouble(3, r) == true); ASSERT_TRUE(r == 3.4567);
  ASSERT_TRUE(s.TryAsDouble(7, r) == true); ASSERT_TRUE(r == 67.);
  s.Assign("///-214748.3648");
  ASSERT_TRUE(s.TryAsDouble(3, r) == true); ASSERT_TRUE(r == -214748.3648);
  ASSERT_TRUE(s.TryAsDouble(5, r) == true); ASSERT_TRUE(r == 14748.3648);
  ASSERT_TRUE(s.TryAsDouble(INT32_MAX, r) == false);
}

TEST_F(StringTestConverters, TestDoubleTryConverterStop)
{
  double r = 0;
  String s("1.");
  ASSERT_TRUE(s.TryAsDouble('\0', r) == true); ASSERT_TRUE(r == 1.);
  s.Assign("-1");
  ASSERT_TRUE(s.TryAsDouble('\0', r) == true); ASSERT_TRUE(r == -1.);
  s.Assign("-1.|");
  ASSERT_TRUE(s.TryAsDouble('|', r) == true); ASSERT_TRUE(r == -1.);
  s.Assign("1234.567/");
  ASSERT_TRUE(s.TryAsDouble('/', r) == true); ASSERT_TRUE(r == 1234.567);
  ASSERT_TRUE(s.TryAsDouble('1', r) == false);
}

TEST_F(StringTestConverters, TestDoubleTryConverterFromStop)
{
  double r = 0;
  String s(" 0");
  ASSERT_TRUE(s.TryAsDouble(1, '\0', r) == true); ASSERT_TRUE(r == 0.);
  s.Assign(" 1.");
  ASSERT_TRUE(s.TryAsDouble(1, '\0', r) == true); ASSERT_TRUE(r == 1.);
  s.Assign(" -1.0|");
  ASSERT_TRUE(s.TryAsDouble(0, '|', r) == false);
  ASSERT_TRUE(s.TryAsDouble(1, '|', r) == true); ASSERT_TRUE(r == -1.);
  ASSERT_TRUE(s.TryAsDouble(2, '|', r) == true); ASSERT_TRUE(r == 1.);
  ASSERT_TRUE(s.TryAsDouble(2, '\0', r) == false);
  s.Assign("(12345.67)");
  ASSERT_TRUE(s.TryAsDouble(1, ')', r) == true); ASSERT_TRUE(r == 12345.67);
  ASSERT_TRUE(s.TryAsDouble(7, ')', r) == true); ASSERT_TRUE(r == 67.);
  s.Assign("///-21474.83648///");
  ASSERT_TRUE(s.TryAsDouble(3, '/', r) == true); ASSERT_TRUE(r == -21474.83648);
  ASSERT_TRUE(s.TryAsDouble(5, '/', r) == true); ASSERT_TRUE(r == 1474.83648);
  ASSERT_TRUE(s.TryAsDouble(6, '4', r) == false);
  ASSERT_TRUE(s.TryAsDouble(INT32_MAX, '\0', r) == false);
}

TEST_F(StringTestConverters, TestConvertIntToHexa)
{
  ASSERT_TRUE(String::ToHexa(0x12BC) == "0x12BC");
  ASSERT_TRUE(String::ToHexa(0xA) == "0xA");
  ASSERT_TRUE(String::ToHexa(-0x1F) == "-0x1F");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFF) == "0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(-0x7FFFFFFF) == "-0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(0x12BC, String::Hexa::Asm) == "$12BC");
  ASSERT_TRUE(String::ToHexa(0x12BC, String::Hexa::None) == "12BC");
}

TEST_F(StringTestConverters, TestConvertIntToHexaMin)
{
  ASSERT_TRUE(String::ToHexa(0x12BC, 7) == "0x00012BC");
  ASSERT_TRUE(String::ToHexa(0xA, 7) == "0x000000A");
  ASSERT_TRUE(String::ToHexa(-0x1F, 7) == "-0x000001F");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFF, 7) == "0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(-0x7FFFFFFF, 7) == "-0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(0x12BC, 7, String::Hexa::Asm) == "$00012BC");
  ASSERT_TRUE(String::ToHexa(0x12BC, 7, String::Hexa::None) == "00012BC");
}

TEST_F(StringTestConverters, TestConvertUIntToHexa)
{
  ASSERT_TRUE(String::ToHexa(0x12BCu) == "0x12BC");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFu) == "0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(0xABCDEF00u) == "0xABCDEF00");
  ASSERT_TRUE(String::ToHexa(0xABCDEF00u, String::Hexa::Asm) == "$ABCDEF00");
  ASSERT_TRUE(String::ToHexa(0xABCDEF00u, String::Hexa::None) == "ABCDEF00");
}

TEST_F(StringTestConverters, TestConvertUIntToHexaMin)
{
  ASSERT_TRUE(String::ToHexa(0x12BCu, 7) == "0x00012BC");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFu, 7) == "0x7FFFFFFF");
  ASSERT_TRUE(String::ToHexa(0xABCDEF00u, 7) == "0xABCDEF00");
  ASSERT_TRUE(String::ToHexa(0xABCDEF00u, 7, String::Hexa::Asm) == "$ABCDEF00");
  ASSERT_TRUE(String::ToHexa(0x0u, 7, String::Hexa::None) == "0000000");
}

TEST_F(StringTestConverters, TestConvertLongLongToHexa)
{
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL) == "0x12BC12345678");
  ASSERT_TRUE(String::ToHexa(0xA12345678LL) == "0xA12345678");
  ASSERT_TRUE(String::ToHexa(-0x1F12345678LL) == "-0x1F12345678");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFFFFFFFFFLL) == "0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(-0x7FFFFFFFFFFFFFFFLL) == "-0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL, String::Hexa::Asm) == "$12BC12345678");
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL, String::Hexa::None) == "12BC12345678");
}

TEST_F(StringTestConverters, TestConvertLongLongToHexaMin)
{
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL, 15) == "0x00012BC12345678");
  ASSERT_TRUE(String::ToHexa(0xA12345678LL, 15) == "0x000000A12345678");
  ASSERT_TRUE(String::ToHexa(-0x1F12345678LL, 15) == "-0x000001F12345678");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFFFFFFFFFLL, 15) == "0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(-0x7FFFFFFFFFFFFFFFLL, 15) == "-0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL, 15, String::Hexa::Asm) == "$00012BC12345678");
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LL, 15, String::Hexa::None) == "00012BC12345678");
}

TEST_F(StringTestConverters, TestConvertULONGLONGToHexa)
{
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LLu) == "0x12BC12345678");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFFFFFFFFFLLu) == "0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(0xABCDEF0012345678LLu) == "0xABCDEF0012345678");
  ASSERT_TRUE(String::ToHexa(0xABCDEF0012345678LLu, String::Hexa::Asm) == "$ABCDEF0012345678");
  ASSERT_TRUE(String::ToHexa(0xABCDEF0012345678LLu, String::Hexa::None) == "ABCDEF0012345678");
}

TEST_F(StringTestConverters, TestConvertULongLongToHexaMin)
{
  ASSERT_TRUE(String::ToHexa(0x12BC12345678LLu, 15) == "0x00012BC12345678");
  ASSERT_TRUE(String::ToHexa(0x7FFFFFFFFFFFFFFFLLu, 15) == "0x7FFFFFFFFFFFFFFF");
  ASSERT_TRUE(String::ToHexa(0xABCDEF0012345678LLu, 15) == "0xABCDEF0012345678");
  ASSERT_TRUE(String::ToHexa(0xABCDEF0012345678LLu, 15, String::Hexa::Asm) == "$ABCDEF0012345678");
  ASSERT_TRUE(String::ToHexa(0x0LLu, 15, String::Hexa::None) == "000000000000000");
}

#pragma clang diagnostic pop