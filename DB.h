#include<string>
#include<vector>
#include<iostream>
#include<unordered_map>
#include<thread>
#include<mutex>
#include<atomic>
#include<fstream>

#include<windows.h>

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

    double right_val;           //right_val=right_val_1+right_val_2;
    //double* left_val;           //left_val是左值的地址

    void display(){
        cout<<table_name<<" "<<primary_key_val<<"  "<<left_val_name<<"  "<<right_val_1<<"  "<<right_val_2<<endl;
    }
};

class TP_Mutex{               //行锁机制，对于每行数据都存在一个行锁   
    private:
        mutex *m;
        bool flag;        //标记该位置是否被占用,原子数据

    public:
        int position;          //记录在表中的实际位置
        TP_Mutex(){
            m=new mutex();
        }
        bool lock(){
            auto& m1=*m;
            if(m1.try_lock()){
                  flag=true;
                  m1.unlock();
                  return true;
            }
            return false;
        }

        void unlock(){
            flag=false;
        }

        ~TP_Mutex(){
            delete m;
        }


};


struct TP{                           //TP事务
    int serial_num;                  //事务的全局编号
    vector<TP_Struct> tp_date;       //事务的所有元数据
    vector<TP_Mutex*> mutex_lock;    //事务涉及到的数据的行锁集合
};

string rand_str(const int len)  /*参数为字符串的长度*/
{
    /*初始化*/
    string str;                 /*声明用来保存随机字符串的str*/
    char c;                     /*声明字符c，用来保存随机生成的字符*/
    int idx;                    /*用来循环的变量*/
    /*循环向字符串中添加随机生成的字符*/
    for(idx = 0;idx < len;idx ++)
    {
        /*rand()%26是取余，余数为0~25加上'a',就是字母a~z,详见asc码表*/
        c = 'a' + rand()%26;
        str.push_back(c);       /*push_back()是string类尾插函数。这里插入随机字符c*/
    }
    return str;                 /*返回生成的随机字符串*/
}


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



                int y=0;
                TP_Mutex tp_mutex;

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


         void tp_test_import( const string tl_name="test_table",const int attr_num=6,const int rd_num=1e5,const int attr_name_len=4){
            /*
            const string tl_name="test_table";       //设置测试的表名
            const int attr_num=6;                //设置表的属性个数
            const int rd_num=1e5;             //设置待生成的记录的数量

            const int attr_name_len=4;        //属性名的长度

            */
            
            table_name=tl_name;


            attribute_name.resize(attr_num);             //表属性生成
            for(int i=0;i<attr_num;i++){
                attribute_name[i]=rand_str(attr_name_len);
                attr_index[attribute_name[i]]=i;
                
            }

           if(1){
                for(int i=0;i<attr_num;i++){
                    cout<<attribute_name[i]<<" ; ";
                }
                cout<<endl;
           }

            attribute_double_val.resize(rd_num);                      //表数据生成
            int y=0;
            TP_Mutex tp_mutex;
            for(int i=0;i<rd_num;i++){
                attribute_double_val[i].resize(attribute_name.size());

                for(int j=0;j<attribute_name.size();j++){
                    if(j==0){
                        attribute_double_val[i][j]=i;      //主键为递增的正整数键值   

                        tp_mutex.position=i;               //构建hash索引，用于寻找所有的记录
                        primary_key_index[i]=tp_mutex;
                        
                    }else{
                        attribute_double_val[i][j]=rand()%INT_MAX/2;
                    }
                }
            }
            cout<<"Table is generated ."<<endl;
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
        DB(){
            tp_serial_num=0;
        }

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

            TP tp;               //构建TP事务
            tp.serial_num=tp_serial_num++;           //添加唯一全局编号
            vector<TP_Struct>& TP_date=tp.tp_date;  //保存SQL语句中的TP信息
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

                TP_process(tp);
                cout<<"TP is finished \n";

            }else{
                cout<<"SQL statement is wrong . Please enter again (0) or exit (1).\n";
                int p;
                cin>>p;
                if(p){return ;}
                else Get_Tp_Data_and_Process();
            };
           
             
        }  


        void test_tp(const int parallel_thread_num=4){
            //int parallel_thread_num=4;          //设置并发的TP线程数量
            cout<<"Creating table: \n";
            Table new_table;
            tbl.push_back(new_table);
            auto tmp=(tbl.end()-1);
            tmp->tp_test_import();
            table_name_index[tmp->table_name]=tbl.size()-1;

           //auto ref_tbl=&(*tmp);
            if(1){    //单线程环境下测试
                 generate_transcation_and_process(*tmp);
            }else{  //并发环境下测试
                thread th[parallel_thread_num];
                
                for(int i=0;i<parallel_thread_num;i++){
                    //th[i]=thread(generate_transcation_and_process,this,ref_tbl);
                    th[i]=thread(generate_transcation_and_process,this,cref(*tmp));
                }
                for(int i=0;i<parallel_thread_num;i++){
                    th[i].join();
                }
            }

            cout<<"TP test is finished "<<endl; 

        }
    private:

        void generate_transcation_and_process(const Table& tmp){         //只生成关于单表的事务

           // const Table& tmp=*ref_tmp;
            cout<<"Creating transtractions ... \n";

            TP tp;               //构建TP事务
            tp_generate(tmp,tp);
            cout<<"Processing transtraction ... \n" ;
            TP_process(tp);     
        }  

        void tp_generate(const Table &tmp,TP &tp,const int tp_num=5){     
           // const int tp_num=5;                   //待生成的tp的数量

            tp.tp_date.resize(tp_num);
            tp.serial_num=tp_serial_num++;           //添加唯一全局编号

            vector<TP_Struct>& TP=tp.tp_date;  //保存SQL语句中的TP信息

            int attri_num=tmp.attribute_name.size();
            int rd_num=tmp.attribute_double_val.size();

            for(int i=0;i<tp_num;i++){
                TP[i].table_name=tmp.table_name;
                TP[i].primary_key_val= (int) rand()%rd_num;
                TP[i].left_val_name=tmp.attribute_name[rand()%attri_num];
                
                TP[i].right_val=rand()%INT_MAX/2;
            }
        }


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

            tp_new.right_val=tp_new.right_val_1+tp_new.right_val_2;

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
        
        void TP_process(TP& tp){      
          /*
          **我们采用行锁保证事务的准确性，并通过设定事务持锁的最长时间阈值，来解决事务行锁机制的死锁问题
          *我们要求必须获得当前数据涉及到的数据所涉及到的所有的锁后，才可以进行数据的实际修改
          *我们的所有锁都是互斥锁
          */
            vector<Table*> tb;                //定位TP数据中的每行所在表
            tb.resize(tp.tp_date.size());
            for(int i=0;i<tb.size();i++){
                tb[i]=&tbl[table_name_index.find(tp.tp_date[i].table_name)->second];  //定位表
            }
            

            int lock_num=0;
            vector<int> tp_lock_flag;
            tp_lock_flag.resize(tp.tp_date.size());

            while(1){      //获取锁
                lock_num=0;
                for(int i=0;i<tp.tp_date.size();i++){

                    //如果当前行未被其他事务占用，或者已被本次事务占用，那么成功
                    if(tb[i]->primary_key_index.find(tp.tp_date[i].primary_key_val)->second.lock()|| tp_lock_flag[i]==tp.serial_num){
                        ++lock_num;
                        tp_lock_flag[i]=tp.serial_num;
                    }else{
                        //tp_lock_flag[i]=0;
                    }
                }
                if(lock_num==tp.tp_date.size()) break;          //获取锁成功
                else{
                    for(int i=0;i<tp_lock_flag.size();i++){       //解锁
                        if(tp_lock_flag[i]==tp.serial_num){
                            tb[i]->primary_key_index.find(tp.tp_date[i].primary_key_val)->second.unlock(); 
                        }
                    }
                    Sleep(rand()%10);
                }
            }
            for(int i=0;i<tp.tp_date.size();i++){    //实际修改数据
                auto row=tb[i]->primary_key_index.find(tp.tp_date[i].primary_key_val)->second.position;  //定位行
                auto column=tb[i]->attr_index.find(tp.tp_date[i].left_val_name)->second;       //定位列
                tb[i]->attribute_double_val[row][column]=tp.tp_date[i].right_val;             //修改值

                //解锁
                tb[i]->primary_key_index.find(tp.tp_date[i].primary_key_val)->second.unlock();

            }

        } 

    private:
        vector<Table> tbl;           //存储所有的表

        unordered_map<string,int> table_name_index;      //快速查找表的索引,int为tbl中的下标    

        atomic_int32_t tp_serial_num;                         //事务的全局编号

}db;
  