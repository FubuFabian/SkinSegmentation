#include "SkinImageSegmentationWidget.h"
#include "ui_SkinImageSegmentationWidget.h"

SkinImageSegmentationWidget::SkinImageSegmentationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkinImageSegmentationWidget)
{
    ui->setupUi(this);
    
    ui->surfaceBtn_2->setEnabled(true);
    ui->thresholdBtn_2->setEnabled(false);
    ui->lowerSlider->setEnabled(false);
    ui->upperSlider->setEnabled(false);
    
    ui->lowerSlider->setTickInterval(1);
    ui->lowerSlider->setRange(0,255);
    ui->lowerSlider->setValue(128);
	
    ui->upperSlider->setTickInterval(1);
    ui->upperSlider->setRange(0,255);
    ui->upperSlider->setValue(255);
    
    ui->lowerThreshold_2->setText("128");
    ui->upperThreshold_2->setText("255");
    
    lowerThreshVal = 128;
    upperThreshVal = 255;
    
    this->seedPointFlag = false;
    this->surfaceImageFlag = false;
    this->thresholdImageFlag = false;
    
    this->vtkSurfaceImage = vtkSmartPointer<vtkImageData>::New();
    this->vtkThresholdImage = vtkSmartPointer<vtkImageData>::New();
    this->vtkRegionGrowingImage = vtkSmartPointer<vtkImageData>::New();
    this->vtkContourImage = vtkSmartPointer<vtkImageData>::New();

   this->skinImageSegmentation = SkinImageSegmentation::New();
}

SkinImageSegmentationWidget::~SkinImageSegmentationWidget()
{
    delete ui;
}

void SkinImageSegmentationWidget::surfaceEnhancement()
{
    this->skinImageSegmentation->computeSurfaceImage();

    this->vtkSurfaceImage = skinImageSegmentation->getSurfaceImage();
    
    this->displayWidget = this->mainWindow->getDisplayWidget();
    this->displayWidget->setAndDisplayImage(vtkSurfaceImage);
    
    this->surfaceImageFlag = true;

    ui->thresholdBtn_2->setEnabled(true);

}

void SkinImageSegmentationWidget::threshold(bool on)
{
    
    if(on){
        
        this->skinImageSegmentation->computeThresholdImage(lowerThreshVal,upperThreshVal);

        this->vtkThresholdImage = skinImageSegmentation->getThresholdImage();
    
        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayImage(vtkThresholdImage);
    
        this->thresholdImageFlag = true;

        ui->lowerSlider->setEnabled(true);
        ui->upperSlider->setEnabled(true);
        
    }else{
        
        ui->lowerSlider->setEnabled(false);
        ui->upperSlider->setEnabled(false);
    }
    
       
}

void SkinImageSegmentationWidget::changeLowerThreshold(int tickValue)
{

    if (tickValue<ui->upperSlider->value()){
		lowerThreshVal = tickValue;
		upperThreshVal = ui->upperSlider->value();
	}else{
		lowerThreshVal = ui->upperSlider->value();
		upperThreshVal = tickValue;
	}
		
	ui->lowerThreshold_2->setText(QString::number(lowerThreshVal));
	ui->upperThreshold_2->setText(QString::number(upperThreshVal));
        
        this->skinImageSegmentation->computeThresholdImage(lowerThreshVal,upperThreshVal);

        this->vtkThresholdImage = skinImageSegmentation->getThresholdImage();
    
        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayImage(vtkThresholdImage);

}

void SkinImageSegmentationWidget::changeUpperThreshold(int tickValue)
{

    if (tickValue<ui->lowerSlider->value()){
		lowerThreshVal = tickValue;
		upperThreshVal = ui->lowerSlider->value();
	}else{
		lowerThreshVal = ui->lowerSlider->value();
		upperThreshVal = tickValue;
	}

	ui->lowerThreshold_2->setText(QString::number(lowerThreshVal));
	ui->upperThreshold_2->setText(QString::number(upperThreshVal));
        
        this->skinImageSegmentation->computeThresholdImage(lowerThreshVal,upperThreshVal);

        this->vtkThresholdImage = skinImageSegmentation->getThresholdImage();
    
        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayImage(vtkThresholdImage);
}

void SkinImageSegmentationWidget::setImage(vtkSmartPointer<vtkImageData> image)
{
	skinImageSegmentation->setImage(image);
}

void SkinImageSegmentationWidget::setMainWindow(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;
}

void SkinImageSegmentationWidget::getCoordinates()
{
//    std::cout<<std::endl;
//    this->seedPoint[0] = mainWindow->getDisplayWidget()->getXPicked();
//    this->seedPoint[1] = mainWindow->getDisplayWidget()->getYPicked();
//    
//    QString str;
//
//    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(str.setNum(seedPoint[0])));
//    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(str.setNum(seedPoint[1])));
//    std::cout << "Seed Point -> (x = " << seedPoint[0] << ", y = " << seedPoint[1] << ")" << std::endl;
//    
//    this->seedPointFlag = true;
//    
//    if(probabilityImageFlag)
//        ui->segmentBtn->setEnabled(true);

}
    
void SkinImageSegmentationWidget::newSeed()
{
//    this->seedPoint[0] = 0;
//    this->seedPoint[1] = 0;
//
//    this->displayWidget->setAndDisplayImage(vtkThresholdImage);
//    
//    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(0));
//    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(0));
//    
//    ui->segmentBtn->setEnabled(false);
//    ui->saveBtn->setEnabled(false);
//    
}
