#include <gtest/gtest.h>
#include <utils/os/fs/Path.h>
#include <climits>
#include <pwd.h>
#include <algorithm>

static const std::string rootTest = "/tmp/googletests/";

class PathTest: public ::testing::Test
{
  protected:
    struct TestSet_
    {
      struct Folder1_
      {
        std::string name = rootTest + "folder1";
        std::string file1 = rootTest + "folder1/file1";
        struct Folder2_
        {
          std::string name = rootTest + "folder1/folder2";
          std::string file2 = rootTest + "folder1/folder2/file2";
          std::string sldir = rootTest + "folder1/folder2/folder4";
          struct Folder3_
          {
            std::string name = rootTest + "folder1/folder2/folder3";
            std::string file3 = rootTest + "folder1/folder2/folder3/file3";
            std::string slfile = rootTest + "folder1/folder2/folder3/file4";
          } Folder3;
        } Folder2;
      } Folder1;
    } TestSet;

    void SetUp() override
    {
      char tmp[PATH_MAX];

      // Get current dir
      currentDirectory = (getcwd(tmp, sizeof(tmp)) != nullptr) ? tmp : "NOCWD";

      // Get home dir
      homeDirectory = getpwuid(getuid())->pw_dir;

      // File & folder test set
      bool ok = true;
      ok &= (system(("mkdir -p " + TestSet.Folder1.Folder2.Folder3.name).c_str()) == 0);
      ok &= (system(("echo text > " + TestSet.Folder1.Folder2.Folder3.file3).c_str()) == 0);
      ok &= (system(("echo text > " + TestSet.Folder1.Folder2.file2).c_str()) == 0);
      ok &= (system(("echo text > " + TestSet.Folder1.file1).c_str()) == 0);
      ok &= (system(("ln -s " + TestSet.Folder1.name + " " + TestSet.Folder1.Folder2.sldir).c_str()) == 0);
      ok &= (system(("ln -s " + TestSet.Folder1.Folder2.file2 + " " + TestSet.Folder1.Folder2.Folder3.slfile).c_str()) == 0);
      ASSERT_TRUE(ok);
    }

    void TearDown() override
    {
      // Remove test set
      ASSERT_EQ(system("rm -rf /tmp/googletests"), 0);
    }

    std::string homeDirectory;
    std::string currentDirectory;
};

TEST_F(PathTest, testNormalizePathStdStrings)
{
  // Tests regular path
  ASSERT_EQ(Path("/path/to/file").ToString(), "/path/to/file");
  ASSERT_EQ(Path("path/to/file").ToString(), "path/to/file");
  ASSERT_EQ(Path("/path/to/file/").ToString(), "/path/to/file");
  ASSERT_EQ(Path("path/to/file/").ToString(), "path/to/file");
  ASSERT_EQ(Path("/").ToString(), "/");
  ASSERT_EQ(Path("~").ToString(), "~");
  ASSERT_EQ(Path("~/").ToString(), "~");
  // Test / and \ convertions
  ASSERT_EQ(Path("path//to/file").ToString(), "path/to/file");
  ASSERT_EQ(Path("path//to\\file").ToString(), "path/to/file");
  ASSERT_EQ(Path("path///to\\\\file").ToString(), "path/to/file");
  ASSERT_EQ(Path("path////to\\\\file").ToString(), "path/to/file");
  // Test . and .. + / and \ convertions
  ASSERT_EQ(Path("./path////to\\\\file").ToString(), "./path/to/file");
  ASSERT_EQ(Path("../path////to\\\\file").ToString(), "../path/to/file");
  ASSERT_EQ(Path("/./path////to\\\\file").ToString(), "/path/to/file");
  ASSERT_EQ(Path("//.\\\\path////to\\\\file").ToString(), "/path/to/file");
  ASSERT_EQ(Path("/../path////to\\\\file").ToString(), "/../path/to/file");
  ASSERT_EQ(Path("/../path////to\\\\file/.").ToString(), "/../path/to/file");
  ASSERT_EQ(Path("/../path////to\\\\file/./").ToString(), "/../path/to/file");
}

TEST_F(PathTest, testNormalizePathRawStrings)
{
  // Tests regular path
  ASSERT_STREQ(Path("/path/to/file").ToChars(), "/path/to/file");
  ASSERT_STREQ(Path("path/to/file").ToChars(), "path/to/file");
  ASSERT_STREQ(Path("/path/to/file/").ToChars(), "/path/to/file");
  ASSERT_STREQ(Path("path/to/file/").ToChars(), "path/to/file");
  // Test / and \ convertions
  ASSERT_STREQ(Path("path//to/file").ToChars(), "path/to/file");
  ASSERT_STREQ(Path("path//to\\file").ToChars(), "path/to/file");
  ASSERT_STREQ(Path("path///to\\\\file").ToChars(), "path/to/file");
  ASSERT_STREQ(Path("path////to\\\\file").ToChars(), "path/to/file");
  // Test . and .. + / and \ convertions
  ASSERT_STREQ(Path("./path////to\\\\file").ToChars(), "./path/to/file");
  ASSERT_STREQ(Path("../path////to\\\\file").ToChars(), "../path/to/file");
  ASSERT_STREQ(Path("/./path////to\\\\file").ToChars(), "/path/to/file");
  ASSERT_STREQ(Path("//.\\\\path////to\\\\file").ToChars(), "/path/to/file");
  ASSERT_STREQ(Path("/../path////to\\\\file").ToChars(), "/../path/to/file");
  ASSERT_STREQ(Path("/../path////to\\\\file/.").ToChars(), "/../path/to/file");
  ASSERT_STREQ(Path("/../path////to\\\\file/./").ToChars(), "/../path/to/file");
}

TEST_F(PathTest, testNormalizePathStdVsChars)
{
  // Tests regular path
  ASSERT_EQ(Path("/path/to/file").ToString(), Path("/path/to/file").ToChars());
  ASSERT_EQ(Path("path/to/file").ToString(), Path("path/to/file").ToChars());
  ASSERT_EQ(Path("/path/to/file/").ToString(), Path("/path/to/file").ToChars());
  ASSERT_EQ(Path("path/to/file/").ToString(), Path("path/to/file").ToChars());
  // Test / and \ convertions
  ASSERT_EQ(Path("path//to/file").ToString(), Path("path//to/file").ToChars());
  ASSERT_EQ(Path("path//to\\file").ToString(), Path("path/to/file").ToChars());
  ASSERT_EQ(Path("path///to\\\\file").ToString(), Path("path/to/file").ToChars());
  ASSERT_EQ(Path("path////to\\\\file").ToString(), Path("path/to/file").ToChars());
  // Test . and .. + / and \ convertions
  ASSERT_EQ(Path("./path////to\\\\file").ToString(), Path("./path/to/file").ToChars());
  ASSERT_EQ(Path("../path////to\\\\file").ToString(), Path("../path/to/file").ToChars());
  ASSERT_EQ(Path("/./path////to\\\\file").ToString(), Path("/./path/to/file").ToChars());
  ASSERT_EQ(Path("//.\\\\path////to\\\\file").ToString(), Path("//.\\\\path/to/file").ToChars());
  ASSERT_EQ(Path("/../path////to\\\\file").ToString(), Path("/../path/to/file").ToChars());
  ASSERT_EQ(Path("/../path////to\\\\file/.").ToString(), Path("/../path/to/file").ToChars());
  ASSERT_EQ(Path("/../path////to\\\\file/./").ToString(), Path("/../path/to/file").ToChars());
}

TEST_F(PathTest, testJoinSimplePath)
{
  // Test joining a subpath with normalization
  ASSERT_EQ(Path("/path/to/file") / "sub", "/path/to/file/sub");
  ASSERT_EQ(Path("path/to/file") / "sub", "path/to/file/sub");
  ASSERT_EQ(Path("/path/to/file") / "sub/", "/path/to/file/sub");
  ASSERT_EQ(Path("path/to/file") / "sub/", "path/to/file/sub");
  ASSERT_EQ(Path("/path/to/file") / "/sub", "/path/to/file/sub");
  ASSERT_EQ(Path("path/to/file") / "/sub", "path/to/file/sub");
  ASSERT_EQ(Path("/path/to/file") / "/sub/", "/path/to/file/sub");
  ASSERT_EQ(Path("path/to/file") / "/sub/", "path/to/file/sub");
}

TEST_F(PathTest, testJoinComplexPathPath)
{
  // Test joining a subpath with normalization
  ASSERT_EQ(Path("/path/to/file") / "to/the/infinite/and/beyond", "/path/to/file/to/the/infinite/and/beyond");
  ASSERT_EQ(Path("path/to/file") / "to/the/infinite/and/beyond", "path/to/file/to/the/infinite/and/beyond");
  ASSERT_EQ(Path("/path/to/file") / "/to/../the/infinite/and/beyond", "/path/to/file/to/../the/infinite/and/beyond");
  ASSERT_EQ(Path("path/to/file") / "/to/../the/infinite/and/beyond", "path/to/file/to/../the/infinite/and/beyond");
  ASSERT_EQ(Path("/path/to/file") / "/to/../the/infinite/./and/beyond", "/path/to/file/to/../the/infinite/and/beyond");
  ASSERT_EQ(Path("path/to/file") / "/to/../the/infinite/./and/beyond", "path/to/file/to/../the/infinite/and/beyond");
}

TEST_F(PathTest, testOperatorEqual)
{
  ASSERT_TRUE(Path("/path/to/file") == (const char*)"/path/to/file");
  ASSERT_TRUE(Path("/path/to/file") == std::string("/path/to/file"));
  ASSERT_TRUE(Path("/path/to/file") == Path("/path/to/file"));
}

TEST_F(PathTest, testOperatorNotEqual)
{
  ASSERT_TRUE(Path("/path/to/file") != (const char*)"/path/to/fall");
  ASSERT_TRUE(Path("/path/to/file") != std::string("/path/to/fall"));
  ASSERT_TRUE(Path("/path/to/file") != Path("/path/to/fall"));
}

TEST_F(PathTest, testOperatorLesserThan)
{
  ASSERT_TRUE(Path("/path/to/file") < (const char*)"/path/to/file.ext");
  ASSERT_TRUE(Path("/path/to/file") < std::string("/path/to/file.ext"));
  ASSERT_TRUE(Path("/path/to/file") < Path("/path/to/file.ext"));
  ASSERT_TRUE(Path("/path/to/file") < (const char*)"/path/to/fyle");
  ASSERT_TRUE(Path("/path/to/file") < std::string("/path/to/fyle"));
  ASSERT_TRUE(Path("/path/to/file") < Path("/path/to/fyle"));
}

TEST_F(PathTest, testOperatorGreaterThan)
{
  ASSERT_FALSE(Path("/path/to/file") > (const char*)"/path/to/file.ext");
  ASSERT_FALSE(Path("/path/to/file") > std::string("/path/to/file.ext"));
  ASSERT_FALSE(Path("/path/to/file") > Path("/path/to/file.ext"));
  ASSERT_FALSE(Path("/path/to/file") > (const char*)"/path/to/fyle");
  ASSERT_FALSE(Path("/path/to/file") > std::string("/path/to/fyle"));
  ASSERT_FALSE(Path("/path/to/file") > Path("/path/to/fyle"));
}

TEST_F(PathTest, testEmptiness)
{
  ASSERT_TRUE(Path().IsEmpty());
  ASSERT_TRUE(Path("").IsEmpty());
  ASSERT_TRUE(Path(std::string()).IsEmpty());
  ASSERT_TRUE(Path(".").IsEmpty());
  ASSERT_TRUE(Path("./").IsEmpty());
  ASSERT_FALSE(Path("/path/to/file").IsEmpty());
  ASSERT_FALSE(Path("/").IsEmpty());
  ASSERT_FALSE(Path("//").IsEmpty());
  ASSERT_FALSE(Path("/./").IsEmpty());
  ASSERT_FALSE(Path("/.").IsEmpty());
  ASSERT_FALSE(Path("..").IsEmpty());
}

TEST_F(PathTest, testConstructor)
{
  ASSERT_EQ(Path(), "");
  ASSERT_EQ(Path("/path/to/file"), "/path/to/file");
  ASSERT_EQ(Path(std::string("/path/to/file")), "/path/to/file");
}

TEST_F(PathTest, testCopyConstructor)
{
  Path path1("/path/to/file");
  Path path2(path1);
  ASSERT_EQ(path1, "/path/to/file");
  ASSERT_EQ(path2, "/path/to/file");
}

TEST_F(PathTest, testMoveConstructor)
{
  Path source("/path/to/file");
  Path destination(std::move(source));
  ASSERT_EQ(destination, "/path/to/file");
  ASSERT_TRUE(source.IsEmpty()); // Safely ignore the tidy-clang warning

  std::string source2("/path/to/file");
  Path destination2(std::move(source2));
  ASSERT_EQ(destination2, "/path/to/file");
  ASSERT_TRUE(source2.empty()); // Safely ignore the tidy-clang warning
}

TEST_F(PathTest, testRoot)
{
  ASSERT_EQ(Path::Root(), "/");
}

TEST_F(PathTest, testHome)
{
  ASSERT_EQ(Path::Home(), homeDirectory);
}

TEST_F(PathTest, testCWD)
{
  ASSERT_EQ(Path::Cwd(), currentDirectory);
}

TEST_F(PathTest, testEscapoedPath)
{
  ASSERT_EQ(Path("/path/(to)/file").MakeEscaped(), "/path/\\(to\\)/file");
  ASSERT_EQ(Path("/path/[to]/file").MakeEscaped(), "/path/\\[to\\]/file");
  ASSERT_EQ(Path("/path/{to}/file").MakeEscaped(), "/path/\\{to\\}/file");
  ASSERT_EQ(Path("/path/ to /file").MakeEscaped(), "/path/\\ to\\ /file");
  ASSERT_EQ(Path("/path/'to'/file").MakeEscaped(), "/path/\\'to\\'/file");
  ASSERT_EQ(Path("/path/\"to\"/file").MakeEscaped(), "/path/\\\"to\\\"/file");
  ASSERT_EQ(Path("/path/<to>/file").MakeEscaped(), "/path/\\<to\\>/file");
  ASSERT_EQ(Path("/path/*to?/file").MakeEscaped(), "/path/\\*to\\?/file");
  ASSERT_EQ(Path("/path/&to$/file").MakeEscaped(), "/path/\\&to\\$/file");
  ASSERT_EQ(Path("/path/!to^/file").MakeEscaped(), "/path/\\!to\\^/file");
  ASSERT_EQ(Path("/path/to;/file").MakeEscaped(), "/path/to\\;/file");
  ASSERT_EQ(Path("/p a t h/(to)/[a]/<file>/'with'/w!e$i^r&d*/chars?/\"in\"/the;{name}").MakeEscaped(),
                 "/p\\ a\\ t\\ h/\\(to\\)/\\[a\\]/\\<file\\>/\\'with\\'/w\\!e\\$i\\^r\\&d\\*/chars\\?/\\\"in\\\"/the\\;\\{name\\}");
}

TEST_F(PathTest, testStartWidth)
{
  ASSERT_TRUE(Path("/path/to/file").StartWidth(Path("/path")));
  ASSERT_TRUE(Path("/path/to/file").StartWidth("/path"));
  ASSERT_TRUE(Path("/path/to/file").StartWidth(Path("/path/to")));
  ASSERT_TRUE(Path("/path/to/file").StartWidth("/path/to"));
  ASSERT_TRUE(Path("/path/to/file").StartWidth(Path("/path/to/file")));
  ASSERT_TRUE(Path("/path/to/file").StartWidth("/path/to/file"));
  ASSERT_FALSE(Path("/path/to/file").StartWidth(Path("/path/to/file/over")));
  ASSERT_FALSE(Path("/path/to/file").StartWidth("/path/to/file/over"));
  ASSERT_FALSE(Path("/path/to/file").StartWidth(Path("/path/to/fil")));
  ASSERT_FALSE(Path("/path/to/file").StartWidth("/path/to/fil"));
  ASSERT_FALSE(Path("/path/to/file").StartWidth(Path("/pa")));
  ASSERT_FALSE(Path("/path/to/file").StartWidth("/pa"));
  ASSERT_FALSE(Path("path/to/file").StartWidth("/path/to/file"));
  ASSERT_FALSE(Path("/path/to/file").StartWidth("path/to/file"));
}

TEST_F(PathTest, testItemCount)
{
  ASSERT_EQ(Path().ItemCount(), 1);
  ASSERT_EQ(Path::Root().ItemCount(), 1);
  ASSERT_EQ(Path("/path").ItemCount(), 1);
  ASSERT_EQ(Path("path").ItemCount(), 1);
  ASSERT_EQ(Path("/path/").ItemCount(), 1);
  ASSERT_EQ(Path("/path/to/file").ItemCount(), 3);
  ASSERT_EQ(Path("path/to/file").ItemCount(), 3);
  ASSERT_EQ(Path("~/path/to/file").ItemCount(), 4);
  ASSERT_EQ(Path("~/path/to/../file").ItemCount(), 5);
}

TEST_F(PathTest, testItems)
{
  ASSERT_EQ(Path().Item(0), "");
  ASSERT_EQ(Path().Item(1), "");
  ASSERT_EQ(Path().Item(-1), "");
  ASSERT_EQ(Path().Item(1000), "");
  ASSERT_EQ(Path::Root().Item(0), "");
  ASSERT_EQ(Path("/path").Item(0), "path");
  ASSERT_EQ(Path("/path").Item(1), "");
  ASSERT_EQ(Path("path").Item(0), "path");
  ASSERT_EQ(Path("/path/").Item(0), "path");
  ASSERT_EQ(Path("/path/").Item(1), "");
  ASSERT_EQ(Path("/path/to/file").Item(0), "path");
  ASSERT_EQ(Path("/path/to/file").Item(1), "to");
  ASSERT_EQ(Path("/path/to/file").Item(2), "file");
  ASSERT_EQ(Path("/path/to/file").Item(3), "");
  ASSERT_EQ(Path("path/to/file").Item(0), "path");
  ASSERT_EQ(Path("path/to/file").Item(1), "to");
  ASSERT_EQ(Path("path/to/file").Item(2), "file");
  ASSERT_EQ(Path("path/to/file").Item(3), "");
  ASSERT_EQ(Path("~/path/to/../file").Item(0), "~");
  ASSERT_EQ(Path("~/path/to/../file").Item(1), "path");
  ASSERT_EQ(Path("~/path/to/../file").Item(2), "to");
  ASSERT_EQ(Path("~/path/to/../file").Item(3), "..");
  ASSERT_EQ(Path("~/path/to/../file").Item(4), "file");
  ASSERT_EQ(Path("~/path/to/../file").Item(5), "");
}

TEST_F(PathTest, testUptoItems)
{
  ASSERT_EQ(Path().UptoItem(0), "");
  ASSERT_EQ(Path().UptoItem(1), "");
  ASSERT_EQ(Path().UptoItem(-1), "");
  ASSERT_EQ(Path().UptoItem(1000), "");
  ASSERT_EQ(Path::Root().UptoItem(0), "/");
  ASSERT_EQ(Path("/path").UptoItem(0), "/path");
  ASSERT_EQ(Path("/path").UptoItem(1), "/path");
  ASSERT_EQ(Path("path").UptoItem(0), "path");
  ASSERT_EQ(Path("/path/").UptoItem(0), "/path");
  ASSERT_EQ(Path("/path/").UptoItem(1), "/path");
  ASSERT_EQ(Path("/path/to/file").UptoItem(0), "/path");
  ASSERT_EQ(Path("/path/to/file").UptoItem(1), "/path/to");
  ASSERT_EQ(Path("/path/to/file").UptoItem(2), "/path/to/file");
  ASSERT_EQ(Path("/path/to/file").UptoItem(3), "/path/to/file");
  ASSERT_EQ(Path("path/to/file").UptoItem(0), "path");
  ASSERT_EQ(Path("path/to/file").UptoItem(1), "path/to");
  ASSERT_EQ(Path("path/to/file").UptoItem(2), "path/to/file");
  ASSERT_EQ(Path("path/to/file").UptoItem(3), "path/to/file");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(0), "~");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(1), "~/path");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(2), "~/path/to");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(3), "~/path/to/..");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(4), "~/path/to/../file");
  ASSERT_EQ(Path("~/path/to/../file").UptoItem(5), "~/path/to/../file");
}

TEST_F(PathTest, testFromItems)
{
  ASSERT_EQ(Path().FromItem(0), "");
  ASSERT_EQ(Path().FromItem(1), "");
  ASSERT_EQ(Path().FromItem(-1), "");
  ASSERT_EQ(Path().FromItem(1000), "");
  ASSERT_EQ(Path::Root().FromItem(0), "");
  ASSERT_EQ(Path("/path").FromItem(0), "path");
  ASSERT_EQ(Path("/path").FromItem(1), "");
  ASSERT_EQ(Path("path").FromItem(0), "path");
  ASSERT_EQ(Path("/path/").FromItem(0), "path");
  ASSERT_EQ(Path("/path/").FromItem(1), "");
  ASSERT_EQ(Path("/path/to/file").FromItem(0), "path/to/file");
  ASSERT_EQ(Path("/path/to/file").FromItem(1), "to/file");
  ASSERT_EQ(Path("/path/to/file").FromItem(2), "file");
  ASSERT_EQ(Path("/path/to/file").FromItem(3), "");
  ASSERT_EQ(Path("path/to/file").FromItem(0), "path/to/file");
  ASSERT_EQ(Path("path/to/file").FromItem(1), "to/file");
  ASSERT_EQ(Path("path/to/file").FromItem(2), "file");
  ASSERT_EQ(Path("path/to/file").FromItem(3), "");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(0), "~/path/to/../file");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(1), "path/to/../file");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(2), "to/../file");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(3), "../file");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(4), "file");
  ASSERT_EQ(Path("~/path/to/../file").FromItem(5), "");
}

TEST_F(PathTest, testDirectory)
{
  ASSERT_EQ(Path().Directory(), "");
  ASSERT_EQ(Path::Root().Directory(), "");
  ASSERT_EQ(Path(".").Directory(), "");
  ASSERT_EQ(Path("..").Directory(), "");
  ASSERT_EQ(Path("/path").Directory(), "");
  ASSERT_EQ(Path("path").Directory(), "");
  ASSERT_EQ(Path("/path/").Directory(), "");
  ASSERT_EQ(Path("~/path/").Directory(), "~");
  ASSERT_EQ(Path("/path/to/file").Directory(), "/path/to");
  ASSERT_EQ(Path("path/to/file").Directory(), "path/to");
  ASSERT_EQ(Path("~/path/to/../file").Directory(), "~/path/to/..");
  ASSERT_EQ(Path("~/path/to/..").Directory(), "~/path");
}

TEST_F(PathTest, testFilename)
{
  ASSERT_EQ(Path().Filename(), "");
  ASSERT_EQ(Path::Root().Filename(), "");
  ASSERT_EQ(Path(".").Filename(), "");
  ASSERT_EQ(Path("..").Filename(), "..");
  ASSERT_EQ(Path("/path").Filename(), "path");
  ASSERT_EQ(Path("path").Filename(), "path");
  ASSERT_EQ(Path("/path/").Filename(), "path");
  ASSERT_EQ(Path("~/path/").Filename(), "path");
  ASSERT_EQ(Path("/path/to/file").Filename(), "file");
  ASSERT_EQ(Path("path/to/file").Filename(), "file");
  ASSERT_EQ(Path("~/path/to/../file").Filename(), "file");
  ASSERT_EQ(Path("~/path/to/..").Filename(), "..");
}

TEST_F(PathTest, testFilenameWithoutExtension)
{
  // Empty
  ASSERT_EQ(Path().FilenameWithoutExtension(), "");
  ASSERT_EQ(Path::Root().FilenameWithoutExtension(), "");
  ASSERT_EQ(Path(".").FilenameWithoutExtension(), "");
  ASSERT_EQ(Path("..").FilenameWithoutExtension(), "..");
  // No ext
  ASSERT_EQ(Path("/path").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("path").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("/path/").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("~/path/").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("/path/to/file").FilenameWithoutExtension(), "file");
  ASSERT_EQ(Path("path/to/file").FilenameWithoutExtension(), "file");
  ASSERT_EQ(Path("~/path/to/../file").FilenameWithoutExtension(), "file");
  ASSERT_EQ(Path("~/path/to/..").FilenameWithoutExtension(), "..");
  // Ext
  ASSERT_EQ(Path("/path.ext").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("path.ext").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("/path.ext/").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("~/path.ext/").FilenameWithoutExtension(), "path");
  ASSERT_EQ(Path("/path/to/file.ext").FilenameWithoutExtension(), "file");
  ASSERT_EQ(Path("path/to/file.ext").FilenameWithoutExtension(), "file");
  ASSERT_EQ(Path("~/path/to/../file.ext").FilenameWithoutExtension(), "file");
  // False ext
  ASSERT_EQ(Path("~/path/to/...falseext").FilenameWithoutExtension(), "...falseext");
  ASSERT_EQ(Path("~/path/to/..falseext").FilenameWithoutExtension(), "..falseext");
  ASSERT_EQ(Path("~/path/to/.falseext").FilenameWithoutExtension(), ".falseext");
  // Double Ext
  ASSERT_EQ(Path("/path.no.ext").FilenameWithoutExtension(), "path.no");
  ASSERT_EQ(Path("path.no.ext").FilenameWithoutExtension(), "path.no");
  ASSERT_EQ(Path("/path.no.ext/").FilenameWithoutExtension(), "path.no");
  ASSERT_EQ(Path("~/path.no.ext/").FilenameWithoutExtension(), "path.no");
  ASSERT_EQ(Path("/path/to/file.no.ext").FilenameWithoutExtension(), "file.no");
  ASSERT_EQ(Path("path/to/file.no.ext").FilenameWithoutExtension(), "file.no");
  ASSERT_EQ(Path("~/path/to/../file.no.ext").FilenameWithoutExtension(), "file.no");
  // False ext + ext
  ASSERT_EQ(Path("~/path/to/...no.falseext").FilenameWithoutExtension(), "...no");
  ASSERT_EQ(Path("~/path/to/..no.falseext").FilenameWithoutExtension(), "..no");
  ASSERT_EQ(Path("~/path/to/.no.falseext").FilenameWithoutExtension(), ".no");
}

TEST_F(PathTest, testExtension)
{
  // Empty
  ASSERT_EQ(Path().Extension(), "");
  ASSERT_EQ(Path::Root().Extension(), "");
  ASSERT_EQ(Path(".").Extension(), "");
  ASSERT_EQ(Path("..").Extension(), "");
  // No ext
  ASSERT_EQ(Path("/path").Extension(), "");
  ASSERT_EQ(Path("path").Extension(), "");
  ASSERT_EQ(Path("/path/").Extension(), "");
  ASSERT_EQ(Path("~/path/").Extension(), "");
  ASSERT_EQ(Path("/path/to/file").Extension(), "");
  ASSERT_EQ(Path("path/to/file").Extension(), "");
  ASSERT_EQ(Path("~/path/to/../file").Extension(), "");
  ASSERT_EQ(Path("~/path/to/..").Extension(), "");
  // Ext
  ASSERT_EQ(Path("/path.ext").Extension(), ".ext");
  ASSERT_EQ(Path("path.ext").Extension(), ".ext");
  ASSERT_EQ(Path("/path.ext/").Extension(), ".ext");
  ASSERT_EQ(Path("~/path.ext/").Extension(), ".ext");
  ASSERT_EQ(Path("/path/to/file.ext").Extension(), ".ext");
  ASSERT_EQ(Path("path/to/file.ext").Extension(), ".ext");
  ASSERT_EQ(Path("~/path/to/../file.ext").Extension(), ".ext");
  // False ext
  ASSERT_EQ(Path("~/path/to/...falseext").Extension(), "");
  ASSERT_EQ(Path("~/path/to/..falseext").Extension(), "");
  ASSERT_EQ(Path("~/path/to/.falseext").Extension(), "");
  // Double Ext
  ASSERT_EQ(Path("/path.no.ext").Extension(), ".ext");
  ASSERT_EQ(Path("path.no.ext").Extension(), ".ext");
  ASSERT_EQ(Path("/path.no.ext/").Extension(), ".ext");
  ASSERT_EQ(Path("~/path.no.ext/").Extension(), ".ext");
  ASSERT_EQ(Path("/path/to/file.no.ext").Extension(), ".ext");
  ASSERT_EQ(Path("path/to/file.no.ext").Extension(), ".ext");
  ASSERT_EQ(Path("~/path/to/../file.no.ext").Extension(), ".ext");
  // False ext + ext
  ASSERT_EQ(Path("~/path/to/...no.falseext").Extension(), ".falseext");
  ASSERT_EQ(Path("~/path/to/..no.falseext").Extension(), ".falseext");
  ASSERT_EQ(Path("~/path/to/.no.falseext").Extension(), ".falseext");
}

TEST_F(PathTest, testChangeExtension)
{
  // Change
  
  // Empty
  ASSERT_EQ(Path().ChangeExtension(".new"), ".new");
  ASSERT_EQ(Path::Root().ChangeExtension(".new"), "/.new");
  ASSERT_EQ(Path(".").ChangeExtension(".new"), ".new");
  ASSERT_EQ(Path("..").ChangeExtension(".new"), "...new");
  // No ext
  ASSERT_EQ(Path("/path").ChangeExtension(".new"), "/path.new");
  ASSERT_EQ(Path("path").ChangeExtension(".new"), "path.new");
  ASSERT_EQ(Path("/path/").ChangeExtension(".new"), "/path.new");
  ASSERT_EQ(Path("~/path/").ChangeExtension(".new"), "~/path.new");
  ASSERT_EQ(Path("/path/to/file").ChangeExtension(".new"), "/path/to/file.new");
  ASSERT_EQ(Path("path/to/file").ChangeExtension(".new"), "path/to/file.new");
  ASSERT_EQ(Path("~/path/to/../file").ChangeExtension(".new"), "~/path/to/../file.new");
  ASSERT_EQ(Path("~/path/to/..").ChangeExtension(".new"), "~/path/to/...new");
  // Ext
  ASSERT_EQ(Path("/path.ext").ChangeExtension(".new"), "/path.new");
  ASSERT_EQ(Path("path.ext").ChangeExtension(".new"), "path.new");
  ASSERT_EQ(Path("/path.ext/").ChangeExtension(".new"), "/path.new");
  ASSERT_EQ(Path("~/path.ext/").ChangeExtension(".new"), "~/path.new");
  ASSERT_EQ(Path("/path/to/file.ext").ChangeExtension(".new"), "/path/to/file.new");
  ASSERT_EQ(Path("path/to/file.ext").ChangeExtension(".new"), "path/to/file.new");
  ASSERT_EQ(Path("~/path/to/../file.ext").ChangeExtension(".new"), "~/path/to/../file.new");
  // False ext
  ASSERT_EQ(Path("~/path/to/...falseext").ChangeExtension(".new"), "~/path/to/...falseext.new");
  ASSERT_EQ(Path("~/path/to/..falseext").ChangeExtension(".new"), "~/path/to/..falseext.new");
  ASSERT_EQ(Path("~/path/to/.falseext").ChangeExtension(".new"), "~/path/to/.falseext.new");
  // Double Ext
  ASSERT_EQ(Path("/path.no.ext").ChangeExtension(".new"), "/path.no.new");
  ASSERT_EQ(Path("path.no.ext").ChangeExtension(".new"), "path.no.new");
  ASSERT_EQ(Path("/path.no.ext/").ChangeExtension(".new"), "/path.no.new");
  ASSERT_EQ(Path("~/path.no.ext/").ChangeExtension(".new"), "~/path.no.new");
  ASSERT_EQ(Path("/path/to/file.no.ext").ChangeExtension(".new"), "/path/to/file.no.new");
  ASSERT_EQ(Path("path/to/file.no.ext").ChangeExtension(".new"), "path/to/file.no.new");
  ASSERT_EQ(Path("~/path/to/../file.no.ext").ChangeExtension(".new"), "~/path/to/../file.no.new");
  // False ext + ext
  ASSERT_EQ(Path("~/path/to/...no.falseext").ChangeExtension(".new"), "~/path/to/...no.new");
  ASSERT_EQ(Path("~/path/to/..no.falseext").ChangeExtension(".new"), "~/path/to/..no.new");
  ASSERT_EQ(Path("~/path/to/.no.falseext").ChangeExtension(".new"), "~/path/to/.no.new");

  // Remove
  
  // Empty
  ASSERT_EQ(Path().ChangeExtension(""), "");
  ASSERT_EQ(Path::Root().ChangeExtension(""), "/");
  ASSERT_EQ(Path(".").ChangeExtension(""), "");
  ASSERT_EQ(Path("..").ChangeExtension(""), "..");
  // No ext
  ASSERT_EQ(Path("/path").ChangeExtension(""), "/path");
  ASSERT_EQ(Path("path").ChangeExtension(""), "path");
  ASSERT_EQ(Path("/path/").ChangeExtension(""), "/path");
  ASSERT_EQ(Path("~/path/").ChangeExtension(""), "~/path");
  ASSERT_EQ(Path("/path/to/file").ChangeExtension(""), "/path/to/file");
  ASSERT_EQ(Path("path/to/file").ChangeExtension(""), "path/to/file");
  ASSERT_EQ(Path("~/path/to/../file").ChangeExtension(""), "~/path/to/../file");
  ASSERT_EQ(Path("~/path/to/..").ChangeExtension(""), "~/path/to/..");
  // Ext
  ASSERT_EQ(Path("/path.ext").ChangeExtension(""), "/path");
  ASSERT_EQ(Path("path.ext").ChangeExtension(""), "path");
  ASSERT_EQ(Path("/path.ext/").ChangeExtension(""), "/path");
  ASSERT_EQ(Path("~/path.ext/").ChangeExtension(""), "~/path");
  ASSERT_EQ(Path("/path/to/file.ext").ChangeExtension(""), "/path/to/file");
  ASSERT_EQ(Path("path/to/file.ext").ChangeExtension(""), "path/to/file");
  ASSERT_EQ(Path("~/path/to/../file.ext").ChangeExtension(""), "~/path/to/../file");
  // False ext
  ASSERT_EQ(Path("~/path/to/...falseext").ChangeExtension(""), "~/path/to/...falseext");
  ASSERT_EQ(Path("~/path/to/..falseext").ChangeExtension(""), "~/path/to/..falseext");
  ASSERT_EQ(Path("~/path/to/.falseext").ChangeExtension(""), "~/path/to/.falseext");
  // Double Ext
  ASSERT_EQ(Path("/path.no.ext").ChangeExtension(""), "/path.no");
  ASSERT_EQ(Path("path.no.ext").ChangeExtension(""), "path.no");
  ASSERT_EQ(Path("/path.no.ext/").ChangeExtension(""), "/path.no");
  ASSERT_EQ(Path("~/path.no.ext/").ChangeExtension(""), "~/path.no");
  ASSERT_EQ(Path("/path/to/file.no.ext").ChangeExtension(""), "/path/to/file.no");
  ASSERT_EQ(Path("path/to/file.no.ext").ChangeExtension(""), "path/to/file.no");
  ASSERT_EQ(Path("~/path/to/../file.no.ext").ChangeExtension(""), "~/path/to/../file.no");
  // False ext + ext
  ASSERT_EQ(Path("~/path/to/...no.falseext").ChangeExtension(""), "~/path/to/...no");
  ASSERT_EQ(Path("~/path/to/..no.falseext").ChangeExtension(""), "~/path/to/..no");
  ASSERT_EQ(Path("~/path/to/.no.falseext").ChangeExtension(""), "~/path/to/.no");
}

TEST_F(PathTest, testExists)
{
  // Not a file
  ASSERT_FALSE(Path("/this/path/does/not/exists").Exists());
  // Existing folder
  ASSERT_TRUE(Path(TestSet.Folder1.name).Exists());
  // Existing file
  ASSERT_TRUE(Path(TestSet.Folder1.file1).Exists());
}

TEST_F(PathTest, testIsFile)
{
  // Not a file
  ASSERT_FALSE(Path("/this/path/does/not/exists").IsFile());
  // Existing folder
  ASSERT_FALSE(Path(TestSet.Folder1.name).IsFile());
  // Existing file
  ASSERT_TRUE(Path(TestSet.Folder1.file1).IsFile());
}

TEST_F(PathTest, testIsDirectory)
{
  // Not a file
  ASSERT_FALSE(Path("/this/path/does/not/exists").IsDirectory());
  // Existing folder
  ASSERT_TRUE(Path(TestSet.Folder1.name).IsDirectory());
  // Existing file
  ASSERT_FALSE(Path(TestSet.Folder1.file1).IsDirectory());
}

TEST_F(PathTest, testIsSymLink)
{
  // Not a file
  ASSERT_FALSE(Path("/this/path/does/not/exists").IsSymLink());
  // Existing folder
  ASSERT_FALSE(Path(TestSet.Folder1.name).IsSymLink());
  ASSERT_TRUE(Path(TestSet.Folder1.Folder2.sldir).IsSymLink());
  // Existing file
  ASSERT_FALSE(Path(TestSet.Folder1.file1).IsSymLink());
  ASSERT_TRUE(Path(TestSet.Folder1.Folder2.Folder3.slfile).IsSymLink());
}

TEST_F(PathTest, testIsHidden)
{
  ASSERT_FALSE(Path(".").IsHidden());
  ASSERT_FALSE(Path("..").IsHidden());
  ASSERT_FALSE(Path("/path/to/.").IsHidden());
  ASSERT_FALSE(Path("/path/to/..").IsHidden());
  ASSERT_FALSE(Path("/path/to/file").IsHidden());
  ASSERT_FALSE(Path("/path/.to/file").IsHidden());
  ASSERT_FALSE(Path(".path/.to/file").IsHidden());
  ASSERT_TRUE(Path(".file").IsHidden());
  ASSERT_TRUE(Path("..file").IsHidden());
  ASSERT_TRUE(Path("/path/to/.file").IsHidden());
  ASSERT_TRUE(Path("/path/to/..file").IsHidden());
}

TEST_F(PathTest, testIsAbsolute)
{
  ASSERT_FALSE(Path(".").IsAbsolute());
  ASSERT_FALSE(Path("..").IsAbsolute());
  ASSERT_FALSE(Path("~/path/to/file").IsAbsolute());
  ASSERT_FALSE(Path("path/to/file").IsAbsolute());
  ASSERT_TRUE(Path("/path/to/file").IsAbsolute());
  ASSERT_TRUE(Path::Root().IsAbsolute());
}

TEST_F(PathTest, testToAbsolute)
{
  // Internal resources
  ASSERT_EQ(Path(":/resource.png").ToAbsolute(), ":/resource.png");
  // Regular path
  ASSERT_EQ(Path().ToAbsolute(), "");
  ASSERT_EQ(Path("/path/to/file").ToAbsolute(), "/path/to/file");
  ASSERT_EQ(Path("~/path/to/file").ToAbsolute(), homeDirectory + "/path/to/file");
  ASSERT_EQ(Path("path/to/file").ToAbsolute(), currentDirectory + "/path/to/file");
}

TEST_F(PathTest, testToAbsoluteWithBase)
{
  Path base("/tmp");
  // Internal resources
  ASSERT_EQ(Path(":/resource.png").ToAbsolute(base), ":/resource.png");
  // Regular path
  ASSERT_EQ(Path().ToAbsolute(), "");
  ASSERT_EQ(Path("/path/to/file").ToAbsolute(base), "/path/to/file");
  ASSERT_EQ(Path("~/path/to/file").ToAbsolute(base), homeDirectory + "/path/to/file");
  ASSERT_EQ(Path("path/to/file").ToAbsolute(base), "/tmp/path/to/file");
}

TEST_F(PathTest, testCreatePath)
{
  Path folder(TestSet.Folder1.Folder2.Folder3.name + "/newfolder1/newfolder2/newfolder3");
  // Ensure it does not exists
  ASSERT_FALSE(folder.Exists());
  // Create once and test
  ASSERT_TRUE(folder.CreatePath());
  ASSERT_TRUE(folder.Exists());
  // Create while it already exists
  ASSERT_TRUE(folder.CreatePath());
  ASSERT_TRUE(folder.Exists());
}

TEST_F(PathTest, testDelete)
{
  Path file(TestSet.Folder1.Folder2.Folder3.name + "/filetodetete.txt");
  // Ensure it does not exists
  ASSERT_FALSE(file.Exists());
  // Create file and check
  ASSERT_EQ(system(("echo text > " + file.ToString()).c_str()), 0);
  ASSERT_TRUE(file.Exists());
  // Delete and check
  ASSERT_TRUE(file.Delete());
  ASSERT_FALSE(file.Exists());
}

TEST_F(PathTest, testToCanonical)
{
  // Internal resources
  ASSERT_EQ(Path(":/resource.png").ToCanonical(), ":/resource.png");
  // Regular path
  ASSERT_EQ(Path().ToCanonical(), "");
  ASSERT_EQ(Path("/path/to/file").ToCanonical(), "/path/to/file");
  ASSERT_EQ(Path("~/path/to/file").ToCanonical(), homeDirectory + "/path/to/file");
  ASSERT_EQ(Path("path/to/file").ToCanonical(), currentDirectory + "/path/to/file");
  // Symlinks
  ASSERT_EQ(Path(TestSet.Folder1.Folder2.sldir + "/file1").ToCanonical(), TestSet.Folder1.file1);
  ASSERT_EQ(Path(TestSet.Folder1.Folder2.Folder3.slfile).ToCanonical(), TestSet.Folder1.Folder2.file2);
}

TEST_F(PathTest, testGetDirectoryContent)
{
  // Not an existing dir
  Path::PathList emptylist = Path("/path/to/file").GetDirectoryContent();
  ASSERT_EQ(emptylist.size(), 0u);
  // Existing file
  Path::PathList file = Path(TestSet.Folder1.file1).GetDirectoryContent();
  ASSERT_EQ(file.size(), 0u);
  // Existing folder
  Path path(TestSet.Folder1.name);
  Path::PathList list = path.GetDirectoryContent();
  std::sort(list.begin(), list.end());
  ASSERT_EQ(list.size(), 2u);
  ASSERT_EQ(list[0].ToString(), TestSet.Folder1.file1);
  ASSERT_EQ(list[1].ToString(), TestSet.Folder1.Folder2.name);
}

TEST_F(PathTest, testMakeRelative)
{
  bool ok;
  // Nok
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path::Empty, ok), "/path/to/file");
  ASSERT_FALSE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("path/to"), ok), "/path/to/file");
  ASSERT_FALSE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path/to/nope"), ok), "/path/to/file");
  ASSERT_FALSE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path/to/fi"), ok), "/path/to/file");
  ASSERT_FALSE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path/t"), ok), "/path/to/file");
  ASSERT_FALSE(ok);
  // Ok
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path"), ok), "to/file");
  ASSERT_TRUE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path/to"), ok), "file");
  ASSERT_TRUE(ok);
  ASSERT_EQ(Path("/path/to/file").MakeRelative(Path("/path/to/file"), ok), "");
  ASSERT_TRUE(ok);
  // Ok, existing dir/files
  ASSERT_EQ(Path(TestSet.Folder1.Folder2.Folder3.name).MakeRelative(Path(TestSet.Folder1.name), ok).ToString(), "folder2/folder3");
  ASSERT_TRUE(ok);
  ASSERT_EQ(Path(TestSet.Folder1.Folder2.Folder3.name).MakeRelative(Path(TestSet.Folder1.file1), ok).ToString(), "folder2/folder3");
  ASSERT_TRUE(ok);
}

TEST_F(PathTest, testFileSize)
{
  ASSERT_EQ(Path("/path/to/file").Size(), 0);
  ASSERT_EQ(Path(TestSet.Folder1.Folder2.Folder3.file3).Size(), 5); // text + \n
  ASSERT_NE(Path(TestSet.Folder1.Folder2.Folder3.name).Size(), 0);
}