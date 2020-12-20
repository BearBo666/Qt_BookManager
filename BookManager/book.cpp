#include "book.h"
#include <QFile>
#include <QTextStream>

Book::Book(long long num,QString na,QString au,QString pre,QString lan,QString age,QString time,double pri,bool sta)
{
    number=num;
    name=na;
    author=au;
    press=pre;
    language=lan;
    suitable_age=age;
    pub_time=time;
    price=pri;
    is_lent=sta;
}

Book::Book(QString str){
    int i;
    QString strTemp[9];
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
    author=strTemp[2];
    price=strTemp[3].toDouble();
    language=strTemp[4];
    press=strTemp[5];
    pub_time=strTemp[6];
    is_lent=strTemp[7].toInt();
    suitable_age=strTemp[8];
}

void Book::Add(QFile * file){
    QTextStream stream(file);
    stream<<number<<" "<<name<<" "<<author<<" "<<price<<" "<<language<<" "<<press<<" "<<pub_time<<" "<<is_lent<<" "<<suitable_age<<endl;
}
