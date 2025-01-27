/**
 * @file config_editor_frame.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 19/12/2024
 */

#ifndef TOOLBOX_CONFIG_EDITOR_FRAME_HPP
#define TOOLBOX_CONFIG_EDITOR_FRAME_HPP

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class ConfigEditorFrame;
}
QT_END_NAMESPACE

class ConfigEditorFrame : public QWidget
{
Q_OBJECT

public:
    explicit ConfigEditorFrame(QWidget *parent = nullptr);

    ~ConfigEditorFrame() override;

private:
    Ui::ConfigEditorFrame *ui;
};

#endif //TOOLBOX_CONFIG_EDITOR_FRAME_HPP
