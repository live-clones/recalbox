#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-function-cognitive-complexity"
//
// Created by bkg2k on 10/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <gtest/gtest.h>
#include <utils/String.h>

class StringTestUtf8: public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StringTestUtf8, TestCountUTF8)
{
  String s("Rêcälbøx îs p€rđ€¢ŧ! ਔ ֆ");
  ASSERT_TRUE(s.CountUtf8() == 24);
  s.Assign("\u0001\u0100\u0200\u0300\u0400\u0500\u0600\u0700\u0800\u0900\u0A00\u0B00\u0C00\u0D00\u0E00\u0F00");
  ASSERT_TRUE(s.CountUtf8() == 16);
  s.Assign("😁😂😃😄😅😆😇😈😉😊😋😌😍😎😏😐😑😒😓😔😕😖😗😘😙😚😛😜😝😞😟😠😡😢😣😤😥😦😧😨😩😪😫😬😭😮😯😰😱😲😳😴😵😶😷😸😹😺😻😼😽😾😿🙀");
  ASSERT_TRUE(s.CountUtf8() == 64);
}

TEST_F(StringTestUtf8, TestReadUTF8)
{
  String s("😁😂😃a9|\u0100\u0800\u0F00");
  int pos = 0;
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x1f601);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x1f602);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x1f603);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x61);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x39);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x7C);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x100);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0x800);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0xF00);
  ASSERT_TRUE(s.ReadUTF8(pos) == 0);
}

TEST_F(StringTestUtf8, TestAssignUTF8)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.AssignUTF8(0x1f601) == "😁");
  ASSERT_TRUE(s.AssignUTF8(0x1f602) == "😂");
  ASSERT_TRUE(s.AssignUTF8(0x1f603) == "😃");
  ASSERT_TRUE(s.AssignUTF8(0x61)    == "a");
  ASSERT_TRUE(s.AssignUTF8(0x39)    == "9");
  ASSERT_TRUE(s.AssignUTF8(0x7C)    == "|");
  ASSERT_TRUE(s.AssignUTF8(0x100)   == "\u0100");
  ASSERT_TRUE(s.AssignUTF8(0x800)   == "\u0800");
  ASSERT_TRUE(s.AssignUTF8(0xF00)   == "\u0F00");
}

TEST_F(StringTestUtf8, TestAssignUTF8Count)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.AssignUTF8(0x1f601, 1) == "😁");
  ASSERT_TRUE(s.AssignUTF8(0x1f602, 3) == "😂😂😂");
  ASSERT_TRUE(s.AssignUTF8(0x39, 5)    == "99999");
  ASSERT_TRUE(s.AssignUTF8(0x7C, 3)    == "|||");
  ASSERT_TRUE(s.AssignUTF8(0xF00, 2)   == "\u0F00\u0F00");
}

TEST_F(StringTestUtf8, TestAppendUTF8)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.AppendUTF8(0x1f601) == "Recalbox😁");
  ASSERT_TRUE(s.AppendUTF8(0x1f602) == "Recalbox😁😂");
  ASSERT_TRUE(s.AppendUTF8(0x1f603) == "Recalbox😁😂😃");
  ASSERT_TRUE(s.AppendUTF8(0x61)    == "Recalbox😁😂😃a");
  ASSERT_TRUE(s.AppendUTF8(0x39)    == "Recalbox😁😂😃a9");
  ASSERT_TRUE(s.AppendUTF8(0x7C)    == "Recalbox😁😂😃a9|");
  ASSERT_TRUE(s.AppendUTF8(0x100)   == "Recalbox😁😂😃a9|\u0100");
  ASSERT_TRUE(s.AppendUTF8(0x800)   == "Recalbox😁😂😃a9|\u0100\u0800");
  ASSERT_TRUE(s.AppendUTF8(0xF00)   == "Recalbox😁😂😃a9|\u0100\u0800\u0F00");
}

TEST_F(StringTestUtf8, TestAppendUTF8Count)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.AppendUTF8(0x1f601, 1) == "Recalbox😁");
  ASSERT_TRUE(s.AppendUTF8(0x1f602, 3) == "Recalbox😁😂😂😂");
  ASSERT_TRUE(s.AppendUTF8(0x39, 5)    == "Recalbox😁😂😂😂99999");
  ASSERT_TRUE(s.AppendUTF8(0x7C, 3)    == "Recalbox😁😂😂😂99999|||");
  ASSERT_TRUE(s.AppendUTF8(0xF00, 2)   == "Recalbox😁😂😂😂99999|||\u0F00\u0F00");
}

TEST_F(StringTestUtf8, TestInsertUTF8)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.InsertUTF8(INT32_MAX, 0x1f601) == "Recalbox😁");
  ASSERT_TRUE(s.InsertUTF8(0, 0x1f602)         == "😂Recalbox😁");
  ASSERT_TRUE(s.InsertUTF8(6, 0x1f603)         == "😂Recal😃box😁");
  ASSERT_TRUE(s.InsertUTF8(4, 0x61)            == "😂Recaal😃box😁");
  ASSERT_TRUE(s.InsertUTF8(-1, 0x39)           == "😂Recaal😃box9😁");
  ASSERT_TRUE(s.InsertUTF8(-5, 0x7C)           == "😂Recaal😃|box9😁");
  ASSERT_TRUE(s.InsertUTF8(INT32_MIN, 0x100)   == "\u0100😂Recaal😃|box9😁");
  ASSERT_TRUE(s.InsertUTF8(200, 0x800)         == "\u0100😂Recaal😃|box9😁\u0800");
  ASSERT_TRUE(s.InsertUTF8(6, 0xF00)           == "\u0100😂Reca\u0F00al😃|box9😁\u0800");
}

TEST_F(StringTestUtf8, TestInsertUTF8Count)
{
  String s("Recalbox"); //😃a9|\u0100\u0800\u0F00");
  ASSERT_TRUE(s.InsertUTF8(INT32_MAX, 0x1f601, 3) == "Recalbox😁😁😁");
  ASSERT_TRUE(s.InsertUTF8(0, 0x1f602, 5)         == "😂😂😂😂😂Recalbox😁😁😁");
  ASSERT_TRUE(s.InsertUTF8(10, 0x1f603, 1)        == "😂😂😂😂😂Recal😃box😁😁😁");
  ASSERT_TRUE(s.InsertUTF8(-1, 0x39, 3)           == "😂😂😂😂😂Recal😃box😁😁999😁");
  ASSERT_TRUE(s.InsertUTF8(-5, 0x7C, 3)           == "😂😂😂😂😂Recal😃box😁|||😁999😁");
  ASSERT_TRUE(s.InsertUTF8(5, 0xF00, 2)           == "😂😂😂😂😂\u0F00\u0F00Recal😃box😁|||😁999😁");
}

#include <utils/Unicode.h>

TEST_F(StringTestUtf8, TestLowerUnicode)
{
  for(int i = (int)(sizeof(UnicodeConverterTable) / sizeof(UnicodeConverterTable[0])); --i >= 0;)
  {
    const UnicodeConverter& c = UnicodeConverterTable[i];
    ASSERT_TRUE(String::LowerUnicode(c.Capital) == c.Small);
  }
  ASSERT_TRUE(String::UpperUnicode('9') == '9');
}

TEST_F(StringTestUtf8, TestUpperUnicode)
{
  for(int i = (int)(sizeof(UnicodeConverterTable) / sizeof(UnicodeConverterTable[0])); --i >= 0;)
  {
    const UnicodeConverter& c = UnicodeConverterTable[i];
    ASSERT_TRUE(String::UpperUnicode(c.Small) == c.Capital);
  }
  ASSERT_TRUE(String::UpperUnicode('9') == '9');
}

TEST_F(StringTestUtf8, TestLowerCaseUTF8)
{
  ASSERT_TRUE(String("AbCdE").LowerCaseUTF8() == "abcde");
  String s;
  String l;
  for(int i = 0xfffff; --i >= 0; )
  {
    s.AppendUTF8((String::Unicode)i);
    l.AppendUTF8(String::LowerUnicode((String::Unicode)i));
  }
  ASSERT_TRUE(s.LowerCaseUTF8() == l);
  ASSERT_TRUE(s == l);
}

TEST_F(StringTestUtf8, TestUpperCaseUTF8)
{
  ASSERT_TRUE(String("AbCdE").UpperCaseUTF8() == "ABCDE");
  String s;
  String u;
  for(int i = 0xfffff; --i >= 0; )
  {
    s.AppendUTF8((String::Unicode)i);
    u.AppendUTF8(String::UpperUnicode((String::Unicode)i));
  }
  ASSERT_TRUE(s.UpperCaseUTF8() == u);
  ASSERT_TRUE(s == u);
}

TEST_F(StringTestUtf8, TestToLowerCaseUTF8)
{
  ASSERT_TRUE(String("AbCdE").ToLowerCaseUTF8() == "abcde");
  String s;
  String l;
  for(int i = 0xfffff; --i >= 0; )
  {
    s.AppendUTF8((String::Unicode)i);
    l.AppendUTF8(String::LowerUnicode((String::Unicode)i));
  }
  ASSERT_TRUE(s.ToLowerCaseUTF8() == l);
  ASSERT_TRUE(s != l);
}

TEST_F(StringTestUtf8, TestToUpperCaseUTF8)
{
  ASSERT_TRUE(String("AbCdE").ToUpperCaseUTF8() == "ABCDE");
  String s;
  String u;
  for(int i = 0xfffff; --i >= 0; )
  {
    s.AppendUTF8((String::Unicode)i);
    u.AppendUTF8(String::UpperUnicode((String::Unicode)i));
  }
  ASSERT_TRUE(s.ToUpperCaseUTF8() == u);
  ASSERT_TRUE(s != u);
}

TEST_F(StringTestUtf8, TestSplitJoinUnicode)
{
  String s("\u0100😂Reca\u0F00l😃|box9😁\u0800");
  String::UnicodeList list = s.SplitUnicodes();
  ASSERT_TRUE(list[0] == 0x100);
  ASSERT_TRUE(list[1] == 0x1f602);
  ASSERT_TRUE(list[2] == 'R');
  ASSERT_TRUE(list[3] == 'e');
  ASSERT_TRUE(list[4] == 'c');
  ASSERT_TRUE(list[5] == 'a');
  ASSERT_TRUE(list[6] == 0xF00);
  ASSERT_TRUE(list[7] == 'l');
  ASSERT_TRUE(list[8] == 0x1f603);
  ASSERT_TRUE(list[9] == '|');
  ASSERT_TRUE(list[10] == 'b');
  ASSERT_TRUE(list[11] == 'o');
  ASSERT_TRUE(list[12] == 'x');
  ASSERT_TRUE(list[13] == '9');
  ASSERT_TRUE(list[14] == 0x1f601);
  ASSERT_TRUE(list[15] == 0x800);
  ASSERT_TRUE(String::JoinUnicodes(list) == s);
}

#pragma clang diagnostic pop