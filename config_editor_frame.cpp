/**
 * @file config_editor_frame.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 19/12/2024
 */

// You may need to build the project (run Qt uic code generator) to get "ui_config_editor_frame.h" resolved

#include "config_editor_frame.hpp"
#include "ui_config_editor_frame.h"


ConfigEditorFrame::ConfigEditorFrame(QWidget *parent) :
        QWidget(parent), ui(new Ui::ConfigEditorFrame)
{
    ui->setupUi(this);
}

ConfigEditorFrame::~ConfigEditorFrame()
{
    delete ui;
}
