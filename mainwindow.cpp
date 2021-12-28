#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include <iostream>
#include <vector>

#include "InfoBeamerParams.hpp"

#include "device.hpp"

using namespace InfoBeamer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netManager = new QNetworkAccessManager(this);
    netReply = nullptr;
    repoReply = nullptr;
    img = new QPixmap();
    setFixedSize(606,469);
}

void MainWindow::clearValues()
{
    ui->picLabel->clear();
    ui->usernameLabel->clear();
    ui->nameLabel->clear();
    ui->repoList->clear();
    ui->repoBox->setValue(0);
    ui->bioEdit->clear();
    ui->followerBox->setValue(0);
    ui->followingBox->setValue(0);
    ui->typeLabel->clear();
    dataBuffer.clear();
}


MainWindow::~MainWindow()
{
    delete ui;
}

static QByteArray const authHeader()
{
    QString concatenated = ":";
    concatenated += API_KEY.c_str();
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    return headerData.toLocal8Bit();
}

const QByteArray AUTHHEADERVAL=authHeader();

void addBasicAuth(QNetworkRequest &req)
{
    req.setRawHeader("Authorization", AUTHHEADERVAL);
}


void MainWindow::on_usernameButton_clicked()
{
    auto username = QInputDialog::getText(this,"Github Username","Enter your GitHub Username");
    if(!username.isEmpty()){
        clearValues();
        QNetworkRequest req{QUrl(QString("https://api.github.com/users/%1").arg(username))};
        QNetworkRequest repoReq{QUrl(QString("https://api.github.com/users/%1/repos").arg(username))};
        netReply = netManager->get(req);
        connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
        connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReading);
        repoReply = netManager->get(repoReq);
        connect(repoReply,&QNetworkReply::readyRead,this,&MainWindow::readDataForRepo);
        connect(repoReply,&QNetworkReply::finished,this,&MainWindow::finishedGettingRepos);
    }
}

void MainWindow::readData()
{
    dataBuffer.append(netReply->readAll());
}

void MainWindow::readDataForRepo()
{
    dataBuffer.append(repoReply->readAll());
}

void MainWindow::finishedGettingRepos()
{
    if(repoReply->error() != QNetworkReply::NoError){
        qDebug() << "Error Getting List of Repo: " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{
        QJsonArray repoInfo = QJsonDocument::fromJson(dataBuffer).array();
        ui->repoBox->setValue(repoInfo.size());
        for(int i{0}; i < ui->repoBox->value(); ++i){
            auto repo = repoInfo.at(i).toObject();
            //            if(repo.value("name") == QJsonValue::Undefined){
            //                qDebug() << "No Repo associated with Account!";
            //                QMessageBox::information(this,"No Repo","No Repository associated with current account!");
            //                return;
            //            }
            QString repoName = repo.value("name").toString();
            ui->repoList->addItem(repoName);
        }
    }
}

void MainWindow::finishReading()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        QJsonObject userJsonInfo = QJsonDocument::fromJson(dataBuffer).object();

        //SET USERNAME
        QString login = userJsonInfo.value("login").toString();
        ui->usernameLabel->setText(login);

        // SET DISPLAY NAME
        QString name = userJsonInfo.value("name").toString();
        ui->nameLabel->setText(name);

        //SET BIO
        auto bio = userJsonInfo.value("bio").toString();
        ui->bioEdit->setText(bio);

        //SET FOLLOWER AND FOLLOWING COUNT
        auto follower = userJsonInfo.value("followers").toInt();
        auto following = userJsonInfo.value("following").toInt();
        ui->followerBox->setValue(follower);
        ui->followingBox->setValue(following);

        //SET ACCOUNT TYPE
        QString type = userJsonInfo.value("type").toString();
        ui->typeLabel->setText(type);

        //SET PICTURE
        auto picLink = userJsonInfo.value("avatar_url").toString();
        QNetworkRequest link{QUrl(picLink)};
        netReply = netManager->get(link);
        connect(netReply,&QNetworkReply::finished,this,&MainWindow::setUserImage);
        dataBuffer.clear();
    }
}

static void printJsonValue(const QJsonValue &value, QString path);
static void printJsonArray(const QJsonArray &array, QString prevPath);
static void printJsonObject(const QJsonObject &obj, QString prevPath="")
{
    for (const auto &k: obj.keys())
    {
        QString path(prevPath);
        if(path.size()>0)
            path+='.';
        path+=k;
        const QJsonValue &value=obj[k];
        switch (value.type())
        {
        //Leaf nodes
        case QJsonValue::Null:
        case QJsonValue::Bool:
        case QJsonValue::Double:
        case QJsonValue::String:
        case QJsonValue::Undefined:
        default:
            printJsonValue(value, path);
            break;

        case QJsonValue::Array:
            printJsonArray(value.toArray(), path);
            break;

        case QJsonValue::Object:
            printJsonObject(value.toObject(), path);
            break;
        }
    }
}

//Process leaf nodes
static void printJsonValue(const QJsonValue &value, QString path)
{
    std::cerr << path.toStdString() << "=";
    switch (value.type())
    {
    //Leaf nodes
    case QJsonValue::Null:
        std::cerr << "Null";
        break;
    case QJsonValue::Bool:
        std::cerr << "Bool(" << (value.toBool() ? "true)" : "false)") << ")";
        break;
    case QJsonValue::Type::Double:
        std::cerr << "Double(" << value.toDouble()<< ")";
        break;
    case QJsonValue::Type::String:
        std::cerr << "String(\"" << value.toString().toStdString() << "\")";
        break;
    case QJsonValue::Type::Undefined:
        std::cerr << "Undefined";
        break;

    case QJsonValue::Type::Array:
    case QJsonValue::Type::Object:
        std::cerr << "Program Error!!!!  " << __func__ << " called with aggregate QJsonValue";

    default:
        std::cerr << "Bad Value!!!! Not a QJsonValue enumerated type";
        printJsonArray(value.toArray(), path);
        break;
    }

    std::cerr << std::endl;
}

static void printJsonArray(const QJsonArray &array, QString prevPath)
{
    for(int i=0; i<array.size(); i++)
    {
        QString path(prevPath);
        path+="[";
        path+=std::to_string(i).c_str();
        path+="]";
        const QJsonValue &value(array[i]);
        switch (value.type())
        {
        case QJsonValue::Object:
            printJsonObject(value.toObject(), path);
            break;
        case QJsonValue::Array:
            printJsonArray(value.toArray(), path);
            break;
        default:
            printJsonValue(value, path);
        }
    }
}

void MainWindow::finishReadingDevices()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        deviceJson = QJsonDocument::fromJson(dataBuffer).object();
        qDebug() << __func__;
        qDebug() << deviceJson;
        printJsonObject(deviceJson);
        try
        {
            Device::poplulate(deviceJson);
        }
        catch (const DeviceException &e)
        {
            qDebug() << __func__
                     << __FILE_NAME__
                     << (QString("line ")+std::to_string(__LINE__).c_str())+": "
                     << "Device::populate failed:"
                     << e.what();
        }
    }
}

void MainWindow::finishReadingPackages()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        deviceJson = QJsonDocument::fromJson(dataBuffer).object();
        qDebug() << __func__;
        qDebug() << deviceJson;
        printJsonObject(deviceJson);
    }
}

void MainWindow::finishReadingSetups()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        deviceJson = QJsonDocument::fromJson(dataBuffer).object();
        qDebug() << __func__;
        qDebug() << deviceJson;
        printJsonObject(deviceJson);
    }
}

void MainWindow::finishReadingAssets()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        deviceJson = QJsonDocument::fromJson(dataBuffer).object();
        qDebug() << __func__;
        qDebug() << deviceJson;
        printJsonObject(deviceJson);
    }
}

void MainWindow::finishReadingAccount()
{
    if(netReply->error() != QNetworkReply::NoError){
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this,"Error",QString("Request[Error] : %1").arg(netReply->errorString()));
    }else{

        //CONVERT THE DATA FROM A JSON DOC TO A JSON OBJECT
        deviceJson = QJsonDocument::fromJson(dataBuffer).object();
        qDebug() << __func__;
        qDebug() << deviceJson;
        printJsonObject(deviceJson);
    }
}

void MainWindow::setUserImage()
{
    qDebug() << "Pixmap stuff starts";
    img->loadFromData(netReply->readAll());
    QPixmap temp = img->scaled(ui->picLabel->size());
    ui->picLabel->setPixmap(temp);
    qDebug() << "Pixmap end";
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}


void MainWindow::on_devicesButton_clicked()
{
    QNetworkRequest req{QUrl(API_URL+"device/list")};
    addBasicAuth(req);
    dataBuffer.clear();
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingDevices);
}

void MainWindow::on_packagesButton_clicked()
{
    QNetworkRequest req{QUrl(API_URL+"package/list")};
    addBasicAuth(req);
    dataBuffer.clear();
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingDevices);
}



void MainWindow::on_setupsButton_clicked()
{
    QNetworkRequest req{QUrl(API_URL+"setup/list")};
    addBasicAuth(req);
    dataBuffer.clear();
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingDevices);
}


void MainWindow::on_assetsButton_clicked()
{
    QNetworkRequest req{QUrl(API_URL+"asset/list")};
    addBasicAuth(req);
    dataBuffer.clear();
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingDevices);
}


void MainWindow::on_acctInfoButton_clicked()
{
    QNetworkRequest req{QUrl(API_URL+"account")};
    addBasicAuth(req);
    dataBuffer.clear();
    netReply = netManager->get(req);
    connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readData);
    connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingDevices);
}
