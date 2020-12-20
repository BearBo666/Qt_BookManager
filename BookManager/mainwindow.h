#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include "book.h"
#include "vip.h"
#include "borrow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //构造与析构
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //搜索后执行渲染
    void Search(QString str);
    //渲染图书数据的方法
    void BookList();
    //渲染会员数据的方法
    void VipList();
    //渲染借阅数据的方法
    void BorrowList();
    //会员借阅分析的渲染方法
    void VipRank();
    //图书借阅分析的渲染方法
    void BookRank();
    //图书适合年龄排名的渲染方法
    void BookRank2();

    //保存左侧树状节点
    QTreeWidgetItem * li[16];
    //保存图书数据
    QVector<Book> books;
    //保存会员信息
    QVector<Vip> vips;
    //保存借阅信息
    QVector<Borrow> borrows;

private slots:
    //树状节点的点击事件
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    //各输入框输入完的校验函数
    void on_numberEdit_2_editingFinished();

    void on_vipNumber_3_editingFinished();

    void on_vipNumber_4_editingFinished();

    void on_bookNumber_4_editingFinished();

    //首页搜索框按下enter键发出此信号
    void on_lineEdit_returnPressed();

public:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
