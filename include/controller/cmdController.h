#pragma once

#include <vector>
#include <string>
#include <functional>

class SceneMgr;
class CmdController
{
    SceneMgr*   m_scene;
    std::string m_history;

    std::function<void()>                   m_renderframe_func;
    std::function<void()>                   m_showProfilerFunc;
    std::function<void(uint32_t, uint32_t)> m_pickPixelFunc;
    std::function<void()>                   m_saveframe_func;

public:
    enum class CmdType
    {
        cInvalid,

        // single cmd
        cNone,
        cRenderFrame,
        cProfiler,
        cPickPixel,
        cSaveFrame,
        cExit,

        // multi cmd
        cClear,
        cReset,
        cHighlight,
        cUnhighlight,
        cRemove,
        cMove,
        cRotate,
        cActive,
        cUnAlive,
        cLoad,

        cCount,
    };

    enum class CmdStatus
    {
        eNone,
        eSuccess,
        eUnSupportedCmd,
        eUnSupportedParam,
        eInvalidCmd,
        eInvalidParam,
        eInvalidID,
        eCount,
    };

public:
    CmdController(SceneMgr* sceneMgr) :
        m_scene(sceneMgr) {}

    ~CmdController() {}

    void set_renderframe_callback(std::function<void()> callback) { m_renderframe_func = callback; }
    void set_showprofiler_callback(std::function<void()> callback) { m_showProfilerFunc = callback; }
    void set_pickpixel_callback(std::function<void(uint32_t, uint32_t)> callback) { m_pickPixelFunc = callback; }
    void set_saveframe_callback(std::function<void()> callback) { m_saveframe_func = callback; }

    CmdStatus execute(const std::string& command);

    void  setHistory(const std::string& history) { m_history = history; }
    auto& getHistory() const { return m_history; }

private:
    // single cmd
    CmdStatus render_frame(const std::vector<std::string>& params);
    CmdStatus profiler(const std::vector<std::string>& params);
    CmdStatus pick_pixel(const std::vector<std::string>& params);
    CmdStatus save_frame(const std::vector<std::string>& params);
    CmdStatus exit(const std::vector<std::string>& params);

    // multi cmd
    CmdStatus clear(const std::vector<std::string>& params);
    CmdStatus reset(const std::vector<std::string>& params);
    CmdStatus hightlight(const std::vector<std::string>& params);
    CmdStatus unhighlight(const std::vector<std::string>& params);
    CmdStatus remove(const std::vector<std::string>& params);
    CmdStatus move(const std::vector<std::string>& params);
    CmdStatus rotate(const std::vector<std::string>& params);
    CmdStatus active(const std::vector<std::string>& params);
    CmdStatus reset_alive(const std::vector<std::string>& params);
    CmdStatus load(const std::vector<std::string>& params);
};
