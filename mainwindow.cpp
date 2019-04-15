#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "QSize"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QFileDialog* fileDialog =  new QFileDialog(this);
    QString file_name = QFileDialog::getOpenFileName(fileDialog,"Picture choose","/Users/denhiroshi/Downloads/实验教学","Image Files(*.bmp)");

    file = new QImage(file_name);
    QImage* image = file;
    QPixmap* pixmap= new QPixmap();
    pixmap->convertFromImage(*image);
    pixmap->scaled(QSize(200,200), Qt::KeepAspectRatio);
    ui->pre->setScaledContents(true);
    ui->pre->setPixmap(*pixmap);
}

void MainWindow::on_pushButton_clicked()
{
    this->close();
}

// 直方图均衡化
void MainWindow::on_pushButton_3_clicked()
{
    QImage copy = file->copy();
    unsigned char* img = copy.bits();
    int height = copy.height();
    int width = copy.width();
    int size = height *width;
    int gray[256] = { 0 };  //记录每个灰度级别下的像素个数
    double gray_prob[256] = { 0 };  //记录灰度分布密度
    double gray_distribution[256] = { 0 };  //记录累计密度
    int gray_equal[256] = { 0 };  //均衡化后的灰度值

    //统计每个灰度下的像素个数
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            gray[img[width*j+i]]++;
        }
    }

    //统计灰度频率
    for (int i = 0; i < 256; i++)
    {
        gray_prob[i] = ((double)gray[i] / size);
    }

    //计算累计密度
    gray_distribution[0] = gray_prob[0];
    for (int i = 1; i < 256; i++)
    {
        gray_distribution[i] = gray_distribution[i-1] +gray_prob[i];
    }

    //重新计算均衡化后的灰度值，四舍五入。参考公式：(N-1)*T+0.5
    for (int i = 0; i < 256; i++)
    {
        gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
    }

    //直方图均衡化,更新原图每个点的像素值
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
        {
            img[width*j+i] =  gray_equal[img[width*j+i]];
        }
    }

    QImage* new_Img = new QImage(img, width, height,file->format());
    QPixmap* pixmap= new QPixmap();
    pixmap->convertFromImage(*new_Img);
    pixmap->scaled(QSize(200,200), Qt::KeepAspectRatio);
    ui->after->setPixmap(*pixmap);
}

// 中值滤波
void MainWindow::on_pushButton_4_clicked()
{
    QImage copy = file->copy();
    unsigned char* img = copy.bits();
    unsigned char* out = copy.bits();
    int height = copy.height();
    int width = copy.width();
    for(int i=0; i< height; i++)
    {
        for(int j=0; j< width; j++)
        {
            if(j!=0 && j!=width-1 && i!=0 && i!=height-1)
            {
                QVector<int> vec(9);
                vec[0] = img[j-1+(i-1)*width]; // 左上
                vec[1] = img[j+(i-1)*width]; // 上
                vec[2] = img[j+1+(i-1)*width]; // 右上
                vec[3] = img[j-1+(i)*width]; // 左
                vec[4] = img[j+(i)*width]; // 中
                vec[5] = img[j+1+(i)*width]; // 右
                vec[6] = img[j-1+(i+1)*width]; //左下
                vec[7] = img[j+(i+1)*width]; // 下
                vec[8] = img[j+1+(i+1)*width]; // 右下
                qSort(vec.begin(),vec.end());
                int r = vec[4];
                out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
            }
        }
    }

    // 上边界处理
    for(int j=1; j<width-1; j++)
    {
        int i=0;
        QVector<int> vec(6);
        vec[0] = img[j-1+(i)*width]; // 左
        vec[1] = img[j+(i)*width]; // 中
        vec[2] = img[j+1+(i)*width]; // 右
        vec[3] = img[j-1+(i+1)*width]; //左下
        vec[4] = img[j+(i+1)*width]; // 下
        vec[5] = img[j+1+(i+1)*width]; // 右下
        qSort(vec.begin(),vec.end());
        int r = vec[2];
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }


    // 下边界处理
    for(int j=1; j<width-1; j++)
    {
        int i=height-1;
        QVector<int> vec(6);
        vec[0] = img[j-1+(i-1)*width]; // 左上
        vec[1] = img[j+(i-1)*width]; // 上
        vec[2] = img[j+1+(i-1)*width]; // 右上
        vec[3] = img[j-1+(i)*width]; // 左
        vec[4] = img[j+(i)*width]; // 中
        vec[5] = img[j+1+(i)*width]; // 右
        qSort(vec.begin(),vec.end());
        int r = vec[2];
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    // 左边界处理
    for(int i=1; i<height-1; i++)
    {
        int j=0;
        QVector<int> vec(6);
        vec[0] = img[j+(i-1)*width]; // 上
        vec[1] = img[j+1+(i-1)*width]; // 右上
        vec[2] = img[j+(i)*width]; // 中
        vec[3] = img[j+1+(i)*width]; // 右
        vec[4] = img[j+(i+1)*width]; // 下
        vec[5] = img[j+1+(i+1)*width]; // 右下
        qSort(vec.begin(),vec.end());
        int r = vec[2];
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    // 右边界处理
    for(int i=1; i<height-1; i++)
    {
        int j=width-1;
        QVector<int> vec(6);
        vec[0] = img[j-1+(i-1)*width]; // 左上
        vec[1] = img[j+(i-1)*width]; // 上
        vec[2] = img[j-1+(i)*width]; // 左
        vec[3] = img[j+(i)*width]; // 中
        vec[4] = img[j-1+(i+1)*width]; //左下
        vec[5] = img[j+(i+1)*width]; // 下
        qSort(vec.begin(),vec.end());
        int r = vec[2];
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    QVector<int> vec(4);
    // 左上点
    int i=0,j=0;
    vec[0] = img[j+(i)*width]; // 中
    vec[1] = img[j+1+(i)*width]; // 右
    vec[2] = img[j+(i+1)*width]; // 下
    vec[3] = img[j+1+(i+1)*width]; // 右下
    qSort(vec.begin(),vec.end());
    int r = vec[1];
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 右上点
    j=width-1;
    vec[0] = img[j+(i)*width]; // 中
    vec[1] = img[j-1+(i)*width]; // 左
    vec[2] = img[j+(i+1)*width]; // 下
    vec[3] = img[j-1+(i+1)*width]; // 左下
    qSort(vec.begin(),vec.end());
    r = vec[1];
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 左下点
    i = height-1;
    j = 0;
    vec[0] = img[j+(i-1)*width]; // 上
    vec[1] = img[j+1+(i-1)*width]; // 右上
    vec[2] = img[j+(i)*width]; // 中
    vec[3] = img[j+1+(i)*width]; // 右
    qSort(vec.begin(),vec.end());
    r = vec[1];
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 右下点
    j=width-1;
    vec[0] = img[j-1+(i-1)*width]; // 左上
    vec[1] = img[j+(i-1)*width]; // 上
    vec[2] = img[j+(i)*width]; // 中
    vec[3] = img[j-1+(i)*width]; // 左
    qSort(vec.begin(),vec.end());
    r = vec[1];
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;


    QImage* new_Img = new QImage(out, width, height,file->format());
    QPixmap* pixmap= new QPixmap();
    pixmap->convertFromImage(*new_Img);
    pixmap->scaled(QSize(200,200), Qt::KeepAspectRatio);
    ui->after->setPixmap(*pixmap);
}


// 均值滤波
void MainWindow::on_pushButton_5_clicked()
{
    QImage copy = file->copy();
    unsigned char* img = copy.bits();
    unsigned char* out = copy.bits();
    int height = copy.height();
    int width = copy.width();
    QVector<int> vec(9);
    for(int i=0; i< height; i++)
    {
        for(int j=0; j< width; j++)
        {
            if(j!=0 && j!=width-1 && i!=0 && i!=height-1)
            {
                int sum = 0;
                sum += img[j-1+(i-1)*width]; // 左上
                sum += img[j+(i-1)*width]; // 上
                sum += img[j+1+(i-1)*width]; // 右上
                sum += img[j-1+(i)*width]; // 左
                sum += img[j+(i)*width]; // 中
                sum += img[j+1+(i)*width]; // 右
                sum += img[j-1+(i+1)*width]; //左下
                sum += img[j+(i+1)*width]; // 下
                sum += img[j+1+(i+1)*width]; // 右下
                int r = (int)(sum / 9);
                out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
            }
        }
    }

    // 上边界处理
    for(int j=1; j<width-1; j++)
    {
        int i=0;
        int sum= 0;
        sum += img[j-1+(i)*width]; // 左
        sum += img[j+(i)*width]; // 中
        sum += img[j+1+(i)*width]; // 右
        sum += img[j-1+(i+1)*width]; //左下
        sum += img[j+(i+1)*width]; // 下
        sum += img[j+1+(i+1)*width]; // 右下
        int r = (int)(sum / 6);
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }


    // 下边界处理
    for(int j=1; j<width-1; j++)
    {
        int i=height-1;
        int sum = 0;
        sum += img[j-1+(i-1)*width]; // 左上
        sum += img[j+(i-1)*width]; // 上
        sum += img[j+1+(i-1)*width]; // 右上
        sum += img[j-1+(i)*width]; // 左
        sum += img[j+(i)*width]; // 中
        sum += img[j+1+(i)*width]; // 右
        int r = (int)(sum / 6);
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    // 左边界处理
    for(int i=1; i<height-1; i++)
    {
        int j=0;
        int sum = 0;
        sum += img[j+(i-1)*width]; // 上
        sum += img[j+1+(i-1)*width]; // 右上
        sum += img[j+(i)*width]; // 中
        sum += img[j+1+(i)*width]; // 右
        sum += img[j+(i+1)*width]; // 下
        sum += img[j+1+(i+1)*width]; // 右下
        int r = (int)(sum / 6);
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    // 右边界处理
    for(int i=1; i<height-1; i++)
    {
        int j=width-1;
        int sum = 0;
        sum += img[j-1+(i-1)*width]; // 左上
        sum += img[j+(i-1)*width]; // 上
        sum += img[j-1+(i)*width]; // 左
        sum += img[j+(i)*width]; // 中
        sum += img[j-1+(i+1)*width]; //左下
        sum += img[j+(i+1)*width]; // 下
        int r = (int)(sum / 6);
        out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;
    }

    int sum = 0;
    // 左上点
    int i=0,j=0;
    sum += img[j+(i)*width]; // 中
    sum += img[j+1+(i)*width]; // 右
    sum += img[j+(i+1)*width]; // 下
    sum += img[j+1+(i+1)*width]; // 右下
    int r = (int)(sum / 4);
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 右上点
    sum = 0;
    j=width-1;
    sum += img[j+(i)*width]; // 中
    sum += img[j-1+(i)*width]; // 左
    sum += img[j+(i+1)*width]; // 下
    sum += img[j-1+(i+1)*width]; // 左下
    r = (int)(sum / 4);
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 左下点
    sum = 0;
    i = height-1;
    j = 0;
    sum += img[j+(i-1)*width]; // 上
    sum += img[j+1+(i-1)*width]; // 右上
    sum += img[j+(i)*width]; // 中
    sum += img[j+1+(i)*width]; // 右
    r = (int)(sum / 4);
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;

    // 右下点
    j=width-1;
    sum += img[j-1+(i-1)*width]; // 左上
    sum += img[j+(i-1)*width]; // 上
    sum += img[j+(i)*width]; // 中
    sum += img[j-1+(i)*width]; // 左
    r = (int)(sum / 4);
    out[i*width+j] = 255<<24 | r<<16 | r<<8 |r;


    QImage* new_Img = new QImage(out, width, height,file->format());
    QPixmap* pixmap= new QPixmap();
    pixmap->convertFromImage(*new_Img);
    pixmap->scaled(QSize(200,200), Qt::KeepAspectRatio);
    ui->after->setPixmap(*pixmap);
}
