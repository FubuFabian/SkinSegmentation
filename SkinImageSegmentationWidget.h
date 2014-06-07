#ifndef SKINIMAGESEGMENTATIONWIDGET_H
#define SKINIMAGESEGMENTATIONWIDGET_H

#include "mainwindow.h"
#include "QVTKImageWidget.h"
#include "SkinImageSegmentation.h"

#include <QWidget>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <itkImage.h>

namespace Ui {
    class SkinImageSegmentationWidget;
}

class SkinImageSegmentationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkinImageSegmentationWidget(QWidget *parent = 0);
    ~SkinImageSegmentationWidget();
    
    /**
     * \brief set the ultrasoun image
     * @param a vtk image
     */
    void setImage(vtkSmartPointer<vtkImageData> image);
    
    /**
     * \brief Set the calling main window
     * @param mainWindow
     */
    void setMainWindow(MainWindow * mainWindow);

private:
    Ui::SkinImageSegmentationWidget *ui;
    
    SkinImageSegmentation * skinImageSegmentation; ///< has all the functions for image segmentation
    
    MainWindow * mainWindow; ///< the calling main window
    
    vtkSmartPointer<vtkImageData> vtkSurfaceImage; ///< the surface image
    
    vtkSmartPointer<vtkImageData> vtkThresholdImage; ///< the surface image
    
    vtkSmartPointer<vtkImageData> vtkRegionGrowingImage; ///< the region growing image
    
    vtkSmartPointer<vtkImageData> vtkContourImage; ///< the tumor contour image
    
    std::vector<ImageType::IndexType> contourPixels; ///< the tumor contour pixels coordinates
    
    QVTKImageWidget *displayWidget; ///< the display widget
    
    bool surfaceImageFlag; ///< indicates if the probability image has been computed
    
    bool thresholdImageFlag; ///< indicates if the probability image has been computed
    
    bool seedPointFlag; ///< indicates if the seed piont has been selected
    
    std::vector<ImageType::IndexType> seedPoints; ///< the region growing seed point
    
    int lowerThreshVal;
    
    int upperThreshVal;
    
    int numberOfSeeds;

private slots:

    void surfaceEnhancement();
    void threshold(bool);
    void changeLowerThreshold(int);
    void changeUpperThreshold(int);
    
    void regionGrowing();
    
    /**
     * \brief choose a new seed point
     */
    void deleteSeed();
    
    /**
    * \brief get the cross point coordinates
    */
    void getCoordinates();
};

#endif // SKINIMAGESEGMENTATIONWIDGET_H
