#include <view/bottom/cmdWidget.h>

#include <model/sceneMgr.h>

#include <tuple>

static QStringList g_cmdList = {
    "clear history",
    "exit",
    "reset view",
    "render",
    "pick",
    "clear hdr",
    "clear scene",
    "highlight entity",
    "highlight geometry",
    "highlight material",
    "unhighlight entity",
    "unhighlight geometry",
    "unhighlight material",
    "remove entity",
    "remove geometry",
    "remove material",
    "unAlive entity",
    "move entity (x,y,z)",
    "move geometry (x,y,z)",
    "move material (x,y,z)",
    "move camera (x,y,z)",
    "rotate entity (x,y,z)",
    "rotate geometry (x,y,z)",
    "rotate material (x,y,z)",
    "rotate camera (x,y,z)",
    "profiler",
    "saveFrame",
};

static std::tuple<QString, std::vector<QString>> splitCommand(const QString& input)
{
    std::istringstream   iss(input.toStdString());
    std::vector<QString> params;
    std::string          token;
    QString              cmd;

    bool isFirst = true;
    while (iss >> token)
    {
        if (isFirst)
        {
            cmd     = QString::fromStdString(token);
            isFirst = false;
            continue;
        }

        params.push_back(QString::fromStdString(token));
    }

    return {cmd, params};
}

static uint32_t toID(std::string entity)
{
    return std::stoi(entity);
}

CmdWidget::CmdWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    m_selector = new QTabWidget(this);
    m_mainLayout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &CmdWidget::onUpdateTab);

    initCmd();

    m_mainLayout->addStretch();
}

void CmdWidget::initCmd()
{
    m_cmdTab      = new QWidget();
    m_cmdLayout   = new QVBoxLayout(m_cmdTab);
    m_cmdLineEdit = new QLineEdit(m_cmdTab);

    m_completer = new QCompleter(g_cmdList, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_cmdLineEdit->setCompleter(m_completer);

    m_cmdLayout->addWidget(m_cmdLineEdit);
    connect(m_cmdLineEdit, &QLineEdit::returnPressed, this, &CmdWidget::onCmdSubmitted);

    m_historyTextEdit = new QTextEdit(m_cmdTab);
    m_historyTextEdit->setReadOnly(true);
    m_cmdLayout->addWidget(m_historyTextEdit);

    m_selector->addTab(m_cmdTab, "Cmd");
    m_cmdLayout->addStretch();
}

void CmdWidget::onUpdateTab()
{
    onCmdTabChanged();
}

void CmdWidget::onCmdTabChanged()
{
    updateCmdInfo();
}

void CmdWidget::updateCmdInfo()
{
}

void CmdWidget::onCmdSubmitted()
{
    QString command = m_cmdLineEdit->text();

    processCommand(command);
}

void CmdWidget::processCommand(QString& command)
{
    auto [cmd, params] = splitCommand(command);

    if (cmd == "") return;

    if (cmd == "clear")
    {
        m_historyTextEdit->clear();
    }
    else
    {
        if (cmd == "highlight")
        {
            if (params.size() < 2)
            {
                command = "Invalid highlight command";
            }
            else
            {
                if (params[0] == "entity")
                {
                    m_scene->setHighlightEntity(toID(params[1].toStdString()));
                }
                else if (params[0] == "geometry")
                {
                    m_scene->setHighlightGeometry(toID(params[1].toStdString()));
                }
                else if (params[0] == "material")
                {
                    m_scene->setHighlightMaterial(toID(params[1].toStdString()));
                }
                else
                {
                    command = "Unknown highlight type: " + params[0];
                }
            }
        }
        else if (cmd == "unhighlight")
        {
            if (params.size() < 2)
            {
                command = "Invalid unhighlight command";
            }
            else
            {
                if (params[0] == "entity")
                {
                    m_scene->unhighlightEntity(toID(params[1].toStdString()));
                }
                else if (params[0] == "geometry")
                {
                    m_scene->unhighlightGeometry(toID(params[1].toStdString()));
                }
                else if (params[0] == "material")
                {
                    m_scene->unhighlightMaterial(toID(params[1].toStdString()));
                }
                else
                {
                    command = "Unknown unhighlight type: " + params[0];
                }
            }
        }
        else if (cmd == "remove")
        {
            if (params.size() < 2)
            {
                command = "Invalid remove command";
            }
            else
            {
                if (params[0] == "entity")
                {
                    m_scene->removeEntity(toID(params[1].toStdString()));
                }
                else if (params[0] == "geometry")
                {
                    m_scene->removeGeometry(toID(params[1].toStdString()));
                }
                else if (params[0] == "material")
                {
                    m_scene->removeMaterial(toID(params[1].toStdString()));
                }
                else
                {
                    command = "Unknown remove type: " + params[0];
                }
            }
        }
        else if (cmd == "unAlive")
        {
            if (params.size() < 2)
            {
                command = "Invalid unAlive command";
            }
            else
            {
                if (params[0] == "entity")
                {
                    m_scene->unAliveEntity(toID(params[1].toStdString()));
                }
                else
                {
                    command = "Unknown unAlive type: " + params[0];
                }
            }
        }
        else if (cmd == "profiler")
        {
            if (params.size() > 2)
            {
                command = "Invalid profiler command";
            }
            else
            {
                m_showProfiler();
            }
        }
        else
        {
            command = "UnSupported Command: " + command;
        }

        QString result = ">> " + command;
        m_historyTextEdit->append(result);
    }

    m_cmdLineEdit->clear();
    m_flushFrame();
}
