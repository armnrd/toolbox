/**
 * @file webview_frame.cpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 22/11/2024
 */

#include "webview_frame.hpp"
#include "ui_webview_frame.h"
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineView>


WebViewFrame::WebViewFrame(QWidget *parent) :
        QWidget(parent), ui(new Ui::WebViewFrame)
{
    ui->setupUi(this);

    // Set up the browser profile and create web_view
    profile = new QWebEngineProfile("toolbox", this);
    web_view = new QWebEngineView(profile, this);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
}

WebViewFrame::~WebViewFrame()
{
    delete ui;
}
