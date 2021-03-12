#include "dataStructure.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include  <map>
using namespace std;
class run{
private:
    string filePath;
    //服务器和虚拟机的所有型号
    //先按核数排序然后按照内存排序，从小到大
    map<int, map<int,set<server> > > serverWarehouse;
    map<int, map<int, set<serverNode> > > serverPossessed;
    vector<int*> leftSource;//4个数记录每个服务器的核数和内存的使用情况
    unordered_map< string, VM* > VMWarehouse;
    unordered_map<int, VMNode* > VMPossessed;
    vector<request*> *requestWarehouse;

    //每一次的决策
    unordered_map<string,int> buyServer;
    vector<string> allocateVM;
    vector<string> mvVM;
    int moveTime;
    int moveMax;

    int days;
    int serverNum;
    int VMNum;
public:
    run(string filePath){
        VMNum=0;
        serverNum=0;
        this->filePath=filePath;
        loadData();
        for(int i=0;i<days;i++)processRequests(i);
    }
    ~run(){
        for(unordered_map<string, VM*>::iterator it=VMWarehouse.begin();it!=VMWarehouse.end();it++)delete it->second;
        for(int *tmp:leftSource)delete[] tmp;
        for(unordered_map<int, VMNode* >::iterator it=VMPossessed.begin();it!=VMPossessed.end();it++)delete it->second;
    }
    void loadData();
    void processRequests(int dayth);
    void generateServer(string &serverType,string &cpuCores,string &memorySize,string &serverCost,string &powerCost);
    void generateVM(string &vmType,string &vmCpuCores,string &vmMemory,string &vmTwoNodes);
    void generateRequest(string &op, string &reqId, int today);
    void generateRequest(string &op, string &reqVmType, string &reqId, int today);
    void moveAction();
    void printStrategy();
    //返回购买型号
    string buy(int coreNeed,int ramNeed);
};

//输出一天的策略
void run::printStrategy(){
    //买服务器策略
    string buyAllStr="(purchase, ";
    buystr+=to_string(buyServer.size())+")";
    cout<<buystr<<endl;;
    for(auto it=buyServer.begin();it!=buyServer.end();it++){
        string buySingleStr="(";
        buySingleStr+=(it->first+", "+to_string(it->second)+")");
        cout<<buySingleStr<<endl;
    }
    //迁移虚拟机策略
    for(string str: mvVM)cout<<mvVM<<endl;;
    //分配服务器
    for(string str:allocateVM)cout<<str<<endl;
}

void run::moveAction(){
    mvVM.push_back("(migration, 0)");
}

string run::buy(int coreNeed,int ramNeed){//买最便宜的
    set<server> satisfied=serverWarehouse.lower_bound(coreNeed)->second.lower_bound(ramNeed)->second;
    return (*satisfied.begin()).serverType;
}

//处理第day天的请求
void run::processRequests(int dayth){
    buyServer.clear();
    allocateVM.clear();
    mvVM.clear();
    moveTime=0;
    moveMax=VMNum/200;
    unsigned int buyCost=0,powerCost=0;
    for(request *req:requestWarehouse[dayth]){
        //给请求分配机器
        int VMID=req->VMID;
        VM *vm=VMWarehouse[req->VMType];
        int coreNeed=vm->coreNum,ramNeed=vm->ram;
        if(req->reType){
            VMNum++;
            bool allocated=false;
            if(vm->doubleNode){//处理双节点请求
               map<int, map<int, set<serverNode> > >::iterator it=serverPossessed.lower_bound(coreNeed);
               while(it!=serverPossessed.end()&&!allocated){
                    map<int, set<serverNode*> >::iterator itin=it->second.lower_bound(ramNeed);
                    if(itin!=it->second.end()){
                        set<serverNode>::iterator posi=itin->second.begin();
                        while(posi!=itin->second.end()){
                            serverNode tmp=*posi;
                            int ID=tmp.ID;
                            if(leftSource[ID][0]>=coreNeed&&leftSource[ID][1]>=ramNeed&&leftSource[ID][2]>=coreNeed&&leftSource[ID][3]>=ramNeed){
                                allocated=true;
                                allocateVM.push_back("("+to_string(tmp.ID)+")");
                                VMPossessed[VMID]=new VMNode(tmp.ID,true,true);
                                if(tmp.A){
                                    serverPossessed[leftSource[ID][0]][leftSource[ID][1]].erase(posi);
                                    leftSource[ID][0]-=coreNeed;leftCore[ID][1]-=ramNeed;
                                    serverPossessed[leftSource[ID][0]][leftSource[ID][1]].insert(tmp);
                                    set<serverNode*>::iterator del;
                                    for(set<serverNode*>::iterator other=serverPossessed[leftSource[ID][2]][leftSource[ID][3]].begin();
                                    other!=serverPossessed[leftSource[ID][2]][leftSource[ID][3]].end();other++){
                                        if(other->ID==ID&&!other->A){
                                            del=other;
                                            break;
                                        }
                                    }
                                    serverPossessed[leftSource[ID][2]][leftSource[ID][3]].erase(del);
                                    leftSource[ID][2]-=coreNeed;leftCore[ID][3]-=ramNeed;
                                    serverPossessed[leftSource[ID][2]][leftSource[ID][3]].insert(*del);
                                }
                                else{
                                    serverPossessed[leftSource[ID][2]][leftSource[ID][3]].erase(posi);
                                    leftSource[ID][2]-=coreNeed;leftCore[ID][3]-=ramNeed;
                                    serverPossessed[leftSource[ID][2]][leftSource[ID][3]].insert(tmp);
                                    set<serverNode*>::iterator del;
                                    for(set<serverNode*>::iterator other=serverPossessed[leftSource[ID][0]][leftSource[ID][1]].begin();
                                    other!=serverPossessed[leftSource[ID][0]][leftSource[ID][1]].end();other++){
                                        if(other->ID==ID&&other->A){
                                            del=other;
                                            break;
                                        }
                                    }
                                    serverPossessed[leftSource[ID][0]][leftSource[ID][1]].erase(del);
                                    leftSource[ID][0]-=coreNeed;leftCore[ID][1]-=ramNeed;
                                    serverPossessed[leftSource[ID][0]][leftSource[ID][1]].insert(*del);
                                }
                            }
                            posi++;
                        }
                    }
                    it++;
               }
            }
            else{//处理单节点请求，找到满足要求的剩余容量最小的机器
                map<int, map<int, set<serverNode> > >::iterator it=serverPossessed.lower_bound(coreNeed);
                while(it!=serverPossessed.end()&&!allocated){
                    map<int, set<serverNode*> >::iterator itin=it->second.lower_bound(ramNeed);
                    if(itin!=it->second.end()){
                        allocated=true;
                        //找到满足条件的每日耗电量最小的
                        serverNode tmp=*(itin->second.begin());
                        string A="B";
                        int coreDel=it->first, ramDel=itin->first,num=tmp.ID;
                        if(tmp.A){
                            A="A";
                            leftSource[num][0]-=coreNeed;
                            leftSource[num][1]-=ramNeed;
                        }
                        else{
                            leftSource[num][2]-=coreNeed;
                            leftSource[num][3]-=ramNeed;
                        }
                        allocateVM.push_back("("+to_string(tmp.ID)+", "+A+")");
                        VMPossessed[VMID]=new VMNode(tmp.ID,(A=="A"),(A=="B"));
                        serverPossessed[coreDel][ramDel].erase(serverPossessed[coreDel][ramDel].begin());
                        coreDel-=coreNeed;ramDel-=ramNeed;
                        serverPossessed[coreDel][ramDel].insert(tmp);
                    }
                    it++;
                }
            }
            //买新机器
            if(!allocated){
                if(vm->doubleNode){
                    coreNeed/=2;
                    ramNeed/=2;
                }
                string buyType=buy(coreNeed,ramNeed);
                server *newServer=&serverWarehouse[buyType];
                buyServer[buyType]++;
                if(vm->doubleNode){
                    serverNode tmp1=serverNode(buyType,serverNum,true);
                    serverNode tmp2=serverNode(buyType,serverNum,false);
                    serverPossessed[newServer->coreNum-coreNeed][newServer->ram-ramNeed].insert(tmp1);
                    serverPossessed[newServer->coreNum-coreNeed][newServer->ram-ramNeed].insert(tmp2);
                    int *left=new int[4];left[0]=newServer->coreNum-coreNeed;left[1]=newServer->ram-ramNeed;
                    left[2]=newServer->coreNum-coreNeed;left[3]=newServer->ram-ramNeed;
                    leftSource.push_back(left);
                    allocateVM.push_back("("+to_string(serverNum)+")");
                    VMPossessed[VMID]=new VMNode(serverNum,true,true);
                }
                else{
                    serverNode tmp1=serverNode(buyType,serverNum,true);
                    serverNode tmp2=serverNode(buyType,serverNum,false);
                    serverPossessed[newServer->coreNum-coreNeed][newServer->ram-ramNeed].insert(tmp1);
                    serverPossessed[newServer->coreNum][newServer->ram].insert(tmp2);
                    int *left=new int[4];left[0]=newServer->coreNum-coreNeed;left[1]=newServer->ram-ramNeed;
                    left[2]=newServer->coreNum;left[3]=newServer->ram;
                    leftSource.push_back(left);
                    allocateVM.push_back("("+to_string(serverNum)+", A)"+);
                    VMPossessed[VMID]=new VMNode(serverNum,true,false);
                }

                serverNum++;
            }
        }
        else{//删除一个虚拟机
            VMNum++;
            //分配在哪一台主机上
            VMNode *host=VMPossessed[VMID];
            int hostID=host->hostID;
            //归还资源
            bool A=host->A,B=host->B;
            int findCore,findRam;
            if(A){
                findCore=leftSource[hostID][0];
                findRam=leftSource[hostID][1];
                leftSource[hostID][0]+=coreNeed;
                leftSource[hostID][1]+=ramNeed;
                serverNode *add;
                set<serverNode> goalSet=serverPossessed[findCore][findRam];
                for(auto goal=goalSet.begin();goal!=goalSet.end();goal++){
                    if(goal->ID==hostID&&goal->A){
                            add=goal;
                    }
                }
                serverPossessed[findCore][findRam].erase(add);
                serverPossessed[leftSource[hostID][0]][leftSource[hostID][1]].insert(*add);
            }
            if(B){
                findCore=leftSource[hostID][2];
                findRam=leftSource[hostID][3];
                leftSource[hostID][2]+=coreNeed;
                leftSource[hostID][3]+=ramNeed;
                serverNode *add;
                set<serverNode> goalSet=serverPossessed[findCore][findRam];
                for(auto goal=goalSet.begin();goal!=goalSet.end();goal++){
                    if(goal->ID==hostID&&!goal->A){
                            add=goal;
                    }
                }
                serverPossessed[findCore][findRam].erase(add);
                serverPossessed[leftSource[hostID][2]][leftSource[hostID][3]].insert(*add);
            }
        }
        moveAction();
        delete req;
    }
    printStrategy();
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

    serverWarehouse[_cpuCores/2][_memorySize/2].insert(server(_serverType, _cpuCores/2,_memorySize/2,_serverCost,_powerCost));
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
    VMWarehouse[_vmType] = new VM(_vmType, _vmCpuCores,_vmMemory,_vmTwoNodes);
}


// 解析用户添加请求
void run::generateRequest(string &op, string &reqVmType, string &reqId,int today){
    string _op,_reqVmType,_reqId;
    _op = op.substr(1,op.size() -1);
    _reqVmType = reqVmType.substr(0,reqVmType.size() -1);
    _reqId = reqId.substr(0,reqId.size() -1);
    requestWarehouse[today].push_back(new request(true,_reqVmType,_reqId));
}

// 解析用户删除请求
void run::generateRequest(string &op, string &reqId, int today){
    string _op,_reqId;
    _reqId = reqId.substr(0,reqId.size() -1);
    _op = op.substr(1,op.size() -1);
    requestWarehouse[today].push_back(new request(false,"",_reqId));
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
    requestWarehouse=new vector<request*>[days];
    string op,reqVmType,reqId;
    for(int i=0;i<days;i++){
        scanf("%d",&RequestNumber);
        for(int j =0;j<RequestNumber;j++){
            cin>>op;
            if(op[1] == 'a'){
                cin>>reqVmType>>reqId;
                generateRequest(op, reqVmType, reqId, i);
            }else{
                cin>>reqId;
                generateRequest(op, reqId, i);
            }
        }
    }
}
