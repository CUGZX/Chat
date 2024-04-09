#include "db.h"
#include <muduo/base/Logging.h>
#include<string>
using namespace std;

static string  server = "127.0.0.1";
static string  user = "chat";
static string  password = "";
static string  dbname = "chat";

MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}

// 释放连接
MySQL::~MySQL()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn);
    }
}

// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        // 代码支持中文，因为默认是ASCII
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success!";
    }
    else
    {
        LOG_INFO << "connect mysql fail!";
    }

    return p;
}

// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << " : " << __LINE__ << " : " << sql << " update error!";
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES* MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << " : " << __LINE__ << " : " << sql << " select error!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

// 获取连接
MYSQL* MySQL::getConnection()
{
    return _conn;
}