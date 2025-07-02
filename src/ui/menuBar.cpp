#include <ui/menuBar.h>
#include <scene/sceneMgr.h>
#include <scene/gltfLoader.h>

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDateTime>

MenuBar::MenuBar(SceneMgr* scene, QWidget* parent) :
    QMenuBar(parent),
    m_scene(scene),
    m_loader(new GLTFLoader(m_scene))
{
    initFileMenu();
    initEditMenu();
    initHelpMenu();
}

void MenuBar::initFileMenu()
{
    m_menu_file = this->addMenu("&File");

    m_menu_file_scene  = m_menu_file->addMenu("Scene");
    m_action_openScene = m_menu_file_scene->addAction("Open");
    m_action_openScene->setShortcut(Qt::CTRL | Qt::Key_O);
    m_action_closeScene = m_menu_file_scene->addAction("Close");
    m_action_closeScene->setShortcut(Qt::CTRL | Qt::Key_E);
    m_action_addScene  = m_menu_file_scene->addAction("Add");
    m_action_saveScene = m_menu_file_scene->addAction("Save");
    connect(m_action_openScene, &QAction::triggered, this, [this]() { onOpenScene(); });
    connect(m_action_closeScene, &QAction::triggered, this, [this]() { onClearScene(); });

    m_menu_file_hdr  = m_menu_file->addMenu("HDR");
    m_action_openHDR = m_menu_file_hdr->addAction("Open");
    m_action_openHDR->setShortcut(Qt::CTRL | Qt::Key_H);
    connect(m_action_openHDR, &QAction::triggered, this, [this]() { onOpenHDR(); });

    m_menu_file_frame  = m_menu_file->addMenu("Frame");
    m_action_saveFrame = m_menu_file_frame->addAction("Save");
    m_action_saveFrame->setShortcut(Qt::CTRL | Qt::Key_F);
    connect(m_action_saveFrame, &QAction::triggered, this, [this]() { onSaveFrame(); });

    m_menu_file->addSeparator();

    m_action_exitApp = m_menu_file->addAction("Exit");
}

void MenuBar::initEditMenu()
{
    m_menu_edit          = this->addMenu("&Edit");
    m_menu_edit_light    = m_menu_edit->addMenu("&Light");
    m_action_addLight    = m_menu_edit_light->addAction("Add");
    m_action_removeLight = m_menu_edit_light->addAction("Remove");

    m_menu_edit_material    = m_menu_edit->addMenu("&Material");
    m_action_addMaterial    = m_menu_edit_material->addAction("Add");
    m_action_removeMaterial = m_menu_edit_material->addAction("Remove");

    m_menu_edit_geometry    = m_menu_edit->addMenu("&Geometry");
    m_action_addGeometry    = m_menu_edit_geometry->addAction("Add");
    m_action_removeGeometry = m_menu_edit_geometry->addAction("Remove");
}

void MenuBar::initHelpMenu()
{
    m_menu_help = this->addMenu("&Help");

    m_menu_help_tips = m_menu_help->addMenu("&Tips");
    m_action_hotkey  = m_menu_help_tips->addAction("HotKey");
    connect(m_action_hotkey, &QAction::triggered, this, [this]() { onShowHotKey(); });

    m_action_about = m_menu_help->addAction("About");
}

void MenuBar::onAddScene()
{
}

void MenuBar::onOpenScene()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open File"));
    fileDialog.setNameFilter(QObject::tr("*.gltf;;*.glb;;All Files (*)"));
    fileDialog.setDirectory(QDir::currentPath());

    if (fileDialog.exec() == QFileDialog::Accepted)
    {
        fileName = (fileDialog.selectedFiles().first()).toStdString();
    }
    else
    {
        qDebug() << "File dialog canceled";
    }

    if (!fileName.empty())
    {
        m_loader->loadScene(fileName);
        // m_scene->createScene();
        m_flushState();
        m_flushFrame();
    }
}

void MenuBar::onClearScene()
{
    m_scene->clearScene();
    m_flushFrame();
}

void MenuBar::onOpenHDR()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open File"));
    fileDialog.setNameFilter(QObject::tr("*.hdr;;*.exr;;All Files (*)"));
    fileDialog.setDirectory(QDir::currentPath());

    if (fileDialog.exec() == QFileDialog::Accepted)
    {
        fileName = (fileDialog.selectedFiles().first()).toStdString();
    }
    else
    {
        qDebug() << "File dialog canceled";
    }

    if (!fileName.empty())
    {
        m_loader->loadHDR(fileName);
        m_flushFrame();
    }
}

void MenuBar::onSaveFrame()
{
    auto dirStr = QDir::currentPath() + "/savedFrame/";
    QDir dir(dirStr);
    if (!dir.exists()) dir.mkdir(dirStr);

    auto qFileName = dirStr + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".bmp";
    auto fileName  = qFileName.toStdString();
    if (!fileName.empty()) m_saveFrame(fileName);
}

void MenuBar::onSaveScene()
{
}

void MenuBar::onExit()
{
}

void MenuBar::onShowHotKey()
{
    QDialog infoDialog = QDialog(this);
    infoDialog.setWindowTitle("HotKey");

    QVBoxLayout layout = QVBoxLayout(&infoDialog);

    QString info;
    info += "key W: move front  \tkey S: move back\n";
    info += "key Q: move down  \tkey E: move up\n";
    info += "key A: move left  \tkey D: move right\n";
    info += "\n";
    info += "key up: rotate up  \tkey down: rotate down\n";
    info += "key left: rotate left  \tkey right: rotate right\n";
    info += "\n";
    info += "key 2: front face  \tkey 8: back face\n";
    info += "key 5: top face    \tkey 0: bottom face\n";
    info += "key 4: left face   \t\tkey 6: right face\n";
    info += "\n";
    info += "key R: mark shader dirty\n";

    QLabel infoLabel = QLabel(info, &infoDialog);
    layout.addWidget(&infoLabel);

    QPushButton okButton = QPushButton("OK", &infoDialog);
    layout.addWidget(&okButton);

    connect(&okButton, &QPushButton::clicked, &infoDialog, &QDialog::accept);

    infoDialog.setLayout(&layout);
    infoDialog.exec();
}
