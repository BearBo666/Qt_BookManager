#ifndef BORROW_H
#define BORROW_H

#include <QObject>
#include <QDateTime>
#include "vip.h"
#include "book.h"

class Borrow
{
public:
    //构造函数
    Borrow(long long num,long long book,long long vip,QDateTime start);
    //从文件中读取数据时的构造函数
    Borrow(QString str);
    //将输入写入文件的函数
    void Add(QFile *file);
    //会员号对应的会员
    Vip& TheVip(QVector<Vip>vips);
    //计算会员欠款金额
    qint64 debt(QVector<Vip>vips);
    //图书号对应的图书
    Book& TheBook(QVector<Book>books);

    //图书号 会员号 借阅信息的流水号
    long long bookNum,vipNum,serialNum;
    //借阅图书的起始和归还时间
    QDateTime startTime,endTime;
    //是否归还了图书
    bool  isBack=false;
};

#endif // BORROW_H
