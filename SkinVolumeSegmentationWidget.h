#ifndef SKINVOLUMESEGMENTATIONWIDGET_H
#define SKINVOLUMESEGMENTATIONWIDGET_H

#include "mainwindow.h"
#include "QVTKImageWidget.h"
#include "SkinVolumeSegmentation.h"
#include "VTKThreeViews.h"

#include <QWidget>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <itkImage.h>

namespace Ui {
    class SkinVolumeSegmentationWidget;
}

class SkinVolumeSegmentationWidget : public QWidget
{
    Q_OBJECT

public:
    
    typedef itk::Image<unsigned char, 3> VolumeType;
    
    explicit SkinVolumeSegmentationWidget(QWidget *parent = 0);
    ~SkinVolumeSegmentationWidget();

    /**
     * \brief set the ultrasound volume
     * @param a vtk volume
     */
    void setVolume(vtkSmartPointer<vtkImageData> volume);

    /**
     * \brief Set the calling main window
     * @param mainWindow
     */
    void setMainWindow(MainWindow * mainWindow);

    void setVTKThreeViews(VTKThreeViews* threeViews);

private:
    Ui::SkinVolumeSegmentationWidget *ui;

    SkinVolumeSegmentation * skinVolumeSegmentation; ///< has all the functions for Volume segmentation

    MainWindow * mainWindow; ///< the calling main window
    
    VTKThreeViews* threeViews;

    vtkSmartPointer<vtkImageData> vtkSurfaceVolume; ///< the surface Volume

    vtkSmartPointer<vtkImageData> vtkThresholdVolume; ///< the surface Volume

    vtkSmartPointer<vtkImageData> vtkRegionGrowingVolume; ///< the region growing Volume

    vtkSmartPointer<vtkImageData> vtkContourVolume; ///< the tumor contour Volume

    std::vector<VolumeType::IndexType> contourPixels; ///< the tumor contour pixels coordinates

    QVTKImageWidget *displayWidget; ///< the display widget

    bool surfaceVolumeFlag; ///< indicates if the probability Volume has been computed

    bool thresholdVolumeFlag; ///< indicates if the probability Volume has been computed

    bool seedPointFlag; ///< indicates if the seed piont has been selected

    std::vector<VolumeType::IndexType> seedPoints; ///< the region growing seed point

    int lowerThreshVal;

    int upperThreshVal;

    int numberOfSeeds;

private slots:

    void surfaceEnhancement();
    void threshold(bool);
    void changeLowerThreshold(int);
    void changeUpperThreshold(int);

    void regionGrowing();

    void outerContour();

    /**
     * \brief choose a new seed point
     */
    void deleteSeed();

    /**
    * \brief get the cross point coordinates
    */
    void getCoordinates();
};

#endif // SKINVOLUMESEGMENTATIONWIDGET_H
