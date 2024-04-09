#include "offlinemessagemodel.h"
#include<iostream>
#include"db.h"
using namespace std;

void OfflineMsgModel::insert(int userid, string msg){

   // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')",
                userid, msg.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        
        mysql.update(sql);
      
    }
}
    


    // 删除用户的离线消息
void OfflineMsgModel::remove(int userid){

   char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where  userid=%d ", userid);

    MySQL mysql;
    if(mysql.connect())
    {
        
        mysql.update(sql);
      
    }


}


 vector<string> OfflineMsgModel::query(int userid){

    //根据用户号码查询用户信息
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d ",userid);

    vector<string> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        
        MYSQL_RES*  res = mysql.query(sql);
        if(res != nullptr)
        {   
            
            // 获取离线消息
            MYSQL_ROW row;
             //  把userid的离线消息放入vec中
            while((row=mysql_fetch_row(res)) != nullptr)
            {
               
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
        
    }
  
    return vec;

 }