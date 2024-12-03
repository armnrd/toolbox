/**
 * @file webview_frame.hpp
 * @brief ${DESCRIPTION}
 *
 * @author Arindam
 * @date 22/11/2024
 */

#ifndef TOOLBOX_WEBVIEW_FRAME_HPP
#define TOOLBOX_WEBVIEW_FRAME_HPP

#include <QWidget>
#include <QWebEngineProfile>
#include <QWebEngineView>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class WebViewFrame;
}
QT_END_NAMESPACE

class WebViewFrame : public QWidget
{
Q_OBJECT

public:
    explicit WebViewFrame(QWidget *parent = nullptr);

    ~WebViewFrame() override;

private:
    Ui::WebViewFrame *ui;
    QWebEngineProfile *profile;
    QWebEngineView *web_view;
};


#endif //TOOLBOX_WEBVIEW_FRAME_HPP
