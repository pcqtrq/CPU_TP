
#include".\DB.h"

using namespace std;



    

int main(){
    int tmp;
    while(1){
        cout<<" 1: import new table \n 2: begin transtraction \n 3: add new data to old tables  \n else : exit\n";
        cin>>tmp;
        
        if(tmp==1){
            db.table_import();
        }else if(tmp==2){
            db.Get_Tp_Data_and_Process();
        }else if(tmp==3){
            cout<<"Not support \n";
        
        }else{
            cout<<"exit "<<endl;
            break;
        }
    }

    return 0;
}