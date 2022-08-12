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

class StringTestOperator: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestOperator, TestStringPlusOperator)
{
  // Non const appending
  String s("Recal");
  String s2("Box");
  ASSERT_TRUE((s += s2) == "RecalBox"); // result pushed in s
  ASSERT_TRUE(s == "RecalBox");
  ASSERT_TRUE(s2 == "Box");

  // Const appending
  const String s3("Recal");
  String s4("Box");
  ASSERT_TRUE((s3 + s4) == "RecalBox"); // result pushed in a tmp string
  ASSERT_TRUE(s3 == "Recal");
  ASSERT_TRUE(s4 == "Box");
}

TEST_F(StringTestOperator, TestCStringPlusOperator)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE((s += "Box") == "RecalBox"); // result pushed in s
  ASSERT_TRUE(s == "RecalBox");

  // Const appending
  const String s3("Recal");
  ASSERT_TRUE(s3 + "Box" == "RecalBox"); // result pushed in a tmp string
  ASSERT_TRUE(s3 == "Recal");
}

TEST_F(StringTestOperator, TestCharPlusOperator)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE((s += 'A') == "RecalA"); // result pushed in s
  ASSERT_TRUE(s == "RecalA");

  // Const appending
  const String s3("Recal");
  ASSERT_TRUE(s3 + 'Z' == "RecalZ"); // result pushed in a tmp string
  ASSERT_TRUE(s3 == "Recal");
}

TEST_F(StringTestOperator, TestBoolPlusOperator)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE(((s += true) += false) == "Recal10"); // result pushed in s
  ASSERT_TRUE(s == "Recal10");

  // Const appending
  const String s3("Recal");
  ASSERT_TRUE(s3 + false + true == "Recal01"); // result pushed in a tmp string
  ASSERT_TRUE(s3 == "Recal");
}

TEST_F(StringTestOperator, TestUnsignedCharPlusOperator)
{
  unsigned char c = 0x61;

  // Non const appending
  String s("Recal");
  ASSERT_TRUE((s += c) == "Recala"); // result pushed in s
  ASSERT_TRUE(s == "Recala");

  // Const appending
  const String s3("Recal");
  ASSERT_TRUE(s3 + c == "Recala"); // result pushed in a tmp string
  ASSERT_TRUE(s3 == "Recal");
}

TEST_F(StringTestOperator, TestWordPlusOperator)
{
  String s("Recalbox");
  ASSERT_TRUE((s += (short)12345) == "Recalbox12345");
  ASSERT_TRUE(s == "Recalbox12345");
  ASSERT_TRUE((s += (short)-12345) == "Recalbox12345-12345");
  ASSERT_TRUE(s == "Recalbox12345-12345");
  ASSERT_TRUE((s += (unsigned short)65535) == "Recalbox12345-1234565535");
  ASSERT_TRUE(s == "Recalbox12345-1234565535");
  ASSERT_TRUE((s += (short)0) == "Recalbox12345-12345655350");
  ASSERT_TRUE(s == "Recalbox12345-12345655350");

  const String sc("Recalbox");
  ASSERT_TRUE(sc + (short)12345 == "Recalbox12345");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + (short)-12345 == "Recalbox-12345");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + (unsigned short)65535 == "Recalbox65535");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + (short)0 == "Recalbox0");
  ASSERT_TRUE(sc == "Recalbox");
}

TEST_F(StringTestOperator, TestDWordPlusOperator)
{
  String s("Recalbox");
  ASSERT_TRUE((s += 0) == "Recalbox0");
  ASSERT_TRUE(s == "Recalbox0");
  ASSERT_TRUE((s += 2012987426) == "Recalbox02012987426");
  ASSERT_TRUE(s == "Recalbox02012987426");
  ASSERT_TRUE((s += -2012987426) == "Recalbox02012987426-2012987426");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426");
  ASSERT_TRUE((s += -1) == "Recalbox02012987426-2012987426-1");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-1");
  ASSERT_TRUE((s += 2147483647) == "Recalbox02012987426-2012987426-12147483647");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647");
  ASSERT_TRUE((s += (int)-2147483648) == "Recalbox02012987426-2012987426-12147483647-2147483648");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647-2147483648");
  ASSERT_TRUE((s += (unsigned int)4294967295) == "Recalbox02012987426-2012987426-12147483647-21474836484294967295");
  ASSERT_TRUE(s == "Recalbox02012987426-2012987426-12147483647-21474836484294967295");

  const String sc("Recalbox");
  ASSERT_TRUE(sc + 0 == "Recalbox0");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 2012987426 == "Recalbox2012987426");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -2012987426 == "Recalbox-2012987426");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -1== "Recalbox-1");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 2147483647 == "Recalbox2147483647");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + (int)-2147483648 == "Recalbox-2147483648");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + (unsigned int)4294967295 == "Recalbox4294967295");
  ASSERT_TRUE(sc == "Recalbox");
}

TEST_F(StringTestOperator, TestSWordPlusOperator)
{
  String s("Recalbox");
  ASSERT_TRUE((s += 0LL) == "Recalbox0");
  ASSERT_TRUE(s == "Recalbox0");
  ASSERT_TRUE((s += 2012987426759123LL) == "Recalbox02012987426759123");
  ASSERT_TRUE(s == "Recalbox02012987426759123");
  ASSERT_TRUE((s += -2012987426856254LL) == "Recalbox02012987426759123-2012987426856254");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254");
  ASSERT_TRUE((s += -1LL) == "Recalbox02012987426759123-2012987426856254-1");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-1");
  ASSERT_TRUE((s += 9223372036854775807LL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807");
  ASSERT_TRUE((s += -9223372036854775807LL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807-9223372036854775807");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807-9223372036854775807");
  ASSERT_TRUE((s += 18446744073709551615ULL) == "Recalbox02012987426759123-2012987426856254-19223372036854775807-922337203685477580718446744073709551615");
  ASSERT_TRUE(s == "Recalbox02012987426759123-2012987426856254-19223372036854775807-922337203685477580718446744073709551615");

  const String sc("Recalbox");
  ASSERT_TRUE(sc + 0LL == "Recalbox0");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 2012987426759123LL == "Recalbox2012987426759123");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -2012987426856254LL == "Recalbox-2012987426856254");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -1LL == "Recalbox-1");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 9223372036854775807LL == "Recalbox9223372036854775807");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -9223372036854775807LL == "Recalbox-9223372036854775807");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 18446744073709551615ULL == "Recalbox18446744073709551615");
  ASSERT_TRUE(sc == "Recalbox");
}

TEST_F(StringTestOperator, TestFloatPlusOperator)
{
  String s("Recalbox");
  ASSERT_TRUE((s += 0.f) == "Recalbox0.00");
  ASSERT_TRUE(s == "Recalbox0.00");
  ASSERT_TRUE((s += 1.f) == "Recalbox0.001.00");
  ASSERT_TRUE(s == "Recalbox0.001.00");
  ASSERT_TRUE((s += -1.f) == "Recalbox0.001.00-1.00");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00");
  ASSERT_TRUE((s += 12345.123f) == "Recalbox0.001.00-1.0012345.12");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.0012345.12");
  ASSERT_TRUE((s += 987654.f) == "Recalbox0.001.00-1.0012345.12987654.00");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.0012345.12987654.00");
  ASSERT_TRUE((s += .1234f) == "Recalbox0.001.00-1.0012345.12987654.000.12");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.0012345.12987654.000.12");
  ASSERT_TRUE((s += -.987f) == "Recalbox0.001.00-1.0012345.12987654.000.12-0.98");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.0012345.12987654.000.12-0.98");

  const String sc("Recalbox");
  ASSERT_TRUE(sc + 0.f == "Recalbox0.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 1.f == "Recalbox1.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -1.f == "Recalbox-1.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 12345.123f == "Recalbox12345.12");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 987654.f == "Recalbox987654.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + .1234f == "Recalbox0.12");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -.987f == "Recalbox-0.98");
  ASSERT_TRUE(sc == "Recalbox");
}

TEST_F(StringTestOperator, TestDoublePlusOperator)
{
  String s("Recalbox");
  ASSERT_TRUE((s += 0.) == "Recalbox0.00");
  ASSERT_TRUE(s == "Recalbox0.00");
  ASSERT_TRUE((s += 1.) == "Recalbox0.001.00");
  ASSERT_TRUE(s == "Recalbox0.001.00");
  ASSERT_TRUE((s += -1.) == "Recalbox0.001.00-1.00");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00");
  ASSERT_TRUE((s += 123456789.12345) == "Recalbox0.001.00-1.00123456789.12");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00123456789.12");
  ASSERT_TRUE((s += 987654321.) == "Recalbox0.001.00-1.00123456789.12987654321.00");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00123456789.12987654321.00");
  ASSERT_TRUE((s += .1234) == "Recalbox0.001.00-1.00123456789.12987654321.000.12");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00123456789.12987654321.000.12");
  ASSERT_TRUE((s += -.987) == "Recalbox0.001.00-1.00123456789.12987654321.000.12-0.98");
  ASSERT_TRUE(s == "Recalbox0.001.00-1.00123456789.12987654321.000.12-0.98");

  const String sc("Recalbox");
  ASSERT_TRUE(sc + 0. == "Recalbox0.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 1. == "Recalbox1.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -1. == "Recalbox-1.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 123456789.12345 == "Recalbox123456789.12");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + 987654321. == "Recalbox987654321.00");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + .1234 == "Recalbox0.12");
  ASSERT_TRUE(sc == "Recalbox");
  ASSERT_TRUE(sc + -.987 == "Recalbox-0.98");
  ASSERT_TRUE(sc == "Recalbox");
}

TEST_F(StringTestOperator, TestCharMinusOperator)
{
  String s("Recalbox Roxx!");
  ASSERT_TRUE((s -= 'x') == "Recalbo Ro!");
  ASSERT_TRUE(s == "Recalbo Ro!");
  ASSERT_TRUE((s -= 'z') == "Recalbo Ro!");
  ASSERT_TRUE(s == "Recalbo Ro!");

  const String sc("Recalbox Roxx!");
  ASSERT_TRUE(sc - 'x' == "Recalbo Ro!");
  ASSERT_TRUE(sc == "Recalbox Roxx!");
  ASSERT_TRUE(sc - 'z' == "Recalbox Roxx!");
  ASSERT_TRUE(sc == "Recalbox Roxx!");
}

TEST_F(StringTestOperator, TestCStringMinusOperator)
{
  String s("Recalbox is not cool! Recalbox is not easy! Recalbox is not magic!");
  ASSERT_TRUE((s -= "not ") == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE((s -= "bad") == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");

  const String sc("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc - "Recalbox " == "is cool! is easy! is magic!");
  ASSERT_TRUE(sc == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc - "bad" == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
}

TEST_F(StringTestOperator, TestStringMinusOperator)
{
  String w("not ");
  String r("Recalbox ");
  String b("bad");
  String s("Recalbox is not cool! Recalbox is not easy! Recalbox is not magic!");
  ASSERT_TRUE((s -= w) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE((s -= b) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");

  const String sc("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc - r == "is cool! is easy! is magic!");
  ASSERT_TRUE(sc == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc - b == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(sc == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToBool)
{
  String s("Recalbox");
  ASSERT_TRUE(s == false);
  ASSERT_TRUE(s != true);
  s.Assign("0");
  ASSERT_TRUE(s == false);
  ASSERT_TRUE(s != true);
  s.Assign("1");
  ASSERT_TRUE(s == true);
  ASSERT_TRUE(s != false);
  s.Assign("false");
  ASSERT_TRUE(s == false);
  ASSERT_TRUE(s != true);
  s.Assign("true");
  ASSERT_TRUE(s == true);
  ASSERT_TRUE(s != false);
}

TEST_F(StringTestOperator, TestBoolOperator)
{
  String s("Recalbox");
  ASSERT_FALSE(s);
  ASSERT_TRUE(!s);
  s.Assign("0");
  ASSERT_FALSE(s);
  ASSERT_TRUE(!s);
  s.Assign("1");
  ASSERT_TRUE(s);
  ASSERT_FALSE(!s);
  s.Assign("false");
  ASSERT_FALSE(s);
  ASSERT_TRUE(!s);
  s.Assign("true");
  ASSERT_TRUE(s);
  ASSERT_FALSE(!s);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToChar)
{
  String s("Recalbox");
  ASSERT_FALSE(s == 'A');
  ASSERT_TRUE(s != 'A');
  s.Assign("A");
  ASSERT_TRUE(s == 'A');
  ASSERT_FALSE(s != 'A');
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s == (short)0);
  ASSERT_FALSE(s != (short)0);
  s.Assign("32767");
  ASSERT_TRUE(s == (short)32767);
  ASSERT_FALSE(s != (short)32767);
  s.Assign("-32768");
  ASSERT_TRUE(s == (short)-32768);
  ASSERT_FALSE(s != (short)-32768);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s == 0);
  ASSERT_FALSE(s != 0);
  s.Assign("2147483647");
  ASSERT_TRUE(s == 2147483647);
  ASSERT_FALSE(s != 2147483647);
  s.Assign("-2147483648");
  ASSERT_TRUE(s == -2147483647-1);
  ASSERT_FALSE(s != -2147483647-1);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToLongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s == 0);
  ASSERT_FALSE(s != 0);
  s.Assign("9223372036854775807");
  ASSERT_TRUE(s == 9223372036854775807LL);
  ASSERT_FALSE(s != 9223372036854775807LL);
  s.Assign("-9223372036854775808");
  ASSERT_TRUE(s == -9223372036854775807LL-1LL);
  ASSERT_FALSE(s != -9223372036854775807LL-1LL);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToUShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s == (short)0u);
  ASSERT_FALSE(s != (short)0u);
  s.Assign("65535");
  ASSERT_TRUE(s == (unsigned short)65535u);
  ASSERT_FALSE(s != (unsigned short)65535u);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToUInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s == 0u);
  ASSERT_FALSE(s != 0u);
  s.Assign("4294967295");
  ASSERT_TRUE(s == 4294967295u);
  ASSERT_FALSE(s != 4294967295u);
}

TEST_F(StringTestOperator, TestOperatorEqualAndNotEqualToULongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s == 0ULL);
  ASSERT_FALSE(s != 0ULL);
  s.Assign("18446744073709551615");
  ASSERT_TRUE(s == 18446744073709551615ULL);
  ASSERT_FALSE(s != 18446744073709551615ULL);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s < (short)1);
  ASSERT_TRUE(s > (short)-1);
  s.Assign("32766");
  ASSERT_TRUE(s < (short)32767);
  ASSERT_TRUE(s > (short)32765);
  s.Assign("-32767");
  ASSERT_TRUE(s < (short)-32766);
  ASSERT_TRUE(s > (short)-32768);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s < 1);
  ASSERT_TRUE(s > -1);
  s.Assign("2147483646");
  ASSERT_TRUE(s < 2147483647);
  ASSERT_TRUE(s > 2147483645);
  s.Assign("-2147483647");
  ASSERT_TRUE(s < -2147483646);
  ASSERT_TRUE(s > -2147483647-1);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToLongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s < 1LL);
  ASSERT_TRUE(s > -1LL);
  s.Assign("9223372036854775806");
  ASSERT_TRUE(s < 9223372036854775807LL);
  ASSERT_TRUE(s > 9223372036854775805LL);
  s.Assign("-9223372036854775807");
  ASSERT_TRUE(s < -9223372036854775806LL);
  ASSERT_TRUE(s > -9223372036854775807LL-1LL);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToUShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s < (unsigned short)1u);
  ASSERT_FALSE(s > (unsigned short)0u);
  s.Assign("65535");
  ASSERT_FALSE(s < (unsigned short)65535u);
  ASSERT_TRUE(s > (unsigned short)65534u);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToUInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s < 1u);
  ASSERT_FALSE(s > 0u);
  s.Assign("4294967295");
  ASSERT_FALSE(s < 4294967295u);
  ASSERT_TRUE(s > 4294967294u);
}

TEST_F(StringTestOperator, TestOperatorLesserAndHigherToULongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s < 1ULL);
  ASSERT_FALSE(s > 0ULL);
  s.Assign("18446744073709551615");
  ASSERT_FALSE(s < 18446744073709551615ULL);
  ASSERT_TRUE(s > 18446744073709551614ULL);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= (short)1);
  ASSERT_TRUE(s >= (short)-1);
  ASSERT_TRUE(s <= (short)0);
  ASSERT_TRUE(s >= (short)0);
  s.Assign("32766");
  ASSERT_TRUE(s <= (short)32767);
  ASSERT_TRUE(s >= (short)32765);
  ASSERT_TRUE(s <= (short)32766);
  ASSERT_TRUE(s >= (short)32766);
  s.Assign("-32767");
  ASSERT_TRUE(s <= (short)-32766);
  ASSERT_TRUE(s >= (short)-32768);
  ASSERT_TRUE(s <= (short)-32767);
  ASSERT_TRUE(s >= (short)-32767);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= 1);
  ASSERT_TRUE(s >= -1);
  ASSERT_TRUE(s <= 0);
  ASSERT_TRUE(s >= 0);
  s.Assign("2147483646");
  ASSERT_TRUE(s <= 2147483647);
  ASSERT_TRUE(s >= 2147483645);
  ASSERT_TRUE(s <= 2147483646);
  ASSERT_TRUE(s >= 2147483646);
  s.Assign("-2147483647");
  ASSERT_TRUE(s <= -2147483646);
  ASSERT_TRUE(s >= -2147483647-1);
  ASSERT_TRUE(s <= -2147483647);
  ASSERT_TRUE(s >= -2147483647);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToLongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= 1LL);
  ASSERT_TRUE(s >= -1LL);
  ASSERT_TRUE(s <= 0LL);
  ASSERT_TRUE(s >= 0LL);
  s.Assign("9223372036854775806");
  ASSERT_TRUE(s <= 9223372036854775807LL);
  ASSERT_TRUE(s >= 9223372036854775805LL);
  ASSERT_TRUE(s <= 9223372036854775806LL);
  ASSERT_TRUE(s >= 9223372036854775806LL);
  s.Assign("-9223372036854775807");
  ASSERT_TRUE(s <= -9223372036854775806LL);
  ASSERT_TRUE(s >= -9223372036854775807LL-1LL);
  ASSERT_TRUE(s <= -9223372036854775807LL);
  ASSERT_TRUE(s >= -9223372036854775807LL);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToUShort)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= (unsigned short)1u);
  ASSERT_FALSE(s >= (unsigned short)1u);
  ASSERT_TRUE(s <= (unsigned short)0u);
  ASSERT_TRUE(s >= (unsigned short)0u);
  s.Assign("65534");
  ASSERT_TRUE(s <= (unsigned short)65535u);
  ASSERT_TRUE(s >= (unsigned short)65533u);
  ASSERT_TRUE(s <= (unsigned short)65534u);
  ASSERT_TRUE(s >= (unsigned short)65534u);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToUInt)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= 1u);
  ASSERT_FALSE(s >= 1u);
  ASSERT_TRUE(s <= 0u);
  ASSERT_TRUE(s >= 0u);
  s.Assign("4294967294");
  ASSERT_TRUE(s <= 4294967295u);
  ASSERT_TRUE(s >= 4294967293u);
  ASSERT_TRUE(s <= 4294967294u);
  ASSERT_TRUE(s >= 4294967294u);
}

TEST_F(StringTestOperator, TestOperatorLesserOrEqualAndHigherOrEqualToULongLong)
{
  String s("Recalbox");
  ASSERT_TRUE(s <= 1ULL);
  ASSERT_FALSE(s >= 1ULL);
  ASSERT_TRUE(s <= 0ULL);
  ASSERT_TRUE(s >= 0ULL);
  s.Assign("18446744073709551614");
  ASSERT_TRUE(s <= 18446744073709551615ULL);
  ASSERT_TRUE(s >= 18446744073709551613ULL);
  ASSERT_TRUE(s <= 18446744073709551614ULL);
  ASSERT_TRUE(s >= 18446744073709551614ULL);
}

TEST_F(StringTestOperator, TestOperatorMultiply)
{
  String s("1234");
  ASSERT_TRUE((s * 3) == "123412341234");
  ASSERT_TRUE(s == "1234");
  String s2("1234");
  ASSERT_TRUE((s2 *= 3) == "123412341234");
  ASSERT_TRUE(s2 == "123412341234");
}

TEST_F(StringTestOperator, TestDivideChar)
{
  String s("Recalbox|Is|The|Best!");
  String::List l = s / '|';
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestOperator, TestDivideCString)
{
  String s("Recalbox<*>Is<*>The<*>Best!");
  String::List l = s / "<*>";
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestOperator, TestDivideString)
{
  String s("Recalbox<*>Is<*>The<*>Best!");
  String::List l = s / String("<*>");
  ASSERT_TRUE(l.size() == 4);
  ASSERT_TRUE(l[0] == "Recalbox");
  ASSERT_TRUE(l[1] == "Is");
  ASSERT_TRUE(l[2] == "The");
  ASSERT_TRUE(l[3] == "Best!");
}

TEST_F(StringTestOperator, TestFormatPlusOperator)
{
  // Non const appending
  String s("Recal");
  ASSERT_TRUE((s += (_F("box is super {0}") / "COOL!")) == "Recalbox is super COOL!"); // result pushed in s
  ASSERT_TRUE(s == "Recalbox is super COOL!");

  // Const appending
  const String s2("Recal");
  ASSERT_TRUE((s2 + (_F("box is super {0}") / "COOL!")) == "Recalbox is super COOL!"); // result pushed in s
  ASSERT_TRUE(s2 == "Recal");
}

#pragma clang diagnostic pop
