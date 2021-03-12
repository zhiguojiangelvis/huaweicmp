#include "dataStructure.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>
using namespace std;
class run{
private:
    string filePath;
    //服务器和虚拟机的所有型号
    unordered_map< string, server* > serverWarehouse;
    unordered_map< string, VM* > VMWarehouse;
    vector<server*> serverPossessed;
    unordered_map<int, string > VMPossessed;
    vector<request*> requestWarehouse;
    int days;
public:
    run(string filePath){
        this->filePath=filePath;
        loadData();
        for(int i=0;i<days;i++)processRequests(i);
    }
    ~run(){
        for(unordered_map<string, server*>::iterator it=serverWarehouse.begin();it!=serverWarehouse.end();it++)delete it->second;
        for(unordered_map<string, VM*>::iterator it=VMWarehouse.begin();it!=VMWarehouse.end();it++)delete it->second;
        for(server *tmp:serverPossessed)delete tmp;
        for(request *tmp:requestWarehouse)delete tmp;
    }
    void loadData();
    void processRequests(int dayth);
    void generateServer(string &serverType,string &cpuCores,string &memorySize,string &serverCost,string &powerCost);
    void generateVM(string &vmType,string &vmCpuCores,string &vmMemory,string &vmTwoNodes);
    void generateRequest(string &op, string &reqId);
    void generateRequest(string &op, string &reqVmType, string &reqId);
};

//处理第day天的请求
void run::processRequests(int dayth){

}


void run::generateServer(string &serverType,string &cpuCores,string &memorySize,string &serverCost,string &powerCost){
    string _serverType="";
    for(int i =1;i<serverType.size() -1;i++){
        _serverType += serverType[i];
    }
    int _cpuCores =0,_memorySize=0,_serverCost=0,_powerCost=0;

    for(int i=0;i<cpuCores.size() -1;i++){
        _cpuCores = 10*_cpuCores + cpuCores[i] - '0';
    }
    for(int i=0;i<memorySize.size() -1;i++){
        _memorySize = 10*_memorySize + memorySize[i] - '0';
    }
    for(int i=0;i<serverCost.size() -1;i++){
        _serverCost = 10*_serverCost + serverCost[i] - '0';
    }
    for(int i=0;i<powerCost.size()-1;i++){
        _powerCost = 10*_powerCost + powerCost[i] - '0';
    }
    serverWarehouse[_serverType] = new server(_cpuCores/2,_memorySize/2,_serverCost,_powerCost);
}


void run::generateVM(string &vmType,string &vmCpuCores,string &vmMemory,string &vmTwoNodes){
    string _vmType ="";

    for(int i=1;i<vmType.size() -1;i++){
        _vmType += vmType[i];
    }

    int _vmCpuCores = 0,_vmMemory=0;
    bool _vmTwoNodes;
    for(int i=0;i<vmCpuCores.size()-1;i++){
        _vmCpuCores = _vmCpuCores*10 + vmCpuCores[i] - '0';
    }
    for(int i=0;i<vmMemory.size()-1;i++){
        _vmMemory = _vmMemory*10 + vmMemory[i] - '0';
    }
    if(vmTwoNodes[0] == '1'){
        _vmTwoNodes = true;
    }
    else{
        _vmTwoNodes =false;
    }
    VMWarehouse[_vmType] = new VM(_vmCpuCores,_vmMemory,_vmTwoNodes);
    cout<<_vmCpuCores<<","<<_vmMemory<<","<<_vmTwoNodes<<endl;
}


// 解析用户添加请求
void run::generateRequest(string &op, string &reqVmType, string &reqId){
    string _op,_reqVmType,_reqId;
    _op = op.substr(1,op.size() -1);
    _reqVmType = reqVmType.substr(0,reqVmType.size() -1);
    _reqId = reqId.substr(0,reqId.size() -1);
    requestWarehouse.push_back(new request(true,_reqVmType,_reqId));
}

// 解析用户删除请求
void run::generateRequest(string &op, string &reqId){
    string _op,_reqId;
    _reqId = reqId.substr(0,reqId.size() -1);
    _op = op.substr(1,op.size() -1);
    requestWarehouse.push_back(new request(false,"",_reqId));
}


void run::loadData(){
    if(filePath!=""){
        std::freopen(filePath.c_str(),"r",stdin);
    }

    //读入服务器型号信息
    int serverNum;
    string serverType,cpuCores,memorySize,serverCost,powerCost;
    scanf("%d",&serverNum);
    for(int i =0;i<serverNum;i++){
        cin>>serverType>>cpuCores>>memorySize>>serverCost>>powerCost;
        generateServer(serverType,cpuCores,memorySize,serverCost,powerCost);
    }

    //读入虚拟机型号信息
    int vmNumber = 0;
    scanf("%d",&vmNumber);
    string vmType,vmCpuCores,vmMemory,vmTwoNodes;
    for(int i =0;i<vmNumber;i++){
        cin>>vmType>>vmCpuCores>>vmMemory>>vmTwoNodes;
        generateVM(vmType,vmCpuCores,vmMemory,vmTwoNodes);
    }

    //读入所有的请求信息
    int RequestNumber = 0;
    scanf("%d",&days);
    string op,reqVmType,reqId;
    for(int i=0;i<days;i++){
        scanf("%d",&RequestNumber);
        for(int j =0;j<RequestNumber;j++){
            cin>>op;
            if(op[1] == 'a'){
                cin>>reqVmType>>reqId;
                generateRequest(op, reqVmType, reqId);
            }else{
                cin>>reqId;
                generateRequest(op, reqId);
            }
        }
    }
}
