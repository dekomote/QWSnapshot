#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtWebKit/QGraphicsWebView>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT    
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QGraphicsWebView* webView;
    QSettings* settings;
    
protected:
    void changeEvent(QEvent *e);
    
protected slots:
    void on_urlEdit_textChanged(const QString &arg1);

    void on_snapButton_clicked();

    void page_loaded(bool);
    void page_progress(int);
    void about_show();

    void on_stopButton_clicked();

private slots:
    void on_saveSettings_clicked();

    void on_loadSettings_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
