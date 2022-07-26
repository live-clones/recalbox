#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 05/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/String.h>

class StringTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTest, TestDefaultConstructor)
{
  String s;
  ASSERT_TRUE(s.empty());
}

TEST_F(StringTest, TestCStringConstructor)
{
  String s("");
  ASSERT_TRUE(s.empty());
  String s2("test");
  ASSERT_TRUE(s2.size() == 4);
  ASSERT_TRUE(s2 == "test");
}

TEST_F(StringTest, TestInitializerListConstructor)
{
  String s({ 0x20, 0x61, 0x7c, 0x33 });
  ASSERT_TRUE(s.size() == 4);
  ASSERT_TRUE(s == " a|3");
}

TEST_F(StringTest, TestSubStringConstructor)
{
  String s("this is a sentence");
  String s2(s, 10); // From only
  ASSERT_TRUE(s2 == "sentence");
  String s3(s, 5, 4); // From + length
  ASSERT_TRUE(s3 == "is a");
  String s4(s, 5, INT32_MAX); // From + out-of-data length
  ASSERT_TRUE(s4 == "is a sentence");
}

TEST_F(StringTest, TestSubStdStringConstructor)
{
  std::string s("this is a sentence");
  String s2(s, 10); // From only
  ASSERT_TRUE(s2 == "sentence");
  String s3(s, 5, 4); // From + length
  ASSERT_TRUE(s3 == "is a");
  String s4(s, 5, INT32_MAX); // From + out-of-data length
  ASSERT_TRUE(s4 == "is a sentence");
}

TEST_F(StringTest, TestSubCStringConstructor)
{
  const char* s = "this is a sentence";
  String s2(s, 9); // Length only
  ASSERT_TRUE(s2 == "this is a");
  String s3(s, 5, 4); // From + length
  ASSERT_TRUE(s3 == "is a");
}

TEST_F(StringTest, TestCharConstructor)
{
  String s('A');
  ASSERT_TRUE(s == "A");
  String s2((unsigned char)'Z'); // From + length
  ASSERT_TRUE(s2 == "Z");
}

TEST_F(StringTest, TestBoolConstructor)
{
  String s(true);
  ASSERT_TRUE(s == "1");
  String s2(false);
  ASSERT_TRUE(s2 == "0");
}

TEST_F(StringTest, TestBoolTextConstructor)
{
  String s(true, false);
  ASSERT_TRUE(s == "1");
  String s2(false, false);
  ASSERT_TRUE(s2 == "0");
  String s3(true, true);
  ASSERT_TRUE(s3 == "true");
  String s4(false, true);
  ASSERT_TRUE(s4 == "false");
}

TEST_F(StringTest, TestWordConstructor)
{
  String s((short)12539);
  ASSERT_TRUE(s == "12539");
  String s2((short)-12539);
  ASSERT_TRUE(s2 == "-12539");
  String s3((unsigned short)65535);
  ASSERT_TRUE(s3 == "65535");
  String s4((short)0);
  ASSERT_TRUE(s4 == "0");
}

TEST_F(StringTest, TestDWordConstructor)
{
  String s(0);
  ASSERT_TRUE(s == "0");
  String s2(2012987426);
  ASSERT_TRUE(s2 == "2012987426");
  String s3(-2012987426);
  ASSERT_TRUE(s3 == "-2012987426");
  String s4(-1);
  ASSERT_TRUE(s4 == "-1");
  String s5(2147483647); // max int
  ASSERT_TRUE(s5 == "2147483647");
  String s6((int)-2147483648); // min int
  ASSERT_TRUE(s6 == "-2147483648");
  String s7((unsigned int)4294967295);
  ASSERT_TRUE(s7 == "4294967295");
}

TEST_F(StringTest, TestQWordConstructor)
{
  String s(0LL);
  ASSERT_TRUE(s == "0");
  String s2(2012987426759123LL);
  ASSERT_TRUE(s2 == "2012987426759123");
  String s3(-2012987426856254LL);
  ASSERT_TRUE(s3 == "-2012987426856254");
  String s4(-1LL);
  ASSERT_TRUE(s4 == "-1");
  String s5(9223372036854775807LL); // max LL
  ASSERT_TRUE(s5 == "9223372036854775807");
  String s6(-9223372036854775807LL); // min LL
  ASSERT_TRUE(s6 == "-9223372036854775807");
  String s7(18446744073709551615ULL); // min LL
  ASSERT_TRUE(s7 == "18446744073709551615");
}

TEST_F(StringTest, TestFloatConstructor)
{
  String s(0.f);
  ASSERT_TRUE(s == "0.00");
  String s2(1.f);
  ASSERT_TRUE(s2 == "1.00");
  String s3(-1.f);
  ASSERT_TRUE(s3 == "-1.00");
  String s4(12345.123f);
  ASSERT_TRUE(s4 == "12345.12");
  String s5(987654.f);
  ASSERT_TRUE(s5 == "987654.00");
  String s6(.1234f);
  ASSERT_TRUE(s6 == "0.12");
  String s7(-.987f);
  ASSERT_TRUE(s7 == "-0.98");
}

TEST_F(StringTest, TestFloatConstructorPrecision)
{
  String s(0.f, 4);
  ASSERT_TRUE(s == "0.0000");
  String s2(1.f, 3);
  ASSERT_TRUE(s2 == "1.000");
  String s3(-1.f, 10);
  ASSERT_TRUE(s3 == "-1.0000000000");
  String s4(12345.123456789f, 5);
  ASSERT_TRUE(s4 == "12345.12304"); // float precision limit
  String s5(987654.f, 15);
  ASSERT_TRUE(s5 == "987654.0000000000");
  String s6(.1234f, 5);
  ASSERT_TRUE(s6 == "0.12340");
  String s7(-.987f, 1);
  ASSERT_TRUE(s7 == "-0.9");
}

TEST_F(StringTest, TestDoubleConstructor)
{
  String s(0.);
  ASSERT_TRUE(s == "0.00");
  String s2(1.);
  ASSERT_TRUE(s2 == "1.00");
  String s3(-1.);
  ASSERT_TRUE(s3 == "-1.00");
  String s4(123456789.12345);
  ASSERT_TRUE(s4 == "123456789.12");
  String s5(987654321.);
  ASSERT_TRUE(s5 == "987654321.00");
  String s6(.1234);
  ASSERT_TRUE(s6 == "0.12");
  String s7(-.987);
  ASSERT_TRUE(s7 == "-0.98");
}

TEST_F(StringTest, TestDoubleConstructorPrecision)
{
  String s(0., 4);
  ASSERT_TRUE(s == "0.0000");
  String s2(1., 3);
  ASSERT_TRUE(s2 == "1.000");
  String s3(-1., 10);
  ASSERT_TRUE(s3 == "-1.0000000000");
  String s4(12345.123456789, 8);
  ASSERT_TRUE(s4 == "12345.12345678");
  String s5(987654., 15);
  ASSERT_TRUE(s5 == "987654.0000000000");
  String s6(.0123456789, 9);
  ASSERT_TRUE(s6 == "0.012345678");
  String s7(-.987, 8);
  ASSERT_TRUE(s7 == "-0.98700000");
}

TEST_F(StringTest, TestCopyConstructor)
{
  // From String
  String s("Recalbox");
  String s2(s);
  ASSERT_TRUE(s == "Recalbox");
  ASSERT_TRUE(s2 == "Recalbox");

  // From std::string
  #ifndef NO_STD_STRING_AUTOBOXING
  std::string s3("Recalbox");
  String s4(s3);
  ASSERT_TRUE(s3 == "Recalbox");
  ASSERT_TRUE(s4 == "Recalbox");
  #endif
}

TEST_F(StringTest, TestMoveConstructor)
{
  // From String
  String s("this string should move to another string");
  String s2(std::move(s));
  ASSERT_TRUE(s.empty());
  ASSERT_TRUE(s2 == "this string should move to another string");

  #ifndef NO_STD_STRING_AUTOBOXING
  // From std::string
  std::string s3("this string should move to another string");
  String s4(std::move(s3));
  //ASSERT_TRUE(s3.empty()); // Move from string seems to Copy instead. Need further investigations
  ASSERT_TRUE(s4 == "this string should move to another string");
  #endif
}

TEST_F(StringTest, TestFormatConstrutor)
{
  ASSERT_TRUE(String(_F("Recalbox is COOL!")) == "Recalbox is COOL!");
  ASSERT_TRUE(String(_F("Recalbox is {0}!") / "COOL") == "Recalbox is COOL!");
}

TEST_F(StringTest, TestCountChar)
{
  String s("0 1 2 3 4 5 6 7 8 9 ");
  ASSERT_TRUE(s.Count(' ') == 10);
  ASSERT_TRUE(s.Count('5') == 1);
  ASSERT_TRUE(s.Count('A') == 0);
}

TEST_F(StringTest, TestCountControlChar)
{
  String s;
  for(int i = 32; --i >= 0;)
    s.Append((char)i).Append((char)('A' + i));
  ASSERT_TRUE(s.CountControl() == 31); // \t (\x09) is not a control char
}

TEST_F(StringTest, TestHash)
{
  ASSERT_TRUE(String("").Hash() != 0);
  ASSERT_TRUE(String("A").Hash() != String("B").Hash());
}

TEST_F(StringTest, TestHash64)
{
  ASSERT_TRUE(String("").Hash64() != 0);
  ASSERT_TRUE(String("A").Hash64() != String("B").Hash64());
  ASSERT_TRUE((String("Recalbox").Hash64() >> 32) !=0);
}

TEST_F(StringTest, TestHashCStringLength)
{
  ASSERT_TRUE(String::Hash(LEGACY_STRING("")) != 0);
  ASSERT_TRUE(String::Hash(LEGACY_STRING("A")) != String::Hash(LEGACY_STRING("B")));
}

TEST_F(StringTest, TestHash64CStringLength)
{
  ASSERT_TRUE(String::Hash64(LEGACY_STRING("")) != 0);
  ASSERT_TRUE(String::Hash64(LEGACY_STRING("A")) != String::Hash64(LEGACY_STRING("B")));
  ASSERT_TRUE((String::Hash64(LEGACY_STRING("Recalbox")) >> 32) !=0);
}

TEST_F(StringTest, TestHashCString)
{
  ASSERT_TRUE(String::Hash("") != 0);
  ASSERT_TRUE(String::Hash("A") != String::Hash("B"));
}

TEST_F(StringTest, TestHash64CString)
{
  ASSERT_TRUE(String::Hash64("") != 0);
  ASSERT_TRUE(String::Hash64("A") != String::Hash64("B"));
  ASSERT_TRUE((String::Hash64("Recalbox") >> 32) !=0);
}

TEST_F(StringTest, TestAssignIntToHexa)
{
  { String s(0x12BC, String::Hexa::C); ASSERT_TRUE(s == "0x12BC"); }
  { String s(0xA, String::Hexa::C); ASSERT_TRUE(s == "0xA"); }
  { String s(-0x1F, String::Hexa::C); ASSERT_TRUE(s == "-0x1F"); }
  { String s(0x7FFFFFFF, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFF"); }
  { String s(-0x7FFFFFFF, String::Hexa::C); ASSERT_TRUE(s == "-0x7FFFFFFF"); }
  { String s(0x12BC, String::Hexa::Asm); ASSERT_TRUE(s == "$12BC"); }
  { String s(0x12BC, String::Hexa::None); ASSERT_TRUE(s == "12BC"); }
}

TEST_F(StringTest, TestAssignIntToHexaMin)
{
  { String s(0x12BC, 7, String::Hexa::C); ASSERT_TRUE(s == "0x00012BC"); }
  { String s(0xA, 7, String::Hexa::C); ASSERT_TRUE(s == "0x000000A"); }
  { String s(-0x1F, 7, String::Hexa::C); ASSERT_TRUE(s == "-0x000001F"); }
  { String s(0x7FFFFFFF, 7, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFF"); }
  { String s(-0x7FFFFFFF, 7, String::Hexa::C); ASSERT_TRUE(s == "-0x7FFFFFFF"); }
  { String s(0x12BC, 7, String::Hexa::Asm); ASSERT_TRUE(s == "$00012BC"); }
  { String s(0x12BC, 7, String::Hexa::None); ASSERT_TRUE(s == "00012BC"); }
}

TEST_F(StringTest, TestAssignUIntToHexa)
{
  { String s(0x12BCu, String::Hexa::C); ASSERT_TRUE(s == "0x12BC"); }
  { String s(0x7FFFFFFFu, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFF"); }
  { String s(0xABCDEF00u, String::Hexa::C); ASSERT_TRUE(s == "0xABCDEF00"); }
  { String s(0xABCDEF00u, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF00"); }
  { String s(0xABCDEF00u, String::Hexa::None); ASSERT_TRUE(s == "ABCDEF00"); }
}

TEST_F(StringTest, TestAssignUIntToHexaMin)
{
  { String s(0x12BCu, 7, String::Hexa::C); ASSERT_TRUE(s == "0x00012BC"); }
  { String s(0x7FFFFFFFu, 7, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFF"); }
  { String s(0xABCDEF00u, 7, String::Hexa::C); ASSERT_TRUE(s == "0xABCDEF00"); }
  { String s(0xABCDEF00u, 7, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF00"); }
  { String s(0x0u, 7, String::Hexa::None); ASSERT_TRUE(s == "0000000"); }
}

TEST_F(StringTest, TestAssignLongLongToHexa)
{
  { String s(0x12BC12345678LL, String::Hexa::C); ASSERT_TRUE(s == "0x12BC12345678"); }
  { String s(0xA12345678LL, String::Hexa::C); ASSERT_TRUE(s == "0xA12345678"); }
  { String s(-0x1F12345678LL, String::Hexa::C); ASSERT_TRUE(s == "-0x1F12345678"); }
  { String s(0x7FFFFFFFFFFFFFFFLL, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF"); }
  { String s(-0x7FFFFFFFFFFFFFFFLL, String::Hexa::C); ASSERT_TRUE(s == "-0x7FFFFFFFFFFFFFFF"); }
  { String s(0x12BC12345678LL, String::Hexa::Asm); ASSERT_TRUE(s == "$12BC12345678"); }
  { String s(0x12BC12345678LL, String::Hexa::None); ASSERT_TRUE(s == "12BC12345678"); }
}

TEST_F(StringTest, TestAssignLongLongToHexaMin)
{
  { String s(0x12BC12345678LL, 15, String::Hexa::C); ASSERT_TRUE(s == "0x00012BC12345678"); }
  { String s(0xA12345678LL, 15, String::Hexa::C); ASSERT_TRUE(s == "0x000000A12345678"); }
  { String s(-0x1F12345678LL, 15, String::Hexa::C); ASSERT_TRUE(s == "-0x000001F12345678"); }
  { String s(0x7FFFFFFFFFFFFFFFLL, 15, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF"); }
  { String s(-0x7FFFFFFFFFFFFFFFLL, 15, String::Hexa::C); ASSERT_TRUE(s == "-0x7FFFFFFFFFFFFFFF"); }
  { String s(0x12BC12345678LL, 15, String::Hexa::Asm); ASSERT_TRUE(s == "$00012BC12345678"); }
  { String s(0x12BC12345678LL, 15, String::Hexa::None); ASSERT_TRUE(s == "00012BC12345678"); }
}

TEST_F(StringTest, TestAssignULONGLONGToHexa)
{
  { String s(0x12BC12345678LLu, String::Hexa::C); ASSERT_TRUE(s == "0x12BC12345678"); }
  { String s(0x7FFFFFFFFFFFFFFFLLu, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF"); }
  { String s(0xABCDEF0012345678LLu, String::Hexa::C); ASSERT_TRUE(s == "0xABCDEF0012345678"); }
  { String s(0xABCDEF0012345678LLu, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF0012345678"); }
  { String s(0xABCDEF0012345678LLu, String::Hexa::None); ASSERT_TRUE(s == "ABCDEF0012345678"); }
}

TEST_F(StringTest, TestAssignULongLongToHexaMin)
{
  { String s(0x12BC12345678LLu, 15, String::Hexa::C); ASSERT_TRUE(s == "0x00012BC12345678"); }
  { String s(0x7FFFFFFFFFFFFFFFLLu, 15, String::Hexa::C); ASSERT_TRUE(s == "0x7FFFFFFFFFFFFFFF"); }
  { String s(0xABCDEF0012345678LLu, 15, String::Hexa::C); ASSERT_TRUE(s == "0xABCDEF0012345678"); }
  { String s(0xABCDEF0012345678LLu, 15, String::Hexa::Asm); ASSERT_TRUE(s == "$ABCDEF0012345678"); }
  { String s(0x0LLu, 15, String::Hexa::None); ASSERT_TRUE(s == "000000000000000"); }
}

#pragma clang diagnostic pop