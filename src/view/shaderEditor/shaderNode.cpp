#include "view/shaderEditor/shaderNode.h"

static std::vector<ShaderNode::Parameter> getOutputParameters()
{
    return {
        {"Surface", false, {}},
        {"Volume", false, {}},
        {"Background", false, {}}};
}

static std::vector<ShaderNode::Parameter> getStandardParameters()
{
    // Typical PBR parameters
    return {
        {"BaseColor", false, {}},
        {"Metallic", false, {}},
        {"Roughness", false, {}},
        {"Normal", false, {}},
        {"Emissive", false, {}},
        {"AO", false, {}},
        {"Opacity", false, {}}};
}

static std::vector<ShaderNode::Parameter> getSimpleParameters()
{
    return {
        {"Color", false, {}},
        {"Roughness", false, {}}};
}

static std::vector<ShaderNode::Parameter> getClothParameters()
{
    return {
        {"BaseColor", false, {}},
        {"Fuzz", false, {}},
        {"Roughness", false, {}}};
}

static std::vector<ShaderNode::Parameter> getHairParameters()
{
    return {
        {"BaseColor", false, {}},
        {"Specular", false, {}},
        {"Roughness", false, {}}};
}

ShaderNode::ShaderNode(Type type, const QString& name, const QPointF& pos) :
    m_type(type), m_name(name), m_position(pos)
{
    switch (type)
    {
        case Type::Standard:
            m_parameters = getStandardParameters();
            break;
        case Type::Simple:
            m_parameters = getSimpleParameters();
            break;
        case Type::Cloth:
            m_parameters = getClothParameters();
            break;
        case Type::Hair:
            m_parameters = getHairParameters();
            break;
        case Type::Output:
            m_parameters = getOutputParameters();
            break;
    }
}

ShaderNode::~ShaderNode() {}

void ShaderNode::setPosition(const QPointF& pos)
{
    m_position = pos;
}

QPointF ShaderNode::getPosition() const
{
    return m_position;
}

QString ShaderNode::getName() const
{
    return m_name;
}

ShaderNode::Type ShaderNode::getType() const
{
    return m_type;
}

void ShaderNode::setType(Type type)
{
    m_type = type;
    switch (type)
    {
        case Type::Standard:
            m_parameters = getStandardParameters();
            break;
        case Type::Simple:
            m_parameters = getSimpleParameters();
            break;
        case Type::Cloth:
            m_parameters = getClothParameters();
            break;
        case Type::Hair:
            m_parameters = getHairParameters();
            break;
        case Type::Output:
            m_parameters = getOutputParameters();
            break;
    }
}
QPointF ShaderNode::getInputPinPos(int paramIdx) const
{
    // Input pin for parameter at left edge, vertically spaced
    return m_position + QPointF(0, 28 + paramIdx * 22 + 11);
}
const std::vector<ShaderNode::Parameter>& ShaderNode::getParameters() const
{
    return m_parameters;
}

void ShaderNode::setParameterInput(int idx, bool hasInput)
{
    if (idx >= 0 && idx < (int)m_parameters.size())
        m_parameters[idx].hasInput = hasInput;
}

QPointF ShaderNode::getOutputPinPos() const
{
    // Output pin at right center of the node (node width: 120, height depends on parameter count)
    double nodeWidth  = 120.0;
    double nodeHeight = 28.0 + 22.0 * m_parameters.size();
    return m_position + QPointF(nodeWidth, nodeHeight / 2.0);
}
