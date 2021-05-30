//
// Created by azhou on 2021/5/29.
//

#ifndef WEBSERVER_SQLCONNRALL_H
#define WEBSERVER_SQLCONNRALL_H

#include "sqlconnpool.h"

//资源在对象构造的时候初始化， 在析构的时候释放
class SqlConnRALL{
public:
    SqlConnRALL(MYSQL** sql, SqlConnPool* connPool){
        assert(connPool);
        *sql = connPool->GetConn();
        sql_ = *sql;
        connPool_ = connPool;
    }

    ~SqlConnRALL(){
        if(sql_){
            connPool_->FreeConn(sql_);
        }
    }
private:
    MYSQL *sql_;
    SqlConnPool* connPool_;
};

#endif //WEBSERVER_SQLCONNRALL_H
