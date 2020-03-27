#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

struct AssetData
{
    QString name;
    QString md5;
    QString size;
    int state;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitConstrols();
    void LoadConfig(int index, const QString& fileName);
    QString GetFilePath(const QString& fileName);
    void AddTabPage(int pageIdx, const QString& fileName);
    QVector<AssetData*>* DiffVersionByPreview(int pre, int next);
    void GetAllEdition();
    QString GetFileNameNoExtra(const QString& fileName);
    QString GetEditionFromFileName(const QString& fileName);

private slots:
    void on_pushButton_clicked();
    void onTabchanged(int index);

    void on_clearBtn_clicked();

    void on_fromNumer_currentTextChanged(const QString &arg1);

    void on_AEdition_currentTextChanged(const QString &arg1);

    void on_toNumber_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QVector<QVector<AssetData*>*> List;
    QString prefix;
    short startPage;
    short endPage;
    QVector<bool> *bInst;
    QVector<QWidget*> widget;
    QVector<QString> fileNames;
    QStringList editions;
    QStringList allFiles;
};

#endif // MAINWINDOW_H
