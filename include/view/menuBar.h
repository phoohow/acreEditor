#pragma once

#include <QMenuBar>
#include <QAction>

#include <functional>

class SceneMgr;
class Loader;
class MenuBar : QMenuBar
{
    SceneMgr* m_scene  = nullptr;
    Loader*   m_loader = nullptr;

    std::function<void()>                            m_renderFrameFunc;
    std::function<void()>                            m_flushStateFunc;
    std::function<void()>                            m_resetViewFunc;
    std::function<void(const std::string& fileName)> m_saveFrameFunc;

    QMenu*   m_menu_file;
    QMenu*   m_menu_file_scene;
    QMenu*   m_menu_file_image;
    QMenu*   m_menu_file_frame;
    QAction* m_action_openScene;
    QAction* m_action_closeScene;
    QAction* m_action_addScene;
    QAction* m_action_saveScene;
    QAction* m_action_saveFrame;
    QAction* m_action_openImage;
    QAction* m_action_openHDR;
    QAction* m_action_openLutGGX;
    QAction* m_action_openLutCharlie;
    QAction* m_action_openLutSheenAlbedoScale;
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
    explicit MenuBar(SceneMgr* scene, QWidget* parent = nullptr);

    auto getMenuBar() { return static_cast<QMenuBar*>(this); }

    void setRenderFrameCallBack(std::function<void()> func) { m_renderFrameFunc = func; }

    void setFlushStateCallBack(std::function<void()> func) { m_flushStateFunc = func; }

    void setResetViewCallBack(std::function<void()> func) { m_resetViewFunc = func; }

    void setSaveFrameCallBack(std::function<void(const std::string& fileName)> func) { m_saveFrameFunc = func; }

    void saveFrame() { onSaveFrame(); }

private:
    void initFileMenu();
    void initEditMenu();
    void initHelpMenu();

    void onAddScene();
    void onClearScene();
    void onOpenScene();
    void onOpenImage();
    void onOpenHDR();
    void onOpenLutGGX();
    void onOpenLutCharlie();
    void onOpenLutSheenAlbedoScale();
    void onSaveScene();
    void onSaveFrame();
    void onExit();
    void onShowHotKey();
};
