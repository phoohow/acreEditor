#include "view/shaderEditor/shaderNodeConnection.h"
#include "view/shaderEditor/shaderNode.h"

ShaderNodeConnection::ShaderNodeConnection(ShaderNode* from, ShaderNode* to, int toParamIdx) :
    m_from(from), m_to(to), m_toParamIdx(toParamIdx) {}

ShaderNode* ShaderNodeConnection::getFrom() const
{
    return m_from;
}

ShaderNode* ShaderNodeConnection::getTo() const
{
    return m_to;
}

int ShaderNodeConnection::getToParamIdx() const
{
    return m_toParamIdx;
}
