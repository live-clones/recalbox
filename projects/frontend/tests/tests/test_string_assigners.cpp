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

class StringTestAssigners: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestAssigners, TestStringAssign)
{
  // Non const appending
  String s("RappelBoite");
  String s2("Recalbox");
  ASSERT_TRUE(s.Assign(s2) == "Recalbox"); // result pushed in s
  ASSERT_TRUE(s == "Recalbox");
}

TEST_F(StringTestAssigners, TestCStringAssign)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Assign("Box") == "Box"); // result pushed in s
  ASSERT_TRUE(s == "Box");
}

TEST_F(StringTestAssigners, TestCharAssign)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Assign('A') == "A"); // result pushed in s
  ASSERT_TRUE(s == "A");
}

TEST_F(StringTestAssigners, TestBoolAssign)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Assign(true) == "1"); // result pushed in s
  ASSERT_TRUE(s.Assign(false) == "0"); // result pushed in s
}

TEST_F(StringTestAssigners, TestBoolTextAssign)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.AssignTextBool(true) == "true"); // result pushed in s
  ASSERT_TRUE(s.AssignTextBool(false) == "false"); // result pushed in s
}

TEST_F(StringTestAssigners, TestUnsignedCharAssign)
{
  unsigned char c = 0x61;

  // Non const appending
  String s("Recal");
  ASSERT_TRUE(s.Assign(c) == "a"); // result pushed in s
  ASSERT_TRUE(s == "a");
}

TEST_F(StringTestAssigners, TestWordAssign)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Assign((short)12345) == "12345");
  ASSERT_TRUE(s == "12345");
  ASSERT_TRUE(s.Assign((short)-12345) == "-12345");
  ASSERT_TRUE(s == "-12345");
  ASSERT_TRUE(s.Assign((unsigned short)65535) == "65535");
  ASSERT_TRUE(s == "65535");
  ASSERT_TRUE(s.Assign((short)0) == "0");
  ASSERT_TRUE(s == "0");
}

TEST_F(StringTestAssigners, TestDWordAssign)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Assign(0) == "0");
  ASSERT_TRUE(s == "0");
  ASSERT_TRUE(s.Assign(2012987426) == "2012987426");
  ASSERT_TRUE(s == "2012987426");
  ASSERT_TRUE(s.Assign(-2012987426) == "-2012987426");
  ASSERT_TRUE(s == "-2012987426");
  ASSERT_TRUE(s.Assign(-1) == "-1");
  ASSERT_TRUE(s == "-1");
  ASSERT_TRUE(s.Assign(2147483647) == "2147483647");
  ASSERT_TRUE(s == "2147483647");
  ASSERT_TRUE(s.Assign((int)-2147483648) == "-2147483648");
  ASSERT_TRUE(s == "-2147483648");
  ASSERT_TRUE(s.Assign((unsigned int)4294967295) == "4294967295");
  ASSERT_TRUE(s == "4294967295");
}
TEST_F(StringTestAssigners, TestQWordAssign)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Assign(0LL) == "0");
  ASSERT_TRUE(s == "0");
  ASSERT_TRUE(s.Assign(2012987426759123LL) == "2012987426759123");
  ASSERT_TRUE(s == "2012987426759123");
  ASSERT_TRUE(s.Assign(-2012987426856254LL) == "-2012987426856254");
  ASSERT_TRUE(s == "-2012987426856254");
  ASSERT_TRUE(s.Assign(-1LL) == "-1");
  ASSERT_TRUE(s == "-1");
  ASSERT_TRUE(s.Assign(9223372036854775807LL) == "9223372036854775807");
  ASSERT_TRUE(s == "9223372036854775807");
  ASSERT_TRUE(s.Assign(-9223372036854775807LL) == "-9223372036854775807");
  ASSERT_TRUE(s == "-9223372036854775807");
  ASSERT_TRUE(s.Assign(18446744073709551615ULL) == "18446744073709551615");
  ASSERT_TRUE(s == "18446744073709551615");
}

TEST_F(StringTestAssigners, TestFloatAssign)
{
  String s;
  s.Assign(0.f);
  ASSERT_TRUE(s == "0.00");
  s.Assign(1.f);
  ASSERT_TRUE(s == "1.00");
  s.Assign(-1.f);
  ASSERT_TRUE(s == "-1.00");
  s.Assign(12345.123f);
  ASSERT_TRUE(s == "12345.12");
  s.Assign(987654.f);
  ASSERT_TRUE(s == "987654.00");
  s.Assign(.1234f);
  ASSERT_TRUE(s == "0.12");
  s.Assign(-.987f);
  ASSERT_TRUE(s == "-0.98");
}

TEST_F(StringTestAssigners, TestFloatAssignPrecision)
{
  String s;
  s.Assign(0.f, 4);
  ASSERT_TRUE(s == "0.0000");
  s.Assign(1.f, 3);
  ASSERT_TRUE(s == "1.000");
  s.Assign(-1.f, 10);
  ASSERT_TRUE(s == "-1.0000000000");
  s.Assign(12345.123456789f, 5);
  ASSERT_TRUE(s == "12345.12304"); // float precision limit
  s.Assign(987654.f, 15);
  ASSERT_TRUE(s == "987654.0000000000");
  s.Assign(.1234f, 5);
  ASSERT_TRUE(s == "0.12340");
  s.Assign(-.987f, 1);
  ASSERT_TRUE(s == "-0.9");
}

TEST_F(StringTestAssigners, TestDoubleAssign)
{
  String s;
  s.Assign(0.);
  ASSERT_TRUE(s == "0.00");
  s.Assign(1.);
  ASSERT_TRUE(s == "1.00");
  s.Assign(-1.);
  ASSERT_TRUE(s == "-1.00");
  s.Assign(123456789.12345);
  ASSERT_TRUE(s == "123456789.12");
  s.Assign(987654321.);
  ASSERT_TRUE(s == "987654321.00");
  s.Assign(.1234);
  ASSERT_TRUE(s == "0.12");
  s.Assign(-.987);
  ASSERT_TRUE(s == "-0.98");
}

TEST_F(StringTestAssigners, TestDoubleAssignPrecision)
{
  String s;
  s.Assign(0., 4);
  ASSERT_TRUE(s == "0.0000");
  s.Assign(1., 3);
  ASSERT_TRUE(s == "1.000");
  s.Assign(-1., 10);
  ASSERT_TRUE(s == "-1.0000000000");
  s.Assign(12345.123456789, 8);
  ASSERT_TRUE(s == "12345.12345678");
  s.Assign(987654., 15);
  ASSERT_TRUE(s == "987654.0000000000");
  s.Assign(.0123456789, 9);
  ASSERT_TRUE(s == "0.012345678");
  s.Assign(-.987, 8);
  ASSERT_TRUE(s == "-0.98700000");
}

TEST_F(StringTestAssigners, TestAssignIntToHexa)
{
  String s;
  s.AssignHexa(0x12BC); ASSERT_TRUE(s == "0x12BC");
  s.AssignHexa(0xA); ASSERT_TRUE(s == "0xA");
  s.AssignHexa(-0x1F); ASSERT_TRUE(s == "-0x1F");
  s.AssignHexa(0x7FFFFFFF); ASSERT_TRUE(s == "0x7FFFFFFF");
  s.AssignHexa(-0x7FFFFFFF); ASSERT_TRUE(s == "-0x7FFFFFFF");
  s.AssignHexa(0x12BC, String::Hexa::Asm); ASSERT_TRUE(s == "$12BC");
  s.AssignHexa(0x12BC, String::Hexa::None); ASSERT_TRUE(s == "12BC");
}

TEST_F(StringTestAssigners, TestAssignIntToHexaMin)
{
  String s;
  s.AssignHexa(0x12BC, 7); ASSERT_TRUE(s == "0x00012BC");
  s.AssignHexa(0xA, 7); ASSERT_TRUE(s == "0x000000A");
  s.AssignHexa(-0x1F, 7); ASSERT_TRUE(s == "-0x000001F");
  s.AssignHexa(0x7FFFFFFF, 7); ASSERT_TRUE(s == "0x7FFFFFFF");
  s.AssignHexa(-0x7FFFFFFF, 7); ASSERT_TRUE(s == "-0x7FFFFFFF");
  s.AssignHexa(0x12BC, 7, String::Hexa::Asm); ASSERT_TRUE(s == "$00012BC");
  s.AssignHexa(0x12BC, 7, String::Hexa::None); ASSERT_TRUE(s == "00012BC");
}

TEST_F(StringTestAssigners, TestAssignUIntToHexa)
{
  String s;
  s.AssignHexa(0x12BCu); ASSERT_TRUE(s == "0x12BC");
  s.AssignHexa(0x7FFFFFFFu); ASSERT_TRUE(s == "0x7FFFFFFF");
  s.AssignHexa(0xABCDEF00u); ASSERT_TRUE(s == "0xABCDEF00");
  s.AssignHexa(0xABCDEF00u, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF00");
  s.AssignHexa(0xABCDEF00u, String::Hexa::None); ASSERT_TRUE(s == "ABCDEF00");
}

TEST_F(StringTestAssigners, TestAssignUIntToHexaMin)
{
  String s;
  s.AssignHexa(0x12BCu, 7); ASSERT_TRUE(s == "0x00012BC");
  s.AssignHexa(0x7FFFFFFFu, 7); ASSERT_TRUE(s == "0x7FFFFFFF");
  s.AssignHexa(0xABCDEF00u, 7); ASSERT_TRUE(s == "0xABCDEF00");
  s.AssignHexa(0xABCDEF00u, 7, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF00");
  s.AssignHexa(0x0u, 7, String::Hexa::None); ASSERT_TRUE(s == "0000000");
}

TEST_F(StringTestAssigners, TestAssignLongLongToHexa)
{
  String s;
  s.AssignHexa(0x12BC12345678LL); ASSERT_TRUE(s == "0x12BC12345678");
  s.AssignHexa(0xA12345678LL); ASSERT_TRUE(s == "0xA12345678");
  s.AssignHexa(-0x1F12345678LL); ASSERT_TRUE(s == "-0x1F12345678");
  s.AssignHexa(0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(-0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "-0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(0x12BC12345678LL, String::Hexa::Asm); ASSERT_TRUE(s == "$12BC12345678");
  s.AssignHexa(0x12BC12345678LL, String::Hexa::None); ASSERT_TRUE(s == "12BC12345678");
}

TEST_F(StringTestAssigners, TestAssignLongLongToHexaMin)
{
  String s;
  s.AssignHexa(0x12BC12345678LL, 15); ASSERT_TRUE(s == "0x00012BC12345678");
  s.AssignHexa(0xA12345678LL, 15); ASSERT_TRUE(s == "0x000000A12345678");
  s.AssignHexa(-0x1F12345678LL, 15); ASSERT_TRUE(s == "-0x000001F12345678");
  s.AssignHexa(0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(-0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "-0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(0x12BC12345678LL, 15, String::Hexa::Asm); ASSERT_TRUE(s == "$00012BC12345678");
  s.AssignHexa(0x12BC12345678LL, 15, String::Hexa::None); ASSERT_TRUE(s == "00012BC12345678");
}

TEST_F(StringTestAssigners, TestAssignULONGLONGToHexa)
{
  String s;
  s.AssignHexa(0x12BC12345678LLu); ASSERT_TRUE(s == "0x12BC12345678");
  s.AssignHexa(0x7FFFFFFFFFFFFFFFLLu); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(0xABCDEF0012345678LLu); ASSERT_TRUE(s == "0xABCDEF0012345678");
  s.AssignHexa(0xABCDEF0012345678LLu, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF0012345678");
  s.AssignHexa(0xABCDEF0012345678LLu, String::Hexa::None); ASSERT_TRUE(s == "ABCDEF0012345678");
}

TEST_F(StringTestAssigners, TestAssignULongLongToHexaMin)
{
  String s;
  s.AssignHexa(0x12BC12345678LLu, 15); ASSERT_TRUE(s == "0x00012BC12345678");
  s.AssignHexa(0x7FFFFFFFFFFFFFFFLLu, 15); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF");
  s.AssignHexa(0xABCDEF0012345678LLu, 15); ASSERT_TRUE(s == "0xABCDEF0012345678");
  s.AssignHexa(0xABCDEF0012345678LLu, 15, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF0012345678");
  s.AssignHexa(0x0LLu, 15, String::Hexa::None); ASSERT_TRUE(s == "000000000000000");
}

TEST_F(StringTestAssigners, TestAssignFormat)
{
  String s;
  ASSERT_TRUE(s.Assign(_F("Recalbox is COOL!")) == "Recalbox is COOL!");
  ASSERT_TRUE(s.Assign(_F("Recalbox is {0}!") / "COOL") == "Recalbox is COOL!");
}

#pragma clang diagnostic pop