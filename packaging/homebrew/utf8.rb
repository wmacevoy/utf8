class Utf8 < Formula
  desc "Small permissive UTF-8 encode/decode library"
  homepage "https://github.com/wmacevoy/utf8"
  url "https://github.com/wmacevoy/utf8/archive/refs/tags/v1.0.0.tar.gz"
  # Update sha256 after creating the release tarball:
  #   shasum -a 256 v1.0.0.tar.gz
  sha256 "PLACEHOLDER"
  license "Unlicense"

  depends_on "cmake" => :build

  def install
    system "cmake", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    (testpath/"test.c").write <<~C
      #include <utf8.h>
      #include <assert.h>
      int main() {
        assert(utf8enclen(0x41) == 1);
        assert(utf8enclen(0x80) == 2);
        assert(utf8enclen(0x800) == 3);
        assert(utf8enclen(0x10000) == 4);
        assert(utf8enclen(0xFFFFFFFF) == 0);
        return 0;
      }
    C
    system ENV.cc, "test.c", "-I#{include}", "-L#{lib}", "-lutf8", "-o", "test"
    system "./test"
  end
end
