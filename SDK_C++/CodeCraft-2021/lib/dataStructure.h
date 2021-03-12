#include <string>
#include <string.h>
using namespace std;
class server{
public:
    string serverType;
    int coreNum[2], ram[2], costBuy, costDay;
    server(string _serverType, int _coreNum, int _ram, int _costBuy, int _costDay){
        serverType=_serverType;coreNum[0]=coreNum[1]=_coreNum; ram[0]=ram[1]=_ram; costBuy=_costBuy; costDay=_costDay;
    }
    bool operator<(const server other){//重载<运算符
        if(this->costBuy!=other.costBuy)return this->costBuy<other.costBuy;
        return this->costDay<other.costDay;
    }
};

class VM{
public:
    int coreNum, ram;
    bool doubleNode;
    string VMType;
    VM(string _VMType, int _coreNum, int _ram, bool _doubleNode){
        VMType=_VMType;coreNum=_coreNum; ram=_ram; doubleNode=_doubleNode;
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

class serverNode{
public:
    string serverType;
    int ID;
    bool A;
    int powerCost;
    serverNode(string _serverType, int _ID,bool _A,int _powerCost){
        serverType=_serverType;ID=_ID;A=_A;powerCost=_powerCost;
    }
    bool operator<(const serverNode other){//重载<运算符
        if(this->ID==other.ID)return false;
        return this->powerCost<other.powerCost;
    }
};

class VMNode{
    int hostID;
    bool A,B;
    VMNode(int _hostID, bool _A, bool _B){
        hostID=_hostID;
        A=_A;
        B=_B;
    }
};
