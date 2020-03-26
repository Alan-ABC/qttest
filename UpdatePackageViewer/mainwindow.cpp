#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDomDocument>
#include <qdebug.h>
#include <QXmlStreamReader>
#include <QFile>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTableView>
#include <qtwidgetsglobal.h>
#include <qsizepolicy.h>
#include <QHeaderView>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    prefix("")
{
    bInst = new QList<bool>();
    ui->setupUi(this);

    InitConstrols();

    ui->tabsVersion->clear();

    //setCentralWidget(this);

    /*QTableView* table = new QTableView(tab1);

    table->resize(QSize(300, 200));
    QWidget* tab2 = new QWidget(ui->tabsVersion);

    ui->tabsVersion->addTab(tab2, icon, QString::fromUtf8("ccccc"));
    QWidget* tab3 = new QWidget(ui->tabsVersion);
    ui->tabsVersion->addTab(tab3, icon, QString::fromUtf8("ddddd"));
    QWidget* tab4 = new QWidget(ui->tabsVersion);
    ui->tabsVersion->addTab(tab4, icon, QString::fromUtf8("eee"));*/
}

MainWindow::~MainWindow()
{
    for (int i = List.count() - 1; i >= 0; --i)
    {
        //delete List->at(i);
        List.removeAt(i);
    }
    delete ui;
}

void MainWindow::InitConstrols()
{
    connect(ui->tabsVersion, SIGNAL(currentChanged(int)), this, SLOT(onTabchanged(int)));
}

QString MainWindow::GetFilePath(QString fileName)
{
    QString filePath = QApplication::applicationDirPath() + "/" + fileName;
    return filePath;
}

void MainWindow::AddTabPage(int pageIdx, QString fileName)
{
    if (pageIdx < 0 && pageIdx >= bInst->count())
    {
        return;
    }

    if (bInst->at(pageIdx) == true)
    {
        return;
    }

    bInst->replace(pageIdx, true);

    LoadConfig(pageIdx, fileName);

    //qDebug()<< QTime::currentTime().msecsSinceStartOfDay();
    //ui->tabsVersion->
    QGridLayout* rgid = new QGridLayout(widget.at(pageIdx));
    QTableView* tView = new QTableView(widget.at(pageIdx));
    rgid->addWidget(tView, 0, 0, 1, 1);
    QStandardItemModel* model = new QStandardItemModel(tView);
    tView->setGeometry(0, 0, 1000, 800);
    //tView->setSizePolicy(QSizePolicy::Expanding);
    tView->setModel(model);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("%1").arg("             名称         　"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("%1").arg("              Md5         　"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("%1").arg("大小　"));
    model->setHeaderData(3, Qt::Horizontal, QString::fromUtf8("%1").arg("状态　"));
    //tView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tView->setColumnWidth(0, 400);
    tView->setColumnWidth(1, 400);
    tView->setColumnWidth(2, 50);
    tView->setColumnWidth(3, 50);

    QList<AssetData*>* temp = nullptr;

    if (pageIdx > 0)
    {
        temp = DiffVersionByPreview(pageIdx - 1, pageIdx);
    }
    else
    {
        temp = List.at(pageIdx);
    }


    for (int i = 0; i < temp->count(); ++i)
    {
        model->setItem(i, 0, new QStandardItem(temp->at(i)->name));
        model->setItem(i, 1, new QStandardItem(temp->at(i)->md5));
        model->setItem(i, 2, new QStandardItem(temp->at(i)->size));

        int state = temp->at(i)->state;
        if (state == 1)
        {
            model->setItem(i, 3, new QStandardItem(QString::fromUtf8("增加")));
        }
        else if (state == 2)
        {
            model->setItem(i, 3, new QStandardItem(QString::fromUtf8("更新")));
        }
        else
        {
            model->setItem(i, 3, new QStandardItem(QString::fromUtf8("增加")));
        }

    }

    if (pageIdx > 0)
    {
        delete temp;
    }

    tView->resizeColumnsToContents();
    tView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    //tView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //tView->setMaximumSize(QSize(10000, 10000));
    tView->show();
    //qDebug()<<QTime::currentTime().msecsSinceStartOfDay();
}

QList<AssetData*>* MainWindow::DiffVersionByPreview(int pre, int next)
{
    QList<AssetData*>* updateItems = new QList<AssetData*>();

    for (int i = 0; i < List.at(next)->count(); ++i)
    {
        bool hasItem = false;
        AssetData* itemNew = List.at(next)->at(i);

        for (int k = 0; k < List.at(pre)->count(); ++k)
        {
            AssetData* itemOld = List.at(pre)->at(k);

            if (itemOld->name == itemNew->name)
            {
                hasItem = true;
                if (itemOld->md5 != itemNew->md5)
                {
                    itemNew->state = 2;
                    updateItems->append(itemNew);
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        if (!hasItem)
        {
            itemNew->state = 3;
            updateItems->append(itemNew);
        }
    }

    return updateItems;
}


void MainWindow::LoadConfig(int index, QString fileName)
{
    //qDebug()<<QApplication::applicationDirPath() + "/ResVersion1.0.2_1.0.48.xml";
    QString filePath = GetFilePath(fileName);
    QFile file(filePath);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot read file %1").arg(filePath));
        return;
    }

    QDomDocument document;
    QString strErr;
    int errLin = 0;
    int errCol = 0;

    if (!document.setContent(&file, false, &strErr, &errLin, &errCol))
    {
        return;
    }

    if (document.isNull())
    {
        return;
    }

    QDomElement root = document.documentElement();
    QDomElement catalogs = root.firstChildElement();

    if (catalogs.isNull())
    {
        return;
    }

    while(!catalogs.isNull())
    {
        AssetData* data = new AssetData();
        data->name = catalogs.attributeNode("n").value();
        data->md5 = catalogs.attributeNode("m").value();
        data->size = catalogs.attributeNode("s").value();
        data->state = 1;
        //qDebug()<<data->name<<data->md5<<data->size;
        if (List.at(index) == nullptr)
        {
            List.replace(index, new QList<AssetData*>());
        }
        List.at(index)->append(data);
        catalogs = catalogs.nextSiblingElement();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = ui->fromVersion->text();
    int idx = fileName.lastIndexOf('.');
    prefix = fileName.left(idx);
    QString start = fileName.right(fileName.count() - idx - 1);
    startPage = start.toShort();

    QString endFileName = ui->toVersion->text();
    idx = endFileName.lastIndexOf('.');
    QString end = endFileName.right(endFileName.count() - idx - 1);
    endPage = end.toShort();

    for (short i = startPage; i <= endPage; ++i)
    {
        bInst->append(false);
        List.append(nullptr);

        QWidget* tab = new QWidget(ui->tabsVersion);
        QIcon icon;
        QString str;
        str.setNum(i);
        QString fileName = prefix + "." + str + ".xml";
        fileNames.append(fileName);
        widget.append(tab);
        ui->tabsVersion->addTab(tab, icon, fileName);
    }


    AddTabPage(0, fileNames[0]);
}

void MainWindow::onTabchanged(int index)
{
    if (index > 0)
    {
        AddTabPage(index, fileNames[index]);
    }
}
