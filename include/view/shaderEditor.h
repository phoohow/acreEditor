#pragma once

#include <QWidget>
#include <QPointF>
#include <vector>
#include <memory>

class ShaderNode;
class ShaderNodeConnection;
class ShaderEditor : public QWidget
{
    std::vector<std::unique_ptr<ShaderNode>>           m_nodes;
    std::vector<std::unique_ptr<ShaderNodeConnection>> m_connections;

    double      m_scale        = 1.0;
    ShaderNode* m_selectedNode = nullptr;
    bool        m_dragging     = false;
    QPointF     m_dragOffset   = QPointF(0, 0);
    QPointF     m_viewOffset   = QPointF(0, 0);

public:
    enum class DragPinType
    {
        None,
        Output,
        Input
    };

    struct DragState
    {
        DragPinType           type     = DragPinType::None;
        ShaderNode*           node     = nullptr;
        int                   paramIdx = -1; // for input pin
        QPointF               mouseScenePos;
        ShaderNodeConnection* movingConn = nullptr;
    };

    struct SelectedPin
    {
        DragPinType type     = DragPinType::None;
        ShaderNode* node     = nullptr;
        int         paramIdx = -1;
    };

public:
    explicit ShaderEditor(QWidget* parent = nullptr);

    ~ShaderEditor();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    // Paint helpers
    void drawConnections(QPainter& painter);
    void drawNodes(QPainter& painter);
    void drawDraggingConnection(QPainter& painter);

    // Mouse/keyboard helpers
    void handleLeftClick(const QPointF& pos);
    void handleRightClick(QMouseEvent* event, const QPointF& pos);
    void handleDeleteKey();
    void handleConnectionRelease(const QPointF& pos);

private:
    DragState   m_dragConn;
    SelectedPin m_selectedPin;
    bool        m_panning = false;
    QPointF     m_panStartMouse;
    QPointF     m_panStartOffset;

private:
    void showNodeTypeMenu(ShaderNode* node, const QPoint& globalPos);
};
