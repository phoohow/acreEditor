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

    std::function<void()>                            m_renderframe_func;
    std::function<void()>                            m_flushstate_func;
    std::function<void()>                            m_resetview_func;
    std::function<void(const std::string& fileName)> m_saveframe_func;
    std::function<void(const std::string& fileName)> m_start_record_func;
    std::function<void()>                            m_stop_record_func;

    QMenu*   m_menu_file;
    QMenu*   m_menu_file_scene;
    QMenu*   m_menu_file_image;
    QMenu*   m_menu_file_frame;
    QAction* m_action_open_scene;
    QAction* m_action_close_scene;
    QAction* m_action_add_scene;
    QAction* m_action_save_scene;
    QAction* m_action_save_frame;
    QAction* m_action_start_record;
    QAction* m_action_stop_record;
    QAction* m_action_open_image;
    QAction* m_action_open_hdr;
    QAction* m_action_open_lut_ggx;
    QAction* m_action_open_lut_charlie;
    QAction* m_action_open_lut_sheen_albedo_scale;
    QAction* m_action_exit_app;

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

    void set_renderframe_callback(std::function<void()> func) { m_renderframe_func = func; }

    void set_flushstate_callback(std::function<void()> func) { m_flushstate_func = func; }

    void set_resetview_callback(std::function<void()> func) { m_resetview_func = func; }

    void set_saveframe_callback(std::function<void(const std::string& fileName)> func) { m_saveframe_func = func; }

    void set_start_record_callback(std::function<void(const std::string& fileName)> func) { m_start_record_func = func; }

    void set_stop_record_callback(std::function<void()> func) { m_stop_record_func = func; }

    void save_frame() { _on_save_frame(); }

private:
    void _init_file_menu();
    void _init_edit_menu();
    void _init_help_menu();

    void _on_add_scene();
    void _on_clear_scene();
    void _on_open_scene();
    void _on_open_image();
    void _on_open_hdr();
    void _on_open_lut_ggx();
    void _on_open_lut_charlie();
    void _on_open_lut_sheen_albedo_scale();
    void _on_save_scene();
    void _on_save_frame();
    void _on_start_record();
    void _on_stop_record();
    void _on_exit();
    void _on_show_hotkey();
};
