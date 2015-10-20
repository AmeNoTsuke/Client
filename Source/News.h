#pragma once

#include <QtWidgets>
#include <QSettings>
#include <QtNetwork>


/** News UI namespace. */
/** News class.
* Class to handle the settings section of the launcher
*/
class News : public QWidget
{
    Q_OBJECT

public slots:
    void setRSSFeed();
    void onRSSReturned(QNetworkReply* reply);

public:
    explicit News(QSettings* p, QWidget* parent = 0);
    ~News();

private:
    QHBoxLayout* hNewsLayout;
    QLineEdit* rssAddress;
    QNetworkAccessManager* manager;
    QSettings* rss;

    void getRSSFeed(QString url);
    void saveFeeds(QString title, QString url);
    void loadFeeds();
};
