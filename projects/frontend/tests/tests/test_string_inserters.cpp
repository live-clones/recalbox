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

class StringTestInserters: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestInserters, TestStdStringInsert)
{
  String s("RappelBoite");
  std::string s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2) == "RappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2) == "RecalboxRappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2) == "RecalboxRappelRecalboxBoiteRecalbox"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2) == "RecalboxRappelRecalboxBoiteRecalRecalboxbox"); // result pushed in s
  ASSERT_TRUE(s == "RecalboxRappelRecalboxBoiteRecalRecalboxbox");
}

TEST_F(StringTestInserters, TestStdStringFromInsert)
{
  String s("RappelBoite");
  std::string s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2, 5) == "RappelboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2, 5) == "boxRappelboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2, 1) == "boxRappelboxBoiteecalbox"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2, 5) == "boxRappelboxBoiteecalboxbox"); // result pushed in s
  ASSERT_TRUE(s == "boxRappelboxBoiteecalboxbox");
}

TEST_F(StringTestInserters, TestStdStringFromLengthInsert)
{
  String s("RappelBoite");
  std::string s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2, 5, 2) == "RappelboBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2, 5, INT32_MAX) == "boxRappelboBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2, 1, 4) == "boxRappelboBoiteecal"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2, 5, 1) == "boxRappelboBoiteebcal"); // result pushed in s
  ASSERT_TRUE(s == "boxRappelboBoiteebcal");
}

TEST_F(StringTestInserters, TestStringInsert)
{
  String s("RappelBoite");
  String s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2) == "RappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2) == "RecalboxRappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2) == "RecalboxRappelRecalboxBoiteRecalbox"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2) == "RecalboxRappelRecalboxBoiteRecalRecalboxbox"); // result pushed in s
  ASSERT_TRUE(s == "RecalboxRappelRecalboxBoiteRecalRecalboxbox");
}

TEST_F(StringTestInserters, TestStringFromInsert)
{
  String s("RappelBoite");
  String s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2, 5) == "RappelboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2, 5) == "boxRappelboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2, 1) == "boxRappelboxBoiteecalbox"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2, 5) == "boxRappelboxBoiteecalboxbox"); // result pushed in s
  ASSERT_TRUE(s == "boxRappelboxBoiteecalboxbox");
}

TEST_F(StringTestInserters, TestStringFromLengthInsert)
{
  String s("RappelBoite");
  String s2("Recalbox");
  ASSERT_TRUE(s.Insert(6, s2, 5, 2) == "RappelboBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, s2, 5, INT32_MAX) == "boxRappelboBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, s2, 1, 4) == "boxRappelboBoiteecal"); // result pushed in s
  ASSERT_TRUE(s.Insert(-3, s2, 5, 1) == "boxRappelboBoiteebcal"); // result pushed in s
  ASSERT_TRUE(s == "boxRappelboBoiteebcal");
}

TEST_F(StringTestInserters, TestCStringInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, "Recalbox") == "RappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, "Recalbox") == "RecalboxRappelRecalboxBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, "Recalbox") == "RecalboxRappelRecalboxBoiteRecalbox"); // result pushed in s
  ASSERT_TRUE(s == "RecalboxRappelRecalboxBoiteRecalbox");
}

TEST_F(StringTestInserters, TestCharInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, 'R') == "RappelRBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, 'R') == "RRappelRBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, 'R') == "RRappelRBoiteR"); // result pushed in s
  ASSERT_TRUE(s == "RRappelRBoiteR");
}

TEST_F(StringTestInserters, TestCharCountInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, 'B', 3) == "RappelBBBBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, 'R', 10) == "RRRRRRRRRRRappelBBBBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, 'x', 5) == "RRRRRRRRRRRappelBBBBoitexxxxx"); // result pushed in s
  ASSERT_TRUE(s == "RRRRRRRRRRRappelBBBBoitexxxxx");
}

TEST_F(StringTestInserters, TestBoolInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, true) == "Rappel1Boite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, false) == "0Rappel1Boite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, true) == "0Rappel1Boite1"); // result pushed in s
  ASSERT_TRUE(s == "0Rappel1Boite1");
}

TEST_F(StringTestInserters, TestBoolTextInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.InsertTextBool(6, true) == "RappeltrueBoite"); // result pushed in s
  ASSERT_TRUE(s.InsertTextBool(0, false) == "falseRappeltrueBoite"); // result pushed in s
  ASSERT_TRUE(s.InsertTextBool(INT32_MAX, true) == "falseRappeltrueBoitetrue"); // result pushed in s
  ASSERT_TRUE(s == "falseRappeltrueBoitetrue");
}

TEST_F(StringTestInserters, TestUnsignedCharInsert)
{
  unsigned char c = (unsigned char)'R';

  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, c) == "RappelRBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(0, c) == "RRappelRBoite"); // result pushed in s
  ASSERT_TRUE(s.Insert(INT32_MAX, c) == "RRappelRBoiteR"); // result pushed in s
  ASSERT_TRUE(s == "RRappelRBoiteR");
}

TEST_F(StringTestInserters, TestWordInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, (short)12345) == "Rappel12345Boite");
  ASSERT_TRUE(s.Insert(INT32_MAX, (short)-12345) == "Rappel12345Boite-12345");
  ASSERT_TRUE(s.Insert(0, (unsigned short)65535) == "65535Rappel12345Boite-12345");
  ASSERT_TRUE(s.Insert(2, (short)0) == "650535Rappel12345Boite-12345");
  ASSERT_TRUE(s == "650535Rappel12345Boite-12345");
}

TEST_F(StringTestInserters, TestDWordInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, 0) == "Rappel0Boite");
  ASSERT_TRUE(s.Insert(INT32_MAX, 2012987426) == "Rappel0Boite2012987426");
  ASSERT_TRUE(s.Insert(1, -2012987426) == "R-2012987426appel0Boite2012987426");
  ASSERT_TRUE(s.Insert(INT32_MAX, -1) == "R-2012987426appel0Boite2012987426-1");
  ASSERT_TRUE(s.Insert(14, 2147483647) == "R-2012987426ap2147483647pel0Boite2012987426-1");
  ASSERT_TRUE(s.Insert(-2, (int)-2147483648) == "R-2012987426ap2147483647pel0Boite2012987426-2147483648-1");
  ASSERT_TRUE(s.Insert(1, (unsigned int)4294967295) == "R4294967295-2012987426ap2147483647pel0Boite2012987426-2147483648-1");
  ASSERT_TRUE(s == "R4294967295-2012987426ap2147483647pel0Boite2012987426-2147483648-1");
}

TEST_F(StringTestInserters, TestQWordInsert)
{
  String s("RappelBoite");
  ASSERT_TRUE(s.Insert(6, 0LL) == "Rappel0Boite");
  ASSERT_TRUE(s.Insert(INT32_MAX, 2012987426759123LL) == "Rappel0Boite2012987426759123");
  ASSERT_TRUE(s.Insert(1, -2012987426856254LL) == "R-2012987426856254appel0Boite2012987426759123");
  ASSERT_TRUE(s.Insert(INT32_MAX, -1LL) == "R-2012987426856254appel0Boite2012987426759123-1");
  ASSERT_TRUE(s.Insert(20, 9223372036854775807LL) == "R-2012987426856254ap9223372036854775807pel0Boite2012987426759123-1");
  ASSERT_TRUE(s.Insert(-2, -9223372036854775807LL) == "R-2012987426856254ap9223372036854775807pel0Boite2012987426759123-9223372036854775807-1");
  ASSERT_TRUE(s.Insert(1,18446744073709551615ULL) == "R18446744073709551615-2012987426856254ap9223372036854775807pel0Boite2012987426759123-9223372036854775807-1");
  ASSERT_TRUE(s == "R18446744073709551615-2012987426856254ap9223372036854775807pel0Boite2012987426759123-9223372036854775807-1");
}

TEST_F(StringTestInserters, TestFloatInsert)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Insert(5, 0.f) == "Recal0.00box");
  ASSERT_TRUE(s.Insert(0, 1.f) == "1.00Recal0.00box");
  ASSERT_TRUE(s.Insert(-1, -1.f) == "1.00Recal0.00bo-1.00x");
  ASSERT_TRUE(s.Insert(INT32_MAX, 12345.123f) == "1.00Recal0.00bo-1.00x12345.12");
  ASSERT_TRUE(s.Insert(1, 987654.f) == "1987654.00.00Recal0.00bo-1.00x12345.12");
  ASSERT_TRUE(s.Insert(-2, .1234f) == "1987654.00.00Recal0.00bo-1.00x12345.0.1212");
  ASSERT_TRUE(s.Insert(16, -.987f) == "1987654.00.00Rec-0.98al0.00bo-1.00x12345.0.1212");
  ASSERT_TRUE(s == "1987654.00.00Rec-0.98al0.00bo-1.00x12345.0.1212");
}

TEST_F(StringTestInserters, TestFloatAssignPrecision)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Insert(5, 0.f, 4) == "Recal0.0000box");
  ASSERT_TRUE(s.Insert(0, 1.f, 3) == "1.000Recal0.0000box");
  ASSERT_TRUE(s.Insert(-1, -1.f, 10) == "1.000Recal0.0000bo-1.0000000000x");
  ASSERT_TRUE(s.Insert(INT32_MAX, 12345.123456789f, 5) == "1.000Recal0.0000bo-1.0000000000x12345.12304");
  ASSERT_TRUE(s.Insert(1, 987654.f, 15) == "1987654.0000000000.000Recal0.0000bo-1.0000000000x12345.12304");
  ASSERT_TRUE(s.Insert(-2, .1234f, 5) == "1987654.0000000000.000Recal0.0000bo-1.0000000000x12345.1230.1234004");
  ASSERT_TRUE(s.Insert(16, -.987f, 1) == "1987654.00000000-0.900.000Recal0.0000bo-1.0000000000x12345.1230.1234004");
  ASSERT_TRUE(s == "1987654.00000000-0.900.000Recal0.0000bo-1.0000000000x12345.1230.1234004");
}

TEST_F(StringTestInserters, TestDoubleAssign)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Insert(5, 0.) == "Recal0.00box");
  ASSERT_TRUE(s.Insert(0, 1.) == "1.00Recal0.00box");
  ASSERT_TRUE(s.Insert(-1, -1.) == "1.00Recal0.00bo-1.00x");
  ASSERT_TRUE(s.Insert(INT32_MAX, 123456789.12345) == "1.00Recal0.00bo-1.00x123456789.12");
  ASSERT_TRUE(s.Insert(1, 987654321.) == "1987654321.00.00Recal0.00bo-1.00x123456789.12");
  ASSERT_TRUE(s.Insert(-2, .1234) == "1987654321.00.00Recal0.00bo-1.00x123456789.0.1212");
  ASSERT_TRUE(s.Insert(16, -.987) == "1987654321.00.00-0.98Recal0.00bo-1.00x123456789.0.1212");
  ASSERT_TRUE(s == "1987654321.00.00-0.98Recal0.00bo-1.00x123456789.0.1212");
}

TEST_F(StringTestInserters, TestDoubleAssignPrecision)
{
  String s("Recalbox");
  ASSERT_TRUE(s.Insert(5, 0., 4) == "Recal0.0000box");
  ASSERT_TRUE(s.Insert(0, 1., 3) == "1.000Recal0.0000box");
  ASSERT_TRUE(s.Insert(-1, -1., 10) == "1.000Recal0.0000bo-1.0000000000x");
  ASSERT_TRUE(s.Insert(INT32_MAX, 12345.123456789, 8) == "1.000Recal0.0000bo-1.0000000000x12345.12345678");
  ASSERT_TRUE(s.Insert(1, 987654., 15) == "1987654.0000000000.000Recal0.0000bo-1.0000000000x12345.12345678");
  ASSERT_TRUE(s.Insert(-2, .0123456789, 9) == "1987654.0000000000.000Recal0.0000bo-1.0000000000x12345.1234560.01234567878");
  ASSERT_TRUE(s.Insert(16, -.987, 8) == "1987654.00000000-0.9870000000.000Recal0.0000bo-1.0000000000x12345.1234560.01234567878");
}

TEST_F(StringTestInserters, TestAssignIntToHexa)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC); ASSERT_TRUE(s == "Recal0x12BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xA); ASSERT_TRUE(s == "Recal0xAbox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x1F); ASSERT_TRUE(s == "Recal-0x1Fbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFF); ASSERT_TRUE(s == "Recal0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x7FFFFFFF); ASSERT_TRUE(s == "Recal-0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$12BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC, String::Hexa::None); ASSERT_TRUE(s == "Recal12BCbox"); }
}

TEST_F(StringTestInserters, TestAssignIntToHexaMin)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC, 7); ASSERT_TRUE(s == "Recal0x00012BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xA, 7); ASSERT_TRUE(s == "Recal0x000000Abox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x1F, 7); ASSERT_TRUE(s == "Recal-0x000001Fbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFF, 7); ASSERT_TRUE(s == "Recal0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x7FFFFFFF, 7); ASSERT_TRUE(s == "Recal-0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC, 7, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$00012BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC, 7, String::Hexa::None); ASSERT_TRUE(s == "Recal00012BCbox"); }
}

TEST_F(StringTestInserters, TestAssignUIntToHexa)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BCu); ASSERT_TRUE(s == "Recal0x12BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFu); ASSERT_TRUE(s == "Recal0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF00u); ASSERT_TRUE(s == "Recal0xABCDEF00box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF00u, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$ABCDEF00box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF00u, String::Hexa::None); ASSERT_TRUE(s == "RecalABCDEF00box"); }
}

TEST_F(StringTestInserters, TestAssignUIntToHexaMin)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BCu, 7); ASSERT_TRUE(s == "Recal0x00012BCbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFu, 7); ASSERT_TRUE(s == "Recal0x7FFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF00u, 7); ASSERT_TRUE(s == "Recal0xABCDEF00box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF00u, 7, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$ABCDEF00box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x0u, 7, String::Hexa::None); ASSERT_TRUE(s == "Recal0000000box"); }
}

TEST_F(StringTestInserters, TestAssignLongLongToHexa)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL); ASSERT_TRUE(s == "Recal0x12BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xA12345678LL); ASSERT_TRUE(s == "Recal0xA12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x1F12345678LL); ASSERT_TRUE(s == "Recal-0x1F12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "Recal0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x7FFFFFFFFFFFFFFFLL); ASSERT_TRUE(s == "Recal-0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$12BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL, String::Hexa::None); ASSERT_TRUE(s == "Recal12BC12345678box"); }
}

TEST_F(StringTestInserters, TestAssignLongLongToHexaMin)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL, 15); ASSERT_TRUE(s == "Recal0x00012BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xA12345678LL, 15); ASSERT_TRUE(s == "Recal0x000000A12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x1F12345678LL, 15); ASSERT_TRUE(s == "Recal-0x000001F12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "Recal0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, -0x7FFFFFFFFFFFFFFFLL, 15); ASSERT_TRUE(s == "Recal-0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL, 15, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$00012BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LL, 15, String::Hexa::None); ASSERT_TRUE(s == "Recal00012BC12345678box"); }
}

TEST_F(StringTestInserters, TestAssignULONGLONGToHexa)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LLu); ASSERT_TRUE(s == "Recal0x12BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFFFFFFFFFLLu); ASSERT_TRUE(s == "Recal0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF0012345678LLu); ASSERT_TRUE(s == "Recal0xABCDEF0012345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF0012345678LLu, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$ABCDEF0012345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF0012345678LLu, String::Hexa::None); ASSERT_TRUE(s == "RecalABCDEF0012345678box"); }
}

TEST_F(StringTestInserters, TestAssignULongLongToHexaMin)
{
  { String s("Recalbox"); s.InsertHexa(5, 0x12BC12345678LLu, 15); ASSERT_TRUE(s == "Recal0x00012BC12345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x7FFFFFFFFFFFFFFFLLu, 15); ASSERT_TRUE(s == "Recal0x7FFFFFFFFFFFFFFFbox"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF0012345678LLu, 15); ASSERT_TRUE(s == "Recal0xABCDEF0012345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0xABCDEF0012345678LLu, 15, String::Hexa::Asm); ASSERT_TRUE(s == "Recal$ABCDEF0012345678box"); }
  { String s("Recalbox"); s.InsertHexa(5, 0x0LLu, 15, String::Hexa::None); ASSERT_TRUE(s == "Recal000000000000000box"); }
}

TEST_F(StringTestInserters, TestInsertFormat)
{
  String s("Recalbox is  and EASY TO USE!");
  ASSERT_TRUE(s.Insert(12, _F("VERY {0}") / "COOL") == "Recalbox is VERY COOL and EASY TO USE!");
}

#pragma clang diagnostic pop