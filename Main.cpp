#include "MD5.hpp"
//   0     1  2
// ./md5  -s str1 str2
// -s字符串md5   -f 文件名计算md5
int main(int argc, char *argv[]) {
    if (argc < 2) {
        return help();
    }

    std::string opt = argv[1];
    if (opt == "-s") {
        for (int i = 2; i < argc; ++i) {
            MD5 m;
            m.StrMD5(argv[i]);
            std::cout << m.getMD5() << std::endl;
        }

    } else if (opt == "-f") {
        for (int i = 2; i < argc; ++i) {
            MD5 m;
            m.FileMD5(argv[i]);
            std::cout << m.getMD5() << std::endl;
        }
    } else {
        return help();
    }

    return 0;
}
