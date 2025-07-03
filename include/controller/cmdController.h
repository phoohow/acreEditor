#pragma once

#include <vector>
#include <string>
#include <functional>

class SceneMgr;
class CmdController
{
    SceneMgr*   m_scene;
    std::string m_history;

    std::function<void()> m_renderFrameFunc;
    std::function<void()> m_saveFrameFunc;
    std::function<void()> m_showProfilerFunc;

public:
    enum class CmdType
    {
        cInvalid,

        // single cmd
        cNone,
        cRender,
        cProfiler,
        cPick,
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

    void setRenderFrameCallBack(std::function<void()> callback) { m_renderFrameFunc = callback; }
    void setSaveFrameCallBack(std::function<void()> callback) { m_saveFrameFunc = callback; }
    void setShowProfilerCallBack(std::function<void()> callback) { m_showProfilerFunc = callback; }

    CmdStatus execute(const std::string& command);

    void  setHistory(const std::string& history) { m_history = history; }
    auto& getHistory() const { return m_history; }

private:
    // single cmd
    CmdStatus render(const std::vector<std::string>& params);
    CmdStatus profiler(const std::vector<std::string>& params);
    CmdStatus pick(const std::vector<std::string>& params);
    CmdStatus saveFrame(const std::vector<std::string>& params);
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
    CmdStatus unAlive(const std::vector<std::string>& params);
    CmdStatus load(const std::vector<std::string>& params);
};
