#include <string>
#include <string.h>
using namespace std;
class server{
public:
    int coreNum[2], ram[2], costBuy, costDay;
    server(int _coreNum, int _ram, int _costBuy, int _costDay){
        coreNum[0]=coreNum[1]=_coreNum; ram[0]=ram[1]=_ram; costBuy=_costBuy; costDay=_costDay;
    }
};

class VM{
public:
    int coreNum, ram;
    bool doubleNode;
    VM(int _coreNum, int _ram, bool _doubleNode){
        coreNum=_coreNum; ram=_ram; doubleNode=_doubleNode;
    }
};

class request{
public:
    bool reType;
    string VMType,VMID;
    request(bool _reType, string _VMType, string _VMID){
        reType=_reType; VMType=_VMType; VMID=_VMID;
    }
};
