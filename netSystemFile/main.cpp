// netSystemFile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "iniFileMan.h"
#include <iostream>

int main()
{
    IniFileMan ini;

    ini.ReadIni("C:\\Users\\인스유틸\\source\\repos\\CSRealtesting\\Debug\\appsetting.txt");

    std::string ip, port;
    ini.GetItemValue("Network", "ip_address", ip);
    ini.GetItemValue("Network", "port_number", port);

    std::cout << ip << std::endl;
    std::cout << port << std::endl;

    return 0;
}

