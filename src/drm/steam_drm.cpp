#include "steam_drm.h"
#include <src/libs/steam_vdf_parse.hpp>
#include <QSettings>
#include <QProcess>

/** Check if Steam is installed on the current computer, if applicable, and sets some values for later pages to
 * check on.
 */
void SteamDRM::checkExists()
{
    QDir steamFolder;

#if defined(__linux__)
    QProcess which;
    which.setProcessChannelMode(QProcess::MergedChannels);
    QStringList args;
    args << "steam";
    which.start("which", args);
    which.waitForFinished();
    if (which.exitCode() == 0)
    {
        steamFolder = QDir::homePath() + "/.local/share/Steam";
    }
#elif defined(_WIN32) || defined(_WIN64)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
    if (!settings.value("SteamPath").isNull())
    {
        steamFolder = QDir(settings.value("SteamPath").toString()).canonicalPath();
    }
#elif defined(__APPLE__)
    steamFolder = QDir(QDir::home().filePath("Library/Application Support/Steam"));
#endif

    if (steamFolder.filePath("").trimmed() != "" && steamFolder.exists() && steamFolder != QDir("."))
    {
        this->setRootDir(steamFolder);
        this->setIsInstalled();
        statusLabel->setPixmap(QPixmap(":/SystemMenu/Icons/Tick.svg"));
        descLabel = new QLabel("Steam found in " + steamFolder.filePath(""));
        QDir steamAppsDir = steamFolder.filePath("steamapps");
        if (!steamAppsDir.exists())
        {
            steamAppsDir = steamFolder.filePath("SteamApps");
        }
        pt::ptree libraryFolders;
        pt::read_info(steamAppsDir.filePath("libraryfolders.vdf").toLocal8Bit().constData(), libraryFolders);
        steamDirectoryList.push_back(steamFolder.filePath(""));
        QString pathString = "";

        for (auto& kv : libraryFolders.get_child("LibraryFolders"))
        {
            if (std::isdigit(static_cast<int>(*kv.first.data())))
            {
                std::string path = kv.second.data();
                QDir dir(QString::fromStdString(path));
                if (dir.exists())
                {
                    steamDirectoryList.push_back(dir.filePath(""));
                    pathString += dir.filePath("");
                    pathString += "\n";
                }
            }
        }
        if (steamDirectoryList.size() >= 2)
        {
            descLabel->setText(descLabel->text() + "\n\nLibrary folders:\n" + pathString);
        }

    }
    else
    {
        statusLabel->setPixmap(QPixmap(":SystemMenu/Icons/Cross.svg"));
        descLabel = new QLabel("Steam not found. Verify installation and try again.");
    }
}

QWidget* SteamDRM::createPane()
{
	return new QWidget();
}

void SteamDRM::findGames()
{
    QDir steamAppsDir = rootDir.filePath("steamapps");
    if (!steamAppsDir.exists())
    {
        steamAppsDir = rootDir.filePath("SteamApps");
    }
    pt::ptree libraryFolders;
    pt::read_info(steamAppsDir.filePath("libraryfolders.vdf").toLocal8Bit().constData(), libraryFolders);
    steamDirectoryList.push_back(rootDir.filePath(""));
    QString pathString = "" + steamDirectoryList.at(0).filePath("") + "\n";

    for (auto& kv : libraryFolders.get_child("LibraryFolders"))
    {
        if (std::isdigit(static_cast<int>(*kv.first.data())))
        {
            std::string path = kv.second.data();
            QDir dir(QString::fromStdString(path));
            if (dir.exists())
            {
                steamDirectoryList.push_back(dir.filePath(""));
                pathString += dir.filePath("");
                pathString += "\n";
            }
        }
    }

    parseAcf();
}

void SteamDRM::parseAcf()
{
    QString vdfPath = rootDir.filePath("appcache/appinfo.vdf");
    qDebug() << "Parsing Steam vdf, located at:" << vdfPath;
    auto games = SteamVdfParse::parseVdf(vdfPath.toLocal8Bit().constData());

    for (auto& iter : steamDirectoryList)
    {
        QDir steamAppsDir(iter);
        if (steamAppsDir.exists("SteamApps"))
        {
            steamAppsDir = steamAppsDir.filePath("SteamApps");
        }
        else
        {
            steamAppsDir = steamAppsDir.filePath("steamapps");
        }

        QStringList fileList = steamAppsDir.entryList(QStringList("*.acf"), QDir::Files | QDir::NoSymLinks);

        for (auto fileIter : fileList)
        {
            boost::property_tree::ptree fileTree;
            std::string acfDir = steamAppsDir.filePath(fileIter).toLocal8Bit().constData();
            boost::property_tree::info_parser::read_info(acfDir, fileTree);

            QString name;
            QString path = steamAppsDir.filePath("common/" + QString::fromStdString(fileTree.get<std::string>("AppState.installdir")));

            boost::optional<std::string> nameTest = fileTree.get_optional<std::string>("AppState.name");
            if (!nameTest)
            {
                nameTest = fileTree.get_optional<std::string>("AppState.UserConfig.name");
            }

            if (nameTest)
            {
                name = QString::fromStdString(nameTest.get());
            }
            else
            {
                name = QDir(path).dirName();
            }

            // TODO: Either add SteamID to db, or add getGameByPath

            //if (!std::get<0>(db.isExistant(name)))
			if (true)
            {
                QString exe;
                QString args;

                int id;
                boost::optional<int> idTest = fileTree.get_optional<int>("AppState.appID");
                if (!idTest)
                {
                    idTest = fileTree.get_optional<int>("AppState.appid");
                }

                id = idTest.get();
                try
                {
                    auto game = games.at(id);
                    auto launch = game.pt.get_child("config.launch");
                    boost::optional<std::string> oslistTest = game.pt.get_optional<std::string>("common.oslist");
                    if (oslistTest)
                    {

                        // Loop through the 0, 1, and 2 configurations
                        for (auto pair : launch)
                        {
                            boost::property_tree::ptree section = pair.second;

                            QString oslist = QString::fromStdString(section.get("config.oslist", "windows"));

#if defined(__linux__)
                            if (oslist == "linux")

#elif defined(_WIN32) || defined(_WIN64)
                            if (oslist == "windows")
#elif defined(__APPLE__)
                                if (oslist == "macos")
#endif
                            {
                                exe = QDir(path).filePath(QString::fromStdString(section.get<std::string>("executable")));
                                exe = QString(QDir::cleanPath(exe));
                                path = QDir(path).filePath(QString::fromStdString(section.get("workingdir", "")));
                                path = QString(QDir::cleanPath(path));
                                args = QString::fromStdString(section.get("arguments", ""));
                                break;
                            }
                        }
                    }
                    else
                    {
                        exe = QDir(path).filePath(QString::fromStdString(launch.get<std::string>("0.executable")));
                        exe = QString(QDir::cleanPath(exe));
                        path = QDir(path).filePath(QString::fromStdString(launch.get("0.workingdir", "")));
                        path = QString(QDir::cleanPath(path));
                        args = QString::fromStdString(launch.get("0.arguments", ""));
                    }
                }
                catch (const std::out_of_range&)
                {
                    qDebug() << "The game id:" << id << "was not found in the Steam appinfo.vdf.";
                    QStringList exeList = QDir(path).entryList(QDir::Files | QDir::NoSymLinks | QDir::Executable);

                    QFileDialog exeDialog;
                    exeDialog.setWindowTitle("Select Executable");
                    exeDialog.setFileMode(QFileDialog::ExistingFile);
                    exeDialog.setDirectory(path);
                    if (exeDialog.exec())
                    {
                        exe = exeDialog.selectedFiles().at(0);
                    }
                }

                steamVector.push_back(Game{0, name, path, exe, args});
            }
        }
    }
}
SteamDRM::SteamDRM() : DRMType("<b>Steam</b>"){}