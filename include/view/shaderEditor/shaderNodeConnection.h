#pragma once

#include <memory>

class ShaderNode;
class ShaderNodeConnection
{
    ShaderNode* m_from;
    ShaderNode* m_to;
    int         m_toParamIdx;

public:
    ShaderNodeConnection(ShaderNode* from, ShaderNode* to, int toParamIdx);
    ShaderNode* getFrom() const;
    ShaderNode* getTo() const;
    int         getToParamIdx() const;
};
