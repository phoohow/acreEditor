#include <controller/cmdController.h>

#include <model/sceneMgr.h>

#include <sstream>
#include <tuple>
#include <map>

static std::map<std::string, CmdController::CmdType> g_cmdTypeMap = {
    // single cmd
    {"", CmdController::CmdType::cNone},
    {"render_frame", CmdController::CmdType::cRenderFrame},
    {"profiler", CmdController::CmdType::cProfiler},
    {"pick_pixel", CmdController::CmdType::cPickPixel},
    {"save_frame", CmdController::CmdType::cSaveFrame},
    {"exit", CmdController::CmdType::cExit},

    // multi cmd
    {"clear", CmdController::CmdType::cClear},
    {"reset", CmdController::CmdType::cReset},
    {"highlight", CmdController::CmdType::cHighlight},
    {"unhighlight", CmdController::CmdType::cUnhighlight},
    {"remove", CmdController::CmdType::cRemove},
    {"move", CmdController::CmdType::cMove},
    {"rotate", CmdController::CmdType::cRotate},
    {"reset_alive", CmdController::CmdType::cUnAlive},
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
        case CmdController::CmdType::cRenderFrame: status = render_frame(params); break;
        case CmdController::CmdType::cProfiler: status = profiler(params); break;
        case CmdController::CmdType::cPickPixel: status = pick_pixel(params); break;
        case CmdController::CmdType::cSaveFrame: status = save_frame(params); break;
        case CmdController::CmdType::cExit: status = exit(params); break;

        // multi cmd
        case CmdController::CmdType::cClear: status = clear(params); break;
        case CmdController::CmdType::cReset: status = reset(params); break;
        case CmdController::CmdType::cHighlight: status = hightlight(params); break;
        case CmdController::CmdType::cUnhighlight: status = unhighlight(params); break;
        case CmdController::CmdType::cRemove: status = remove(params); break;
        case CmdController::CmdType::cMove: status = move(params); break;
        case CmdController::CmdType::cRotate: status = rotate(params); break;
        case CmdController::CmdType::cUnAlive: status = reset_alive(params); break;
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

    m_history.append(result + "\n");

    m_renderframe_func();

    return status;
}

// single cmd
CmdController::CmdStatus CmdController::render_frame(const std::vector<std::string>& params)
{
    m_renderframe_func();
    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::profiler(const std::vector<std::string>& params)
{
    m_showProfilerFunc();
    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::pick_pixel(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto x = std::stoi(params[0]);
    auto y = std::stoi(params[1]);

    m_pickPixelFunc(x, y);

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::save_frame(const std::vector<std::string>& params)
{
    m_saveframe_func();

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
        m_scene->clear_scene();
    }
    else if (params[0] == "hdr")
    {
        // m_scene->clearHDR();
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
        auto node = m_scene->find<acre::EntityID>(id);
        m_scene->highlight_entity(node->id<acre::EntityID>());
    }
    else if (params[0] == "geometry")
    {
        auto node = m_scene->find<acre::GeometryID>(id);
        m_scene->highlight_geometry(node->id<acre::GeometryID>());
    }
    else if (params[0] == "material")
    {
        auto node = m_scene->find<acre::MaterialID>(id);
        m_scene->highlight_material(node->id<acre::MaterialID>());
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
        auto node = m_scene->find<acre::EntityID>(id);
        m_scene->unhighlight_entity(node->id<acre::EntityID>());
    }
    else if (params[0] == "geometry")
    {
        auto node = m_scene->find<acre::GeometryID>(id);
        m_scene->unhighlight_geometry(node->id<acre::GeometryID>());
    }
    else if (params[0] == "material")
    {
        auto node = m_scene->find<acre::MaterialID>(id);
        m_scene->unhighlight_material(node->id<acre::MaterialID>());
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
        auto node = m_scene->find<acre::EntityID>(id);
        m_scene->remove(node);
    }
    else if (params[0] == "geometry")
    {
        auto node = m_scene->find<acre::GeometryID>(id);
        m_scene->remove(node);
    }
    else if (params[0] == "material")
    {
        auto node = m_scene->find<acre::MaterialID>(id);
        m_scene->remove(node);
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
        auto node = m_scene->find<acre::EntityID>(id);
        m_scene->alive_entity(node->id<acre::EntityID>());
    }
    else
    {
        return CmdStatus::eUnSupportedParam;
    }

    return CmdStatus::eSuccess;
}

CmdController::CmdStatus CmdController::reset_alive(const std::vector<std::string>& params)
{
    if (params.size() != 2) return CmdStatus::eInvalidParam;

    auto id = toID(params[1]);
    if (id == RESOURCE_ID_VALID) return CmdStatus::eInvalidID;

    if (params[0] == "entity")
    {
        auto node = m_scene->find<acre::EntityID>(id);
        m_scene->unalive_entity(node->id<acre::EntityID>());
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
