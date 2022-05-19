#include <gtest/gtest.h>
#include "utils/network/Http.h"
#include <algorithm>
#include <utils/Files.h>

static const std::string rootTest = "/tmp/googletests/";

class HttpTest: public ::testing::Test
{
  protected:
    void SetUp() override
    {
      ASSERT_EQ(system(("mkdir -p " + rootTest).c_str()), 0);
    }

    void TearDown() override
    {
      // Remove test set
      ASSERT_EQ(system("rm -rf /tmp/googletests"), 0);
    }
};

TEST_F(HttpTest, TestGetString)
{
  Http http;
  std::string output;

  ASSERT_TRUE(http.Execute("http://www.perdu.com", output));
  ASSERT_EQ(output, "<html><head><title>Vous Etes Perdu ?</title></head><body><h1>Perdu sur l'Internet ?</h1><h2>Pas de panique, on va vous aider</h2><strong><pre>    * <----- vous &ecirc;tes ici</pre></strong></body></html>\n");
}

TEST_F(HttpTest, TestGetFile)
{
  Http http;

  Path path = Path(rootTest) / "test.txt";
  ASSERT_TRUE(http.Execute("http://www.perdu.com", path));
  std::string output = Files::LoadFile(path);
  ASSERT_EQ(output, "<html><head><title>Vous Etes Perdu ?</title></head><body><h1>Perdu sur l'Internet ?</h1><h2>Pas de panique, on va vous aider</h2><strong><pre>    * <----- vous &ecirc;tes ici</pre></strong></body></html>\n");
}

TEST_F(HttpTest, TestGetBigFile)
{
  Http http;

  Path path = Path(rootTest) / "recalbox.tar";
  ASSERT_TRUE(http.Execute("https://gitlab.com/recalbox/recalbox/-/archive/6.1.1-Dragonblaze/recalbox-6.1.1-Dragonblaze.tar", path));
  ASSERT_EQ(system("md5sum -b /tmp/googletests/recalbox.tar > /tmp/googletests/md5"), 0);
  std::string output = Files::LoadFile(Path("/tmp/googletests/md5"));
  ASSERT_EQ(output, "9256758cea05989f98aac1241937d803 */tmp/googletests/recalbox.tar\n");
}