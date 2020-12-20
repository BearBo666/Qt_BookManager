#include "vip.h"
#include <QTextStream>
Vip::Vip(int ag,long long num,QString na,QString se,long long pho,bool sta)
{
    age=ag;
    number=num;
    name=na;
    sex=se;
    phone=pho;
    is_Svip=sta;
}

Vip::Vip(QString str){
    int i;
    QString strTemp[6];
    QString::const_iterator cit=nullptr;
    for (cit=str.cbegin(),i=0;cit<str.cend();cit++) {
        strTemp[i]+=(*cit);
        if((*cit)==" "){
            //去掉最后一个空格
            strTemp[i].chop(1);
            i++;
        }
    }
    number=strTemp[0].toLongLong();
    name=strTemp[1];
    sex=strTemp[2];
    age=strTemp[3].toInt();
    phone=strTemp[4].toLongLong();
    is_Svip=strTemp[5].toInt();
}

void Vip::Add(QFile *file){
    QTextStream stream(file);
    stream<<number<<" "<<name<<" "<<sex<<" "<<age<<" "<<phone<<" "<<is_Svip<<endl;
}
