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
