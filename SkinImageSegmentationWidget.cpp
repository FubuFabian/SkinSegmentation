#include <vector>

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
    
    numberOfSeeds = 0;
    
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

void SkinImageSegmentationWidget::regionGrowing()
{
    this->skinImageSegmentation->setSeeds(seedPoints);
    this->skinImageSegmentation->computeRegionGrowing();
    
    this->vtkRegionGrowingImage = skinImageSegmentation->getRegionGrowingImage();
    this->displayWidget->setAndDisplayImage(vtkRegionGrowingImage);
}

void SkinImageSegmentationWidget::setImage(vtkSmartPointer<vtkImageData> image)
{
    this->skinImageSegmentation->setImage(image);
}

void SkinImageSegmentationWidget::setMainWindow(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;
}

void SkinImageSegmentationWidget::getCoordinates()
{
    
    ImageType::IndexType seedPoint;

    std::cout<<std::endl;
    seedPoint[0] = mainWindow->getDisplayWidget()->getXPicked();
    seedPoint[1] = mainWindow->getDisplayWidget()->getYPicked();
    
    QString str;
    
    ui->tableWidget->insertRow(numberOfSeeds);

    ui->tableWidget->setItem(numberOfSeeds, 0, new QTableWidgetItem(str.setNum(seedPoint[0])));
    ui->tableWidget->setItem(numberOfSeeds, 1, new QTableWidgetItem(str.setNum(seedPoint[1])));
    std::cout << "Seed Point -> (x = " << seedPoint[0] << ", y = " << seedPoint[1] << ")" << std::endl;
    
    this->seedPoints.push_back(seedPoint);
    
    this->numberOfSeeds++;
    this->seedPointFlag = true;
    
}
    
void SkinImageSegmentationWidget::deleteSeed()
{
    int row = ui->tableWidget->currentRow();  
    ui->tableWidget->removeRow(row);
    
    std::cout<<std::endl;    
    seedPoints.erase(seedPoints.begin()+row);
    
    this->numberOfSeeds--;
    
}
