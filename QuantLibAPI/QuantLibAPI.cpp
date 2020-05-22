#include <iostream>
#include <windows.h>
#include "Service.h"

int main()
{
    std::string localhost = "127.0.0.1";
    std::string port = "8888";
    std::string endpoint = "/api/v1/";
    
    Service serv(utility::conversions::to_string_t(localhost), utility::conversions::to_string_t(port));
    serv.setEndpoint(utility::conversions::to_string_t(endpoint));
    serv.accept().wait();

    std::cout << "Server start at http://" + localhost + ":" + port << std::endl;

    system("pause");
    return 0;
}


