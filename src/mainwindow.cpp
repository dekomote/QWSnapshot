#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QtWebKit/QGraphicsWebView>
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

    // Initialize webView
    webView = new QGraphicsWebView();

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

void MainWindow::on_urlEdit_textChanged(const QString &arg1)
{

    if(ui->urlEdit->text() != ""){
        ui -> snapButton -> setEnabled(true);
    }
    else{
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

    webView->setUrl(url);
}

void MainWindow::page_loaded(bool ok)
{
    ui->urlEdit->setEnabled(true);
    ui->snapButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->snapButton->setVisible(true);
    ui->stopButton->setVisible(false);

    if(ok){
        // The page loaded successfully. Start rendering the image

        // Set webkit's viewport size to match content length and chosen width
        webView->setResizesToContents(true);
        webView->page()->setPreferredContentsSize(QSize(ui->viewportWidth->value(), 600));

        // Initialize the image, render the webkit widget into it
        QImage *img = new QImage(webView->page()->viewportSize(), QImage::Format_ARGB32);
        QPainter *paint = new QPainter(img);
        webView->page()->currentFrame()->render(paint);
        paint->end();

        // Prepare the path, filename
        QString selFmt = ui->imgFmtCmb->itemText(ui->imgFmtCmb->currentIndex());
        QString timestamp = QString().setNum(QDateTime::currentDateTime().toTime_t());;
        QString fileName = QString("%1%2%3_%4.%5").arg(ui->saveLocationEdit->text(), QString(QDir::separator()), QString("QWSnap"), timestamp, selFmt);

        // Save the image
        img->save(fileName, selFmt.toLatin1());

        // Open the image if checked
        if(ui->openImageChk->checkState() == Qt::Checked){
            QUrl fileUrl = QUrl().fromLocalFile(fileName);
            QDesktopServices::openUrl(fileUrl);
        }
    }
    else{
        QMessageBox::warning(this, "Oops...", "Either your URL is malformed (use https://), the website is unreachable or you don't have an internet connection.");
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



