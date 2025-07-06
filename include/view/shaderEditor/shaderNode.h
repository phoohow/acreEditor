#pragma once

#include <QPointF>
#include <QString>

#include <vector>

class ShaderNode
{
    QString m_name;
    QPointF m_position;

public:
    enum class Type
    {
        Standard,
        Simple,
        Cloth,
        Hair,
        Output
    };

    struct Parameter
    {
        QString name;
        bool    hasInput = false; // true if this parameter is connected to another node
        QPointF inputPinOffset;   // relative position for drawing
    };

public:
    ShaderNode(Type type, const QString& name, const QPointF& pos);
    ~ShaderNode();

    void    setPosition(const QPointF& pos);
    QPointF getPosition() const;
    QString getName() const;
    Type    getType() const;
    void    setType(Type type);

    // Parameters
    const std::vector<Parameter>& getParameters() const;
    void                          setParameterInput(int idx, bool hasInput);

    // Get input pin position for a parameter index
    QPointF getInputPinPos(int paramIdx) const;

    // Output pin position
    QPointF getOutputPinPos() const;

private:
    std::vector<Parameter> m_parameters;

    Type m_type;
};
