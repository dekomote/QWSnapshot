#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QDir>
#include <QDateTime>
#include <QDesktopServices>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stopButton->setVisible(false);

    ui->mainToolBar->setVisible(false);

    // Initialize webView
    webView = new QWebView();

    // Initialize and load settings
    settings = new QSettings("Atomidata", "QWSnapshot");
    ui->saveLocationEdit->setText(settings->value("mainwindow/save_location", QDir::homePath()).toString());
    ui->viewportWidth->setValue(settings->value("mainwindow/viewport_width", 1024).toInt());
    ui->imgFmtCmb->setCurrentIndex(settings->value("mainwindow/image_format", 0).toInt());
    ui->openImageChk->setChecked(settings->value("mainwindow/open_image", true).toBool());

    // Connect signals and slots
    connect(webView, SIGNAL(loadFinished(bool)), this ,SLOT(page_loaded(bool)));
    connect(webView, SIGNAL(loadProgress(int)), this ,SLOT(page_progress(int)));
    connect(ui->actionAbout, SIGNAL(activated()), this ,SLOT(about_show()));
}


MainWindow::~MainWindow()
{
    delete ui;
    delete webView;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_urlEdit_textChanged()
{
    if (ui->urlEdit->text() != "") {
        ui->snapButton->setEnabled(true);
    } else {
        ui->snapButton->setEnabled(false);
    }
}

void MainWindow::on_snapButton_clicked()
{
    ui->urlEdit->setEnabled(false);
    ui->snapButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->snapButton->setVisible(false);
    ui->stopButton->setVisible(true);

    QUrl url = ui->urlEdit->text();
    if(url.scheme()==""){
        url.setScheme("http");
    }

    webView->setUrl(url);
}

void MainWindow::page_loaded(bool ok)
{
    ui->urlEdit->setEnabled(true);
    ui->snapButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->snapButton->setVisible(true);
    ui->stopButton->setVisible(false);

    if (ok) {
        // The page loaded successfully. Start rendering the image

        // Set webkit's viewport size to match content length and chosen width
        webView->page()->setPreferredContentsSize(QSize(ui->viewportWidth->value(), 600));
        webView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        webView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
        webView->page()->setViewportSize(QSize(ui->viewportWidth->value(), webView->page()->mainFrame()->contentsSize().height()));

        // Initialize the image, render the webkit widget into it
        QImage *img = new QImage(webView->page()->viewportSize(), QImage::Format_ARGB32);
        QPainter *paint = new QPainter(img);
        webView->page()->mainFrame()->render(paint);
        paint->end();

        // Prepare the path, filename
        QString selFmt = ui->imgFmtCmb->itemText(ui->imgFmtCmb->currentIndex());
        QString timestamp = QString().setNum(QDateTime::currentDateTime().toTime_t());;
        QString fileName = QString("%1%2%3_%4.%5").arg(ui->saveLocationEdit->text(), QString(QDir::separator()), QString("QWSnap"), timestamp, selFmt);

        // Save the image
        if (!img->save(fileName, selFmt.toLatin1())) {
            QMessageBox::warning(this, "Oops...", "The file can not be written. Check folder permissions.");
        } else {
            // Open the image if checked
            if (ui->openImageChk->checkState() == Qt::Checked) {
                QUrl fileUrl = QUrl().fromLocalFile(fileName);
                QDesktopServices::openUrl(fileUrl);
            }
        }
    } else {
        QMessageBox::warning(this, "Oops...", "Either your URL is malformed, the website is unreachable or you don't have an internet connection.");
    }
}

void MainWindow::page_progress(int progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::about_show()
{
    QMessageBox::about (this , "About QWSnapshot", "QWSnapshot - take snapshots from websites, customize image types and client width." );
}

void MainWindow::on_stopButton_clicked()
{
    webView->stop();
}

void MainWindow::on_saveSettings_clicked()
{
    settings->setValue("mainwindow/save_location", ui->saveLocationEdit->text());
    settings->setValue("mainwindow/viewport_width", ui->viewportWidth->value());
    settings->setValue("mainwindow/image_format", ui->imgFmtCmb->currentIndex());
    settings->setValue("mainwindow/open_image", ui->openImageChk->checkState() == Qt::Checked);
}

void MainWindow::on_loadSettings_clicked()
{
    ui->saveLocationEdit->setText(settings->value("mainwindow/save_location", QDir::homePath()).toString());
    ui->viewportWidth->setValue(settings->value("mainwindow/viewport_width", 1024).toInt());
    ui->imgFmtCmb->setCurrentIndex(settings->value("mainwindow/image_format", 0).toInt());
    ui->openImageChk->setChecked(settings->value("mainwindow/open_image", true).toBool());
}

void MainWindow::on_saveLocationEdit_editingFinished()
{
    if (!QDir().exists(ui->saveLocationEdit->text())) {

        QMessageBox msgBox;
        msgBox.setText("The folder does not exits.");
        msgBox.setInformativeText("Do you want to create this folder?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Yes:
            if (!QDir().mkpath(ui->saveLocationEdit->text())) {
                QMessageBox::warning(this, "Oops...", "The path can not be created. Check if the path is valid or path permissions.");
            }
            break;

        case QMessageBox::No:
            break;
        }
    }
}
