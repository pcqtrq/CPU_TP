#include<iostream>
#include<vector>
#include<string>
#include<map>

using namespace std;

struct TP_Data{
    int operational_type;      //操作符类型
    /**
     * operational_type=0 添加数据
     * operational_type=1 对已有数据进行加法操作（%INT_MAX）
     * operational_type=2 对已有数据进行减法操作 (%INT_MAX) 
     */
    int data[3];
};

class TP{
    public:
        void Get_Tp_Data(){          //获取事务的指令，数据信息
            int num,val=1;
            cout<<"TP instructions nums ："<<endl;
            cin>>num;
            dt.resize(num);
            for(int i=0;i<num;i++){
                cout<<"operational_type : insert-0,add-1 , sub-2 "<<endl;
                cin>>dt[i].operational_type;
                cout<<"Enter data : "<<endl;
                if(dt[i].operational_type==0) cin>>dt[i].data[0];
                else{
                    while(val){
                        for(int j=0;j<3;j++) {
                            cin>>dt[i].data[j];
                        }
                    }

                }
            }
        }

        void Execute_Tp(){          //串行执行事务
            for(int i=0;i<dt.size();i++){
                switch(dt[i].operational_type){
                    case 0: record.push_back(dt[i].data[0]); break;
                    case 1: record[dt[i].data[0]]=(dt[i].data[1]+dt[i].data[2])%INT_MAX ;break;
                    case 2: record[dt[i].data[0]]=(dt[i].data[1]-dt[i].data[2])%INT_MAX;break;
                    default: cout<<"Enter Error!\n";
                }
            }
        }    

    private:
        vector<TP_Data> dt;                     //TP数据
        static vector<int> record;                 //数据的实际存储，以数据在vector中的下标作为数据的唯一键值
        //static map<int,int> data_occupancy;        //多线程时，判断数据的锁状态
};


vector<int> TP::record;



int main(){
    cout<<"Begin Transtraction Deal:\n";
   
    string tmp;
    while(1){
        cout<<"Begin a transcation ,yes or no?"<<endl;
        cin>>tmp;
        if(tmp=="no"||tmp=="No") break;
        
        TP tp;
        tp.Get_Tp_Data();
        tp.Execute_Tp();
    }

    return 0;
}