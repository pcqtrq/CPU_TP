#include<string>
#include<vector>
#include<iostream>
#include<unordered_map>

using namespace std;

/****
 * id  age  hight
 * 1    21  170
 * 2    24  172
 * 
 */




class Table{          //单表结构，行存储
    public:
        friend class Tables;
        void data_import(){
            cout<<"Enter Tables name : \n";
            cin>>table_name;

            int num;
            cout<<"Enter tables attribute nums : \n";
            cin>>num;

            attribute_name.resize(num);
            cout<<"Enter tables attribute names :\n";
            for(int i=0;i<num;i++){
                cin>>attribute_name[i];
            }

            cout<<" 1: Import from file( Not support )  \n other nums: manual import \n";
            cin>>num;
            if(num==0){
                cout<<"Not support \n";

            }else{
                cout<<"Record nums ?\n";
                cin>>num;

                attribute_double_val.resize(num);
                cout<<"Enter record : \n";

                for(int i=0;i<num;i++){
                    attribute_double_val[i].resize(attribute_name.size());
                    
                    for(int j=0;j<attribute_name.size();j++){
                        cin>>attribute_double_val[i][j];
                    }
                }
            }
            cout<<"Data import is finished\n";

        }
    

    private:
        // 目前假设所有属性的类型均为有理数
        string table_name;                                        //表名
        vector<string> attribute_name;                           //表的属性的名字

        
        //vector<int>  attribute_type;                           //属性的类型，例如：0-有理数，1-字符串
        vector<vector<double>> attribute_double_val;            //类型为有理数的属性集合
        //vector<vector<string>> attribute_string_val;          //类型为字符串的属性集合
        //vector<int>  attribtute_offset;                       //属性在对应属性集合中的具体位置，也就是偏移量
        
   
};

struct TP_Struct{           //面向 people.(8).age=3+5;
    string table_name;                  //table_name.(primary_key_val).left_val_name=right_val_1+right_val_2
    double primary_key_val;               //主键的值

    string left_val_name;

    double right_val_1;
    double right_val_2;
    // string right_val_1;
    // string right_val_2;

    void display(){
        cout<<table_name<<" "<<primary_key_val<<"  "<<left_val_name<<"  "<<right_val_1<<"  "<<right_val_2<<endl;
    }
};

class Tables{          //多表结构
    public:
        vector<Table> tbl;           //存储所有的表

      //  unordered_map<string,int> tables_srach_index;      //快速查找表的索引

        bool check_sql(string table_name,string val_name){            //检查表明和数据名的正确性
           
            for(int i=0;i<tbl.size();i++){
                if(table_name==tbl[i].table_name){
                    auto& tbl_attr=tbl[i].attribute_name;
                    for(int i=0;i<tbl_attr.size();i++){
                        if(tbl_attr[i]==val_name){
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        
        bool insert_new_sql(vector<TP_Struct>& dt_data,string tmp){
             TP_Struct tp_new;
          
            cout<<"the sql statement is:  "<<tmp<<endl;

            string subString;
            int i=0,j=0;
            while(tmp[j]!='.') j++;
            tp_new.table_name=tmp.substr(i,j-i);

            i=j+2;
            j=i;
            while(tmp[j]!=')') j++;
            tp_new.primary_key_val=stod(tmp.substr(i,j-i));
           
            i=j+2;
            j=i;
            while(tmp[j]!='=') j++;
            tp_new.left_val_name=tmp.substr(i,j-i);


            i=j+1;
            j=i;
            while(tmp[j]!='+') j++;
            tp_new.right_val_1= stod( tmp.substr(i,j-i));
     
            tp_new.right_val_2=stod(tmp.substr(j+1,tmp.size()-(j+1)));
            tp_new.display();

            if(!check_sql(tp_new.table_name,tp_new.left_val_name)){
                cout<<"Sql statement entered is wrong \n";
                dt_data.clear();
                return false;
            }

            dt_data.push_back(tp_new);
            return true;
        }

        //当前事务只支持单个数据的处理
        void Get_Tp_Data_and_Process(){          //获取事务的指令，数据信息
            cout<<"Enter SQL statement in a single line without space (only support SQL like : 'tableA.(6).id_name=3+5;tablesB.(8).id_name=6+7;' ,(6 ,8 is primary_key value)): \n";
            string sql;
            cin>>sql;

            vector<TP_Struct> TP_date;  //保存SQL语句中的TP信息
            string tmp;
            bool flag=1;
            int i=0,j=0;
            for( i=0,j=0;i<sql.size();i++){
                if(sql[i]==';'){
                    tmp=sql.substr(j,i-j);
                    j=i+1;
                    flag=flag&&insert_new_sql(TP_date,tmp);            //添加语句
                }
            }
            // tmp=sql.substr(j,i-j);
            // flag=flag&&insert_new_sql(TP_date,tmp);            //添加语句

         
            if(flag){
                cout<<"SQL statement is checked , the TP is running \n";
                TP_process(TP_date);
            }else{
                cout<<"SQL statement is wrong . Please enter again (0) or exit (1).\n";
                int p;
                cin>>p;
                if(p){return ;}
                else Get_Tp_Data_and_Process();
            };
           
             
        }

       
        
        void TP_process(vector<TP_Struct>& dt_data){      
            //可以采用数据库锁，表锁，行锁
            //可以使用mvcc（需要修改底层数据结构）
            


                 //清空当前事务的数据
        } 
           

}tables;
      

int main(){
    int tmp;
    while(1){
        cout<<" 1: import new table \n 2: begin transtraction \n 3: add new data to old tables ： \n other num: exit\n";
        cin>>tmp;
        if(tmp==1){
            Table new_table;
            tables.tbl.push_back(new_table);
            auto& tbl=*(tables.tbl.end()-1);
            tbl.data_import();
        }else if(tmp==2){
            tables.Get_Tp_Data_and_Process();
        }else if(tmp==3){
            cout<<"Not support \n";
        
        }else{
            cout<<"exit "<<endl;
            break;
        }
    }

    return 0;
}