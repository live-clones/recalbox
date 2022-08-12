//
// Created by bkg2k on 16/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#include <gtest/gtest.h>
#include <utils/Sizes.h>

class SizesTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SizesTest, TestConverters)
{
  long long size = 0x7FFFFFFFFFFFFFFFLL;
  Sizes s(size);
  ASSERT_TRUE(s.ToBytes() == "9223372036854775807B");
  ASSERT_TRUE(s.ToKiloBytes() == "9007199254740992.00KB");
  ASSERT_TRUE(s.ToMegaBytes() == "8796093022207.99MB");
  ASSERT_TRUE(s.ToGigaBytes() == "8589934591.99GB");
  ASSERT_TRUE(s.ToTeraBytes() == "8388607.99TB");
}

TEST_F(SizesTest, TestConvertToHumanSize)
{
  ASSERT_TRUE(Sizes(123LL).ToHumanSize() == "123B");
  ASSERT_TRUE(Sizes(0x1EE00LL).ToHumanSize() == "123.50KB");
  ASSERT_TRUE(Sizes(0x7B80000LL).ToHumanSize() == "123.50MB");
  ASSERT_TRUE(Sizes(0x1EE0000000LL).ToHumanSize() == "123.50GB");
  ASSERT_TRUE(Sizes(0x7B8000000000LL).ToHumanSize() == "123.50TB");
}