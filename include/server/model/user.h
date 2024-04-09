#ifndef USER_H
#define USER_H

#include<string>
using namespace std;

class User
{
    public:
        //构造函数
        User(int id=0,string name="",string password="",string state="offline")
        {
            this->id=id;
            this->name=name;
            this->password=password;    
            this->state=state;
        }

        void setID(int id)
        {
            this->id=id;
        }
        void setName(string name)
        {
                this->name=name;
        }
        void setPwd(string password)
        {
                this->password=password;
        }
        void setState(string state)
        {
            this->state=state;
        }

        int getID()
        {
            return id;
        }
        string getName()
        {
            return name;
        }
        string getPwd()
        {
            return password;
        }
        string getState()
        {
            return state;
        }       



    private:
        int id;
        string name;
        string password;
        string state;
};




#endif