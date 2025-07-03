#include <controller/cmdController.h>

#include <model/sceneMgr.h>

#include <sstream>
#include <tuple>
#include <map>

static std::map<std::string, CmdController::CmdType> g_cmdTypeMap = {
    // single cmd
    {"", CmdController::CmdType::cNone},
    {"render", CmdController::CmdType::cRender},
    {"profiler", CmdController::CmdType::cProfiler},
    {"pick", CmdController::CmdType::cPick},
    {"saveFrame", CmdController::CmdType::cSaveFrame},
    {"exit", CmdController::CmdType::cExit},

    // multi cmd
    {"clear", CmdController::CmdType::cClear},
    {"reset", CmdController::CmdType::cReset},
    {"highlight", CmdController::CmdType::cHighlight},
    {"unhighlight", CmdController::CmdType::cUnhighlight},
    {"remove", CmdController::CmdType::cRemove},
    {"move", CmdController::CmdType::cMove},
    {"rotate", CmdController::CmdType::cRotate},
    {"unAlive", CmdController::CmdType::cUnAlive},
    {"load", CmdController::CmdType::cLoad},
};

static auto findCmdType(const std::string& token)
{
    auto it = g_cmdTypeMap.find(token);
    if (it == g_cmdTypeMap.end())
    {
        return CmdController::CmdType::cInvalid;
    }

    return it->second;
}

static auto splitCommand(const std::string& input)
{
    std::istringstream       iss(input);
    std::vector<std::string> params;
    std::string              token;
    CmdController::CmdType   cmd = CmdController::CmdType::cInvalid;

    bool isFirst = true;
    while (iss >> token)
    {
        if (!isFirst)
        {
            params.push_back(token);
            continue;
        }

        isFirst = false;
        cmd     = findCmdType(token);
    }

    return std::make_tuple(cmd, params);
}

static uint32_t toID(std::string entity)
{
    return std::stoi(entity);
}

CmdController::CmdStatus CmdController::execute(const std::string& command)
{
    auto [cmd, params] = splitCommand(command);
    auto status        = CmdStatus::eSuccess;

    switch (cmd)
    {
        // single cmd
        case CmdController::CmdType::cInvalid: status = CmdStatus::eInvalidCmd; break;
        case CmdController::CmdType::cNone: status = CmdStatus::eNone; break;
        case CmdController::CmdType::cRender: status = render(params); break;
        case CmdController::CmdType::cProfiler: status = profiler(params); break;
        case CmdController::CmdType::cPick: status = pick(params); break;
        case CmdController::CmdType::cSaveFrame: status = saveFrame(params); break;
        case CmdController::CmdType::cExit: status = exit(params); break;

        // multi cmd
        case CmdController::CmdType::cClear: status = clear(params); break;
        case CmdController::CmdType::cReset: status = reset(params); break;
        case CmdController::CmdType::cHighlight: status = hightlight(params); break;
        case CmdController::CmdType::cUnhighlight: status = unhighlight(params); break;
        case CmdController::CmdType::cRemove: status = remove(params); break;
        case CmdController::CmdType::cMove: status = move(params); break;
        case CmdController::CmdType::cRotate: status = rotate(params); break;
        case CmdController::CmdType::cUnAlive: status = unAlive(params); break;
        case CmdController::CmdType::cLoad: status = load(params); break;
    }

    std::string result = ">> ";
    switch (status)
    {
        case CmdStatus::eNone: break;
        case CmdStatus::eSuccess: result += "Success: " + command; break;
        case CmdStatus::eUnSupportedCmd: result += "Unsupport cmd: " + command; break;
        case CmdStatus::eUnSupportedParam: result += "Unsupport parameter: " + command; break;
        case CmdStatus::eInvalidCmd: result += "Invalid command: " + command; break;
        case CmdStatus::eInvalidParam: result += "Invalid parameter: " + command; break;
        case CmdStatus::eInvalidID: result += "Invalid id: " + command; break;
        default: break;
    }

    m_history.append(result);
    m_flushFrameFunc();

    return status;
}

// single cmd
CmdController::CmdStatus CmdController::render(const std::vector<std::string>& params)
{
    m_flushFrameFunc();
    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::profiler(const std::vector<std::string>& params)
{
    m_showProfilerFunc();
    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::pick(const std::vector<std::string>& params)
{
    return CmdStatus::eUnSupportedCmd;
}

CmdController::CmdStatus CmdController::saveFrame(const std::vector<std::string>& params)
{
    m_saveFrameFunc();

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::exit(const std::vector<std::string>& params)
{
    return CmdStatus::eUnSupportedCmd;
}

// multi cmd
CmdController::CmdStatus CmdController::clear(const std::vector<std::string>& params)
{
    if (params.size() < 1) return CmdStatus::eInvalidParam;

    if (params[0] == "history")
    {
        m_history.clear();
    }
    else if (params[0] == "scene")
    {
        m_scene->clearScene();
    }
    else if (params[0] == "hdr")
    {
        m_scene->clearHDR();
    }
    else if (params[0] == "sunlight")
    {
        // m_scene->clearSunlight();
        return CmdStatus::eUnSupportedCmd;
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::reset(const std::vector<std::string>& params)
{
    return CmdStatus::eUnSupportedCmd;
}

CmdController::CmdStatus CmdController::hightlight(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        m_scene->setHighlightEntity(id);
    }
    else if (params[0] == "geometry")
    {
        m_scene->setHighlightGeometry(id);
    }
    else if (params[0] == "material")
    {
        m_scene->setHighlightMaterial(id);
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::unhighlight(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        m_scene->unhighlightEntity(id);
    }
    else if (params[0] == "geometry")
    {
        m_scene->unhighlightGeometry(id);
    }
    else if (params[0] == "material")
    {
        m_scene->unhighlightMaterial(id);
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::remove(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        m_scene->removeEntity(id);
    }
    else if (params[0] == "geometry")
    {
        m_scene->removeGeometry(id);
    }
    else if (params[0] == "material")
    {
        m_scene->removeMaterial(id);
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::move(const std::vector<std::string>& params)
{
    if (params.size() != 3) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
    }
    else if (params[0] == "geometry")
    {
    }
    else if (params[0] == "camera")
    {
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::rotate(const std::vector<std::string>& params)
{
    if (params.size() != 3) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
    }
    else if (params[0] == "geometry")
    {
    }
    else if (params[0] == "camera")
    {
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::active(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        m_scene->aliveEntity(id);
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::unAlive(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        m_scene->unAliveEntity(id);
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::load(const std::vector<std::string>& params)
{
    if (params.size() != 3) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "image")
    {
    }
    else if (params[0] == "scene")
    {
    }
    else if (params[0] == "obj")
    {
    }
    else if (params[0] == "gltf")
    {
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}
