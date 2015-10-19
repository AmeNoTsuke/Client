
#include "News.h"
#include "ui_News.h"

#include <QtWidgets>
#include <QDebug>
#include <QSettings>
#include <iostream>

class rssAddress;

/** Settings constructor
* Initialize the news UI
* \param p Inherited palette configuration for setting StyleSheets.
* \param parent Pointer to parent widget.
*/
News::News(QSettings* p, QWidget* parent) : QWidget(parent)
{
    QHBoxLayout* horLayout = new QHBoxLayout(this);
    this->setStyleSheet("QListWidget { background-color: " + p->value("Primary/SecondaryBase").toString() + ";} "
            "QPushButton {"
            "color: " + p->value("Primary/LightText").toString() + "; "
                                "background-color: " + p->value("Primary/DarkElement").toString() + "; "
                                "border: none; margin: 0px; padding: 0px;} "
                                "QPushButton:hover {"
                                "background-color: " + p->value("Primary/InactiveSelection").toString() + ";} "
                                "color: " + p->value("Primary/LightText").toString() + ";");
    QFont buttonFont("SourceSansPro", 9);
    RSSList = new QListWidget(this);
    RSSListLabel = new QLabel();
    RSSListLabel->setStyleSheet("color: " + p->value("Primary/LightText").toString() + "; ");
    QVBoxLayout* rssVertLayout = new QVBoxLayout();
    rssVertLayout->addWidget(RSSListLabel);
    rssVertLayout->addWidget(RSSList);
    rssAddress = new QLineEdit();
    horLayout->addWidget(rssAddress);
    horLayout->addLayout(rssVertLayout);
    /*QListWidget* RSSList2 = new QListWidget(this);
    horLayout->addWidget(RSSList2);
    QListWidget* RSSList3 = new QListWidget(this);
    horLayout->addWidget(RSSList3);*/
    QPushButton* setRSS = new QPushButton();
    setRSS->setText("Set RSS");
    horLayout->addWidget(setRSS);
    connect(setRSS, SIGNAL(clicked()), this, SLOT(setRSSFeed()));

}

void News::setRSSFeed()
{
    QString url = rssAddress->text();
    GetRSSFeed(url);
}

void News::GetRSSFeed(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    reply = manager.get(request);
    connect(reply, SIGNAL(finished()),this, SLOT(onRSSReturned()));
    qDebug() << "TestRSS";
}

void News::onRSSReturned()
{
    QByteArray data = reply->readAll();
    QXmlStreamReader xml(data);
    qDebug() << data;
    while(!xml.atEnd())
    {
        if(xml.isStartElement()) {
            if (xml.name() == "channel")
            {
                xml.readNext();
                if (xml.name() == "title") {
                    RSSListLabel->setText(xml.readElementText());
                }
            }
            if (xml.name() == "item")
            {
                xml.readNext();
                RSSList->addItem(xml.readElementText());
            }
        }
        xml.readNext();
    }
    xml.clear();
    qDebug() << "Reply";
    qDebug() << data;
}

void News::parseXml()
{

}

News::~News()
{
}
