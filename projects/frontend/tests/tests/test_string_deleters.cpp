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

class StringTestDeleters: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestDeleters, TestCharDeleteAt)
{
  String s("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  for(int i = s.Count() - 2; -- i>= 0; )
  {
    int pos = 1 + ((i * 17) % (s.Count() - 2));
    String s2 = s.SubString(0, pos) + s.SubString(pos + 1);
    ASSERT_TRUE(s.Delete(pos) == s2);
  }
  ASSERT_TRUE(s == "R!");
  ASSERT_TRUE(s.Delete(1) == "R");
  ASSERT_TRUE(s.Delete(0).empty());
}

TEST_F(StringTestDeleters, TestCharsDeleteAtLength)
{
  String s("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Delete(0, 18) == "Recalbox is easy! Recalbox is magic!");
  s.Assign("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Delete(35, 19) == "Recalbox is cool! Recalbox is easy!");
  ASSERT_TRUE(s.Delete(35, 19) == "Recalbox is cool! Recalbox is easy!"); // Check no error when OOB
  s.Assign("Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Delete(17, 18) == "Recalbox is cool! Recalbox is magic!");
  ASSERT_TRUE(s.Delete(8, 18) == "Recalbox is magic!");
}

TEST_F(StringTestDeleters, TestCharDelete)
{
  String s("Recalbox Roxx!");
  ASSERT_TRUE(s.Remove('x') == "Recalbo Ro!");
  ASSERT_TRUE(s == "Recalbo Ro!");
  ASSERT_TRUE(s.Remove('z') == "Recalbo Ro!");
  ASSERT_TRUE(s == "Recalbo Ro!");
}

TEST_F(StringTestDeleters, TestControlCharDelete)
{
  String s, c;
  for (int i = 32; --i >= 0;)
  {
    s.Append((char) i).Append((char) ('A' + i));
    c.Append((char) ('A' + i));
  }
  ASSERT_TRUE(s.RemoveControl() == c);
}
TEST_F(StringTestDeleters, TestCStringDelete)
{
  String s("Recalbox is not cool! Recalbox is not easy! Recalbox is not magic!");
  ASSERT_TRUE(s.Remove("not ") == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Remove("bad") == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
}

TEST_F(StringTestDeleters, TestCStringLengthDelete)
{
  String s("Recalbox is not cool! Recalbox is not easy! Recalbox is not magic!");
  ASSERT_TRUE(s.Remove(LEGACY_STRING("not ")) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Remove(LEGACY_STRING("bad")) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
}

TEST_F(StringTestDeleters, TestStringDelete)
{
  String w("not ");
  String b("bad");
  String s("Recalbox is not cool! Recalbox is not easy! Recalbox is not magic!");
  ASSERT_TRUE(s.Remove(w) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s.Remove(b) == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
  ASSERT_TRUE(s == "Recalbox is cool! Recalbox is easy! Recalbox is magic!");
}

#pragma clang diagnostic pop