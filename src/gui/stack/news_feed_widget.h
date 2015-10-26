#pragma once

#include <QSettings>
#include <QtNetwork>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>

/** News UI namespace. */
/** News class.
* Class to handle the settings section of the launcher
*/
class NewsFeedWidget : public QWidget
{
Q_OBJECT
private:
    QListWidget* rssList;
    QLabel* feedTitle;
    QPushButton* deleteWidget;
    QSettings* rss;

public slots:
    void onListItemClicked(QListWidgetItem* item);
    void deleteRSSWidget();

public:
    void addRSSItem(QString title, QString url);
    void setRSSTitle(QString feedTitle);
    explicit NewsFeedWidget(QWidget* parent = 0);
    ~NewsFeedWidget();
};
