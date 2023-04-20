
#include "mainwindow.h"
#include "ui_mainwindow.h"

//数据库初始化
void MainWindow::CreateDB()
{
    DB=QSqlDatabase::addDatabase("QSQLITE");
    DB.setDatabaseName("legacies.db");
    DB.open();
}

//对应数据表初始化
void MainWindow::InitMap()
{
    SubStrMap["固定生命值"]="dataSubSLife";
    SubStrMap["百分比生命值"]="dataSubBLife";
    SubStrMap["百分比防御力"]="dataSubBDef";
    SubStrMap["百分比攻击力"]="dataSubBAtk";
    SubStrMap["元素精通"]="dataSubElmstr";
    SubStrMap["元素充能效率"]="dataSubElmChg";
    SubStrMap["暴击率"]="dataSubCrtR";
    SubStrMap["暴击伤害"]="dataSubCrtD";
    SubStrMap["火元素伤害加成"]="dataSubOtR";
    SubStrMap["雷元素伤害加成"]="dataSubOtR";
    SubStrMap["岩元素伤害加成"]="dataSubOtR";
    SubStrMap["风元素伤害加成"]="dataSubOtR";
    SubStrMap["水元素伤害加成"]="dataSubOtR";
    SubStrMap["冰元素伤害加成"]="dataSubOtR";
    SubStrMap["草元素伤害加成"]="dataSubOtR";
    SubStrMap["物理伤害加成"]="dataSubOtR";
    SubStrMap["治疗加成"]="dataSubOtR";
    SubStrMap["固定攻击力"]="dataSubSAtk";

}

//初始化已有记录
void MainWindow::ReadId()
{
    qDebug()<<"readidSetup";
    QSqlQuery Query;
    QString Select("select 标识码 from List");
    if(Query.exec(Select))
    {
        qDebug()<<"读取";
    }
    while(Query.next())
        IdList.append(Query.value(0).toString());
}

//设置当前行
void MainWindow::ListInit(quint32 num)
{
    ui->LegacyList->setCurrentRow(num);
}

//生成记录函数
void MainWindow::GenerateInit()
{
    //选取主词条
    rand = QRandomGenerator::global()->generate()%500;
    qDebug()<<"主词条随机数"<<rand;
    QString Select("select 圣遗物类型, 圣遗物主词条, 概率区间数 from dataMain");
    QString Maintype;
    QString Mainstr;
    QSqlQuery Query;
    if(Query.exec(Select))
    {
        qDebug()<<"主词条选取初始化成功";
    }
    while(Query.next())
    {
        qDebug()<<"主词条区间"<<Query.value(2);
            if(Query.value(2).toUInt()>=rand+1)
        {
            Maintype=Query.value(0).toString();
            Mainstr=Query.value(1).toString();
            break;
        }
    }
    qDebug()<<Maintype;
    qDebug()<<Mainstr;

    //选取副词条
    QString SubstrDist[4];
    for(quint32 i=0;i<4;i++)
    {
        QString Search("select 副词条类型 from ");
        Search+=SubStrMap[Mainstr];
        if(Query.exec(Search))
        {
            qDebug()<<"选取副词条初始化正常";
        }
        rand = QRandomGenerator::global()->generate()%100;
        for(quint32 j=1;j<=rand+1;j++)
        {
            Query.next();
        }
        bool flag=true;
        for(quint32 j=0;j<i;j++)
        {
            if(Query.value(0).toString()==SubstrDist[j])
            {
                flag=false;
                break;
            }
        }
        if(flag==false)
        {
            i=i-1;
        }else
        {
            SubstrDist[i]=Query.value(0).toString();
        }
    }



    //读取副词条成长值,写入初始值
    float InitValue[4]={0,0,0,0};
    Select=("select 副词条类型, 成长值 from growth");
    for(quint32 i =0;i<4;i++)
    {
        float temp[4];
        Query.exec(Select);
        while(Query.next())
        {
            if(Query.value(0).toString()==SubstrDist[i])
            {
                qDebug()<<"选定的类型"<<Query.value(0).toString();
                for(quint32 j=0;j<4;j++)
                {
                    qDebug()<<"对应成长为"<<Query.value(1).toFloat();
                    temp[j]=Query.value(1).toFloat();
                    Query.next();
                }
                break;
            }
        }
        rand = QRandomGenerator::global()->generate()%4;
        InitValue[i]=temp[rand];
    }

    rand = QRandomGenerator::global()->generate()%5;
    if(rand>0)
        InitValue[3]=0;
    QUuid id = QUuid::createUuid();
    QString StrId=id.toString();
    IdList.append(StrId);

    //最终写入初始值
    QString Insert("insert into List values (?, ?, ?, ?,?, ?,?,?,?,?,?,?)");
    Query.prepare(Insert);
    Query.addBindValue(Maintype);
    Query.addBindValue(Mainstr);
    Query.addBindValue(SubstrDist[0]);
    Query.addBindValue(SubstrDist[1]);
    Query.addBindValue(SubstrDist[2]);
    Query.addBindValue(SubstrDist[3]);
    Query.addBindValue(InitValue[0]);
    Query.addBindValue(InitValue[1]);
    Query.addBindValue(InitValue[2]);
    Query.addBindValue(InitValue[3]);
    Query.addBindValue(StrId);
    Query.addBindValue(1);
    Query.exec();
    RenewList();
    ListInit(IdList.count()-1);
    quint32 Crrntl=ui->LegacyList->currentRow();
    ui->LegacyList->itemClicked(ui->LegacyList->item(Crrntl));
}

//删除记录函数
void MainWindow::Del()
{
    if(ui->LegacyList->count()!=0)
    {
        QSqlQuery Query;
        int CrrntLn = ui->LegacyList->currentRow();
        qDebug()<<CrrntLn;
        qDebug()<<"当前行数"<<CrrntLn;
            QString PrcssStr= IdList.at(CrrntLn);
        QString Del("delete from List where 标识码 = ?");
        Query.prepare(Del);
        Query.addBindValue(PrcssStr);
        if(Query.exec())
            qDebug()<<"删除成功";
        IdList.removeAt(CrrntLn);
        if(CrrntLn==IdList.size())
            CrrntLn--;
        RenewList(CrrntLn);
        //使得删除后信息会随时更新
        if(ui->LegacyList->count()>0)
        {
            quint32 Crrntl=ui->LegacyList->currentRow();
            ui->LegacyList->itemClicked(ui->LegacyList->item(Crrntl));
        }
    }else{
        QMessageBox::information(this,"警告","请先更新或者生成记录！");
    }
}

//升级函数
void MainWindow::Upgrade(QString ID)
{

    QString Select=QString("select 圣遗物类型, 圣遗物主词条, 圣遗物副词条1, 圣遗物副词条2,圣遗物副词条3,圣遗物副词条4,副词条数值1,副词条数值2,副词条数值3,副词条数值4, 等级 from List where 标识码 = '%1'").arg(ID);
    QSqlQuery Query;
    quint32 level;
    quint32 temp;

    if(Query.exec(Select))
    {
        qDebug()<<"升级启动正常";
    }
    Query.next();


    if(Query.value(9).toFloat()==0)
    {
        temp=3;
    }else
    {
        rand = QRandomGenerator::global()->generate()%4;
        temp=rand;
    }

    QString tmpconnect=QString::number(temp+1);
    QString tmpString="副词条数值"+tmpconnect;

    QString tmpString2= Query.value(temp+2).toString();
    float tmpValue=Query.value(temp+6).toFloat();
    level=Query.value(10).toInt();


    Select=("select 副词条类型, 成长值 from growth");
    Query.exec(Select);
    float tempval[4];
    while(Query.next())
    {
        if(Query.value(0).toString()==tmpString2)
        {
            for(quint32 j=0;j<4;j++)
            {
                tempval[j]=Query.value(1).toFloat();
                Query.next();
            }
        }
    }
    rand = QRandomGenerator::global()->generate()%4;
    qDebug()<<"成长值为"<<tempval[rand];
    tmpValue+=tempval[rand];
    if(level==1)
    {
        level=4;
    }else{
        level+=4;
    }

    QString Update=QString("UPDATE List SET %1 = '%2',等级 = '%3' WHERE 标识码 = '%4'").arg(tmpString).arg(tmpValue).arg(level).arg(ID);
    qDebug()<<Update;
    if(Query.exec(Update))
        qDebug()<<"升级正常";
    quint32 Crrntl=ui->LegacyList->currentRow();
    RenewList(Crrntl);
    ui->LegacyList->itemClicked(ui->LegacyList->item(Crrntl));


}

//刷新记录函数
void MainWindow::RenewList(quint32 num)
{

    QSqlQuery Query;
    ui->LegacyList->clear();
    for(quint32 i=0;i<IdList.size();i++)
    {

        QString Select=QString("select 圣遗物类型, 圣遗物主词条, 等级 from List where 标识码 = '%1'").arg(IdList.at(i));
        Query.exec(Select);

        Query.next();

        QString Output=Query.value(0).toString()+" "+Query.value(1).toString()+" "+QString::number(Query.value(2).toUInt())+"级";

        ui->LegacyList->addItem(Output);
        ListInit(num);
    }



}


//窗口构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CreateDB();
    ReadId();
    InitMap();
}

//窗口析构函数
MainWindow::~MainWindow()
{
    delete ui;
}

//生成记录按钮
void MainWindow::on_LegacyGen_clicked()
{
    GenerateInit();
}

//刷新记录按钮
void MainWindow::on_renew_clicked()
{
    RenewList();
}

//删除记录按钮
void MainWindow::on_DeleteOne_clicked()
{
    Del();
}


//可变的副词条信息显示
void MainWindow::on_LegacyList_itemClicked(QListWidgetItem *item)
{



    QSqlQuery Query;
    QString Sub1,Sub2,Sub3,Sub4;
    float Val1,Val2,Val3,Val4;
    QString Select=QString("select 圣遗物副词条1, 圣遗物副词条2, 圣遗物副词条3,圣遗物副词条4,副词条数值1, 副词条数值2, 副词条数值3,副词条数值4,等级 from List where 标识码 = '%1'").arg(IdList.at(ui->LegacyList->currentRow()));
    QString Output;
    if(Query.exec(Select))
        qDebug()<<"changeok";
    Query.next();
    Sub1=" "+Query.value(0).toString()+" ";
    Sub2=" "+Query.value(1).toString()+" ";
    Sub3=" "+Query.value(2).toString()+" ";
    Sub4=" "+Query.value(3).toString()+" ";

    Val1=Query.value(4).toFloat();
    Val2=Query.value(5).toFloat();
    Val3=Query.value(6).toFloat();
    Val4=Query.value(7).toFloat();

    qDebug()<<"检查val";
    qDebug()<<Val1;
    qDebug()<<Val2;
    qDebug()<<Val3;
    qDebug()<<Val4;

    //最大等级为20级
    if(Query.value(8).toUInt()==20)
    {
        ui->UpGrd->setEnabled(false);
    }else
    {
        ui->UpGrd->setEnabled(true);
    }

    if(Val4!=0)
    {
        Output=Sub1+QString::number(Val1,'f',1)+"\n"+Sub2+QString::number(Val2,'f',1)+"\n"+Sub3+QString::number(Val3,'f',1)+"\n"+Sub4+QString::number(Val4,'f',1);
    }else
    {
        Output=Sub1+QString::number(Val1,'f',1)+"\n"+Sub2+QString::number(Val2,'f',1)+"\n"+Sub3+QString::number(Val3,'f',1);
    }

    qDebug()<<Output;

    ui->SubInfo->setText(Output);

}

//升级按钮
void MainWindow::on_UpGrd_clicked()
{
    if(ui->LegacyList->count()!=0)
    {
        QString ID=IdList.at(ui->LegacyList->currentRow());
        Upgrade(ID);
    }else{
        QMessageBox::information(this,"警告","请先更新或者生成记录！");
    }

}

