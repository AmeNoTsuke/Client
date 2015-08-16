#pragma once

#include "TabWidget.h"
#include "Database.h"
#include "Sidebar.h"

#include <QStackedWidget>
#include <QSettings>

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    MainPanel(QWidget* parent = 0);
    ~MainPanel()
    {
        delete p;
    }

public slots:
    virtual void pushButtonMinimize() = 0;
    virtual void pushButtonMaximize() = 0;
    virtual void pushButtonClose() = 0;

protected:
    Database db;

    QStackedWidget* stack;
    QWidget* libraryPtr;

    void init();

private:
    QSettings* p;

    Sidebar* sidebar;
    TabWidget* activeTab;

private slots:
    void setHome()
    {
        if (!(activeTab == sidebar->homeTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->homeTab;
            activeTab->toggleActive();
        }
    }
    void setStore()
    {
        if (!(activeTab == sidebar->storeTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->storeTab;
            activeTab->toggleActive();
        }
    }
    void setGames()
    {
        if (!(activeTab == sidebar->gamesTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->gamesTab;
            activeTab->toggleActive();
        }
    }
    void setCommunity()
    {
        if (!(activeTab == sidebar->communityTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->communityTab;
            activeTab->toggleActive();
        }
    }
    void setNews()
    {
        if (!(activeTab == sidebar->newsTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->newsTab;
            activeTab->toggleActive();
        }
    }
    void setDownloads()
    {
        if (!(activeTab == sidebar->downloadsTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->downloadsTab;
            activeTab->toggleActive();
        }
    }
    void setSettings()
    {
        if (!(activeTab == sidebar->settingsTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->settingsTab;
            activeTab->toggleActive();
        }
    }
    void setExit()
    {
        if (!(activeTab == sidebar->exitTab))
        {
            activeTab->toggleInactive();
            activeTab = sidebar->exitTab;
            activeTab->toggleActive();
        }
    }
};
