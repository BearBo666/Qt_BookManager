#ifndef BOOK_H
#define BOOK_H

#include <QObject>
#include<QFile>

class Book
{  
public:
   //构造函数
   Book(long long num,QString na,QString au,QString pre,QString lan,QString age,QString time,double pri,bool sta);
   //从文件中读取时的构造函数
   Book(QString str);
   //将Book的数据添加到文件中
   void Add(QFile *file);

   //Book的数据
   long long number;
   QString name,author,press,language,suitable_age,pub_time;
   double price;
   bool is_lent;
};

#endif // BOOK_H
