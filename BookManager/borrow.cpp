#include "borrow.h"
#include "vip.h"
#include <QDateTime>
#include <QTextStream>

Borrow::Borrow(long long num,long long book,long long vip,QDateTime start)
{
    serialNum=num;
    bookNum=book;
    vipNum=vip;
    startTime=start;
}

Borrow::Borrow(QString str){
    int i;
    QString strTemp[6];
    QString::const_iterator cit=nullptr;
    for (cit=str.cbegin(),i=0;cit<str.cend();cit++) {
        strTemp[i]+=(*cit);
        if((*cit)==" "){
            strTemp[i].chop(1);
            i++;
        }
    }
    serialNum=strTemp[0].toLongLong();
    bookNum=strTemp[1].toLongLong();
    vipNum=strTemp[2].toLongLong();
    startTime=QDateTime::fromString(strTemp[3],"yyyy-MM-ddhh:mm:ss");
    isBack=strTemp[5].toInt();
    endTime=isBack?QDateTime::fromString(strTemp[4],"yyyy-MM-ddhh:mm:ss"):endTime;
}

void Borrow::Add(QFile *file){
    QTextStream stream(file);
    if(isBack){
        stream<<serialNum<<" "<<bookNum<<" "<<vipNum<<" "<<startTime.toString("yyyy-MM-ddhh:mm:ss")<<" "<<endTime.toString("yyyy-MM-ddhh:mm:ss")<<" "<<isBack<<endl;
    }
    else{
        stream<<serialNum<<" "<<bookNum<<" "<<vipNum<<" "<<startTime.toString("yyyy-MM-ddhh:mm:ss")<<" "<<0<<" "<<isBack<<endl;
    }
}

Vip& Borrow::TheVip(QVector<Vip>vips){
    for(int i=0;i<vips.size();i++){
        if(vips[i].number==vipNum){
            return vips[i];
        }
    }
    return vips[vips.size()-1];
}

Book& Borrow::TheBook(QVector<Book> books){
    for (int i=0;i<books.size();i++) {
        if(books[i].number==bookNum){
            return books[i];
        }
    }
    return books[books.size()-1];
}


qint64 Borrow::debt(QVector<Vip> vips){
    QString mess;
    if(TheVip(vips).is_Svip){
        return 0;
    }else{
        qint64 bet=startTime.daysTo(endTime);
        if(bet>30){
            return  bet-30;
        }else{
            return 0;
        }
    }
}
