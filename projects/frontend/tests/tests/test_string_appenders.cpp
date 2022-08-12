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

class StringTestAppenders: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestAppenders, TestStringAppend)
{
  // Non const appending
  String s("Recal");
  String s2("Box");
  ASSERT_TRUE(s.Append(s2) == "RecalBox"); // result pushed in s
  ASSERT_TRUE(s == "RecalBox");
  ASSERT_TRUE(s2 == "Box");
}

TEST_F(StringTestAppenders, TestStringAppendFrom)
{
  // Non const appending
  String s("Recal");
  String s2("Box");
  ASSERT_TRUE(s.Append(s2, 1) == "Recalox"); // result pushed in s
  ASSERT_TRUE(s == "Recalox");
  ASSERT_TRUE(s2 == "Box");
}

TEST_F(StringTestAppenders, TestStringAppendFromLength)
{
  // Non const appending
  String s("Recal");
  String s2("Box");
  ASSERT_TRUE(s.Append(s2, 1, 1) == "Recalo"); // result pushed in s
  ASSERT_TRUE(s == "Recalo");
  ASSERT_TRUE(s2 == "Box");
  ASSERT_TRUE(s.Append(s2, 1, INT32_MAX) == "Recaloox"); // result pushed in s
  ASSERT_TRUE(s == "Recaloox");
}

TEST_F(StringTestAppenders, TestCStringAppend)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Append("Box") == "RecalBox"); // result pushed in s
  ASSERT_TRUE(s == "RecalBox");
}

TEST_F(StringTestAppenders, TestCharAppend)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Append('A') == "RecalA"); // result pushed in s
  ASSERT_TRUE(s == "RecalA");
}

TEST_F(StringTestAppenders, TestBoolAppend)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Append(true).Append(false) == "Recal10"); // result pushed in s
  ASSERT_TRUE(s == "Recal10");
}

TEST_F(StringTestAppenders, TestBoolTextAppend)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.AppendTextBool(true).AppendTextBool(false) == "Recaltruefalse"); // result pushed in s
  ASSERT_TRUE(s == "Recaltruefalse");
}

TEST_F(StringTestAppenders, TestUnsignedCharAppend)
{
  unsigned char c = 0x61;

  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Append(c) == "Recala"); // result pushed in s
  ASSERT_TRUE(s == "Recala");
}

TEST_F(StringTestAppenders, TestWordAppend)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Append((short)12345) == "Recalbox12345");
  ASSERT_TRUE(s == "Recalbox12345");
  ASSERT_TRUE(s.Append((short)-12345) == "Recalbox12345-12345");
  ASSERT_TRUE(s == "Recalbox12345-12345");
  ASSERT_TRUE(s.Append((unsigned short)65535) == "Recalbox12345-1234565535");
  ASSERT_TRUE(s == "Recalbox12345-1234565535");
  ASSERT_TRUE(s.Append((short)0) == "Recalbox12345-12345655350");
  ASSERT_TRUE(s == "Recalbox12345-12345655350");
}

TEST_F(StringTestAppenders, TestDWordAppend)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Append(0) == "Recalbox0");
  ASSERT_TRUE(s == "Recalbox0");
  ASSERT_TRUE(s.Append(2012987426) == "Recalbox02012987426");
  ASSERT_TRUE(s == "Recalbox02012987426");
  ASSERT_TRUE(s.Append(-2012987426) == "Recalbox02012987426-2012987426");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426");
  ASSERT_TRUE(s.Append(-1) == "Recalbox02012987426-2012987426-1");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-1");
  ASSERT_TRUE(s.Append(2147483647) == "Recalbox02012987426-2012987426-12147483647");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647");
  ASSERT_TRUE(s.Append((int)-2147483648) == "Recalbox02012987426-2012987426-12147483647-2147483648");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647-2147483648");
  ASSERT_TRUE(s.Append((unsigned int)4294967295) == "Recalbox02012987426-2012987426-12147483647-21474836484294967295");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647-21474836484294967295");
}
TEST_F(StringTestAppenders, TestQWordAppend)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Append(0LL) == "Recalbox0");
  ASSERT_TRUE(s == "Recalbox0");
  ASSERT_TRUE(s.Append(2012987426759123LL) == "Recalbox02012987426759123");
  ASSERT_TRUE(s == "Recalbox02012987426759123");
  ASSERT_TRUE(s.Append(-2012987426856254LL) == "Recalbox02012987426759123-2012987426856254");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254");
  ASSERT_TRUE(s.Append(-1LL) == "Recalbox02012987426759123-2012987426856254-1");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-1");
  ASSERT_TRUE(s.Append(9223372036854775807LL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807");
  ASSERT_TRUE(s.Append(-9223372036854775807LL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807-9223372036854775807");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807-9223372036854775807");
  ASSERT_TRUE(s.Append(18446744073709551615ULL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807-922337203685477580718446744073709551615");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807-922337203685477580718446744073709551615");
}

TEST_F(StringTestAppenders, TestFloatAppend)
{
  String s;
  s.Append(0.f);
  ASSERT_TRUE(s == "0.00");
  s.Append(1.f);
  ASSERT_TRUE(s == "0.001.00");
  s.Append(-1.f);
  ASSERT_TRUE(s == "0.001.00-1.00");
  s.Append(12345.123f);
  ASSERT_TRUE(s == "0.001.00-1.0012345.12");
  s.Append(987654.f);
  ASSERT_TRUE(s == "0.001.00-1.0012345.12987654.00");
  s.Append(.1234f);
  ASSERT_TRUE(s == "0.001.00-1.0012345.12987654.000.12");
  s.Append(-.987f);
  ASSERT_TRUE(s == "0.001.00-1.0012345.12987654.000.12-0.98");
}

TEST_F(StringTestAppenders, TestFloatAppendPrecision)
{
  String s;
  s.Append(0.f, 4);
  ASSERT_TRUE(s == "0.0000");
  s.Append(1.f, 3);
  ASSERT_TRUE(s == "0.00001.000");
  s.Append(-1.f, 10);
  ASSERT_TRUE(s == "0.00001.000-1.0000000000");
  s.Append(12345.123456789f, 5);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12304"); // float precision limit
  s.Append(987654.f, 15);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12304987654.0000000000");
  s.Append(.1234f, 5);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12304987654.00000000000.12340");
  s.Append(-.987f, 1);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12304987654.00000000000.12340-0.9");
}

TEST_F(StringTestAppenders, TestDoubleAppend)
{
  String s;
  s.Append(0.);
  ASSERT_TRUE(s == "0.00");
  s.Append(1.);
  ASSERT_TRUE(s == "0.001.00");
  s.Append(-1.);
  ASSERT_TRUE(s == "0.001.00-1.00");
  s.Append(123456789.12345);
  ASSERT_TRUE(s == "0.001.00-1.00123456789.12");
  s.Append(987654321.);
  ASSERT_TRUE(s == "0.001.00-1.00123456789.12987654321.00");
  s.Append(.1234);
  ASSERT_TRUE(s == "0.001.00-1.00123456789.12987654321.000.12");
  s.Append(-.987);
  ASSERT_TRUE(s == "0.001.00-1.00123456789.12987654321.000.12-0.98");
}

TEST_F(StringTestAppenders, TestDoubleAppendPrecision)
{
  String s;
  s.Append(0., 4);
  ASSERT_TRUE(s == "0.0000");
  s.Append(1., 3);
  ASSERT_TRUE(s == "0.00001.000");
  s.Append(-1., 10);
  ASSERT_TRUE(s == "0.00001.000-1.0000000000");
  s.Append(12345.123456789, 8);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12345678");
  s.Append(987654., 15);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12345678987654.0000000000");
  s.Append(.0123456789, 9);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12345678987654.00000000000.012345678");
  s.Append(-.987, 8);
  ASSERT_TRUE(s == "0.00001.000-1.000000000012345.12345678987654.00000000000.012345678-0.98700000");
}

TEST_F(StringTestAppenders, TestAppendIntToHexa)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC); ASSERT_TRUE(s == "Recalbox0x12BC");
  s.AppendHexa(0xA); ASSERT_TRUE(s == "Recalbox0x12BC0xA");
  s.AppendHexa(-0x1F); ASSERT_TRUE(s == "Recalbox0x12BC0xA-0x1F");
  s.AppendHexa(0x7FFFFFFF); ASSERT_TRUE(s == "Recalbox0x12BC0xA-0x1F0x7FFFFFFF");
  s.AppendHexa(-0x7FFFFFFF); ASSERT_TRUE(s == "Recalbox0x12BC0xA-0x1F0x7FFFFFFF-0x7FFFFFFF");
  s.AppendHexa(0x12BC, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x12BC0xA-0x1F0x7FFFFFFF-0x7FFFFFFF$12BC");
  s.AppendHexa(0x12BC, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x12BC0xA-0x1F0x7FFFFFFF-0x7FFFFFFF$12BC12BC");
}

TEST_F(StringTestAppenders, TestAssignIntToHexaMin)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC, 7); ASSERT_TRUE(s == "Recalbox0x00012BC");
  s.AppendHexa(0xA, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A");
  s.AppendHexa(-0x1F, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A-0x000001F");
  s.AppendHexa(0x7FFFFFFF, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A-0x000001F0x7FFFFFFF");
  s.AppendHexa(-0x7FFFFFFF, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A-0x000001F0x7FFFFFFF-0x7FFFFFFF");
  s.AppendHexa(0x12BC, 7, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A-0x000001F0x7FFFFFFF-0x7FFFFFFF$00012BC");
  s.AppendHexa(0x12BC, 7, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x00012BC0x000000A-0x000001F0x7FFFFFFF-0x7FFFFFFF$00012BC00012BC");
}

TEST_F(StringTestAppenders, TestAssignUIntToHexa)
{
  String s("Recalbox");
  s.AppendHexa(0x12BCu); ASSERT_TRUE(s == "Recalbox0x12BC");
  s.AppendHexa(0x7FFFFFFFu); ASSERT_TRUE(s == "Recalbox0x12BC0x7FFFFFFF");
  s.AppendHexa(0xABCDEF00u); ASSERT_TRUE(s == "Recalbox0x12BC0x7FFFFFFF0xABCDEF00");
  s.AppendHexa(0xABCDEF00u, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x12BC0x7FFFFFFF0xABCDEF00$ABCDEF00");
  s.AppendHexa(0xABCDEF00u, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x12BC0x7FFFFFFF0xABCDEF00$ABCDEF00ABCDEF00");
}

TEST_F(StringTestAppenders, TestAssignUIntToHexaMin)
{
  String s("Recalbox");
  s.AppendHexa(0x12BCu, 7); ASSERT_TRUE(s == "Recalbox0x00012BC");
  s.AppendHexa(0x7FFFFFFFu, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x7FFFFFFF");
  s.AppendHexa(0xABCDEF00u, 7); ASSERT_TRUE(s == "Recalbox0x00012BC0x7FFFFFFF0xABCDEF00");
  s.AppendHexa(0xABCDEF00u, 7, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x00012BC0x7FFFFFFF0xABCDEF00$ABCDEF00");
  s.AppendHexa(0x0u, 7, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x00012BC0x7FFFFFFF0xABCDEF00$ABCDEF000000000");
}

TEST_F(StringTestAppenders, TestAssignLongLongToHexa)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC12345678LL); ASSERT_TRUE(s == "Recalbox0x12BC12345678");
  s.AppendHexa(0xA12345678LL); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678");
  s.AppendHexa(-0x1F12345678LL); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678-0x1F12345678");
  s.AppendHexa(0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678-0x1F123456780x7FFFFFFFFFFFFFFF");
  s.AppendHexa(-0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678-0x1F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF");
  s.AppendHexa(0x12BC12345678LL, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678-0x1F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF$12BC12345678");
  s.AppendHexa(0x12BC12345678LL, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x12BC123456780xA12345678-0x1F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF$12BC1234567812BC12345678");
}

TEST_F(StringTestAppenders, TestAssignLongLongToHexaMin)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC12345678LL, 15); ASSERT_TRUE(s == "Recalbox0x00012BC12345678");
  s.AppendHexa(0xA12345678LL, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678");
  s.AppendHexa(-0x1F12345678LL, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678-0x000001F12345678");
  s.AppendHexa(0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678-0x000001F123456780x7FFFFFFFFFFFFFFF");
  s.AppendHexa(-0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678-0x000001F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF");
  s.AppendHexa(0x12BC12345678LL, 15, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678-0x000001F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF$00012BC12345678");
  s.AppendHexa(0x12BC12345678LL, 15, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x000000A12345678-0x000001F123456780x7FFFFFFFFFFFFFFF-0x7FFFFFFFFFFFFFFF$00012BC1234567800012BC12345678");
}

TEST_F(StringTestAppenders, TestAssignULONGLONGToHexa)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC12345678LLu); ASSERT_TRUE(s == "Recalbox0x12BC12345678");
  s.AppendHexa(0x7FFFFFFFFFFFFFFFLLu); ASSERT_TRUE(s == "Recalbox0x12BC123456780x7FFFFFFFFFFFFFFF");
  s.AppendHexa(0xABCDEF0012345678LLu); ASSERT_TRUE(s == "Recalbox0x12BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678");
  s.AppendHexa(0xABCDEF0012345678LLu, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x12BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678$ABCDEF0012345678");
  s.AppendHexa(0xABCDEF0012345678LLu, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x12BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678$ABCDEF0012345678ABCDEF0012345678");
}

TEST_F(StringTestAppenders, TestAssignULongLongToHexaMin)
{
  String s("Recalbox");
  s.AppendHexa(0x12BC12345678LLu, 15); ASSERT_TRUE(s == "Recalbox0x00012BC12345678");
  s.AppendHexa(0x7FFFFFFFFFFFFFFFLLu, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x7FFFFFFFFFFFFFFF");
  s.AppendHexa(0xABCDEF0012345678LLu, 15); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678");
  s.AppendHexa(0xABCDEF0012345678LLu, 15, String::Hexa::Asm); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678$ABCDEF0012345678");
  s.AppendHexa(0x0LLu, 15, String::Hexa::None); ASSERT_TRUE(s == "Recalbox0x00012BC123456780x7FFFFFFFFFFFFFFF0xABCDEF0012345678$ABCDEF0012345678000000000000000");
}

TEST_F(StringTestAppenders, TestAppendFormat)
{
  String s("Hey! ");
  ASSERT_TRUE(s.Append(_F("Recalbox is COOL!")) == "Hey! Recalbox is COOL!");
  ASSERT_TRUE(s.Append(_F(" No! Recalbox is VERY {0}!") / "COOL") == "Hey! Recalbox is COOL! No! Recalbox is VERY COOL!");
}

#pragma clang diagnostic pop