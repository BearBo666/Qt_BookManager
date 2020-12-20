#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include "book.h"
#include "borrow.h"
#include "dialog.h"
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTextCodec>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*
     * 以下为页面基础布局渲染，以及数据的加载
    */
    ui->setupUi(this);
    //设置图标
    setWindowIcon(QIcon(":/res/Book.png"));

    //设置标题
    QString str="当前用户:admin";
    ui->label->setText(str);

    //从文件中读取图书数据
    QFile bookFile("./bookData.txt");
    //文件内容非空
    if(bookFile.size()!=0){
        bookFile.open(QIODevice::ReadOnly);
        while (!bookFile.atEnd()) {
            QByteArray arr=bookFile.readLine();
            //默认用gbk格式打开文件
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            QString str=codec->toUnicode(arr);
            Book b(str);
            if(b.name!=""&&b.number!=0)
                books.append(b);
        }
        bookFile.close();
    }

    //从文件中读取会员数据
    QFile vipFile("./vipData.txt");
    //文件内容非空
    if(vipFile.size()!=0){
        vipFile.open(QIODevice::ReadOnly);
        while (!vipFile.atEnd()) {
            QByteArray arr=vipFile.readLine();
            //默认用gbk格式打开文件
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            QString str=codec->toUnicode(arr);
            Vip v(str);
            if(v.name!=""&&v.number!=0)
                vips.append(v);
        }
        vipFile.close();
    }

    //从文件中读取借阅数据
    QFile borrowFile("./borrowData.txt");
    //文件内容非空
    if(borrowFile.size()!=0){
        borrowFile.open(QIODevice::ReadOnly);
        while (!borrowFile.atEnd()) {
            QByteArray arr=borrowFile.readLine();
            //默认用gbk格式打开文件
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            QString str=codec->toUnicode(arr);
            Borrow b(str);
            if(b.serialNum!=0)
                borrows.append(b);
        }
        vipFile.close();
    }

    //首页的树状节点设置
    QTreeWidgetItem * item=new QTreeWidgetItem(QStringList()<<"首页");
    item->setIcon(0,QIcon(":/res/Home.png"));
    ui->treeWidget->addTopLevelItem(item);
    li[0]=new QTreeWidgetItem(QStringList()<<"搜索");
    item->addChild(li[0]);

    //绘本管理的树状节点设置
    QTreeWidgetItem * item1=new QTreeWidgetItem(QStringList()<<"绘本管理");
    item1->setIcon(0,QIcon(":/res/Book.png"));    //设置图标
    ui->treeWidget->addTopLevelItem(item1);
    li[1]=new QTreeWidgetItem(QStringList()<<"录入绘本");
    li[2]=new QTreeWidgetItem(QStringList()<<"修改绘本信息");
    li[3]=new QTreeWidgetItem(QStringList()<<"删除绘本");
    li[4]=new QTreeWidgetItem(QStringList()<<"浏览绘本");
    item1->addChild(li[1]);
    item1->addChild(li[4]);
    item1->addChild(li[3]);
    item1->addChild(li[2]);

    //会员管理的树状节点设置
    QTreeWidgetItem * item2=new QTreeWidgetItem(QStringList()<<"会员管理");
    item2->setIcon(0,QIcon(":/res/Vip.png"));
    ui->treeWidget->addTopLevelItem(item2);
    li[5]=new QTreeWidgetItem(QStringList()<<"录入会员");
    li[6]=new QTreeWidgetItem(QStringList()<<"修改会员信息");
    li[7]=new QTreeWidgetItem(QStringList()<<"删除会员");
    li[8]=new QTreeWidgetItem(QStringList()<<"浏览会员");
    item2->addChild(li[5]);
    item2->addChild(li[8]);
    item2->addChild(li[7]);
    item2->addChild(li[6]);

    //借阅管理的树状节点设置
    QTreeWidgetItem * item3=new QTreeWidgetItem(QStringList()<<"借阅管理");
    item3->setIcon(0,QIcon(":/res/Borrow.png"));
    ui->treeWidget->addTopLevelItem(item3);
    li[9]=new QTreeWidgetItem(QStringList()<<"书本借阅");
    li[10]=new QTreeWidgetItem(QStringList()<<"书本归还");
    li[11]=new QTreeWidgetItem(QStringList()<<"删除信息");
    li[12]=new QTreeWidgetItem(QStringList()<<"浏览借阅信息");
    item3->addChild(li[9]);
    item3->addChild(li[11]);
    item3->addChild(li[10]);
    item3->addChild(li[12]);

    //统计分析的树状节点设置
    QTreeWidgetItem * item4=new QTreeWidgetItem(QStringList()<<"统计分析");
    item4->setIcon(0,QIcon(":/res/Stats.png"));
    ui->treeWidget->addTopLevelItem(item4);
    li[13]=new QTreeWidgetItem(QStringList()<<"会员借阅分析");
    li[14]=new QTreeWidgetItem(QStringList()<<"绘本借阅分析");
    li[15]=new QTreeWidgetItem(QStringList()<<"绘本年龄排名");
    item4->addChild(li[13]);
    item4->addChild(li[14]);
    item4->addChild(li[15]);

    //树状节点内容的设置
    ui->treeWidget->setStyleSheet("QTreeWidget::item{height:40px;}");  //设置树状节点的高度
    ui->treeWidget->header()->setVisible(false);    //隐藏树状表格的表头

    /*
     *   页面布局完成
    */

    //搜索按钮的实现
    connect(ui->SearchBtn,&QPushButton::clicked,[=](){
        //先清空信息显示再执行搜索功能
       ui->listWidget->clear();
       Search(ui->lineEdit->text());
    });

    //返回按钮的实现
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        if( QMessageBox::Save == QMessageBox::question(this,"是否保存更改?","返回之前是否保存数据?",QMessageBox::Save|QMessageBox::Cancel)){
            //执行数据保存操作

            QFile file1("./bookData.txt");
            file1.open(QIODevice::ReadWrite);
            for (int i=0;i<books.size();i++) {
                books[i].Add(&file1);
            }
            file1.close();

            QFile file2("./vipData.txt");
            file2.open(QIODevice::ReadWrite);
            for (int i=0;i<vips.size();i++) {
                vips[i].Add(&file2);
            }
            file2.close();

            QFile file3("./borrowData.txt");
            file3.open(QIODevice::ReadWrite);
            for (int i=0;i<borrows.size();i++) {
                borrows[i].Add(&file3);
            }
        }

        //关闭本窗口 返回登录窗口
        Widget * logIn=new Widget;
        logIn->show();
        this->close();
    });

    //输入标准的设置
    QDoubleValidator * v=new QDoubleValidator;
    QIntValidator * I=new QIntValidator;

    //控制价格输入框只能输入Double类型
    ui->priceEdit->setValidator(v);
    ui->priceEdit_2->setValidator(v);

    //控制编号输入框只能输入int型
    ui->bookNumber->setValidator(I);
    ui->suitable_ageEdit->setValidator(I);
    ui->suitable_ageEdit_2->setValidator(I);
    ui->numberEdit_2->setValidator(I);
    ui->bookNumber_4->setValidator(I);
    ui->vipNumber_4->setValidator(I);

    //绘本:添加
    connect(ui->addBookBtn,&QPushButton::clicked,[=](){
            if(!ui->nameEdit->text().isEmpty()&&!ui->authorEdit->text().isEmpty()&&!ui->pressEdit->text().isEmpty()&&!ui->languageEdit->text().isEmpty()&&!ui->pub_timeEdit->text().isEmpty())
            {
                long long num=books.size()==0?1:books[books.size()-1].number+1;
                Book b(num,ui->nameEdit->text(),ui->authorEdit->text(),ui->pressEdit->text(),ui->languageEdit->text(),ui->suitable_ageEdit->text(),ui->pub_timeEdit->text(),ui->priceEdit->text().toDouble(),ui->is_Lent->isChecked());
                books.append(b);
                //提示用户并清空输入框
                QMessageBox::information(this,"成功","绘本添加成功");
                ui->nameEdit->setText("");
                ui->authorEdit->setText("");
                ui->priceEdit->setText("");
                ui->languageEdit->setText("");
                ui->pressEdit->setText("");
                ui->pub_timeEdit->setText("");
                ui->suitable_ageEdit->setText("");
                ui->is_Lent->setChecked(false);
            }else {
                QMessageBox::warning(this,"警告","请先添加完信息");
            }
    });

    //绘本:删除
    connect(ui->deleteBookBtn,&QPushButton::clicked,[=](){
        //输入的要删除的编号
        int checkedNum=ui->bookNumber->text().toInt();
        for (int i=0;i<books.size();i++) {
            if(books[i].number==checkedNum){
                //实例化确定对话框
                Dialog * ensure=new Dialog(this);
                ensure->setWindowIcon(QIcon(":/res/Warning.png"));
                ensure->setMessage("确定删除吗？");
                ensure->show();
                connect(ensure,&Dialog::Ensure,[=](){
                    books[i].number=0;
                    books.remove(i);
                    //提示用户并清空输入框
                    QMessageBox::information(this,"成功","绘本删除成功");
                    ui->bookNumber->clear();
                });
                break;
            }
        }
        qDebug()<<books.size();
    });

    //绘本:修改
    ui->tip->hide();    //隐藏错误提示文字
    connect(ui->modifyBookBtn,&QPushButton::clicked,[=](){
        //只有存在的书本编号才能触发修改
        if(ui->tip->isHidden()){
            int key=ui->numberEdit_2->text().toInt();
            for (int i=0;i<books.size();i++) {
                if(books[i].number==key){
                    //修改对应绘本的信息
                    books[i].name=ui->nameEdit_2->text();
                    books[i].author=ui->authorEdit_2->text();
                    books[i].press=ui->pressEdit_2->text();
                    books[i].pub_time=ui->pub_timeEdit_2->text();
                    books[i].suitable_age=ui->suitable_ageEdit_2->text();
                    books[i].price=ui->priceEdit_2->text().toDouble();
                    books[i].language=ui->languageEdit_2->text();
                    books[i].is_lent=ui->is_Lent_2->isChecked();
                    //提示用户并清空输入框
                    QMessageBox::information(this,"成功","绘本信息修改成功!");
                    ui->numberEdit_2->clear();
                    ui->nameEdit_2->clear();
                    ui->authorEdit_2->clear();
                    ui->pressEdit_2->clear();
                    ui->suitable_ageEdit_2->clear();
                    ui->languageEdit_2->clear();
                    ui->priceEdit_2->clear();
                    ui->pub_timeEdit_2->clear();
                    ui->is_Lent_2->setChecked(false);
                    break;
                }
            }
        }
    });

    //会员:添加
    connect(ui->addVipBtn,&QPushButton::clicked,[=](){
        if(! ui->vipName->text().isEmpty() && ! ui->vipAge->text().isEmpty() && ! ui->vipSex->text().isEmpty() && ! ui->vipPhone->text().isEmpty()){
            long long num=vips.size()==0?1:vips[vips.size()-1].number+1;
            Vip v(ui->vipAge->text().toInt(),num,ui->vipName->text(),ui->vipSex->text(),ui->vipPhone->text().toLongLong(),ui->is_Svip->isChecked());
            vips.append(v);
            //提示用户并清空输入框
            QMessageBox::information(this,"成功","会员信息添加成功");
            ui->vipName->setText("");
            ui->vipSex->setText("");
            ui->vipPhone->setText("");
            ui->vipAge->setText("");
            ui->is_Svip->setChecked(false);
        }else {
            QMessageBox::warning(this,"警告","请先添加完信息");
        }
    });

    //会员:删除
    connect(ui->deleteVipBtn,&QPushButton::clicked,[=](){
        //输入的要删除的编号
        int checkedNum=ui->vipNumber_2->text().toInt();
        for (int i=0;i<vips.size();i++) {
            if(vips[i].number==checkedNum){
                //实例化确定对话框
                Dialog * ensure=new Dialog(this);
                ensure->setWindowIcon(QIcon(":/res/Warning.png"));
                ensure->setMessage("确定删除吗？");
                ensure->show();
                connect(ensure,&Dialog::Ensure,[=](){
                    vips[i].number=0;
                    vips.remove(i);
                    //提示用户并清空输入框
                    QMessageBox::information(this,"成功","信息删除成功");
                    ui->vipNumber_2->clear();
                });
                break;
            }
        }
        qDebug()<<vips.size();
    });

    //会员:修改
    ui->tip_2->hide();  //隐藏错误提示文字
    connect(ui->modifyVipBtn,&QPushButton::clicked,[=](){
        //只有存在的书本编号才能触发修改
        if(ui->tip_2->isHidden()){
            int key=ui->vipNumber_3->text().toInt();
            for (int i=0;i<vips.size();i++) {
                if(vips[i].number==key){
                    //信息修改
                    vips[i].name=ui->vipName_2->text();
                    vips[i].phone=ui->vipPhone_2->text().toLongLong();
                    vips[i].sex=ui->vipSex_2->text();
                    vips[i].age=ui->vipAge_2->text().toInt();
                    vips[i].is_Svip=ui->is_Svip_2->isChecked();
                    //提示用户并清空输入框
                    QMessageBox::information(this,"成功","会员信息修改成功!");
                    ui->vipName_2->clear();
                    ui->vipPhone_2->clear();
                    ui->vipSex_2->clear();
                    ui->vipAge_2->clear();
                    ui->is_Svip_2->setChecked(false);
                    break;
                }
            }
        }
    });

    //图书借阅
    //隐藏输入错误的提示
    ui->tip_3->hide();
    ui->tip_4->hide();
    connect(ui->borrowAddBtn,&QPushButton::clicked,[=](){
        //无错误提示说明输入正确,存在该图书与会员
        if(ui->tip_3->isHidden()&&ui->tip_4->isHidden()){
            //输入的图书号与会员号
            long long bookNum=ui->bookNumber_4->text().toLongLong();
            long long vipNum=ui->vipNumber_4->text().toLongLong();
            //生成借阅信息
            long long num=borrows.size()==0?1:borrows[borrows.size()-1].serialNum+1;
            Borrow b(num,bookNum,vipNum,QDateTime::currentDateTime());

            //遍历找到要借阅的书籍
            int key=0;
            for (int i=0;i<books.size();i++) {
                if(books[i].number==bookNum)
                    key=i;
            }
            //判断书籍当前是否可借
            if(books[key].is_lent){
                //判断该会员是否是超级会员
                if(b.TheVip(vips).is_Svip){
                    //是超级会员,借阅成功
                    borrows.append(b);
                    //将该书置为不可借状态
                    books[key].is_lent=false;
                    //提示用户并清空输入框
                    QString mess="借阅编号:"+QString::number(b.serialNum)+",借阅成功!";
                    QMessageBox::information(this,"提示",mess);
                    ui->bookNumber_4->clear();
                    ui->vipNumber_4->clear();
                }else {
                    //不是超级会员,判断是否借了十本书
                    if(borrows.size()<=10){
                        borrows.append(b);
                        //将该书置为不可借状态
                        books[key].is_lent=false;
                        //提示用户并清空输入框
                        QString mess="借阅编号:"+QString::number(b.serialNum)+",借阅成功!";
                        QMessageBox::information(this,"提示",mess);
                        ui->bookNumber_4->clear();
                        ui->vipNumber_4->clear();
                    }else{
                        int count=0;
                        //遍历借阅信息,计算该会员未归还的书籍数量
                        for (int i=0;i<borrows.size();i++) {
                            if(borrows[i].vipNum==vipNum&&borrows[i].isBack==false)
                                count++;
                        }
                        if(count>=10){
                            QMessageBox::warning(this,"失败","非超级会员仅可借阅十本书,请先归还书籍或者充值超级会员!");
                        }else {
                            borrows.append(b);
                            //将该书置为不可借状态
                            books[key].is_lent=false;
                            //提示用户并清空输入框
                            QString mess="借阅编号:"+QString::number(b.serialNum)+",借阅成功!";
                            QMessageBox::information(this,"提示",mess);
                            ui->bookNumber_4->clear();
                            ui->vipNumber_4->clear();
                        }
                    }
                }
            }else{
                QMessageBox::warning(this,"失败","此书当前不可借!");
            }
        }else{
            QMessageBox::warning(this,"警告","请填写正确的编号");
        }
    });

    //图书归还
    connect(ui->bookReturnBtn,&QPushButton::clicked,[=](){
        //是否找到借阅信息的标志
        int key=-1;

        //获取要归还的借阅类的编号
        long long checkedNum=ui->bookReturn->text().toLongLong();

        if(borrows.size()==0){
            QMessageBox::warning(this,"提示","无此借阅消息");
            return;
        }

        //遍历借阅信息
        for (int i=0;i<borrows.size();i++) {
            if(borrows[i].serialNum==checkedNum){
                //有此借阅信息,执行归还操作
                if(borrows[i].isBack){
                    //该书已经归还,提示用户
                    QMessageBox::warning(this,"警告","已经归还无需再归还");
                }else{
                    //该书并未归还,执行归还
                    key=i; //记录下此借阅信息的下标

                    //遍历找到要借阅的书籍
                    int flag=0;
                    for (int i=0;i<books.size();i++) {
                        if(books[i].number==borrows[key].bookNum){
                            flag=i;
                            break;
                        }
                    }
                    //将书籍重置为可借状态
                    books[flag].is_lent=true;

                    //记录下归还的时间 并对用户进行提示并清空输入框
                    borrows[i].isBack=true;
                    borrows[i].endTime=QDateTime::currentDateTime();
                    QString mess="归还成功,欠款"+QString::number(borrows[i].debt(vips))+"元!";
                    QMessageBox::information(this,"提示",mess);
                    ui->bookReturn->clear();
                }
                break;
            }
        }
        //未找到借阅信息
        if(key==-1){
            QMessageBox::warning(this,"提示","无此借阅消息");
        }
    });

    //信息删除
    connect(ui->borrowDeleBtn,&QPushButton::clicked,[=](){
        //输入的要删除的编号
        int checkedNum=ui->borrowDelete->text().toInt();
        for (int i=0;i<borrows.size();i++) {
            if(borrows[i].serialNum==checkedNum){
                //实例化确定对话框
                Dialog * ensure=new Dialog(this);
                ensure->setWindowIcon(QIcon(":/res/Warning.png"));
                ensure->setMessage("确定删除吗？");
                ensure->show();
                connect(ensure,&Dialog::Ensure,[=](){
                    borrows[i].serialNum=0;
                    borrows.remove(i);
                    QMessageBox::information(this,"成功","借阅信息删除成功!");
                });
                break;
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//渲染图书信息表格
void MainWindow::BookList(){
    //设置行数,列数
    ui->tableWidget->setRowCount(books.size());
    ui->tableWidget->setColumnCount(9);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"编号"<<"书名"<<"作者"<<"语言"<<"价格"<<"出版社"<<"适合年龄"<<"出版时间"<<"是否可借");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setMinimumSectionSize(50);
    //将books信息渲染到列表上
    if(books.size()==0) return;
    for (int i=0;i<books.size();i++) {
        //内容渲染
        QString price="￥"+QString::number(books[i].price);
        QString str=books[i].is_lent?"可借":"不可借";
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(books[i].number)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(books[i].name));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(books[i].author));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(books[i].language));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(price));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(books[i].press));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(QString::number(books[i].suitable_age.toInt())));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(books[i].pub_time));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(str));
        //设置内容居中显示
        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,8)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//渲染会员信息表格
void MainWindow::VipList(){
    //设置行数,列数,表头
    ui->tableWidget_2->setRowCount(vips.size());
    ui->tableWidget_2->setColumnCount(6);
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList()<<"编号"<<"会员名"<<"性别"<<"年龄"<<"电话"<<"身份");
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //将vips信息渲染到列表上
    if(vips.size()==0) return;
    for (int i=0;i<vips.size();i++) {
        //内容渲染
        QString str=vips[i].is_Svip?"高级会员":"普通会员";
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString::number(vips[i].number)));
        ui->tableWidget_2->setItem(i,1,new QTableWidgetItem(vips[i].name));
        ui->tableWidget_2->setItem(i,2,new QTableWidgetItem(vips[i].sex));
        ui->tableWidget_2->setItem(i,3,new QTableWidgetItem(QString::number(vips[i].age)));
        ui->tableWidget_2->setItem(i,4,new QTableWidgetItem(QString::number(vips[i].phone)));
        ui->tableWidget_2->setItem(i,5,new QTableWidgetItem(str));
        //设置内容居中显示
        ui->tableWidget_2->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_2->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_2->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_2->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_2->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_2->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//渲染借阅信息表格
void MainWindow::BorrowList(){
    //设置行数,列数,表头
    ui->tableWidget_3->setRowCount(borrows.size());
    ui->tableWidget_3->setColumnCount(9);
    ui->tableWidget_3->setHorizontalHeaderLabels(QStringList()<<"编号"<<"图书号"<<"图书名"<<"会员号"<<"会员名"<<"借阅时间"<<"归还时间"<<"是否归还"<<"欠款金额");
    ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(borrows.size()==0) return;
    for (int i=0;i<borrows.size();i++) {
        QString book="《"+borrows[i].TheBook(books).name+"》";
        QString isBack=borrows[i].isBack?"已归还":"未归还";
        QString start=borrows[i].startTime.toString("yyyy-MM-dd");
        QString back=borrows[i].isBack?borrows[i].endTime.toString("yyyy-MM-dd"):"";
        ui->tableWidget_3->setItem(i,0,new QTableWidgetItem(QString::number(borrows[i].serialNum)));
        ui->tableWidget_3->setItem(i,1,new QTableWidgetItem(QString::number(borrows[i].bookNum)));
        ui->tableWidget_3->setItem(i,2,new QTableWidgetItem(book));
        ui->tableWidget_3->setItem(i,3,new QTableWidgetItem(QString::number(borrows[i].vipNum)));
        ui->tableWidget_3->setItem(i,4,new QTableWidgetItem(borrows[i].TheVip(vips).name));
        ui->tableWidget_3->setItem(i,5,new QTableWidgetItem(start));
        ui->tableWidget_3->setItem(i,6,new QTableWidgetItem(back));
        ui->tableWidget_3->setItem(i,7,new QTableWidgetItem(isBack));
        ui->tableWidget_3->setItem(i,8,new QTableWidgetItem(QString::number(borrows[i].debt(vips))));
        //设置内容居中显示
        ui->tableWidget_3->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_3->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//渲染会员排名表格
void MainWindow::VipRank(){
    //设置行数,列数,表头
    ui->tableWidget_4->setRowCount(vips.size());
    ui->tableWidget_4->setColumnCount(4);
    ui->tableWidget_4->setHorizontalHeaderLabels(QStringList()<<"排名"<<"会员编号"<<"会员姓名"<<"借阅次数");
    ui->tableWidget_4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(vips.size()==0) return;
    //计算每个会员的借阅次数并渲染
    int count[vips.size()];
    for(int i=0;i<vips.size();i++){
        count[i]=0;
        for (int j=0;j<borrows.size();j++) {
            if(borrows[j].vipNum==vips[i].number){
                count[i]++;
            }
        }
    }
    QVector<Vip>vips2=vips;
    for (int i=0;i<vips2.size()-1;i++) {
        for (int j=i+1;j<vips2.size();j++) {
            if(count[j]>count[i]){
                int t=count[j];
                count[j]=count[i];
                count[i]=t;
                Vip v=vips2[j];
                vips2[j]=vips2[i];
                vips2[i]=v;
            }
        }
    }
    for (int i=0;i<vips2.size();i++) {
        ui->tableWidget_4->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->tableWidget_4->setItem(i,1,new QTableWidgetItem(QString::number(vips2[i].number)));
        ui->tableWidget_4->setItem(i,2,new QTableWidgetItem(vips2[i].name));
        ui->tableWidget_4->setItem(i,3,new QTableWidgetItem(QString::number(count[i])));
        //设置内容居中显示
        ui->tableWidget_4->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_4->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_4->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_4->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//渲染图书排名表格
void MainWindow::BookRank(){
    //设置行数,列数,表头
    ui->tableWidget_5->setRowCount(books.size());
    ui->tableWidget_5->setColumnCount(4);
    ui->tableWidget_5->setHorizontalHeaderLabels(QStringList()<<"排名"<<"绘本编号"<<"绘本名称"<<"借阅次数");
    ui->tableWidget_5->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(books.size()==0) return;
    //计算每个绘本的借阅次数并渲染
    int count[books.size()];
    for (int i=0;i<books.size();i++) {
        count[i]=0;
        for (int j=0;j<borrows.size();j++) {
            if(borrows[j].bookNum==books[i].number){
                count[i]++;
            }
        }
    }
    QVector<Book>books2=books;
    for (int i=0;i<books2.size()-1;i++) {
        for (int j=i+1;j<books2.size();j++) {
            if(count[j]>count[i]){
                int t=count[j];
                count[j]=count[i];
                count[i]=t;
                Book b=books2[j];
                books2[j]=books2[i];
                books2[i]=b;
            }
        }
    }
    for (int i=0;i<books2.size();i++) {
        ui->tableWidget_5->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->tableWidget_5->setItem(i,1,new QTableWidgetItem(QString::number(books2[i].number)));
        ui->tableWidget_5->setItem(i,2,new QTableWidgetItem(books2[i].name));
        ui->tableWidget_5->setItem(i,3,new QTableWidgetItem(QString::number(count[i])));
        //设置内容居中显示
        ui->tableWidget_5->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_5->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_5->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_5->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//渲染图书排名表格(按适合年龄排名)
void MainWindow::BookRank2(){
    //设置行数,列数,表头
    ui->tableWidget_6->setRowCount(books.size());
    ui->tableWidget_6->setColumnCount(5);
    ui->tableWidget_6->setHorizontalHeaderLabels(QStringList()<<"排名"<<"绘本编号"<<"绘本名称"<<"作者名称"<<"适合年龄");
    ui->tableWidget_6->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_6->verticalHeader()->setMinimumSectionSize(50);
    if(books.size()==0) return;
    //绘本排序
    QVector<Book>books2=books;
    for (int i=0;i<books2.size()-1;i++) {
        for (int j=i+1;j<books2.size();j++) {
            if(books2[j].suitable_age.toInt()<books2[i].suitable_age.toInt()){
                Book t=books2[j];
                books2[j]=books2[i];
                books2[i]=t;
            }
        }
    }
    //绘本渲染
    for (int i=0;i<books2.size();i++) {
        ui->tableWidget_6->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->tableWidget_6->setItem(i,1,new QTableWidgetItem(QString::number(books2[i].number)));
        ui->tableWidget_6->setItem(i,2,new QTableWidgetItem(books2[i].name));
        ui->tableWidget_6->setItem(i,3,new QTableWidgetItem(books2[i].author));
        ui->tableWidget_6->setItem(i,4,new QTableWidgetItem(QString::number(books2[i].suitable_age.toInt())));
        //设置内容居中显示
        ui->tableWidget_6->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_6->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_6->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_6->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_6->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//给树状节点添加双击事件
void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item->text(column)==li[0]->text(0)){
        //跳转到第0页
        ui->stackedWidget->setCurrentIndex(0);
    }else if(item->text(column)==li[1]->text(0)){
        //跳转到第1页
        ui->stackedWidget->setCurrentIndex(1);
    }else if(item->text(column)==li[4]->text(0)){
        //跳转到第2页
        ui->stackedWidget->setCurrentIndex(2);
        BookList();
    }else if(item->text(column)==li[3]->text(0)){
        //跳转到第3页
        ui->stackedWidget->setCurrentIndex(3);
    }else if(item->text(column)==li[2]->text(0)){
        //跳转到第4页
        ui->stackedWidget->setCurrentIndex(4);
    }else if(item->text(column)==li[5]->text(0)){
        //跳转到第5页
        ui->stackedWidget->setCurrentIndex(5);
    }else if(item->text(column)==li[8]->text(0)){
        //跳转到第6页
        ui->stackedWidget->setCurrentIndex(6);
        VipList();
    }else if(item->text(column)==li[7]->text(0)){
        //跳转到第7页
        ui->stackedWidget->setCurrentIndex(7);
    }else if (item->text(column)==li[6]->text(0)) {
        //跳转到第8页
        ui->stackedWidget->setCurrentIndex(8);
    }else if (item->text(column)==li[9]->text(0)) {
        //跳转到第9页
        ui->stackedWidget->setCurrentIndex(9);
    }else if (item->text(column)==li[11]->text(0)) {
        //跳转到第10页
        ui->stackedWidget->setCurrentIndex(10);
    }else if (item->text(column)==li[10]->text(0)) {
        //跳转到第11页
        ui->stackedWidget->setCurrentIndex(11);
    }else if (item->text(column)==li[12]->text(0)) {
        //跳转到第12页
        ui->stackedWidget->setCurrentIndex(12);
        BorrowList();
    }else if (item->text(column)==li[13]->text(0)) {
        //跳转到第13页
        ui->stackedWidget->setCurrentIndex(13);
        VipRank();
    }else if (item->text(column)==li[14]->text(0)) {
        //跳转到第14页
        ui->stackedWidget->setCurrentIndex(14);
        BookRank();
    }else if (item->text(column)==li[15]->text(0)) {
        //跳转到第15页
        ui->stackedWidget->setCurrentIndex(15);
        BookRank2();
    }
}

//修改绘本页面的编号校验
void MainWindow::on_numberEdit_2_editingFinished()
{
    int key=-1;
    for (int i=0;i<books.size();i++) {
        if(books[i].number==ui->numberEdit_2->text().toInt()){
            key=i;
            break;
        }
    }
    if(key!=-1){
        ui->tip->hide();
        ui->nameEdit_2->setText(books[key].name);
        ui->authorEdit_2->setText(books[key].author);
        ui->priceEdit_2->setText(QString::number(books[key].price));
        ui->languageEdit_2->setText(books[key].language);
        ui->pressEdit_2->setText(books[key].press);
        ui->pub_timeEdit_2->setText(books[key].pub_time);
        ui->suitable_ageEdit_2->setText(books[key].suitable_age);
        ui->is_Lent_2->setChecked(books[key].is_lent);
    }else {
        ui->tip->show();
        ui->nameEdit_2->setText("");
        ui->authorEdit_2->setText("");
        ui->priceEdit_2->setText("");
        ui->languageEdit_2->setText("");
        ui->pressEdit_2->setText("");
        ui->pub_timeEdit_2->setText("");
        ui->suitable_ageEdit_2->setText("");
        ui->is_Lent_2->setChecked(false);
    }
}

//修改会员页面的编号校验
void MainWindow::on_vipNumber_3_editingFinished()
{
    int key=-1;
    for (int i=0;i<vips.size();i++) {
        if(vips[i].number==ui->vipNumber_3->text().toInt()){
            key=i;
            break;
        }
    }
    if(key!=-1){
        ui->tip_2->hide();
        ui->vipAge_2->setText(QString::number(vips[key].age));
        ui->vipName_2->setText(vips[key].name);
        ui->vipSex_2->setText(vips[key].sex);
        ui->vipPhone_2->setText(QString::number(vips[key].phone));
       ui->is_Svip_2->setChecked(vips[key].is_Svip);
    }else {
        ui->tip_2->show();
        ui->vipAge_2->setText("");
        ui->vipName_2->setText("");
        ui->vipSex_2->setText("");
        ui->vipPhone_2->setText("");
       ui->is_Svip_2->setChecked(false);
    }
}

//绘本借阅页面的会员号校验
void MainWindow::on_vipNumber_4_editingFinished()
{
    int key=-1;
    if(vips.size()==0){
        ui->tip_3->show();
        return;
    }
    for (int i=0;i<vips.size();i++) {
        if(vips[i].number==ui->vipNumber_4->text().toInt()){
            key=i;
            break;
        }
    }
    if(key!=-1){
        ui->tip_3->hide();
    }else {
        ui->tip_3->show();
    }
}

//绘本借阅页面的会员号校验
void MainWindow::on_bookNumber_4_editingFinished()
{
    int key=-1;
    if(books.size()==0){
        ui->tip_4->show();
        return;
    }
    for (int i=0;i<books.size();i++) {
        if(books[i].number==ui->bookNumber_4->text().toInt()){
            key=i;
            break;
        }}
    if(key!=-1){
        ui->tip_4->hide();
    }else {
        ui->tip_4->show();
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    //先清空后搜索
    ui->listWidget->clear();
    Search(ui->lineEdit->text());
}

void MainWindow::Search(QString str){
    //搜索图书信息
    for (int i=0;i<books.size();i++) {
        //图书编号,名称或作者包含输入框内容时进行渲染
        if(QString::number(books[i].number).contains(str,Qt::CaseSensitive)||books[i].name.contains(str,Qt::CaseSensitive)||books[i].author.contains(str,Qt::CaseSensitive)){
            QString isLent=books[i].is_lent?"可借":"不可借";
            //设置渲染的信息
            QString mess="图书编号:"+QString::number(books[i].number)+"  《"+books[i].name+"》  "+books[i].author+"  "+books[i].press+"  "+isLent;
            QListWidgetItem *item=new QListWidgetItem(mess);
            //设置宽高
            item->setSizeHint(QSize(ui->listWidget->width(),50));
            //设置字体
            item->setFont(QFont("宋体",12));
            //设置对齐方式
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->listWidget->addItem(item);
        }
    }
    //搜索会员信息
    for (int i=0;i<vips.size();i++) {
        //会员编号,姓名或电话包含输入框内容时进行渲染
        if(QString::number(vips[i].number).contains(str,Qt::CaseSensitive)||vips[i].name.contains(str,Qt::CaseSensitive)||QString::number(vips[i].phone).contains(str,Qt::CaseSensitive)){
            QString isSvip=vips[i].is_Svip?"高级会员":"普通会员";
            //设置渲染的信息
            QString mess="会员编号:"+QString::number(vips[i].number)+"  "+vips[i].name+"  "+QString::number(vips[i].phone)+"  "+isSvip;
            QListWidgetItem *item=new QListWidgetItem(mess);
            //设置宽高
            item->setSizeHint(QSize(ui->listWidget->width(),50));
            //设置字体
            item->setFont(QFont("宋体",12));
            //设置对齐方式
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->listWidget->addItem(item);
        }
    }
    //搜索借阅信息
    for (int i=0;i<borrows.size();i++) {
        //会员编号,图书编号包含输入框内容时进行渲染
        if(QString::number(borrows[i].serialNum).contains(str,Qt::CaseSensitive)||QString::number(borrows[i].bookNum).contains(str,Qt::CaseSensitive)||QString::number(borrows[i].vipNum).contains(str,Qt::CaseSensitive)){
            //设置渲染的信息
            QString mess="借阅编号:"+QString::number(borrows[i].serialNum)+"    图书编号:"+QString::number(borrows[i].bookNum)+"  《"+borrows[i].TheBook(books).name+"》  "+"会员编号:"+QString::number(borrows[i].vipNum);
            QListWidgetItem *item=new QListWidgetItem(mess);
            //设置宽高
            item->setSizeHint(QSize(ui->listWidget->width(),50));
            //设置字体
            item->setFont(QFont("宋体",12));
            //设置对齐方式
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->listWidget->addItem(item);
        }
    }
}
