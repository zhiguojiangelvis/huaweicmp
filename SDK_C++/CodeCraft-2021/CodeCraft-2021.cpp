#include "./lib/run.h"
using namespace std;
const string filePath="../../../training-data/training-2.txt";
int main(){
    run *r=new run(filePath);
    delete r;
    return 0;
}

