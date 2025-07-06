#include "view/shaderEditor.h"
#include "view/shaderEditor/shaderNode.h"
#include "view/shaderEditor/shaderNodeConnection.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

ShaderEditor::ShaderEditor(QWidget* parent) :
    QWidget(parent)
{
    // Create default Standard and Output nodes, and connect them
    auto        standardNode = std::make_unique<ShaderNode>(ShaderNode::Type::Standard, "Standard", QPointF(100, 100));
    auto        outputNode   = std::make_unique<ShaderNode>(ShaderNode::Type::Output, "Output", QPointF(400, 120));
    ShaderNode* standardPtr  = standardNode.get();
    ShaderNode* outputPtr    = outputNode.get();
    m_nodes.emplace_back(std::move(standardNode));
    m_nodes.emplace_back(std::move(outputNode));
    m_connections.emplace_back(std::make_unique<ShaderNodeConnection>(standardPtr, outputPtr, 0));
    outputPtr->setParameterInput(0, true);
}

ShaderEditor::~ShaderEditor() = default;

void ShaderEditor::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.save();
    painter.translate(m_viewOffset);
    painter.scale(m_scale, m_scale);

    drawConnections(painter);
    drawNodes(painter);
    drawDraggingConnection(painter);

    painter.restore();
}

void ShaderEditor::drawConnections(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 2));
    for (const auto& conn : m_connections)
    {
        auto*   fromNode   = conn->getFrom();
        auto*   toNode     = conn->getTo();
        int     toParamIdx = conn->getToParamIdx();
        QPointF fromPos    = fromNode->getOutputPinPos();
        QPointF toPos      = toNode->getInputPinPos(toParamIdx);
        painter.drawLine(fromPos, toPos);
    }
}

void ShaderEditor::drawNodes(QPainter& painter)
{
    for (const auto& node : m_nodes)
    {
        QRectF rect(node->getPosition(), QSizeF(120, 28 + 22 * node->getParameters().size()));
        painter.setBrush(Qt::lightGray);
        painter.setPen(Qt::black);
        painter.drawRect(rect);
        painter.drawText(node->getPosition() + QPointF(10, 20), node->getName());

        int idx = 0;
        for (const auto& param : node->getParameters())
        {
            QPointF pinPos        = node->getInputPinPos(idx);
            bool    isSelectedPin = (node.get() == m_selectedPin.node && m_selectedPin.type == DragPinType::Input && m_selectedPin.paramIdx == idx);
            QColor  pinColor      = param.hasInput ? Qt::green : (isSelectedPin ? QColor(255, 255, 128) : Qt::white);
            painter.setBrush(pinColor);
            painter.drawEllipse(pinPos, 6, 6);
            QPointF textPos = node->getPosition() + QPointF(18, 28 + idx * 22 + 16);
            if (isSelectedPin)
            {
                QRectF textRect(textPos.x() - 2, textPos.y() - 14, 60, 18);
                painter.setBrush(QColor(255, 255, 128));
                painter.setPen(Qt::NoPen);
                painter.drawRect(textRect);
                painter.setPen(Qt::black);
            }
            painter.drawText(textPos, param.name);
            ++idx;
        }
        if (node->getType() != ShaderNode::Type::Output)
        {
            QPointF outPin = node->getOutputPinPos();
            painter.setBrush(Qt::blue);
            painter.drawEllipse(outPin, 6, 6);
        }
    }
}

void ShaderEditor::drawDraggingConnection(QPainter& painter)
{
    if (m_dragConn.type == DragPinType::Output && m_dragConn.node)
    {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.drawLine(m_dragConn.node->getOutputPinPos(), m_dragConn.mouseScenePos);
    }
    else if (m_dragConn.type == DragPinType::Input && m_dragConn.node)
    {
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        painter.drawLine(m_dragConn.mouseScenePos, m_dragConn.node->getInputPinPos(m_dragConn.paramIdx));
    }
}

void ShaderEditor::mousePressEvent(QMouseEvent* event)
{
    QPointF pos = (event->pos() - m_viewOffset) / m_scale;
    if (event->button() == Qt::LeftButton)
    {
        handleLeftClick(pos);
    }
    else if (event->button() == Qt::RightButton)
    {
        handleRightClick(event, pos);
    }
    else if (event->button() == Qt::MiddleButton)
    {
        m_panning = true;
        m_panStartMouse = event->pos();
        m_panStartOffset = m_viewOffset;
        setCursor(Qt::ClosedHandCursor);
    }
}

void ShaderEditor::handleLeftClick(const QPointF& pos)
{
    m_selectedPin = {};
    // Check if clicking on property (input pin name area)
    for (auto& node : m_nodes)
    {
        int idx = 0;
        for (const auto& param : node->getParameters())
        {
            QPointF textPos = node->getPosition() + QPointF(18, 28 + idx * 22 + 16);
            QRectF  propRect(textPos.x() - 2, textPos.y() - 14, 60, 18);
            if (propRect.contains(pos))
            {
                m_selectedPin = {DragPinType::Input, node.get(), idx};
                update();
                return;
            }
            ++idx;
        }
    }
    // Check if clicking on output pin
    for (auto& node : m_nodes)
    {
        if (node->getType() == ShaderNode::Type::Output) continue;
        QPointF outPin = node->getOutputPinPos();
        if (QLineF(pos, outPin).length() < 10)
        {
            ShaderNodeConnection* foundConn = nullptr;
            for (auto& conn : m_connections)
            {
                if (conn->getFrom() == node.get())
                {
                    foundConn = conn.get();
                    break;
                }
            }
            m_dragConn    = {DragPinType::Output, node.get(), -1, pos, foundConn};
            m_selectedPin = {DragPinType::Output, node.get(), -1};
            update();
            return;
        }
    }
    // Check if clicking on input pin (circle)
    for (auto& node : m_nodes)
    {
        int idx = 0;
        for (const auto& param : node->getParameters())
        {
            QPointF inPin = node->getInputPinPos(idx);
            if (QLineF(pos, inPin).length() < 10)
            {
                ShaderNodeConnection* foundConn = nullptr;
                for (auto& conn : m_connections)
                {
                    if (conn->getTo() == node.get() && conn->getToParamIdx() == idx)
                    {
                        foundConn = conn.get();
                        break;
                    }
                }
                m_dragConn    = {DragPinType::Input, node.get(), idx, pos, foundConn};
                m_selectedPin = {DragPinType::Input, node.get(), idx};
                update();
                return;
            }
            ++idx;
        }
    }
    // Otherwise, try to move node (no highlight for selection)
    for (auto& node : m_nodes)
    {
        QRectF rect(node->getPosition(), QSizeF(120, 28 + 22 * node->getParameters().size()));
        if (rect.contains(pos))
        {
            m_selectedNode = node.get();
            m_dragOffset   = pos - node->getPosition();
            m_dragging     = true;
            update();
            return;
        }
    }
    m_selectedNode = nullptr;
    m_dragging     = false;
    update();
}

void ShaderEditor::handleRightClick(QMouseEvent* event, const QPointF& pos)
{
    // Right click on pin: show delete connection menu if has connection
    // Output pin
    for (auto& node : m_nodes)
    {
        if (node->getType() == ShaderNode::Type::Output) continue;
        QPointF outPin = node->getOutputPinPos();
        if (QLineF(pos, outPin).length() < 10)
        {
            for (auto& conn : m_connections)
            {
                if (conn->getFrom() == node.get())
                {
                    QMenu    menu;
                    QAction* delAct   = menu.addAction("Delete Connection");
                    QAction* selected = menu.exec(event->globalPos());
                    if (selected == delAct)
                    {
                        conn->getTo()->setParameterInput(conn->getToParamIdx(), false);
                        m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [&](const std::unique_ptr<ShaderNodeConnection>& c) { return c.get() == conn.get(); }), m_connections.end());
                        update();
                    }
                    return;
                }
            }
        }
    }
    // Input pin
    for (auto& node : m_nodes)
    {
        int idx = 0;
        for (const auto& param : node->getParameters())
        {
            QPointF inPin = node->getInputPinPos(idx);
            if (QLineF(pos, inPin).length() < 10)
            {
                for (auto& conn : m_connections)
                {
                    if (conn->getTo() == node.get() && conn->getToParamIdx() == idx)
                    {
                        QMenu    menu;
                        QAction* delAct   = menu.addAction("Delete Connection");
                        QAction* selected = menu.exec(event->globalPos());
                        if (selected == delAct)
                        {
                            node->setParameterInput(idx, false);
                            m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [&](const std::unique_ptr<ShaderNodeConnection>& c) { return c.get() == conn.get(); }), m_connections.end());
                            update();
                        }
                        return;
                    }
                }
            }
            ++idx;
        }
    }
    // Right click on node body: show node type menu
    for (auto& node : m_nodes)
    {
        QRectF rect(node->getPosition(), QSizeF(120, 28 + 22 * node->getParameters().size()));
        if (rect.contains(pos))
        {
            showNodeTypeMenu(node.get(), event->globalPos());
            return;
        }
    }
}

// No longer needed: property selection is now handled by left click in mousePressEvent
void ShaderEditor::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete && m_selectedPin.type != DragPinType::None && m_selectedPin.node)
    {
        handleDeleteKey();
    }
}

void ShaderEditor::handleDeleteKey()
{
    if (m_selectedPin.type == DragPinType::Output)
    {
        for (auto it = m_connections.begin(); it != m_connections.end();)
        {
            if ((*it)->getFrom() == m_selectedPin.node)
            {
                (*it)->getTo()->setParameterInput((*it)->getToParamIdx(), false);
                it = m_connections.erase(it);
            }
            else
            {
                ++it;
            }
        }
        update();
    }
    else if (m_selectedPin.type == DragPinType::Input)
    {
        for (auto it = m_connections.begin(); it != m_connections.end();)
        {
            if ((*it)->getTo() == m_selectedPin.node && (*it)->getToParamIdx() == m_selectedPin.paramIdx)
            {
                m_selectedPin.node->setParameterInput(m_selectedPin.paramIdx, false);
                it = m_connections.erase(it);
            }
            else
            {
                ++it;
            }
        }
        update();
    }
}

void ShaderEditor::mouseMoveEvent(QMouseEvent* event)
{
    QPointF pos = (event->pos() - m_viewOffset) / m_scale;
    if (m_panning)
    {
        QPointF delta = event->pos() - m_panStartMouse;
        m_viewOffset = m_panStartOffset + delta;
        update();
        return;
    }
    if (m_dragging && m_selectedNode)
    {
        m_selectedNode->setPosition(pos - m_dragOffset);
        update();
    }
    if (m_dragConn.type != DragPinType::None)
    {
        m_dragConn.mouseScenePos = pos;
        update();
    }
}

void ShaderEditor::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF pos = (event->pos() - m_viewOffset) / m_scale;
    if (event->button() == Qt::LeftButton && m_dragging)
    {
        m_dragging = false;
    }
    if (event->button() == Qt::LeftButton && m_dragConn.type != DragPinType::None)
    {
        handleConnectionRelease(pos);
    }
    if (event->button() == Qt::MiddleButton && m_panning)
    {
        m_panning = false;
        unsetCursor();
    }
}

void ShaderEditor::handleConnectionRelease(const QPointF& pos)
{
    // Try to connect to a pin
    if (m_dragConn.type == DragPinType::Output)
    {
        // Try to connect to an input pin
        for (auto& node : m_nodes)
        {
            int idx = 0;
            for (const auto& param : node->getParameters())
            {
                QPointF inPin = node->getInputPinPos(idx);
                if (QLineF(pos, inPin).length() < 10)
                {
                    if (m_dragConn.movingConn)
                    {
                        m_dragConn.movingConn->getTo()->setParameterInput(m_dragConn.movingConn->getToParamIdx(), false);
                        m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [&](const std::unique_ptr<ShaderNodeConnection>& c) { return c.get() == m_dragConn.movingConn; }), m_connections.end());
                    }
                    for (auto it = m_connections.begin(); it != m_connections.end();)
                    {
                        if ((*it)->getTo() == node.get() && (*it)->getToParamIdx() == idx)
                        {
                            node->setParameterInput(idx, false);
                            it = m_connections.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                    m_connections.emplace_back(std::make_unique<ShaderNodeConnection>(m_dragConn.node, node.get(), idx));
                    node->setParameterInput(idx, true);
                    m_dragConn = {};
                    update();
                    return;
                }
                ++idx;
            }
        }
    }
    else if (m_dragConn.type == DragPinType::Input)
    {
        for (auto& node : m_nodes)
        {
            if (node->getType() == ShaderNode::Type::Output) continue;
            QPointF outPin = node->getOutputPinPos();
            if (QLineF(pos, outPin).length() < 10)
            {
                if (m_dragConn.movingConn)
                {
                    m_dragConn.node->setParameterInput(m_dragConn.paramIdx, false);
                    m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [&](const std::unique_ptr<ShaderNodeConnection>& c) { return c.get() == m_dragConn.movingConn; }), m_connections.end());
                }
                for (auto it = m_connections.begin(); it != m_connections.end();)
                {
                    if ((*it)->getFrom() == node.get())
                    {
                        (*it)->getTo()->setParameterInput((*it)->getToParamIdx(), false);
                        it = m_connections.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
                m_connections.emplace_back(std::make_unique<ShaderNodeConnection>(node.get(), m_dragConn.node, m_dragConn.paramIdx));
                m_dragConn.node->setParameterInput(m_dragConn.paramIdx, true);
                m_dragConn = {};
                update();
                return;
            }
        }
    }
    m_dragConn = {};
    update();
}

void ShaderEditor::wheelEvent(QWheelEvent* event)
{
    constexpr double scaleStep      = 1.15;
    QPointF          mousePos       = event->position();
    QPointF          scenePosBefore = (mousePos - m_viewOffset) / m_scale;
    if (event->angleDelta().y() > 0)
        m_scale *= scaleStep;
    else
        m_scale /= scaleStep;
    m_scale               = std::clamp(m_scale, 0.2, 3.0);
    QPointF scenePosAfter = (mousePos - m_viewOffset) / m_scale;
    m_viewOffset += (scenePosAfter - scenePosBefore) * m_scale;
    update();
}

void ShaderEditor::showNodeTypeMenu(ShaderNode* node, const QPoint& globalPos)
{
    QMenu    menu;
    QAction* standardAct = menu.addAction("Standard");
    QAction* simpleAct   = menu.addAction("Simple");
    QAction* clothAct    = menu.addAction("Cloth");
    QAction* hairAct     = menu.addAction("Hair");
    QAction* outputAct   = menu.addAction("Output");
    QAction* selected    = menu.exec(globalPos);
    if (!selected) return;
    if (selected == standardAct) node->setType(ShaderNode::Type::Standard);
    else if (selected == simpleAct)
        node->setType(ShaderNode::Type::Simple);
    else if (selected == clothAct)
        node->setType(ShaderNode::Type::Cloth);
    else if (selected == hairAct)
        node->setType(ShaderNode::Type::Hair);
    else if (selected == outputAct)
        node->setType(ShaderNode::Type::Output);
    update();
}

void ShaderEditor::contextMenuEvent(QContextMenuEvent* event)
{
    QPointF pos    = (event->pos() - m_viewOffset) / m_scale;
    bool    onNode = false;
    for (const auto& node : m_nodes)
    {
        QRectF rect(node->getPosition(), QSizeF(120, 28 + 22 * node->getParameters().size()));
        if (rect.contains(pos))
        {
            onNode = true;
            break;
        }
    }
    if (!onNode)
    {
        QMenu    menu;
        QAction* standardAct = menu.addAction("Add Standard Node");
        QAction* simpleAct   = menu.addAction("Add Simple Node");
        QAction* clothAct    = menu.addAction("Add Cloth Node");
        QAction* hairAct     = menu.addAction("Add Hair Node");
        QAction* outputAct   = menu.addAction("Add Output Node");
        QAction* selected    = menu.exec(event->globalPos());
        if (!selected) return;
        ShaderNode::Type type = ShaderNode::Type::Standard;
        QString          name = "Standard";
        if (selected == simpleAct)
        {
            type = ShaderNode::Type::Simple;
            name = "Simple";
        }
        else if (selected == clothAct)
        {
            type = ShaderNode::Type::Cloth;
            name = "Cloth";
        }
        else if (selected == hairAct)
        {
            type = ShaderNode::Type::Hair;
            name = "Hair";
        }
        else if (selected == outputAct)
        {
            type = ShaderNode::Type::Output;
            name = "Output";
        }
        m_nodes.emplace_back(std::make_unique<ShaderNode>(type, name, pos));
        update();
        return;
    }
    event->ignore();
}
