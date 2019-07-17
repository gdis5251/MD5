#include "MD5.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << argv[0] << "\tfilename" << std::endl;
        return 1;
    }
    
    MD5 md5;
    int option = *argv[1];
    switch(option)
    {
    case '0':
        std::cout << md5.getFileMD5(argv[2]) << std::endl;
        break;
    case '1':
        std::cout << md5.getStringMD5(argv[2]) << std::endl;
    default:
        break;
    }

    return 0;
}
