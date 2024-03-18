#include "Cgi.hpp"

int main()
{
    try{
        Cgi test;
        write(test.getFdWrite(), "test&test", 9);
        test.launchCgi("./test.php");
        wait(0);
        char c;
        std::ofstream t("outest");
        while ((read(test.getFdRead(), &c, 1)) > 0) {
            t<<c;
        }
    }
    catch (const std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
    }
}