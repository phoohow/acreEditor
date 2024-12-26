#pragma once

#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <functional>

class BaseScene;
class MenuBar : QMenuBar
{
    BaseScene* m_scene = nullptr;

    std::function<void()>                            m_flushFrame;
    std::function<void(const std::string& fileName)> m_saveFrame;

    QMenu*   m_menu_file;
    QMenu*   m_menu_file_scene;
    QMenu*   m_menu_file_hdr;
    QMenu*   m_menu_file_frame;
    QAction* m_action_openScene;
    QAction* m_action_closeScene;
    QAction* m_action_addScene;
    QAction* m_action_saveScene;
    QAction* m_action_saveFrame;
    QAction* m_action_openHDR;
    QAction* m_action_exitApp;

    QMenu*   m_menu_edit;
    QMenu*   m_menu_edit_light;
    QMenu*   m_menu_edit_material;
    QMenu*   m_menu_edit_geometry;
    QAction* m_action_addLight;
    QAction* m_action_removeLight;
    QAction* m_action_addMaterial;
    QAction* m_action_removeMaterial;
    QAction* m_action_addGeometry;
    QAction* m_action_removeGeometry;

    QMenu*   m_menu_help;
    QMenu*   m_menu_help_tips;
    QAction* m_action_hotkey;
    QAction* m_action_about;

public:
    explicit MenuBar(BaseScene* scene, QWidget* parent = nullptr);

    auto getMenuBar() { return static_cast<QMenuBar*>(this); }

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void setSaveFrameCallBack(std::function<void(const std::string& fileName)> func) { m_saveFrame = func; }

private:
    void initFileMenu();
    void initEditMenu();
    void initHelpMenu();

    void onAddScene();
    void onClearScene();
    void onOpenScene();
    void onOpenHDR();
    void onSaveScene();
    void onSaveFrame();
    void onExit();
    void onShowHotKey();
};
