#include <iostream>
#include <vector>
#include <algorithm>

#include <sys/stat.h>

#include "file_io.hpp"

bool is_sparse(const char *buf, size_t size)
{
    auto*  lbuf  = reinterpret_cast<const long*>(buf);
    size_t lsize = size / sizeof(long);

    auto sparse = std::all_of(lbuf, lbuf + lsize, [](long i){ return i == 0; });
    if (!sparse) return false;

    auto rest = size % sizeof(long);
    if (!rest) return true;

    auto* end   = buf + size;
    auto* begin = end - rest;
    return std::all_of(begin, end, [](char b){ return b == 0; });
}

void do_copy(sys::File_des src, sys::File_des dst)
{
    struct ::stat st{};
    if (::fstat(src.get(), &st) == -1) throw sys::Error{};
    std::cout << "Block size: " << st.st_blksize << "\n";

    size_t blocks = st.st_size / st.st_blksize
                  + (st.st_size % st.st_blksize ? 1 : 0);
    std::vector<char> buf(st.st_blksize);
    size_t scnt = 0;

    for (size_t c = 0;;) {
        auto res = sys::read(src, buf.data(), buf.size());
        if (res == 0) break;

        if (is_sparse(buf.data(), res)) {
            sys::lseek(dst, res, SEEK_CUR);
            ++scnt;
        } else {
            sys::write(dst, buf.data(), res);
        }
        std::cout << "\rCopied: " << ++c << ":" << scnt << "/" << blocks;

        if (res < buf.size()) break;
    }

    ::ftruncate(dst.get(), st.st_size);
    std::cout << "\n";
}

int main(int argc, const char *argv[])
{
    auto res = 0;

    if (argc < 3) {
        std::cerr << "Usage: sparsecopy <source> <destination>\n";
        return 0;
    }

    try {
        auto src = sys::open(argv[1], O_RDONLY);
        auto dst = sys::creat(argv[2], 0644);

        do_copy(src, dst);

        sys::close(src);
        sys::close(dst);
    } catch (sys::Open_error &e) {
        std::cerr << "Cannot open " << argv[1] << "\n"
                  << e.what() << "\n";
        res = 1;
    } catch (sys::Creat_error &e) {
        std::cerr << "Cannot create " << argv[2] << "\n"
                  << e.what() << "\n";
        res = 2;
    } catch (sys::Close_error &e) {
        std::cerr << "Error occured while closing a file " << e.what() << "\n";
        res = 3;
    }

    return res;
}
