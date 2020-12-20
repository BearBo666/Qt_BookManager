#ifndef VIP_H
#define VIP_H

#include<QObject>
#include<QFile>

class Vip
{
public:
    //构造函数
    Vip(int ag,long long num,QString na,QString se,long long pho,bool sta);
    //从文件中读取数据时的构造函数
    Vip(QString str);
    //将Vip的输入写入文件
    void Add(QFile *file);

    //Vip的数据
    int age;
    QString name,sex;
    long long number,phone;
    bool is_Svip;
};

#endif // VIP_H
