#pragma once

#include <QtWidgets>
#include <vector>
#include "Database.h"
#include "Library.h"
#include <boost/property_tree/ptree.hpp>

class IntroPage;
class ResultsPage;
class DRMPage;
class FinalPage;

enum pages { INTRO, DRM, RESULTS, FINAL };

class Wizard : public QWizard
{
Q_OBJECT
public:
    Database db;
    Wizard(QWidget* parent = 0, QString dbPath = "./");
    DRMPage* drmPage;
    ResultsPage* resultsPage;
    FinalPage* finalPage;
};

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget* parent = 0);
};

class DRMPage : public QWizardPage
{
    Q_OBJECT
    QGridLayout* layout;

    QCheckBox* steamBox;
    QCheckBox* originBox;
    QCheckBox* uplayBox;

    QLabel* statusLabel;
    QLabel* platformLabel;
    QLabel* descLabel;

    void checkSteamExists();
    void checkUplayExists();
    void checkOriginExists();

public:
    QString steamPath;
    QString originPath;
    QString uplayPath;
    DRMPage(QWidget* parent = 0);
};

class ResultsPage : public QWizardPage
{
    Q_OBJECT

    QStringList recursiveFindFiles(QDir dir, QStringList ignoreList);
    void parseAcf(QDir steamRoot);
    QTabWidget* tabWidget;
    QGridLayout* top_layout;
    QButtonGroup* btnGroup;
    QGridLayout* layout;
    QScrollArea* scrollArea;
    QStringList steamDirectoryList;
    Database db;

    QDir steamRoot;
    QDir uplayRoot;
    QDir originRoot;

    GameList steamVector;
    GameList originVector;
    GameList uplayVector;

    void printTree(boost::property_tree::ptree& pt, int level);

public slots:
    void tabSelected();
    void selectAll();
    void deselectAll();
    void invert();

protected:
    void initializePage() Q_DECL_OVERRIDE;

public:
    ResultsPage(Database db, DRMPage& drmPage, QWidget* parent = 0);
    void findOriginGames();
    void findUplayGames();
    void findSteamGames();
    int nextId() const Q_DECL_OVERRIDE;
};

class FinalPage : public QWizardPage
{
    Q_OBJECT
    Database db;

protected:
    void initializePage() Q_DECL_OVERRIDE;

public:
    FinalPage(Database db, QWidget* parent = 0);
};