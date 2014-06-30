#include <vector>

#include "SkinVolumeSegmentationWidget.h"
#include "ui_SkinVolumeSegmentationWidget.h"

SkinVolumeSegmentationWidget::SkinVolumeSegmentationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkinVolumeSegmentationWidget)
{
    ui->setupUi(this);

    ui->surfaceBtn_2->setEnabled(true);
    ui->thresholdBtn_2->setEnabled(false);
    ui->lowerSlider->setEnabled(false);
    ui->upperSlider->setEnabled(false);

    ui->lowerSlider->setTickInterval(1);
    ui->lowerSlider->setRange(0,15);
    ui->lowerSlider->setValue(0);

    ui->upperSlider->setTickInterval(1);
    ui->upperSlider->setRange(0,255);
    ui->upperSlider->setValue(255);

    ui->lowerThreshold_2->setText("0");
    ui->upperThreshold_2->setText("15");

    ui->sigmaSize->setText("10");

    lowerThreshVal = 128;
    upperThreshVal = 255;

    numberOfSeeds = 0;

    this->seedPointFlag = false;
    this->surfaceVolumeFlag = false;
    this->thresholdVolumeFlag = false;

    this->vtkSurfaceVolume = vtkSmartPointer<vtkImageData>::New();
    this->vtkThresholdVolume = vtkSmartPointer<vtkImageData>::New();
    this->vtkRegionGrowingVolume = vtkSmartPointer<vtkImageData>::New();
    this->vtkContourVolume = vtkSmartPointer<vtkImageData>::New();

   this->skinVolumeSegmentation = SkinVolumeSegmentation::New();
}

SkinVolumeSegmentationWidget::~SkinVolumeSegmentationWidget()
{
    delete ui;
}

void SkinVolumeSegmentationWidget::surfaceEnhancement()
{
        
    this->skinVolumeSegmentation->computeSurfaceVolume(ui->sigmaSize->text().toInt());

    this->vtkSurfaceVolume = skinVolumeSegmentation->getSurfaceVolume();

    this->displayWidget = this->mainWindow->getDisplayWidget();
    this->displayWidget->setAndDisplayVolume(vtkSurfaceVolume);

    this->surfaceVolumeFlag = true;
    ui->thresholdBtn_2->setEnabled(true);

}

void SkinVolumeSegmentationWidget::threshold(bool on)
{

    if(on){

        this->skinVolumeSegmentation->computeThresholdVolume(lowerThreshVal,upperThreshVal);

        this->vtkThresholdVolume = skinVolumeSegmentation->getThresholdVolume();

        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayVolume(vtkThresholdVolume);

        this->thresholdVolumeFlag = true;

        ui->lowerSlider->setEnabled(true);
        ui->upperSlider->setEnabled(true);

    }else{

        ui->lowerSlider->setEnabled(false);
        ui->upperSlider->setEnabled(false);
    }

}

void SkinVolumeSegmentationWidget::changeLowerThreshold(int tickValue)
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

        this->skinVolumeSegmentation->computeThresholdVolume(lowerThreshVal,upperThreshVal);

        this->vtkThresholdVolume = skinVolumeSegmentation->getThresholdVolume();

        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayVolume(vtkThresholdVolume);

}

void SkinVolumeSegmentationWidget::changeUpperThreshold(int tickValue)
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

        this->skinVolumeSegmentation->computeThresholdVolume(lowerThreshVal,upperThreshVal);

        this->vtkThresholdVolume = skinVolumeSegmentation->getThresholdVolume();

        this->displayWidget = this->mainWindow->getDisplayWidget();
        this->displayWidget->setAndDisplayVolume(vtkThresholdVolume);
}

void SkinVolumeSegmentationWidget::regionGrowing()
{
    this->skinVolumeSegmentation->setSeeds(seedPoints);
    this->skinVolumeSegmentation->computeRegionGrowing();

    this->vtkRegionGrowingVolume = skinVolumeSegmentation->getRegionGrowingVolume();

    this->vtkContourVolume = skinVolumeSegmentation->getContourVolume();

    this->displayWidget->setAndDisplayVolume(vtkRegionGrowingVolume);
}

void SkinVolumeSegmentationWidget::outerContour()
{
    this->skinVolumeSegmentation->computeOuterContour();
    this->vtkContourVolume = skinVolumeSegmentation->getContourVolume();
    this->displayWidget->setAndDisplayVolume(vtkContourVolume);

}

void SkinVolumeSegmentationWidget::setVolume(vtkSmartPointer<vtkImageData> Volume)
{
    this->skinVolumeSegmentation->setVolume(Volume);
}

void SkinVolumeSegmentationWidget::setMainWindow(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;
}

void SkinVolumeSegmentationWidget::setVTKThreeViews(VTKThreeViews* threeViews)
{
    this->threeViews = threeViews;
}

void SkinVolumeSegmentationWidget::getCoordinates()
{

    VolumeType::IndexType seedPoint;

    std::vector<int>* pickedCoordinates = threeViews->getPickedCoordinates(); 
    
    seedPoint[0] = pickedCoordinates->at(0);
    seedPoint[1] = pickedCoordinates->at(1);
    seedPoint[2] = pickedCoordinates->at(2);
    
    QString str;

    ui->tableWidget->insertRow(numberOfSeeds);

    ui->tableWidget->setItem(numberOfSeeds, 0, new QTableWidgetItem(str.setNum(seedPoint[0])));
    ui->tableWidget->setItem(numberOfSeeds, 1, new QTableWidgetItem(str.setNum(seedPoint[1])));
    ui->tableWidget->setItem(numberOfSeeds, 2, new QTableWidgetItem(str.setNum(seedPoint[2])));
    ui->tableWidget->setItem(numberOfSeeds, 3, new QTableWidgetItem(str.setNum(vtkThresholdVolume->GetScalarComponentAsDouble(seedPoint[0],seedPoint[1],seedPoint[2],0))));

    this->seedPoints.push_back(seedPoint);

    this->numberOfSeeds++;
    this->seedPointFlag = true;

}

void SkinVolumeSegmentationWidget::deleteSeed()
{
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(row);

    std::cout<<std::endl;
    seedPoints.erase(seedPoints.begin()+row);

    this->numberOfSeeds--;

}
