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


struct TP_Struct{           //TP事务的基础结构，面向 people.(8).age=3+5;
    string table_name;                  //table_name.(primary_key_val).left_val_name=right_val_1+right_val_2
    double primary_key_val;               //主键的值

    string left_val_name;

    double right_val_1;
    double right_val_2;

    void display(){
        cout<<table_name<<" "<<primary_key_val<<"  "<<left_val_name<<"  "<<right_val_1<<"  "<<right_val_2<<endl;
    }
};

struct TP_Mutex{
    bool flag;
    int position;
}tp_mutex;



class Table{          //单表结构，行存储
    public:
        friend class DB;
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
                attr_index[attribute_name[i]]=i;
            }

            cout<<" 0 : Import from file( Not support now )  \n else : manual import \n";
            cin>>num;
            if(num==0){
                cout<<"Not support \n";

            }else{
                cout<<"Record nums ?\n";
                cin>>num;
                if(num<1) return ;

                attribute_double_val.resize(num);
                cout<<"Enter record : \n";

                tp_mutex.flag=0;
                tp_mutex.position=0;

                int y=0;
                for(int i=0;i<num;i++){
                
                    attribute_double_val[i].resize(attribute_name.size());

                    for(int j=0;j<attribute_name.size();j++){
                        cin>>attribute_double_val[i][j];
                        if(j==0){
                            int y=attribute_double_val[i][j];
                            if(primary_key_index.find(y)==primary_key_index.end()){         //构建hash索引，用于寻找所有的记录
                                tp_mutex.position=i;
                                primary_key_index[y]=tp_mutex;
                            }else{
                                cout<<"The primary key already exits ! retry"<<endl;         //当前主键已存在，重新输入
                                j--;

                            }

                        }
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
        
   
        unordered_map<double,TP_Mutex> primary_key_index;          //主键的索引，同时负责行锁 ,主键值不可以被改变
        unordered_map<string,int> attr_index;          //属性的索引
};




class DB{          //数据库，保存多个表
    public:
        void table_import(){              //导入新表
            Table new_table;
            tbl.push_back(new_table);
            auto tmp=(tbl.end()-1);
            tmp->data_import();
            table_name_index[tmp->table_name]=tbl.size()-1;
        }

                //当前事务只支持单线程的处理
        void Get_Tp_Data_and_Process(){          //获取事务的指令，数据信息
            cout<<"Enter SQL statement in a single line without space (only support SQL like : 'tableA.(6).id_name=3+5;tablesB.(8).id_name=6+7;' ,(6 ,8 is primary_key value)): \n";
            string sql;
            cin>>sql;

            vector<TP_Struct> TP_date;  //保存SQL语句中的TP信息
            string tmp;
            bool flag=1;
            //int i=0,j=0;
            for(int i=0,j=0;i<sql.size()&&flag;i++){
                if(sql[i]==';'){
                    tmp=sql.substr(j,i-j);
                    j=i+1;
                    flag=flag&&insert_new_sql(TP_date,tmp);            //添加语句
                }
            }

         
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

    private:

        bool insert_new_sql(vector<TP_Struct>& dt_data,string tmp){       //tp语句解析，并判断是否存在指向性错误
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

            if(!check_sql(tp_new.table_name,tp_new.primary_key_val,tp_new.left_val_name)){
                cout<<"Sql statement entered is wrong \n";
               // dt_data.clear();
                return false;
            }

            dt_data.push_back(tp_new);
            return true;
        }

        bool check_sql(string table_name,double main_key,string val_name){            //检查sql语句的正确性
            auto tmp1=table_name_index.find(table_name);
           if(tmp1!=table_name_index.end()){
               auto& tmp2=tbl[tmp1->second];
               cout<<tmp2.table_name<<"  ***\n";

               if(tmp2.attr_index.find(val_name)!=tmp2.attr_index.end()&&tmp2.primary_key_index.find(main_key)!=tmp2.primary_key_index.end()){
                   if(val_name!=tmp2.attribute_name[0])   //主键值不可修改
                        return true;
               }
           }else{
               cout<<"tables name not exit "<<endl;
           }
            return false;
        }
        
        void TP_process(vector<TP_Struct>& dt_data){      
          /*
          **我们采用行锁保证事务的准确性，并通过加锁有次序，并设定事务持锁的最长时间阈值，来解决事务行锁机制的死锁问题
          */

            for(int i=0;i<dt_data.size();i++){
                Get_Mutex(dt_data[i]);
            }


                 //清空当前事务的数据
            dt_data.clear();
        } 

        void Calcu_Data(){
            
        }

        void Get_Mutex(TP_Struct& dt_data){

        }

    private:
        vector<Table> tbl;           //存储所有的表

        unordered_map<string,int> table_name_index;      //快速查找表的索引    

}db;
  