#pragma once

#include <QMainWindow>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void clearValues();
    ~MainWindow();

private slots:
    void on_usernameButton_clicked();
    void readData();
    void readDataForRepo();
    void finishedGettingRepos();
    void finishReading();
    void finishReadingDevices();
    void finishReadingPackages();
    void finishReadingSetups();
    void finishReadingAssets();
    void finishReadingAccount();
    void setUserImage();
    void on_actionAbout_Qt_triggered();


    void on_devicesButton_clicked();
    void on_packagesButton_clicked();

    void on_setupsButton_clicked();

    void on_assetsButton_clicked();

    void on_acctInfoButton_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *netManager;
    QNetworkReply *netReply;
    QNetworkReply *repoReply;
    QByteArray dataBuffer;
    QPixmap *img;
    QJsonObject deviceJson, packageJson, setupJson, assetJson, acctJason;
};
