#include"usermodel.h"
#include"db.h"
#include<iostream>


using namespace std;

bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
                user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        
        if(mysql.update(sql))
        {
            user.setID(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
User UserModel::query(int id)
{
    // 组装sql语句

    //根据用户号码查询用户信息
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d ",
                id);

    MySQL mysql;
    if(mysql.connect())
    {
        
        MYSQL_RES*  res = mysql.query(sql);
        if(res != NULL)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != NULL)
            {
                User user;
                user.setID(atoi(row[0]));   
                user.setName(row[1]);       
                user.setPwd(row[2]);       
                user.setState(row[3]);       
                mysql_free_result(res);
                return user;
            }
        }
        
    }
    return User();
    
}

bool UserModel::updateState(User &user){

    char sql[1024] = {0};
        sprintf(sql, "update user set state = '%s'  where id = %d",
                    user.getState().c_str(), user.getID());

        MySQL mysql;
        if(mysql.connect())
        {
            
            if(mysql.update(sql))
            {
                
                return true;
            }
        }
        return false;


}


//重置用户状态信息
void UserModel::resetState(){

char sql[1024] = {0};
        sprintf(sql, "update user set state = 'offline'  where state = 'online'");

        MySQL mysql;
        if(mysql.connect())
        {
            mysql.update(sql);
           
        }
       

}