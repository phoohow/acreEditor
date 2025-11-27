#include <view/menuBar.h>

#include <controller/loader/gltfLoader.h>
#include <controller/loader/triangleLoader.h>

#include <model/sceneMgr.h>

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDateTime>
#include <QLabel>

MenuBar::MenuBar(SceneMgr* scene, QWidget* parent) :
    QMenuBar(parent),
    m_scene(scene),
    // m_loader(new TriangleLoader(m_scene))
    m_loader(new GLTFLoader(m_scene))
{
    _init_file_menu();
    _init_edit_menu();
    _init_help_menu();
}

void MenuBar::_init_file_menu()
{
    m_menu_file = this->addMenu("&File");

    m_menu_file_scene   = m_menu_file->addMenu("Scene");
    m_action_open_scene = m_menu_file_scene->addAction("Open");
    m_action_open_scene->setShortcut(Qt::CTRL | Qt::Key_O);
    m_action_close_scene = m_menu_file_scene->addAction("Close");
    m_action_close_scene->setShortcut(Qt::CTRL | Qt::Key_E);
    m_action_add_scene  = m_menu_file_scene->addAction("Add");
    m_action_save_scene = m_menu_file_scene->addAction("Save");
    connect(m_action_open_scene, &QAction::triggered, this, [this]() { _on_open_scene(); });
    connect(m_action_close_scene, &QAction::triggered, this, [this]() { _on_clear_scene(); });

    m_menu_file_image   = m_menu_file->addMenu("Image");
    m_action_open_image = m_menu_file_image->addAction("Open Image");
    m_action_open_image->setShortcut(Qt::CTRL | Qt::Key_I);
    connect(m_action_open_image, &QAction::triggered, this, [this]() { _on_open_image(); });

    m_action_open_hdr = m_menu_file_image->addAction("Open HDR");
    m_action_open_hdr->setShortcut(Qt::CTRL | Qt::Key_H);
    connect(m_action_open_hdr, &QAction::triggered, this, [this]() { _on_open_hdr(); });

    m_action_open_lut_ggx = m_menu_file_image->addAction("Open LUT GGX");
    connect(m_action_open_lut_ggx, &QAction::triggered, this, [this]() { _on_open_lut_ggx(); });
    m_action_open_lut_charlie = m_menu_file_image->addAction("Open LUT Charlie");
    connect(m_action_open_lut_charlie, &QAction::triggered, this, [this]() { _on_open_lut_charlie(); });
    m_action_open_lut_sheen_albedo_scale = m_menu_file_image->addAction("Open LUT Sheen Albedo Scale");
    connect(m_action_open_lut_sheen_albedo_scale, &QAction::triggered, this, [this]() { _on_open_lut_sheen_albedo_scale(); });

    m_menu_file_frame   = m_menu_file->addMenu("Frame");
    m_action_save_frame = m_menu_file_frame->addAction("Save");
    m_action_save_frame->setShortcut(Qt::CTRL | Qt::Key_F);
    connect(m_action_save_frame, &QAction::triggered, this, [this]() { _on_save_frame(); });

    m_action_start_record = m_menu_file_frame->addAction("Start Recording");
    m_action_start_record->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_R);
    connect(m_action_start_record, &QAction::triggered, this, [this]() { _on_start_record(); });

    m_action_stop_record = m_menu_file_frame->addAction("Stop Recording");
    m_action_stop_record->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_S);
    connect(m_action_stop_record, &QAction::triggered, this, [this]() { _on_stop_record(); });

    m_menu_file->addSeparator();

    m_action_exit_app = m_menu_file->addAction("Exit");
}

void MenuBar::_init_edit_menu()
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

void MenuBar::_init_help_menu()
{
    m_menu_help = this->addMenu("&Help");

    m_menu_help_tips = m_menu_help->addMenu("&Tips");
    m_action_hotkey  = m_menu_help_tips->addAction("HotKey");
    connect(m_action_hotkey, &QAction::triggered, this, [this]() { _on_show_hotkey(); });

    m_action_about = m_menu_help->addAction("About");
}

void MenuBar::_on_add_scene()
{
}

void MenuBar::_on_open_scene()
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
        m_scene->clear_scene();
        m_loader->loadScene(fileName);

        m_resetview_func();
        m_flushstate_func();
        m_renderframe_func();
    }
}

void MenuBar::_on_clear_scene()
{
    m_scene->clear_scene();
    m_renderframe_func();
}

void MenuBar::_on_open_image()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open Image"));
    fileDialog.setNameFilter(QObject::tr("*.png;;*.jpg;;*.jpeg;;*.bmp;;All Files (*)"));
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
        m_loader->loadImage(fileName);
        m_renderframe_func();
    }
}

void MenuBar::_on_open_hdr()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open HDR"));
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
        m_renderframe_func();
    }
}

void MenuBar::_on_open_lut_ggx()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open LUT GGX"));
    fileDialog.setNameFilter(QObject::tr("*.png;;*.jpg;;*.jpeg;;*.bmp;;All Files (*)"));
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
        m_loader->loadLutGGX(fileName);
        m_renderframe_func();
    }
}

void MenuBar::_on_open_lut_charlie()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open LUT Charlie"));
    fileDialog.setNameFilter(QObject::tr("*.png;;*.jpg;;*.jpeg;;*.bmp;;All Files (*)"));
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
        m_loader->loadLutCharlie(fileName);
        m_renderframe_func();
    }
}

void MenuBar::_on_open_lut_sheen_albedo_scale()
{
    std::string fileName;

    QFileDialog fileDialog;
    fileDialog.setWindowTitle(QObject::tr("Open LUT Sheen Albedo Scale"));
    fileDialog.setNameFilter(QObject::tr("*.png;;*.jpg;;*.jpeg;;*.bmp;;All Files (*)"));
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
        m_loader->loadLutSheenAlbedoScale(fileName);
        m_renderframe_func();
    }
}

void MenuBar::_on_save_frame()
{
    auto dirStr = QDir::currentPath() + "/savedFrame/";
    QDir dir(dirStr);
    if (!dir.exists()) dir.mkdir(dirStr);

    auto qFileName = dirStr + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".bmp";
    auto fileName  = qFileName.toStdString();
    if (!fileName.empty()) m_saveframe_func(fileName);
}

void MenuBar::_on_save_scene()
{
}

void MenuBar::_on_start_record()
{
    auto dirStr = QDir::currentPath() + "/recordings/";
    QDir dir(dirStr);
    if (!dir.exists()) dir.mkdir(dirStr);

    auto qFileName = dirStr + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz") + ".h264";
    auto fileName  = qFileName.toStdString();
    if (!fileName.empty() && m_start_record_func) m_start_record_func(fileName);
}

void MenuBar::_on_stop_record()
{
    if (m_stop_record_func) m_stop_record_func();
}

void MenuBar::_on_exit()
{
}

void MenuBar::_on_show_hotkey()
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
