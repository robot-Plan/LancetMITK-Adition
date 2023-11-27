/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "DentalAccuracy.h"

// Qt
#include <QMessageBox>

// mitk image
#include <mitkImage.h>
#include <QFileDialog>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCardinalSpline.h>
#include <vtkCellArray.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkConnectivityFilter.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkImageIterator.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkRendererCollection.h>
#include <vtkSplineFilter.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "lancetTrackingDeviceSourceConfigurator.h"
#include "lancetVegaTrackingDevice.h"
#include "leastsquaresfit.h"
#include "mitkGizmo.h"
#include "mitkImageToSurfaceFilter.h"
#include "mitkMatrixConvert.h"
#include "mitkNavigationToolStorageDeserializer.h"
#include "mitkPointSet.h"
#include "QmitkDataStorageTreeModel.h"
#include "QmitkRenderWindow.h"
#include "surfaceregistraion.h"

const std::string DentalAccuracy::VIEW_ID = "org.mitk.views.dentalaccuracy";

void DentalAccuracy::SetFocus()
{
  m_Controls.pushButton_planeAdjust->setFocus();
}

void DentalAccuracy::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  connect(m_Controls.pushButton_planeAdjust, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_planeAdjust_clicked);
  connect(m_Controls.pushButton_splineAndPanorama, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_splineAndPanorama_clicked);
  connect(m_Controls.pushButton_viewPano, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_viewPano_clicked);
  connect(m_Controls.pushButton_viewCursiveMPR, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_viewCursiveMPR_clicked);

  m_Controls.horizontalSlider->setMinimum(0);
  m_Controls.horizontalSlider->setMaximum(200);
  connect(m_Controls.horizontalSlider, &QSlider::valueChanged, this, &DentalAccuracy::valueChanged_horizontalSlider);

  connect(m_Controls.pushButton_CBCTreconstruct, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_CBCTreconstruct_clicked);
  connect(m_Controls.pushButton_iosCBCT, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_iosCBCT_clicked);
  connect(m_Controls.pushButton_setCrown, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_setCrown_clicked);
  connect(m_Controls.pushButton_setImplant, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_setImplant_clicked);
  connect(m_Controls.pushButton_steelballExtract, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_steelballExtract_clicked);
  connect(m_Controls.pushButton_connectVega, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_connectVega_clicked);
  connect(m_Controls.pushButton_imageRegis, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_imageRegis_clicked);
  connect(m_Controls.pushButton_calibrateDrill, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_calibrateDrill_clicked);
  connect(m_Controls.pushButton_genSplineAndAppend, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_genSplineAndAppend_clicked);
  connect(m_Controls.pushButton_GenSeeds, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_GenSeeds_clicked);
  connect(m_Controls.pushButton_collectDitch, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_collectDitch_clicked);
  connect(m_Controls.pushButton_imageRegisNew, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_imageRegisNew_clicked);
  connect(m_Controls.pushButton_resetImageRegis, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_resetImageRegis_clicked);
  connect(m_Controls.pushButton_implantFocus, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_implantFocus_clicked);

  connect(m_Controls.pushButton_U_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_U_ax_clicked);
  connect(m_Controls.pushButton_D_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_D_ax_clicked);
  connect(m_Controls.pushButton_R_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_R_ax_clicked);
  connect(m_Controls.pushButton_L_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_L_ax_clicked);
  connect(m_Controls.pushButton_D_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_D_cor_clicked);
  connect(m_Controls.pushButton_D_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_D_sag_clicked);
  connect(m_Controls.pushButton_U_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_U_cor_clicked);
  connect(m_Controls.pushButton_U_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_U_sag_clicked);
  connect(m_Controls.pushButton_L_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_L_cor_clicked);
  connect(m_Controls.pushButton_R_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_R_cor_clicked);
  connect(m_Controls.pushButton_L_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_L_sag_clicked);
  connect(m_Controls.pushButton_R_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_R_sag_clicked);
  connect(m_Controls.pushButton_clock_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_clock_cor_clicked);
  connect(m_Controls.pushButton_counter_cor, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_counter_cor_clicked);
  connect(m_Controls.pushButton_clock_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_clock_sag_clicked);
  connect(m_Controls.pushButton_counter_sag, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_counter_sag_clicked);
  connect(m_Controls.pushButton_clock_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_clock_ax_clicked);
  connect(m_Controls.pushButton_counter_ax, &QPushButton::clicked, this, &DentalAccuracy::on_pushButton_counter_ax_clicked);



}






void DentalAccuracy::on_pushButton_resetImageRegis_clicked()
{
	m_Controls.label_15->setText("0");
	m_probeDitchPset_rf = mitk::PointSet::New();
	m_ImageRegistrationMatrix->Identity();
}


void DentalAccuracy::on_pushButton_collectDitch_clicked()
{
	if (GetDataStorage()->GetNamedNode("probePoints_cmm") == nullptr)
	{
		m_Controls.textBrowser->append("probePoints_cmm is missing.");
		return;
	}

	m_probeDitchPset_cmm = GetDataStorage()->GetNamedObject<mitk::PointSet>("probePoints_cmm");

	if(m_probeDitchPset_rf == nullptr)
	{
		m_probeDitchPset_rf = mitk::PointSet::New();
	}

	if(m_probeDitchPset_rf->GetSize() == m_probeDitchPset_cmm->GetSize())
	{
		m_Controls.textBrowser->append("Enough points have been captured");
		return;
	}

	// Calculate T_drillDRFtoCalibratorDRF
	auto patientRFindex = m_VegaToolStorage->GetToolIndexByName("patientRF");
	auto drillDRFindex = m_VegaToolStorage->GetToolIndexByName("drillRF");
	if (patientRFindex == -1 || drillDRFindex == -1)
	{
		m_Controls.textBrowser->append("There is no 'patientRF' or 'drillDRF' in the toolStorage!");
		return;
	}

	mitk::NavigationData::Pointer nd_ndiToPatientRF = m_VegaSource->GetOutput(patientRFindex);
	mitk::NavigationData::Pointer nd_ndiTodrillDRF = m_VegaSource->GetOutput(drillDRFindex);

	if (nd_ndiToPatientRF->IsDataValid() == 0 || nd_ndiTodrillDRF->IsDataValid() == 0)
	{
		m_Controls.textBrowser->append("'patientRF' or 'drillDRF' is missing");
		return;
	}

	mitk::NavigationData::Pointer nd_patientRFtoDrillRF = GetNavigationDataInRef(nd_ndiTodrillDRF, nd_ndiToPatientRF);

	vtkMatrix4x4* T_patientRFtoDrillDRF = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_patientRFtoDrillRF->GetAffineTransform3D().GetPointer(), T_patientRFtoDrillDRF);

	auto drillRFtoTipMatrix = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(m_VegaToolStorage->GetToolByName("drillRF")->GetToolRegistrationMatrix().GetPointer(), drillRFtoTipMatrix);

	if (drillRFtoTipMatrix->IsIdentity())
	{
		m_Controls.textBrowser->append("The drill has not been calibrated yet.");
		return;
	}

	auto tmpTrans = vtkTransform::New();
	tmpTrans->Identity();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(drillRFtoTipMatrix);
	tmpTrans->Concatenate(T_patientRFtoDrillDRF);
	tmpTrans->Update();

	auto patientRFtoDrillTipMatrix = tmpTrans->GetMatrix();

	mitk::Point3D tipPointUnderPatientRF;
	tipPointUnderPatientRF[0] = patientRFtoDrillTipMatrix->GetElement(0, 3);
	tipPointUnderPatientRF[1] = patientRFtoDrillTipMatrix->GetElement(1, 3);
	tipPointUnderPatientRF[2] = patientRFtoDrillTipMatrix->GetElement(2, 3);

	m_probeDitchPset_rf->InsertPoint(tipPointUnderPatientRF);

	m_Controls.textBrowser->append("Captured point " + QString::number(m_probeDitchPset_rf->GetSize()));

	m_Controls.label_15->setText(QString::number(m_probeDitchPset_rf->GetSize()));

}

void DentalAccuracy::on_pushButton_imageRegisNew_clicked()
{
	auto cmm_Ball_node = GetDataStorage()->GetNamedNode("steelball_cmm"); // steelballs from CMM

	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image"); // steelballs in image

	auto cmm_ProbeDitch_node = GetDataStorage()->GetNamedNode("probePoints_cmm"); // probe ditch points from CMM

	if(cmm_Ball_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_cmm is missing");
		return;
	}

	if(extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("'steelball_image' is missing");
		return;
	}

	if(cmm_ProbeDitch_node == nullptr)
	{
		m_Controls.textBrowser->append("probePoints_cmm is missing");
		return;
	}

	// Step 1: Calculate the transform from steelballs_cmm to steelballs_image
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_cmm")->GetData()));
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("steelball_image")->GetData()));

	landmarkRegistrator->ComputeLandMarkResult();

	auto tmpMatrix = landmarkRegistrator->GetResult();

	// Step 2: Apply tmpMatrix to 'probePoints_cmm' to get 'probePoints_image'
	auto probePoints_cmm = GetDataStorage()->GetNamedObject<mitk::PointSet>("probePoints_cmm");
	probePoints_cmm->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	auto probePoints_image = mitk::PointSet::New();

	for(int i{0}; i < probePoints_cmm->GetSize(); i++)
	{
		probePoints_image->InsertPoint(probePoints_cmm->GetPoint(i));
	}

	
	// Step 3: Check if enough probe ditch points have been collected
	if (m_probeDitchPset_rf == nullptr)
	{
		m_Controls.textBrowser->append("No probe ditch point has been captured");
		return;
	}

	if (m_probeDitchPset_rf->GetSize() < probePoints_cmm->GetSize())
	{
		m_Controls.textBrowser->append("More probe ditch point should be captured");
		return;
	}

	// Step 4: Calculate surface to objectRf matrix and reconnect the IGT pipeline

	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");

	if (surfaceNode == nullptr)
	{
		m_Controls.textBrowser->append("Reconstructed CBCT surface is missing!");
		return;
	}

	m_NavigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if (matrix->IsIdentity() == false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	}

	m_NavigatedImage->SetDataNode(surfaceNode);

	m_NavigatedImage->SetLandmarks(probePoints_image);

	m_NavigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- NavigatedImage has been set up ---");

	m_NavigatedImage->SetLandmarks_probe(m_probeDitchPset_rf);

	/// Apply image registration
	m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);

	m_NavigatedImage->UpdateObjectToRfMatrix();
	m_Controls.textBrowser->append("Avg landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_avgError()));
	m_Controls.textBrowser->append("Max landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_maxError()));

	m_ImageRegistrationMatrix->DeepCopy(m_NavigatedImage->GetT_Object2ReferenceFrame());

	auto tmpMatrix_1 = mitk::AffineTransform3D::New();

	mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix, tmpMatrix_1.GetPointer());

	m_VegaToolStorage->GetToolByName("patientRF")->SetToolRegistrationMatrix(tmpMatrix_1);

	m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("patientRF")->GetToolRegistrationMatrix());

	m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("patientRF"));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();


}


void DentalAccuracy::on_pushButton_GenSeeds_clicked()
{
	if(GetDataStorage()->GetNamedNode("Initial seeds") == nullptr)
	{
		m_Controls.textBrowser->append("Initial seeds are missing");
		return;
	}

	auto initialSeeds = GetDataStorage()->GetNamedObject<mitk::PointSet>("Initial seeds");

	int halfROIsliceNum{ 36 };
	double z_spacing{ 1 };

	for(int i{0}; i < 2 * halfROIsliceNum; i++)
	{
		
		auto tmpPset = mitk::PointSet::New();

		for(int j{0}; j < initialSeeds->GetSize(); j++)
		{
			mitk::Point3D tmpPoint;
			tmpPoint[0] = initialSeeds->GetPoint(j)[0];
			tmpPoint[1] = initialSeeds->GetPoint(j)[1];
			tmpPoint[2] = initialSeeds->GetPoint(j)[2] + i*z_spacing - halfROIsliceNum * z_spacing;

			tmpPset->InsertPoint(tmpPoint);
		}

		auto tmpNode = mitk::DataNode::New();
		tmpNode->SetData(tmpPset);
		auto nodeName = QString("pset_") + QString::number(i);
		tmpNode->SetName(nodeName.toStdString());
		GetDataStorage()->Add(tmpNode);
	}

}


void DentalAccuracy::on_pushButton_genSplineAndAppend_clicked()
{
	auto appender = vtkAppendPolyData::New();
	int linePointNum{0};
	int lineNum = 72;

	for(int i{0}; i < lineNum; i++)
	{
		auto index = QString::number(i);
		auto nodeName = QString("pset_") + index;

		// GetDataStorage()->GetNamedNode(nodeName.toStdString());

		auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>(nodeName.toStdString());

		vtkNew<vtkPoints> points;
		for (int i{ 0 }; i < mitkPset->GetSize(); i++)
		{
			auto mitkPoint = mitkPset->GetPoint(i);
			points->InsertNextPoint(mitkPoint[0], mitkPoint[1], mitkPoint[2]);
		}

		// vtkCellArrays
		vtkNew<vtkCellArray> lines;
		lines->InsertNextCell(mitkPset->GetSize());
		for (unsigned int i = 0; i < mitkPset->GetSize(); ++i)
		{
			lines->InsertCellPoint(i);
		}

		// vtkPolyData
		auto polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->SetPoints(points);
		polyData->SetLines(lines);

		auto spline = vtkCardinalSpline::New();
		spline->SetLeftConstraint(2);
		spline->SetLeftValue(0.0);
		spline->SetRightConstraint(2);
		spline->SetRightValue(0.0);

		// double segLength{ 0.3 };
		int subDivideNum{ 200 };

		vtkNew<vtkSplineFilter> splineFilter;
		splineFilter->SetInputData(polyData);
		// splineFilter->SetLength(segLength);
		// splineFilter->SetSubdivideToLength();
		splineFilter->SetSubdivideToSpecified();
		splineFilter->SetNumberOfSubdivisions(subDivideNum);
		splineFilter->SetSpline(spline);
		splineFilter->Update();

		auto spline_PolyData = splineFilter->GetOutput();

		linePointNum = spline_PolyData->GetPoints()->GetNumberOfPoints();

		appender->AddInputData(spline_PolyData);

	}

	appender->Update();

	auto appendedSplines = appender->GetOutput();

	auto tmpSurface = mitk::Surface::New();
	tmpSurface->SetVtkPolyData(appendedSplines);
	auto curveNode = mitk::DataNode::New();
	curveNode->SetData(tmpSurface);
	curveNode->SetName("Dental curves");
	GetDataStorage()->Add(curveNode);

	// Generate surface with the splines
	int rows = linePointNum;
	int cols = lineNum;

	vtkNew<vtkPolyData> surface;

	// Generate the points.
	int numberOfPoints = rows * cols;
	int numberOfPolys = (rows - 1) * (cols - 1);
	vtkNew<vtkPoints> points;
	m_Controls.textBrowser->append("linePointNum: " + QString::number(linePointNum));
	m_Controls.textBrowser->append("Num of pts: " + QString::number(numberOfPoints));
	m_Controls.textBrowser->append("Num of polys: " + QString::number(numberOfPolys * 3));
	points->Allocate(numberOfPoints);
	vtkNew<vtkCellArray> polys;
	polys->Allocate(numberOfPolys * 4);

	points->DeepCopy(appendedSplines->GetPoints());

	vtkIdType pts[4];
	for (unsigned int row = 0; row < rows - 1; row++)
	{
		for (unsigned int col = 0; col < cols - 1; col++)
		{
			pts[0] = row + rows * (col);
			pts[1] = pts[0] + 1;
			pts[2] = pts[0] + rows + 1;
			pts[3] = pts[0] + rows;
			polys->InsertNextCell(4, pts);
		}
	}
	surface->SetPoints(points);
	surface->SetPolys(polys);

	vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
	smoothFilter->SetInputData(surface);
	smoothFilter->SetNumberOfIterations(20);
	smoothFilter->SetRelaxationFactor(0.2);
	smoothFilter->FeatureEdgeSmoothingOff();
	smoothFilter->BoundarySmoothingOn();
	smoothFilter->Update();


	auto tmpSurface1 = mitk::Surface::New();
	tmpSurface1->SetVtkPolyData(smoothFilter->GetOutput());
	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetData(tmpSurface1);
	tmpNode->SetName("Smoothed Probe surface");
	GetDataStorage()->Add(tmpNode);

	auto tmpSurface2 = mitk::Surface::New();
	tmpSurface2->SetVtkPolyData(surface);
	auto tmpNode1 = mitk::DataNode::New();
	tmpNode1->SetData(tmpSurface2);
	tmpNode1->SetName("Probe surface");
	GetDataStorage()->Add(tmpNode1);


	// Probe the image
	// Apply the inverse geometry of the MITK image to the probe surface
	auto geometryMatrix = vtkMatrix4x4::New();
	if(GetDataStorage()->GetNamedNode("CBCT") == nullptr)
	{
		m_Controls.textBrowser->append("CBCT is missing");
		return;
	}
	geometryMatrix->DeepCopy(GetDataStorage()->GetNamedNode("CBCT")->GetData()->GetGeometry()->GetVtkMatrix());
	auto spacing = GetDataStorage()->GetNamedNode("CBCT")->GetData()->GetGeometry()->GetSpacing();

	for (int j{ 0 }; j < 3; j++)
	{
		geometryMatrix->SetElement(j, 0, geometryMatrix->GetElement(j, 0) / spacing[0]);
		geometryMatrix->SetElement(j, 1, geometryMatrix->GetElement(j, 1) / spacing[1]);
		geometryMatrix->SetElement(j, 2, geometryMatrix->GetElement(j, 2) / spacing[2]);
	}

	geometryMatrix->Invert();

	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(geometryMatrix);
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(smoothFilter->GetOutput());
	tmpTransFilter->Update();

	auto vtkImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT")->GetVtkImageData();

	vtkNew<vtkProbeFilter> sampleVolume;
	sampleVolume->SetSourceData(vtkImage);
	sampleVolume->SetInputData(tmpTransFilter->GetPolyDataOutput());

	sampleVolume->Update();

	auto probeData = sampleVolume->GetOutput();

	auto probePointData = probeData->GetPointData();

	auto tmpArray = probePointData->GetScalars();

	auto testimageData = vtkImageData::New();
	testimageData->SetDimensions(rows, cols, 1);
	//testimageData->SetDimensions( spline_PolyData->GetNumberOfPoints(), cols + 1, 1);


	testimageData->SetSpacing(1, 1, 1);
	testimageData->SetOrigin(0, 0, 0);
	testimageData->AllocateScalars(VTK_INT, 1);
	testimageData->GetPointData()->SetScalars(tmpArray);

	auto mitkAppendedImage = mitk::Image::New();

	mitkAppendedImage->Initialize(testimageData);
	mitkAppendedImage->SetVolume(testimageData->GetScalarPointer());

	auto testNode = mitk::DataNode::New();
	testNode->SetData(mitkAppendedImage);
	testNode->SetName("Test Panorama");
	GetDataStorage()->Add(testNode);

}



void DentalAccuracy::on_pushButton_calibrateDrill_clicked()
{
	// Calculate T_drillDRFtoCalibratorDRF
	auto calibratorDRFindex = m_VegaToolStorage->GetToolIndexByName("calibrator");
	auto drillDRFindex = m_VegaToolStorage->GetToolIndexByName("drillRF");
	if (calibratorDRFindex == -1 || drillDRFindex == -1)
	{
		m_Controls.textBrowser->append("There is no 'calibrator' or 'drillDRF' in the toolStorage!");
	}

	mitk::NavigationData::Pointer nd_ndiTocalibratorDRF = m_VegaSource->GetOutput(calibratorDRFindex);
	mitk::NavigationData::Pointer nd_ndiTodrillDRF = m_VegaSource->GetOutput(drillDRFindex);

	if(nd_ndiTocalibratorDRF->IsDataValid() == 0 || nd_ndiTodrillDRF->IsDataValid() == 0)
	{
		m_Controls.textBrowser->append("calibrator or drillRF is missing");
		return;
	}

	mitk::NavigationData::Pointer nd_drillDRFtoCalibratorDRF = GetNavigationDataInRef(nd_ndiTocalibratorDRF, nd_ndiTodrillDRF);

	vtkMatrix4x4* T_drillDRFtoCalibratorDRF = vtkMatrix4x4::New();
	mitk::TransferItkTransformToVtkMatrix(nd_drillDRFtoCalibratorDRF->GetAffineTransform3D().GetPointer(), T_drillDRFtoCalibratorDRF);

	m_Controls.textBrowser->append("Drill calibration matrix");
	for (int i{ 0 }; i < 4; i++)
	{
		for (int j{ 0 }; j < 4; j++)
		{
			m_Controls.textBrowser->append(QString::number(T_drillDRFtoCalibratorDRF->GetElement(i, j)));
		}
	}

	m_VegaToolStorage->GetToolByName("drillRF")->SetToolRegistrationMatrix(nd_drillDRFtoCalibratorDRF->GetAffineTransform3D());


}

mitk::NavigationData::Pointer DentalAccuracy::GetNavigationDataInRef(mitk::NavigationData::Pointer nd,
	mitk::NavigationData::Pointer nd_ref)
{
	mitk::NavigationData::Pointer res = mitk::NavigationData::New();
	res->Graft(nd);
	res->Compose(nd_ref->GetInverse());
	return res;
}

void DentalAccuracy::on_pushButton_imageRegis_clicked()
{
	if(m_ImageRegistrationMatrix->IsIdentity() == false)
	{
		m_Controls.textBrowser->append("Image registration has been done.");
		return;
	}

	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image");
	
	auto stdBall_node = GetDataStorage()->GetNamedNode("steelball_rf");

	if(extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_image is missing");
		return;
	}

	if(stdBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_rf is missing");
		return;
	}

	auto extractedBall_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_image");
	auto stdball_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_rf");
	int extracted_num = extractedBall_pset->GetSize();

	if(extracted_num < stdball_pset->GetSize())
	{
		m_Controls.textBrowser->append("steelball_image extraction incomplete");
		return;
	}

	
	// The surface node should have no offset, i.e., should have an identity matrix!
	auto surfaceNode = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");

	if (surfaceNode == nullptr )
	{
		m_Controls.textBrowser->append("Reconstructed CBCT surface is missing!");
		return;
	}

	m_NavigatedImage = lancet::NavigationObject::New();

	auto matrix = dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->GetVtkMatrix();

	if (matrix->IsIdentity() == false)
	{
		vtkNew<vtkMatrix4x4> identityMatrix;
		identityMatrix->Identity();
		dynamic_cast<mitk::Surface*>(surfaceNode->GetData())->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(identityMatrix);

		m_Controls.textBrowser->append("Warning: the initial surface has a non-identity offset matrix; the matrix has been reset to identity!");
	}

	m_NavigatedImage->SetDataNode(surfaceNode);

	m_NavigatedImage->SetLandmarks(extractedBall_pset);

	m_NavigatedImage->SetReferencFrameName(surfaceNode->GetName());

	m_Controls.textBrowser->append("--- NavigatedImage has been set up ---");

	m_NavigatedImage->SetLandmarks_probe(stdball_pset);

	/// Apply image registration
	m_SurfaceRegistrationStaticImageFilter = lancet::ApplySurfaceRegistratioinStaticImageFilter::New();
	m_SurfaceRegistrationStaticImageFilter->ConnectTo(m_VegaSource);

	m_NavigatedImage->UpdateObjectToRfMatrix();
	m_Controls.textBrowser->append("Avg landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_avgError()));
	m_Controls.textBrowser->append("Max landmark error:" + QString::number(m_NavigatedImage->GetlandmarkRegis_maxError()));

	m_ImageRegistrationMatrix->DeepCopy(m_NavigatedImage->GetT_Object2ReferenceFrame());

	auto tmpMatrix = mitk::AffineTransform3D::New();

	mitk::TransferVtkMatrixToItkTransform(m_ImageRegistrationMatrix, tmpMatrix.GetPointer());

	m_VegaToolStorage->GetToolByName("patientRF")->SetToolRegistrationMatrix(tmpMatrix);

	m_SurfaceRegistrationStaticImageFilter->SetRegistrationMatrix(m_VegaToolStorage->GetToolByName("patientRF")->GetToolRegistrationMatrix());

	m_SurfaceRegistrationStaticImageFilter->SetNavigationDataOfRF(m_VegaSource->GetOutput("patientRF"));

	m_VegaVisualizeTimer->stop();
	m_VegaVisualizer->ConnectTo(m_SurfaceRegistrationStaticImageFilter);
	m_VegaVisualizeTimer->start();

}


void DentalAccuracy::on_pushButton_connectVega_clicked()
{
	//read in filename
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open Tool Storage"), "/",
		tr("Tool Storage Files (*.IGTToolStorage)"));
	if (filename.isNull()) return;

	//read tool storage from disk
	std::string errorMessage = "";
	mitk::NavigationToolStorageDeserializer::Pointer myDeserializer = mitk::NavigationToolStorageDeserializer::New(
		GetDataStorage());
	m_VegaToolStorage = myDeserializer->Deserialize(filename.toStdString());
	m_VegaToolStorage->SetName(filename.toStdString());


	MITK_INFO << "Vega tracking";
	lancet::NDIVegaTrackingDevice::Pointer vegaTrackingDevice = lancet::NDIVegaTrackingDevice::New(); //instantiate

	//Create Navigation Data Source with the factory class, and the visualize filter.
	lancet::TrackingDeviceSourceConfiguratorLancet::Pointer vegaSourceFactory =
		lancet::TrackingDeviceSourceConfiguratorLancet::New(m_VegaToolStorage, vegaTrackingDevice);

	m_VegaSource = vegaSourceFactory->CreateTrackingDeviceSource(m_VegaVisualizer);
	m_VegaSource->SetToolMetaDataCollection(m_VegaToolStorage);
	m_VegaSource->Connect();

	m_VegaSource->StartTracking();

	//update visualize filter by timer
	if (m_VegaVisualizeTimer == nullptr)
	{
		m_VegaVisualizeTimer = new QTimer(this); //create a new timer
	}
	connect(m_VegaVisualizeTimer, &QTimer::timeout, this, &DentalAccuracy::OnVegaVisualizeTimer);

	ShowToolStatus_Vega();

	m_VegaVisualizeTimer->start(100); //Every 100ms the method OnTimer() is called. -> 10fps

	auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
	mitk::RenderingManager::GetInstance()->InitializeViews(geo);

	m_ImageRegistrationMatrix->Identity();

}

void DentalAccuracy::UpdateToolStatusWidget()
{
	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();
	// m_Controls.m_StatusWidgetKukaToolToShow->Refresh();
}


void DentalAccuracy::OnVegaVisualizeTimer()
{
	//Here we call the Update() method from the Visualization Filter. Internally the filter checks if
	//new NavigationData is available. If we have a new NavigationData the cone position and orientation
	//will be adapted.
	
	if (m_VegaVisualizer.IsNotNull())
	{
		m_VegaVisualizer->Update();
		// auto geo = this->GetDataStorage()->ComputeBoundingGeometry3D(this->GetDataStorage()->GetAll());
		// mitk::RenderingManager::GetInstance()->InitializeViews(geo);
		this->RequestRenderWindowUpdate();
	}

	m_Controls.m_StatusWidgetVegaToolToShow->Refresh();

}


void DentalAccuracy::ShowToolStatus_Vega()
{
	m_VegaNavigationData.clear();
	for (std::size_t i = 0; i < m_VegaSource->GetNumberOfOutputs(); i++)
	{
		m_VegaNavigationData.push_back(m_VegaSource->GetOutput(i));
	}
	//initialize widget
	m_Controls.m_StatusWidgetVegaToolToShow->RemoveStatusLabels();
	m_Controls.m_StatusWidgetVegaToolToShow->SetShowPositions(true);
	m_Controls.m_StatusWidgetVegaToolToShow->SetTextAlignment(Qt::AlignLeft);
	m_Controls.m_StatusWidgetVegaToolToShow->SetNavigationDatas(&m_VegaNavigationData);
	m_Controls.m_StatusWidgetVegaToolToShow->ShowStatusLabels();
}


void DentalAccuracy::on_pushButton_steelballExtract_clicked()
{
	// Initial preparation
	m_Controls.textBrowser->append("------- Started steelball searching -------");
	
	auto standartSteelballCenters = mitk::PointSet::New();
	int stdCenterNum{ 7 };

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	// if (m_Controls.checkBox_generateStandardBallCenters->isChecked())
	// {
	// 	auto stdSteelballNode = mitk::DataNode::New();
	// 	stdSteelballNode->SetName("std centers (full)");
	// 	stdSteelballNode->SetData(standartSteelballCenters);
	// 	GetDataStorage()->Add(stdSteelballNode);
	// }

	UpdateAllBallFingerPrint(standartSteelballCenters);

	// Get maximal and minimal voxel
	auto inputCtImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");

	auto inputVtkImage = inputCtImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToDouble();
	caster->Update();
	double tmpMaxVoxel{ 0 };
	double tmpMinVoxel{ 0 };

	vtkImageIterator<double> iter(caster->GetOutput(), tmpRegion);
	while (!iter.IsAtEnd())
	{
		double* inSI = iter.BeginSpan();
		double* inSIEnd = iter.EndSpan();

		while (inSI != inSIEnd)
		{
			if (double(*inSI) > tmpMaxVoxel)
			{
				tmpMaxVoxel = double(*inSI);
			}
			if (double(*inSI) < tmpMinVoxel)
			{
				tmpMinVoxel = double(*inSI);
			}
			++inSI;
		}
		iter.NextSpan();

	}

	int searchIterations{ 7 }; // optimal voxel value
	tmpMaxVoxel = 15000; // tmp fix  March 03, 2023
	tmpMinVoxel = 1500; // tmp fix  March 03, 2023


	double voxelThres{ tmpMaxVoxel };
	int foundCleanCenterNum{ 0 };
	int foundCenterNum{ 0 };
	int foundIDs[7]{ 0 };

	
	

	// Search for the best voxel value threshold
	for (int i{ 0 }; i < (searchIterations + 1); i++)
	{
		// double tmpVoxelThreshold = (1 -  double(i) / searchIterations) * (tmpMaxVoxel - tmpMinVoxel) + tmpMinVoxel;
		double tmpVoxelThreshold = tmpMaxVoxel - (tmpMaxVoxel - tmpMinVoxel) * i / searchIterations;

		GetCoarseSteelballCenters(tmpVoxelThreshold);

		foundCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
		m_Controls.textBrowser->append("Current HU value: " + QString::number(tmpVoxelThreshold));
		if (foundCenterNum >= 40)
		{
			break;
		}

		IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);

		if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() >= foundCleanCenterNum)
		{
			foundCleanCenterNum = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
			voxelThres = tmpVoxelThreshold;
			if (foundCleanCenterNum >= stdCenterNum)
			{
				IterativeScreenCoarseSteelballCenters(6, 6, foundIDs);
				if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == stdCenterNum)
				{
					//m_Controls.textBrowser->append("~~All steelballs have been found~~");
					break;
				}

			}
		}

	}


	GetCoarseSteelballCenters(voxelThres);
	//m_Controls.lineEdit_ballGrayValue->setText(QString::number(voxelThres));
	IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() > 7)
	{
		IterativeScreenCoarseSteelballCenters(6, 6, foundIDs);
	}

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == 0)
	{
		GetCoarseSteelballCenters(voxelThres);
		//m_Controls.lineEdit_ballGrayValue->setText(QString::number(voxelThres));
		IterativeScreenCoarseSteelballCenters(4, 6, foundIDs);
	}

	RearrangeSteelballs(6, foundIDs); // this function is redundant ??


	auto partialStdPointset = mitk::PointSet::New();
	for (int q{ 0 }; q < 7; q++) {
		if (foundIDs[q] == 1)
		{
			partialStdPointset->InsertPoint(standartSteelballCenters->GetPoint(q));
		}
	}

	// if (m_Controls.checkBox_generateStandardBallCenters->isChecked())
	// {
	// 	auto tmpNode = mitk::DataNode::New();
	// 	tmpNode->SetName("std centers (partial)");
	// 	tmpNode->SetData(partialStdPointset);
	// 	GetDataStorage()->Add(tmpNode);
	// }



	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	landmarkRegistrator->SetLandmarksSrc(partialStdPointset);
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData()));

	landmarkRegistrator->ComputeLandMarkResult();
	double maxError = landmarkRegistrator->GetmaxLandmarkError();
	double avgError = landmarkRegistrator->GetavgLandmarkError();

	m_Controls.textBrowser->append("Maximum steelball error: " + QString::number(maxError));
	m_Controls.textBrowser->append("Average steelball error: " + QString::number(avgError));

	if (dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize() == 7)
	{
		m_Controls.textBrowser->append("~~All steelballs have been found!~~");
	}
	else
	{
		m_Controls.textBrowser->append("!!!Warning: Only found " + QString::number(dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize())
			+ " steelballs!!!!");
		m_Controls.textBrowser->append("Please compare 'Steelball centers', 'std centers (partial)' and 'std centers (full)' carefully!");
	}

	if (avgError > 1)
	{
		m_Controls.textBrowser->append("!!!Warning: The found centers are highly problematic!!!");
	}

	m_Controls.textBrowser->append("------- End of steelball searching -------");

	// auto tmpMatrix = landmarkRegistrator->GetResult();
	//
	// GetDataStorage()->GetNamedNode("std_modelWithPlan")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	//auto parentNode = m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode();
	auto tmpPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	auto childNode = mitk::DataNode::New();
	childNode->SetName("Steelball centers2");
	childNode->SetData(tmpPointSet);
	GetDataStorage()->Add(childNode);
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	childNode->SetName("steelball_image");


	////////////////////// Move the dental splint surface ///////////////////////
	auto extractedBall_node = GetDataStorage()->GetNamedNode("steelball_image");
	auto splint_node = GetDataStorage()->GetNamedNode("dentalSplint");
	auto stdBall_node = GetDataStorage()->GetNamedNode("steelball_rf");

	if (extractedBall_node == nullptr)
	{
		m_Controls.textBrowser->append("steelball_image is missing");
		return;
	}

	if (splint_node == nullptr || stdBall_node == nullptr)
	{
		m_Controls.textBrowser->append("dentalSplint or std_steelball is missing");
		return;
	}

	auto extractedBall_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_image");
	auto stdball_pset = GetDataStorage()->GetNamedObject<mitk::PointSet>("steelball_rf");
	int extracted_num = extractedBall_pset->GetSize();

	if (extracted_num < stdball_pset->GetSize())
	{
		m_Controls.textBrowser->append("steelball_image extraction incomplete");
		return;
	}

	auto landmarkRegistrator2 = mitk::SurfaceRegistration::New();
	landmarkRegistrator2->SetLandmarksSrc(stdball_pset);
	landmarkRegistrator2->SetLandmarksTarget(extractedBall_pset);
	
	landmarkRegistrator2->ComputeLandMarkResult();
	
	auto result_matrix = landmarkRegistrator2->GetResult();

	auto splint_surface = GetDataStorage()->GetNamedObject<mitk::Surface>("dentalSplint");

	splint_surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(result_matrix);

	GetDataStorage()->GetNamedNode("dentalSplint")->SetVisibility(true);
}


void DentalAccuracy::on_pushButton_setImplant_clicked()
{
	auto crownNode = GetDataStorage()->GetNamedNode("crown");

	if (crownNode == nullptr)
	{
		m_Controls.textBrowser->append("crown is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	if (implantNode == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped") == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	
	TurnOffAllNodesVisibility();
	GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->SetVisibility(true);
	if (GetDataStorage()->GetNamedNode("merged ios") != nullptr)
	{
		GetDataStorage()->GetNamedNode("merged ios")->SetVisibility(true);
	}
	implantNode->SetVisibility(true);
	crownNode->SetVisibility(true);

	if (mitk::Gizmo::HasGizmoAttached(implantNode, GetDataStorage()) == 1)
	{
		mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
		m_Controls.pushButton_setImplant->setText("Set implant");
	}
	else
	{
		
		mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());
		m_Controls.pushButton_setImplant->setText("Finish");
	}

	// ResetView();

	on_pushButton_implantFocus_clicked();

	// Display the alveolar nerve segmentation
	if (GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped-labels_3D-interpolation") != nullptr)
	{
		GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped-labels_3D-interpolation")->SetVisibility(true);
	}
}


void DentalAccuracy::on_pushButton_setCrown_clicked()
{
	auto crownNode = GetDataStorage()->GetNamedNode("crown");

	if (crownNode == nullptr)
	{
		m_Controls.textBrowser->append("crown is missing");
		return;
	}
	
	if(GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped") == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	TurnOffAllNodesVisibility();
	GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("crown")->SetVisibility(true);

	if (GetDataStorage()->GetNamedNode("merged ios") != nullptr)
	{
		GetDataStorage()->GetNamedNode("merged ios")->SetVisibility(true);
	}

	ResetView();

	if (mitk::Gizmo::HasGizmoAttached(crownNode, GetDataStorage()) == 1)
	{
		mitk::Gizmo::RemoveGizmoFromNode(crownNode, GetDataStorage());
		m_Controls.pushButton_setCrown->setText("Set crown");
	}
	else
	{
		mitk::Gizmo::AddGizmoToNode(crownNode, GetDataStorage());
		m_Controls.pushButton_setCrown->setText("Finish");
	}

	if (GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped-labels_3D-interpolation") != nullptr)
	{
		GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped-labels_3D-interpolation")->SetVisibility(true);
	}

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->DeepCopy(crownNode->GetData()->GetGeometry()->GetVtkMatrix());
	Eigen::Vector3d x;
	Eigen::Vector3d y;
	Eigen::Vector3d z;

	for (int i{ 0 }; i < 3; i++)
	{
		x[i] = tmpMatrix->GetElement(i, 0);
		y[i] = tmpMatrix->GetElement(i, 1);
		z[i] = tmpMatrix->GetElement(i, 2);
	}
	x.normalize();
	y.normalize();
	z.normalize();
	for (int i{ 0 }; i < 3; i++)
	{
		tmpMatrix->SetElement(i, 0, x[i]);
		tmpMatrix->SetElement(i, 1, y[i]);
		tmpMatrix->SetElement(i, 2, z[i]);
	}
	if (GetDataStorage()->GetNamedNode("implant") != nullptr)
	{
		auto implantNode = GetDataStorage()->GetNamedNode("implant");
		implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);
	}

}


void DentalAccuracy::on_pushButton_iosCBCT_clicked()
{
	auto node_landmark_src = GetDataStorage()->GetNamedNode("landmark_src");
	auto node_landmark_target = GetDataStorage()->GetNamedNode("landmark_dst");
	auto node_ios = GetDataStorage()->GetNamedNode("ios");
	auto node_icp_target = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");

	if (node_landmark_src == nullptr)
	{
		m_Controls.textBrowser->append("landmark_src is missing");
		return;
	}
	if (dynamic_cast<mitk::PointSet*>(node_landmark_src->GetData())->IsEmpty())
	{
		m_Controls.textBrowser->append("landmark_src is empty");
		return;
	}
	if (node_landmark_target == nullptr)
	{
		m_Controls.textBrowser->append("landmark_dst is missing");
		return;
	}
	if (dynamic_cast<mitk::PointSet*>(node_landmark_target->GetData())->IsEmpty())
	{
		m_Controls.textBrowser->append("landmark_dst is empty");
		return;
	}
	if (node_ios == nullptr)
	{
		m_Controls.textBrowser->append("ios is missing");
		return;
	}
	if (node_icp_target == nullptr)
	{

		m_Controls.textBrowser->append("Reconstructed CBCT surface is missing");
		return;

	}

	ClipTeeth();

	auto node_icp_src = GetDataStorage()->GetNamedNode("Clipped data");

	// Landmark registration
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	landmarkRegistrator->SetLandmarksSrc(dynamic_cast<mitk::PointSet*>(node_landmark_src->GetData()));
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(node_landmark_target->GetData()));
	landmarkRegistrator->ComputeLandMarkResult();


	auto landmarkResult = landmarkRegistrator->GetResult();
	Eigen::Matrix4d a{ landmarkResult->GetData() };
	MITK_INFO << a;
	auto landmarkTransform = vtkTransform::New();
	landmarkTransform->Identity();
	landmarkTransform->PostMultiply();
	landmarkTransform->SetMatrix(landmarkResult);
	landmarkTransform->Update();

	// ICP registration
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	// Prepare icp source: Clipped data
	vtkNew<vtkTransformFilter> sourceFilter;
	auto targetSurface = dynamic_cast<mitk::Surface*>(node_icp_src->GetData());
	sourceFilter->SetInputData(targetSurface->GetVtkPolyData());
	sourceFilter->SetTransform(landmarkTransform);
	sourceFilter->Update();

	auto icpSource = mitk::Surface::New();
	icpSource->SetVtkPolyData(sourceFilter->GetPolyDataOutput());

	icpRegistrator->SetSurfaceSrc(icpSource);
	icpRegistrator->SetSurfaceTarget(dynamic_cast<mitk::Surface*>(node_icp_target->GetData()));
	icpRegistrator->ComputeSurfaceIcpResult();
	auto icpResult = icpRegistrator->GetResult();

	// Apply landmark & icp results to ios
	auto combinedTransform = vtkTransform::New();
	combinedTransform->Identity();
	combinedTransform->PostMultiply();
	
	combinedTransform->Concatenate(node_ios->GetData()->GetGeometry()->GetVtkMatrix());
	combinedTransform->Concatenate(landmarkResult);
	combinedTransform->Concatenate(icpResult);
	combinedTransform->Update();

	// node_ios->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(combinedTransform->GetMatrix());

	if(GetDataStorage()->GetNamedNode("merged ios") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("merged ios"));
	}

	auto mergedIosPoly = vtkPolyData::New();
	mergedIosPoly->DeepCopy(GetDataStorage()->GetNamedObject<mitk::Surface>("ios")->GetVtkPolyData());

	auto transFilter = vtkTransformFilter::New();
	transFilter->SetTransform(combinedTransform);
	transFilter->SetInputData(mergedIosPoly);
	transFilter->Update();

	auto mergedSurface = mitk::Surface::New();
	mergedSurface->SetVtkPolyData(transFilter->GetPolyDataOutput());
	auto mergedNode = mitk::DataNode::New();
	mergedNode->SetData(mergedSurface);
	mergedNode->SetName("merged ios");
	mergedNode->SetColor(1,1,1);
	GetDataStorage()->Add(mergedNode);

	TurnOffAllNodesVisibility();
	mergedNode->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Reconstructed CBCT surface")->SetVisibility(true);
	ResetView();

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Clipped data"));

	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	m_Controls.textBrowser->append("------ Registration succeeded ------");
}


void DentalAccuracy::on_pushButton_CBCTreconstruct_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped");

	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	if(GetDataStorage()->GetNamedNode("ios") == nullptr)
	{
		m_Controls.textBrowser->append("Intraoral scan (ios) is missing");
	
	}


	auto inputCtImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");

	// The isosurface of the CT data into a single polydata
	double threshold = 1500;
	auto mitkRecontructedSurfaces = mitk::Surface::New();
	mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

	// Set smoothing parameters
	// if (m_Controls.checkBox_useSmooth->isChecked())
	// {
	// 	int iteration = m_Controls.lineEdit_smoothIteration->text().toInt();
	// 	double relaxtion = m_Controls.lineEdit_smoothRelaxtion->text().toDouble();
	// 	imageToSurfaceFilter->SmoothOn();
	// 	imageToSurfaceFilter->SetSmoothRelaxation(relaxtion);
	// 	imageToSurfaceFilter->SetSmoothIteration(iteration);
	// }

	imageToSurfaceFilter->SetInput(inputCtImage);
	imageToSurfaceFilter->SetThreshold(threshold);
	mitkRecontructedSurfaces = imageToSurfaceFilter->GetOutput();

	// draw extracted surface
	auto cbctSurfaceNode = mitk::DataNode::New();
	cbctSurfaceNode->SetName("Reconstructed CBCT surface");
	cbctSurfaceNode->SetColor(1, 0.5, 0);
	// add new node
	cbctSurfaceNode->SetData(mitkRecontructedSurfaces);

	if(GetDataStorage()->GetNamedNode("Reconstructed CBCT surface") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Reconstructed CBCT surface"));
	}

	GetDataStorage()->Add(cbctSurfaceNode);

	TurnOffAllNodesVisibility();
	GetDataStorage()->GetNamedNode("Reconstructed CBCT surface")->SetVisibility(true);

	if (GetDataStorage()->GetNamedNode("ios") != nullptr)
	{
		GetDataStorage()->GetNamedNode("ios")->SetVisibility(true);
	}
	
	ResetView();
}


void DentalAccuracy::on_pushButton_U_ax_clicked()
{
	if(GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d x_projection = x_std - z_mpr * (x_std.dot(z_mpr));
	x_projection.normalize();

	Eigen::Vector3d y_projection = z_mpr.cross(x_projection);
	y_projection.normalize();


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-y_mpr[0] * stepSize,
		-y_mpr[1] * stepSize,
		-y_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_D_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(y_mpr[0] * stepSize,
		y_mpr[1] * stepSize,
		y_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_R_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();
	
	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(x_mpr[0] * stepSize,
		x_mpr[1] * stepSize,
		x_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_L_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-x_mpr[0] * stepSize,
		-x_mpr[1] * stepSize,
		-x_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_U_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(z_mpr[0] * stepSize,
		z_mpr[1] * stepSize,
		z_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_U_cor_clicked()
{
	on_pushButton_U_sag_clicked();
}
void DentalAccuracy::on_pushButton_D_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}


	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(-z_mpr[0] * stepSize,
		-z_mpr[1] * stepSize,
		-z_mpr[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_D_cor_clicked()
{
	on_pushButton_D_sag_clicked();
}
void DentalAccuracy::on_pushButton_L_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d leftSide = z_mpr.cross(y_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(leftSide[0] * stepSize,
		leftSide[1] * stepSize,
		leftSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_R_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d rightSide = -z_mpr.cross(y_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(rightSide[0] * stepSize,
		rightSide[1] * stepSize,
		rightSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_R_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d rightSide = z_mpr.cross(x_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(rightSide[0] * stepSize,
		rightSide[1] * stepSize,
		rightSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_L_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	Eigen::Vector3d leftSide = -z_mpr.cross(x_mpr);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	tmpTrans->Translate(leftSide[0] * stepSize,
		leftSide[1] * stepSize,
		leftSide[2] * stepSize);
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_clock_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, y_mpr[0], y_mpr[1], y_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_counter_cor_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	Eigen::Vector3d y_mpr = z_mpr.cross(x_mpr);
	y_mpr.normalize();

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, y_mpr[0], y_mpr[1], y_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);
	
	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_clock_sag_clicked()
{

	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	Eigen::Vector3d x_std{ 1,0,0 };

	if (x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, x_mpr[0], x_mpr[1], x_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();

}
void DentalAccuracy::on_pushButton_counter_sag_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d x_mpr{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
		implantMatrix->GetElement(2,0)
	};

	Eigen::Vector3d x_std{ 1,0,0 };

	if(x_mpr.dot(x_std) < 0)
	{
		x_mpr = -x_mpr;
	}

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, x_mpr[0], x_mpr[1], x_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_clock_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}
	
	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(stepSize, z_mpr[0], z_mpr[1], z_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}
void DentalAccuracy::on_pushButton_counter_ax_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	auto implantNode = GetDataStorage()->GetNamedNode("implant");

	double stepSize = m_Controls.lineEdit_stepSize->text().toDouble();

	auto implantMatrix = implantNode->GetData()->GetGeometry()->GetVtkMatrix();

	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d z_std{ 0,0,1 };

	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}
	// Eigen::Vector3d leftSide = z_mpr.cross(y_mpr);

	auto maxBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMaximum();
	auto minBound_init = implantNode->GetData()->GetGeometry()->GetBoundingBox()->GetMinimum();

	mitk::Point3D boundCenter_init;
	boundCenter_init[0] = (maxBound_init.GetElement(0) + minBound_init.GetElement(0)) / 2;
	boundCenter_init[1] = (maxBound_init.GetElement(1) + minBound_init.GetElement(1)) / 2;
	boundCenter_init[2] = (maxBound_init.GetElement(2) + minBound_init.GetElement(2)) / 2;

	auto tmpPset = mitk::PointSet::New();
	tmpPset->InsertPoint(boundCenter_init);
	tmpPset->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(implantMatrix);

	auto implantEndPoint = tmpPset->GetPoint(0);

	auto tmpTrans = vtkTransform::New();
	tmpTrans->PostMultiply();
	tmpTrans->SetMatrix(implantMatrix);
	// tmpTrans->Translate(leftSide[0] * stepSize,
	// 	leftSide[1] * stepSize,
	// 	leftSide[2] * stepSize);
	tmpTrans->Translate(-implantEndPoint[0], -implantEndPoint[1], -implantEndPoint[2]);
	tmpTrans->RotateWXYZ(-stepSize, z_mpr[0], z_mpr[1], z_mpr[2]);
	tmpTrans->Translate(implantEndPoint[0], implantEndPoint[1], implantEndPoint[2]);

	tmpTrans->Update();

	implantNode->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTrans->GetMatrix());

	// Update the attached gizmo
	mitk::Gizmo::RemoveGizmoFromNode(implantNode, GetDataStorage());
	mitk::Gizmo::AddGizmoToNode(implantNode, GetDataStorage());

	on_pushButton_implantFocus_clicked();
}





void DentalAccuracy::on_pushButton_implantFocus_clicked()
{
	if (GetDataStorage()->GetNamedNode("implant") == nullptr)
	{
		m_Controls.textBrowser->append("implant is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("roi_implantMPR") == nullptr)
	{
		m_Controls.textBrowser->append("roi_implantMPR is missing");
		return;
	}
	
	auto iRenderWindowPart = GetRenderWindowPart();

	TurnOffAllNodesVisibility();

	GetDataStorage()->GetNamedNode("roi_implantMPR")->SetVisibility(true);

	auto implantMatrix = GetDataStorage()->GetNamedNode("implant")->GetData()->GetGeometry()->GetVtkMatrix();

	mitk::Point3D implantPoint = GetDataStorage()->GetNamedNode("implant")->GetData()->GetGeometry()->GetCenter();

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->Identity();
	tmpMatrix->SetElement(0,3,implantPoint[0]);
	tmpMatrix->SetElement(1, 3, implantPoint[1]);
	tmpMatrix->SetElement(2, 3, implantPoint[2]);

	
	Eigen::Vector3d z_mpr{
		implantMatrix->GetElement(0,2),
		implantMatrix->GetElement(1,2),
		implantMatrix->GetElement(2,2)
	};

	Eigen::Vector3d x_std{ 1,0,0 };
	Eigen::Vector3d y_std{ 0,1,0 };
	Eigen::Vector3d z_std{ 0,0,1 };


	Eigen::Vector3d x_implant{
		implantMatrix->GetElement(0,0),
		implantMatrix->GetElement(1,0),
	    implantMatrix->GetElement(2,0)
	};

	x_implant.normalize();

	if(x_implant.dot(x_std) < 0)
	{
		x_implant = -x_implant;
	}
	
	if (z_mpr.dot(z_std) < 0)
	{
		z_mpr = -z_mpr;
	}


	// Eigen::Vector3d y_std{ implantMatrix->GetElement(1,0),implantMatrix->GetElement(1,1),0 };
	// y_std.normalize();

	Eigen::Vector3d y_mpr = z_mpr.cross(x_implant);
	y_mpr.normalize();

	Eigen::Vector3d x_mpr = y_mpr.cross(z_mpr);
	x_mpr.normalize();

	GetDataStorage()->GetNamedNode("roi_implantMPR")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	iRenderWindowPart->SetSelectedPosition(implantPoint);

	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("3d");

	if (renderWindow)
	{

		auto camera = renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

		auto camera_copy = vtkCamera::New();

		camera_copy->DeepCopy(camera);

		renderWindow->ResetView();

		renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(camera_copy);

	}

	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("coronal");
	if (renderWindow)
	{
		// Eigen::Vector3d x_projection = x_std - y_mpr * (x_std.dot(y_mpr));
		// x_projection.normalize();

		Eigen::Vector3d x_projection = y_mpr.cross(z_std);

		Eigen::Vector3d z_projection = x_projection.cross(y_mpr);
		z_projection.normalize();

		mitk::Vector3D a;
		a[0] = x_projection[0];
		a[1] = x_projection[1];
		a[2] = x_projection[2];
		

		mitk::Vector3D b;
		b[0] = z_projection[0];
		b[1] = z_projection[1];
		b[2] = z_projection[2];

		// renderWindow->ResetView();
		// renderWindow->CrosshairVisibilityChanged(false);

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);

		// mitk::Point3D origin;
		// FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);


	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("sagittal");
	if (renderWindow)
	{
		// Eigen::Vector3d y_projection = y_std - x_mpr * (y_std.dot(x_mpr));
		// y_projection.normalize();

		Eigen::Vector3d y_projection = z_std.cross(x_mpr);

		Eigen::Vector3d z_projection = x_mpr.cross(y_projection);
		y_projection.normalize();

		mitk::Vector3D a;
		a[0] = y_projection[0];
		a[1] = y_projection[1];
		a[2] = y_projection[2];


		mitk::Vector3D b;
		b[0] = z_projection[0];
		b[1] = z_projection[1];
		b[2] = z_projection[2];

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);
		

		// mitk::Point3D origin;
		// FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	if (renderWindow)
	{
		// Eigen::Vector3d x_projection = x_std - z_mpr * (x_std.dot(z_mpr));
		// x_projection.normalize();

		Eigen::Vector3d x_projection = y_std.cross(z_mpr);

		Eigen::Vector3d y_projection = z_mpr.cross(x_projection);
		y_projection.normalize();

		mitk::Vector3D a;
		a[0] = x_projection[0];
		a[1] = x_projection[1];
		a[2] = x_projection[2];


		mitk::Vector3D b;
		b[0] = -y_projection[0];
		b[1] = -y_projection[1];
		b[2] = -y_projection[2];

		renderWindow->GetSliceNavigationController()->ReorientSlices(implantPoint, a, b);
		
	}

	GetDataStorage()->GetNamedNode("roi_implantMPR")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("implant")->SetVisibility(true);

	if(GetDataStorage()->GetNamedNode("Gizmo") != nullptr)
	{
		GetDataStorage()->GetNamedNode("Gizmo")->SetVisibility(true);
	}

	GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->SetVisibility(true);
}



void DentalAccuracy::valueChanged_horizontalSlider()
{
	if (GetDataStorage()->GetNamedNode("Dental curve") == nullptr)
	{
		m_Controls.textBrowser->append("Dental curve is missing");
		return;
	}

	if (GetDataStorage()->GetNamedNode("roi_dentalCurveMPR") == nullptr)
	{
		m_Controls.textBrowser->append("roi_dentalCurveMPR is missing");
		return;
	}

	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve");

	double sliderValue = m_Controls.horizontalSlider->value();

	int currentIndex = floor(sliderValue * (mitkPset->GetSize() - 1) / m_Controls.horizontalSlider->maximum());

	if (currentIndex == (mitkPset->GetSize() - 1))
	{
		currentIndex -= 1;
	}

	auto currentPoint = mitkPset->GetPoint(currentIndex);
	auto nextPoint = mitkPset->GetPoint(currentIndex + 1);


	auto iRenderWindowPart = GetRenderWindowPart();

	auto worldPoint = iRenderWindowPart->GetSelectedPosition();

	worldPoint[0] = currentPoint[0];
	worldPoint[1] = currentPoint[1];
	worldPoint[2] = floor(abs(worldPoint[2])) * (worldPoint[2] / abs(worldPoint[2]));

	// m_Controls.textBrowser->append(QString::number(worldPoint[2]));

	TurnOffAllNodesVisibility();

	GetDataStorage()->GetNamedNode("roi_dentalCurveMPR")->SetVisibility(true);

	Eigen::Vector3d x_tmp;
	x_tmp[0] = nextPoint[0] - currentPoint[0];
	x_tmp[1] = nextPoint[1] - currentPoint[1];
	x_tmp[2] = nextPoint[2] - currentPoint[2];
	x_tmp.normalize();

	Eigen::Vector3d z_tmp;
	z_tmp[0] = 0;
	z_tmp[1] = 0;
	z_tmp[2] = 1;

	Eigen::Vector3d y_tmp = z_tmp.cross(x_tmp);
	y_tmp.normalize();

	auto tmpMatrix = vtkMatrix4x4::New();
	tmpMatrix->Identity();
	for(int i{0}; i < 3; i++)
	{
		//tmpMatrix->SetElement(i,0,x_tmp[i]);
		//tmpMatrix->SetElement(i, 1, y_tmp[i]);
		//tmpMatrix->SetElement(i, 2, z_tmp[i]);
		tmpMatrix->SetElement(i, 3, currentPoint[i]);
	}

	GetDataStorage()->GetNamedNode("roi_dentalCurveMPR")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpMatrix);

	iRenderWindowPart->SetSelectedPosition(worldPoint);


	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetActiveQmitkRenderWindow();

	if (renderWindow)
	{

		auto camera = renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

		auto camera_copy = vtkCamera::New();

		camera_copy->DeepCopy(camera);

		renderWindow->ResetView();

		renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(camera_copy);

	}

	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("coronal");
	if (renderWindow)
	{
		mitk::Vector3D a;
		a[0] = nextPoint[0] - currentPoint[0];
		a[1] = nextPoint[1] - currentPoint[1];
		a[2] = nextPoint[2] - currentPoint[2];
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = 0;
		b[2] = 1;

		// renderWindow->ResetView();

		renderWindow->GetVtkRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();

		// renderWindow->CrosshairVisibilityChanged(false);

		// renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);

		mitk::Point3D origin;
		FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);


	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("sagittal");
	if (renderWindow)
	{
		mitk::Vector3D a;
		a[0] = -(nextPoint[1] - currentPoint[1]);
		a[1] = (nextPoint[0] - currentPoint[0]);
		a[2] = nextPoint[2] - currentPoint[2];
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = 0;
		b[2] = 1;


		renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);
		mitk::Point3D origin;
		FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
	}


	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	if (renderWindow)
	{
		mitk::Vector3D a;
		a[0] = 1;
		a[1] = 0;
		a[2] = 0;
		a.Normalize();

		mitk::Vector3D b;
		b[0] = 0;
		b[1] = -1;
		b[2] = 0;

		renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);

		mitk::Point3D origin;
		FillVector3D(origin, 0.0, 0.0, 0.0);
		//renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
		//renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);

	}

	
	GetDataStorage()->GetNamedNode("roi_dentalCurveMPR")->SetVisibility(false);
	GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped")->SetVisibility(true);

}

// void DentalAccuracy::valueChanged_horizontalSlider()
// {
// 	if(GetDataStorage()->GetNamedNode("Dental curve") == nullptr)
// 	{
// 		m_Controls.textBrowser->append("Dental curve missing");
// 		return;
// 	}
//
// 	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve");
//
// 	double sliderValue = m_Controls.horizontalSlider->value();
//
// 	int currentIndex = floor(sliderValue * (mitkPset->GetSize() - 1) / m_Controls.horizontalSlider->maximum());
//
// 	if (currentIndex == (mitkPset->GetSize() - 1))
// 	{
// 		currentIndex -= 1;
// 	}
//
// 	auto currentPoint = mitkPset->GetPoint(currentIndex);
// 	auto nextPoint = mitkPset->GetPoint(currentIndex + 1);
//
//
// 	auto iRenderWindowPart = GetRenderWindowPart();
//
// 	auto worldPoint = iRenderWindowPart->GetSelectedPosition();
// 	
// 	worldPoint[0] = currentPoint[0];
// 	worldPoint[1] = currentPoint[1];
// 	
// 	iRenderWindowPart->SetSelectedPosition(worldPoint);
//
// 	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("coronal");
// 	if (renderWindow)
// 	{	
// 		mitk::Vector3D a;
// 		a[0] = nextPoint[0] - currentPoint[0];
// 		a[1] = nextPoint[1] - currentPoint[1];
// 		a[2] = nextPoint[2] - currentPoint[2];
// 		a.Normalize();
// 	
// 		mitk::Vector3D b;
// 		b[0] = 0;
// 		b[1] = 0;
// 		b[2] = 1;
// 	
// 		// renderWindow->ResetView();
// 	
// 		// renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);
//
// 		mitk::Point3D origin;
// 		FillVector3D(origin, 0.0, 0.0, 0.0);
// 		renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
// 		renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
//
//
// 	}
// 	
//
// 	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("sagittal");
// 	if (renderWindow)
// 	{
// 		mitk::Vector3D a;
// 		a[0] = -(nextPoint[1] - currentPoint[1]);
// 		a[1] = (nextPoint[0] - currentPoint[0]);
// 		a[2] = nextPoint[2] - currentPoint[2];
// 		a.Normalize();
// 	
// 		mitk::Vector3D b;
// 		b[0] = 0;
// 		b[1] = 0;
// 		b[2] = 1;
// 	
// 	
// 		// renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);
// 		mitk::Point3D origin;
// 		FillVector3D(origin, 0.0, 0.0, 0.0);
// 		renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
// 		renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
// 	}
// 	
// 	
// 	renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
// 	if (renderWindow)
// 	{
// 		mitk::Vector3D a;
// 		a[0] = 1;
// 		a[1] = 0;
// 		a[2] = 0;
// 		a.Normalize();
// 	
// 		mitk::Vector3D b;
// 		b[0] = 0;
// 		b[1] = -1;
// 		b[2] = 0;
// 	
// 		// renderWindow->GetSliceNavigationController()->ReorientSlices(worldPoint, a, b);
//
// 		mitk::Point3D origin;
// 		FillVector3D(origin, 0.0, 0.0, 0.0);
// 		renderWindow->GetSliceNavigationController()->ReorientSlices(origin, a, b);
// 		renderWindow->GetSliceNavigationController()->SelectSliceByPoint(worldPoint);
// 		
// 	}
//
// }


void DentalAccuracy::on_pushButton_viewPano_clicked()
{
	if(GetDataStorage()->GetNamedNode("Panorama") == nullptr)
	{
		m_Controls.textBrowser->append("Panorama is missing");
		return;
	}

	TurnOffAllNodesVisibility();

	GetDataStorage()->GetNamedNode("Panorama")->SetVisibility(true);

	
	ResetView();
	
}


void DentalAccuracy::on_pushButton_planeAdjust_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped");

	if(attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	// auto croppedImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");

	// Turn off all other nodes
	auto nodetreeModel = new QmitkDataStorageTreeModel(this->GetDataStorage());
	// nodetreeModel->get

	TurnOffAllNodesVisibility();

	attemptNode->SetVisibility(true);

	if (mitk::Gizmo::HasGizmoAttached(attemptNode, GetDataStorage()) == 1)
	{
		mitk::Gizmo::RemoveGizmoFromNode(attemptNode, GetDataStorage());
		m_Controls.pushButton_planeAdjust->setText("Adjust occlusal plane");
	}
	else
	{
		ResetView();

		mitk::Gizmo::AddGizmoToNode(attemptNode, GetDataStorage());
		m_Controls.pushButton_planeAdjust->setText("Finish adjustment");
	}

}

void DentalAccuracy::on_pushButton_splineAndPanorama_clicked()
{
	if (GetDataStorage()->GetNamedNode("Dental curve seeds") == nullptr)
	{
		m_Controls.textBrowser->append("Dental curve seeds missing");
		return;
	}

	auto mitkPset = GetDataStorage()->GetNamedObject<mitk::PointSet>("Dental curve seeds");

	if(mitkPset->GetSize() <= 2)
	{
		m_Controls.textBrowser->append("At least 3 dental curve seeds are required");
		return;
	}

	vtkNew<vtkPoints> points;
	for (int i{ 0 }; i < mitkPset->GetSize(); i++)
	{
		auto mitkPoint = mitkPset->GetPoint(i);
		points->InsertNextPoint(mitkPoint[0], mitkPoint[1], mitkPoint[2]);
	}

	// vtkCellArrays
	vtkNew<vtkCellArray> lines;
	lines->InsertNextCell(mitkPset->GetSize());
	for (unsigned int i = 0; i < mitkPset->GetSize(); ++i)
	{
		lines->InsertCellPoint(i);
	}

	// vtkPolyData
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);

	auto spline = vtkCardinalSpline::New();
	spline->SetLeftConstraint(2);
	spline->SetLeftValue(0.0);
	spline->SetRightConstraint(2);
	spline->SetRightValue(0.0);

	double segLength{ 0.3 };

	vtkNew<vtkSplineFilter> splineFilter;
	splineFilter->SetInputData(polyData);
	splineFilter->SetLength(segLength);
	splineFilter->SetSubdivideToLength();
	splineFilter->SetSpline(spline);
	splineFilter->Update();

	auto spline_PolyData = splineFilter->GetOutput();

	// Add the spline polydata into datastorage
	if(GetDataStorage()->GetNamedNode("Dental curve") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Dental curve"));
	}

	auto mitkPsetNew = mitk::PointSet::New();

	for (int i{ 0 }; i < spline_PolyData->GetPoints()->GetNumberOfPoints(); i++)
	{
		mitk::Point3D a;
		a[0] = spline_PolyData->GetPoint(i)[0];
		a[1] = spline_PolyData->GetPoint(i)[1];
		a[2] = spline_PolyData->GetPoint(i)[2];

		mitkPsetNew->InsertPoint(a);
	}

	auto curveNode = mitk::DataNode::New();
	curveNode->SetData(mitkPsetNew);
	curveNode->SetName("Dental curve");
	curveNode->SetFloatProperty("point 2D size", 0.3);
	GetDataStorage()->Add(curveNode);


	vtkSmartPointer<vtkImageAppend> append = vtkSmartPointer<vtkImageAppend>::New();

	append->SetAppendAxis(2);

	int thickness{ 30 };

	auto attemptImageNode = GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped");

	if (attemptImageNode == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	auto vtkImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped")->GetVtkImageData();

	// Note: the vtkImage above may not occupy the same space as the mitk Image because "GetVtkImageData()" loses the MITK geometry transform
	// therefore we have to apply the inverse transform to the probe polydata instead

	for (int i{ 0 }; i < 2 * thickness; i++)
	{
		double stepSize = segLength * (-thickness + i);

		auto expandedSpline = ExpandSpline(spline_PolyData, spline_PolyData->GetNumberOfPoints() - 1, stepSize);

		// Sweep the line to form a surface.
		double direction[3];
		direction[0] = 0.0;
		direction[1] = 0.0;
		direction[2] = 1.0;
		unsigned cols = 250;

		double distance = cols * segLength;
		auto surface =
			SweepLine_2Sides(expandedSpline, direction, distance, cols);

		if(i == thickness)
		{
			if(GetDataStorage()->GetNamedNode("Probe surface") != nullptr)
			{
				GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Probe surface"));
			}

			auto probeSurface = mitk::Surface::New();
			probeSurface->SetVtkPolyData(surface);
			auto probeNode = mitk::DataNode::New();
			probeNode->SetData(probeSurface);
			probeNode->SetName("Probe surface");
			float color[3]{1,0,0};
			probeNode->SetColor(color);
			probeNode->SetOpacity(0.5);
			GetDataStorage()->Add(probeNode);
		}


		// Apply the inverse geometry of the MITK image to the probe surface
		auto geometryMatrix = vtkMatrix4x4::New();
		geometryMatrix->DeepCopy(attemptImageNode->GetData()->GetGeometry()->GetVtkMatrix());
		auto spacing = attemptImageNode->GetData()->GetGeometry()->GetSpacing();

		for (int j{ 0 }; j < 3; j++)
		{
			geometryMatrix->SetElement(j, 0, geometryMatrix->GetElement(j, 0) / spacing[0]);
			geometryMatrix->SetElement(j, 1, geometryMatrix->GetElement(j, 1) / spacing[1]);
			geometryMatrix->SetElement(j, 2, geometryMatrix->GetElement(j, 2) / spacing[2]);
		}

		geometryMatrix->Invert();

		vtkNew<vtkTransformFilter> tmpTransFilter;
		vtkNew<vtkTransform> tmpTransform;
		tmpTransform->SetMatrix(geometryMatrix);
		tmpTransFilter->SetTransform(tmpTransform);
		tmpTransFilter->SetInputData(surface);
		tmpTransFilter->Update();

		vtkNew<vtkProbeFilter> sampleVolume;
		sampleVolume->SetSourceData(vtkImage);
		sampleVolume->SetInputData(tmpTransFilter->GetPolyDataOutput());

		sampleVolume->Update();

		auto probeData = sampleVolume->GetOutput();

		auto probePointData = probeData->GetPointData();

		auto tmpArray = probePointData->GetScalars();

		auto testimageData = vtkImageData::New();
		testimageData->SetDimensions(cols + 1, spline_PolyData->GetNumberOfPoints(), 1);
		//testimageData->SetDimensions( spline_PolyData->GetNumberOfPoints(), cols + 1, 1);


		testimageData->SetSpacing(segLength, segLength, 1);
		testimageData->SetOrigin(0, 0, 0);
		testimageData->AllocateScalars(VTK_INT, 1);
		testimageData->GetPointData()->SetScalars(tmpArray);

		// auto tmpImage = vtkImageData::New();
		// tmpImage->DeepCopy(testimageData);

		append->AddInputData(testimageData);

	}

	append->Update();
	auto appenedImage = append->GetOutput();
	appenedImage->SetSpacing(segLength, segLength, segLength);

	auto mitkAppendedImage = mitk::Image::New();

	mitkAppendedImage->Initialize(appenedImage);
	mitkAppendedImage->SetVolume(appenedImage->GetScalarPointer());

	// Rotate the image by -90 degree along the +z axis
	auto rotateTrans = vtkTransform::New();
	rotateTrans->PostMultiply();
	rotateTrans->SetMatrix(mitkAppendedImage->GetGeometry()->GetVtkMatrix());
	rotateTrans->RotateZ(-90);
	rotateTrans->Update();

	mitkAppendedImage->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(rotateTrans->GetMatrix());

	if (GetDataStorage()->GetNamedNode("Panorama") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Panorama"));
	}

	auto tmpNode_ = mitk::DataNode::New();
	tmpNode_->SetData(mitkAppendedImage);
	tmpNode_->SetName("Panorama");
	GetDataStorage()->Add(tmpNode_);


	TurnOffAllNodesVisibility();
	GetDataStorage()->GetNamedNode("Dental curve")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("Dental curve seeds")->SetVisibility(true);
	ResetView();

	attemptImageNode->SetVisibility(true);
}


void DentalAccuracy::on_pushButton_viewCursiveMPR_clicked()
{
	auto attemptNode = GetDataStorage()->GetNamedNode("CBCT Bounding Shape_cropped");
	auto curveNode = GetDataStorage()->GetNamedNode("Dental curve");
	auto probeSurfaceNode = GetDataStorage()->GetNamedNode("Probe surface");

	if (curveNode == nullptr)
	{
		m_Controls.textBrowser->append("Dental curve is missing");
		return;
	}

	if (attemptNode == nullptr)
	{
		m_Controls.textBrowser->append("CBCT Bounding Shape_cropped is missing");
		return;
	}

	if (probeSurfaceNode == nullptr)
	{
		m_Controls.textBrowser->append("Probe surface is missing");
		return;
	}

	TurnOffAllNodesVisibility();

	if(m_Controls.horizontalSlider->isEnabled())
	{
		m_Controls.horizontalSlider->setDisabled(true);
		m_Controls.pushButton_viewCursiveMPR->setText("View cursive MPR");
		m_Controls.horizontalSlider->setValue(0);
		GetDataStorage()->GetNamedNode("Probe surface")->SetVisibility(false);
	}else
	{
		m_Controls.horizontalSlider->setEnabled(true);
		m_Controls.pushButton_viewCursiveMPR->setText("Stop viewing cursive MPR");
		m_Controls.horizontalSlider->setValue(0);
		// probeSurfaceNode->SetVisibility(true);
	}

	attemptNode->SetVisibility(true);
	

	ResetView();

}


void DentalAccuracy::TurnOffAllNodesVisibility()
{
	auto dataNodes = GetDataStorage()->GetAll();
	for (auto item = dataNodes->begin(); item != dataNodes->end(); ++item)
	{
		(*item)->SetVisibility(false);
	}

	GetDataStorage()->GetNamedNode("stdmulti.widget0.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget1.plane")->SetVisibility(true);
	GetDataStorage()->GetNamedNode("stdmulti.widget2.plane")->SetVisibility(true);
}

vtkSmartPointer<vtkPolyData> DentalAccuracy::ExpandSpline(vtkPolyData* line, int divisionNum,
	double stepSize)
{

	vtkNew<vtkPoints> points;
	for (int i{ 0 }; i < line->GetNumberOfPoints(); i++)
	{
		Eigen::Vector3d currentPoint;
		currentPoint[0] = line->GetPoint(i)[0];
		currentPoint[1] = line->GetPoint(i)[1];
		currentPoint[2] = line->GetPoint(i)[2];

		Eigen::Vector3d z_axis;
		z_axis[0] = 0;
		z_axis[1] = 0;
		z_axis[2] = 1;

		Eigen::Vector3d ptpVector;

		if (i == (line->GetNumberOfPoints() - 1))
		{
			ptpVector[0] = -line->GetPoint(i - 1)[0] + currentPoint[0];
			ptpVector[1] = -line->GetPoint(i - 1)[1] + currentPoint[1];
			ptpVector[2] = -line->GetPoint(i - 1)[2] + currentPoint[2];
		}
		else
		{
			ptpVector[0] = line->GetPoint(i + 1)[0] - currentPoint[0];
			ptpVector[1] = line->GetPoint(i + 1)[1] - currentPoint[1];
			ptpVector[2] = line->GetPoint(i + 1)[2] - currentPoint[2];
		}

		Eigen::Vector3d tmpVector;

		tmpVector = z_axis.cross(ptpVector);

		tmpVector.normalize();

		points->InsertNextPoint(currentPoint[0] + tmpVector[0] * stepSize,
			currentPoint[1] + tmpVector[1] * stepSize,
			currentPoint[2] + tmpVector[2] * stepSize);

	}

	// vtkCellArrays
	vtkNew<vtkCellArray> lines;
	lines->InsertNextCell(points->GetNumberOfPoints());
	for (unsigned int i = 0; i < points->GetNumberOfPoints(); ++i)
	{
		lines->InsertCellPoint(i);
	}

	// vtkPolyData
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(lines);

	auto spline = vtkCardinalSpline::New();
	spline->SetLeftConstraint(2);
	spline->SetLeftValue(0.0);
	spline->SetRightConstraint(2);
	spline->SetRightValue(0.0);

	vtkNew<vtkSplineFilter> splineFilter;
	splineFilter->SetInputData(polyData);
	splineFilter->SetSubdivideToSpecified();
	splineFilter->SetNumberOfSubdivisions(divisionNum);
	splineFilter->SetSpline(spline);
	splineFilter->Update();

	auto spline_PolyData = splineFilter->GetOutput();

	return spline_PolyData;
}

vtkSmartPointer<vtkPolyData> DentalAccuracy::SweepLine_2Sides(vtkPolyData* line, double direction[3],
	double distance, unsigned int cols)
{
	unsigned int rows = line->GetNumberOfPoints();
	double spacing = distance / cols;
	vtkNew<vtkPolyData> surface;

	// Generate the points.
	cols++;
	unsigned int numberOfPoints = rows * cols;
	unsigned int numberOfPolys = (rows - 1) * (cols - 1);
	vtkNew<vtkPoints> points;
	points->Allocate(numberOfPoints);
	vtkNew<vtkCellArray> polys;
	polys->Allocate(numberOfPolys * 4);

	double x[3];
	unsigned int cnt = 0;
	for (unsigned int row = 0; row < rows; row++)
	{
		for (unsigned int col = 0; col < cols; col++)
		{
			double p[3];
			line->GetPoint(row, p);
			x[0] = p[0] - distance * direction[0] / 2 + direction[0] * col * spacing;
			x[1] = p[1] - distance * direction[1] / 2 + direction[1] * col * spacing;
			x[2] = p[2] - distance * direction[2] / 2 + direction[2] * col * spacing;
			points->InsertPoint(cnt++, x);
		}
	}
	// Generate the quads.
	vtkIdType pts[4];
	for (unsigned int row = 0; row < rows - 1; row++)
	{
		for (unsigned int col = 0; col < cols - 1; col++)
		{
			pts[0] = col + row * (cols);
			pts[1] = pts[0] + 1;
			pts[2] = pts[0] + cols + 1;
			pts[3] = pts[0] + cols;
			polys->InsertNextCell(4, pts);
		}
	}
	surface->SetPoints(points);
	surface->SetPolys(polys);


	return surface;
}


void DentalAccuracy::ResetView()
{
	auto iRenderWindowPart = GetRenderWindowPart();
	QmitkRenderWindow* renderWindow = iRenderWindowPart->GetQmitkRenderWindow("axial");
	renderWindow->ResetView();
}


void DentalAccuracy::ClipTeeth()
{
	auto inputPolyData = dynamic_cast<mitk::Surface*>
		(GetDataStorage()->GetNamedNode("ios")->GetData());

	auto inputPointSet = dynamic_cast<mitk::PointSet*>
		(GetDataStorage()->GetNamedNode("landmark_src")->GetData());

	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter =
		vtkSmartPointer<vtkCleanPolyData>::New();

	int inputPointNum = inputPointSet->GetSize();

	for (int i{ 0 }; i < inputPointNum; i++)
	{
		double tmpPoint[3]
		{
			inputPointSet->GetPoint(i)[0],
			inputPointSet->GetPoint(i)[1],
			inputPointSet->GetPoint(i)[2],
		};

		vtkNew<vtkClipPolyData> clip;
		vtkNew<vtkPlanes> planes;

		double boxSize{ 6 };

		planes->SetBounds(
			tmpPoint[0] - boxSize / 2,
			tmpPoint[0] + boxSize / 2,
			tmpPoint[1] - boxSize / 2,
			tmpPoint[1] + boxSize / 2,
			tmpPoint[2] - boxSize / 2,
			tmpPoint[2] + boxSize / 2
		);

		clip->SetInputData(inputPolyData->GetVtkPolyData());
		clip->SetClipFunction(planes);
		clip->InsideOutOn();
		clip->GenerateClippedOutputOn();
		clip->Update();

		appendFilter->AddInputData(clip->GetOutput());
		appendFilter->Update();
	}

	cleanFilter->SetInputData(appendFilter->GetOutput());
	cleanFilter->Update();

	auto tmpNode = mitk::DataNode::New();
	auto tmpData = mitk::Surface::New();
	tmpData->SetVtkPolyData(cleanFilter->GetOutput());
	tmpNode->SetData(tmpData);
	tmpNode->SetName("Clipped data");
	tmpNode->SetVisibility(false);
	GetDataStorage()->Add(tmpNode);
}


void DentalAccuracy::UpdateAllBallFingerPrint(mitk::PointSet::Pointer stdSteelballCenters)
{
	int pointNum = stdSteelballCenters->GetSize();

	int currentIndex{ 0 };
	for (int i{ 0 }; i < pointNum; i++)
	{

		int currentColumn{ 0 };
		for (int j{ 0 }; j < pointNum; j++)
		{
			if (i != j)
			{
				allBallFingerPrint[currentIndex] = GetPointDistance(stdSteelballCenters->GetPoint(i), stdSteelballCenters->GetPoint(j));
				//m_Controls.textBrowser->append(QString::number(currentIndex)+": "+ QString::number(allBallFingerPrint[3 * i + currentColumn]));
				currentColumn += 1;
				currentIndex += 1;
			}
		}
	}

}

double DentalAccuracy::GetPointDistance(const mitk::Point3D p0, const mitk::Point3D p1)
{
	double distance = sqrt(
		pow(p0[0] - p1[0], 2) +
		pow(p0[1] - p1[1], 2) +
		pow(p0[2] - p1[2], 2)
	);

	return distance;
}

bool DentalAccuracy::GetCoarseSteelballCenters(double steelballVoxel)
{
	if (GetDataStorage()->GetNamedNode("Steelball centers") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));
	}

	// INPUT 1: inputCtImage (MITK image)
	auto inputCtImage = GetDataStorage()->GetNamedObject<mitk::Image>("CBCT Bounding Shape_cropped");
	
	auto mitkSteelBallSurfaces = mitk::Surface::New();
	mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

	imageToSurfaceFilter->SetInput(inputCtImage);
	imageToSurfaceFilter->SetThreshold(steelballVoxel);
	mitkSteelBallSurfaces = imageToSurfaceFilter->GetOutput();


	if (mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells() > 300000) // To ensure the speed
	{
		auto tmpPointset = mitk::PointSet::New();
		auto nodeSortedSteelballCenters = mitk::DataNode::New();
		nodeSortedSteelballCenters->SetName("Steelball centers");
		// add new node
		nodeSortedSteelballCenters->SetData(tmpPointset);
		GetDataStorage()->Add(nodeSortedSteelballCenters);

		return false;
	}

	// m_Controls.textBrowser->append("cells: "+QString::number(mitkSteelBallSurfaces->GetVtkPolyData()->GetNumberOfCells()));

	// Separate steelball surface by examining their connectivity
	vtkNew<vtkConnectivityFilter> vtkConnectivityFilter;
	vtkConnectivityFilter->SetInputData(mitkSteelBallSurfaces->GetVtkPolyData());

	vtkConnectivityFilter->SetExtractionModeToAllRegions();
	vtkConnectivityFilter->Update();
	int numberOfPotentialSteelBalls = vtkConnectivityFilter->GetNumberOfExtractedRegions();


	auto mitkSingleSteelballCenterPointset = mitk::PointSet::New(); // store each steelball's center
	double centerOfAllSteelballs[3]{ 0, 0, 0 };                       // the center of all steel balls

	// Iterate over all the potential steelball regions, extract the real ones by assessing their size (facets numbers)
	vtkConnectivityFilter->SetExtractionModeToSpecifiedRegions();
	for (int m = 0; m < numberOfPotentialSteelBalls; m++)
	{
		vtkConnectivityFilter->InitializeSpecifiedRegionList();
		vtkConnectivityFilter->AddSpecifiedRegion(m);
		vtkConnectivityFilter->Update();
		// m_Controls.textBrowser->append("Connectivity passed");

		auto vtkSingleSteelBallSurface = vtkConnectivityFilter->GetPolyDataOutput();

		auto numberOfCells =
			vtkSingleSteelBallSurface->GetNumberOfCells(); // the total number of cells of a single mesh surface; each cell
														   // stores one facet of the mesh surface

		  // INPUT 3 & 4: facetNumberUpperThreshold (int) & facetNumberUpperThreshold (int)
		int facetNumberUpperThreshold = 2000;// m_Controls.lineEdit_ballMaxCell->text().toInt();
		int facetNumberLowerThreshold = 150; // m_Controls.lineEdit_ballMinCell->text().toInt();

		if (numberOfCells < facetNumberUpperThreshold && numberOfCells > facetNumberLowerThreshold)
		{
			std::vector<double> inp_x(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_y(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface
			std::vector<double> inp_z(
				numberOfCells); // inp_x, inp_y and inp_z store one point of each facet on the mesh surface

			for (int n = 0; n < numberOfCells; n++)
			{
				auto tmpPoint = vtkSingleSteelBallSurface->GetCell(n)->GetPoints()->GetPoint(0);

				inp_x[n] = tmpPoint[0];
				inp_y[n] = tmpPoint[1];
				inp_z[n] = tmpPoint[2];
			}

			// use inp_x, inp_y and inp_z to simulate a sphere
			double cx, cy, cz;
			double R;

			lancetAlgorithm::fit_sphere(inp_x, inp_y, inp_z, cx, cy, cz, R);

			mitk::Point3D mitkTmpCenterPoint3D;
			mitkTmpCenterPoint3D[0] = cx;
			mitkTmpCenterPoint3D[1] = cy;
			mitkTmpCenterPoint3D[2] = cz;
			mitkSingleSteelballCenterPointset->InsertPoint(mitkTmpCenterPoint3D);

			centerOfAllSteelballs[0] = centerOfAllSteelballs[0] + cx;
			centerOfAllSteelballs[1] = centerOfAllSteelballs[1] + cy;
			centerOfAllSteelballs[2] = centerOfAllSteelballs[2] + cz;

		}


	}

	int numberOfActualSteelballs = mitkSingleSteelballCenterPointset->GetSize();

	centerOfAllSteelballs[0] = centerOfAllSteelballs[0] / numberOfActualSteelballs;
	centerOfAllSteelballs[1] = centerOfAllSteelballs[1] / numberOfActualSteelballs;
	centerOfAllSteelballs[2] = centerOfAllSteelballs[2] / numberOfActualSteelballs;

	// Sort the centers of the separate steelballs according to their distances to the group center
	std::vector<double> distancesToPointSetCenter(numberOfActualSteelballs);
	std::vector<int> distanceRanks(numberOfActualSteelballs);

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		distancesToPointSetCenter[i] =
			sqrt(pow(centerOfAllSteelballs[0] - mitkSingleSteelballCenterPointset->GetPoint(i)[0], 2) +
				pow(centerOfAllSteelballs[1] - mitkSingleSteelballCenterPointset->GetPoint(i)[1], 2) +
				pow(centerOfAllSteelballs[2] - mitkSingleSteelballCenterPointset->GetPoint(i)[2], 2));

		distanceRanks[i] = i;
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance before sorting: " << distancesToPointSetCenter[i];
	}

	for (int i = 0; i < numberOfActualSteelballs - 2; i++)
	{
		for (int j = 0; j < numberOfActualSteelballs - 1 - i; j++)
		{
			double temp = 0;
			double temp2 = 0;
			if (distancesToPointSetCenter[j] > distancesToPointSetCenter[j + 1])
			{
				temp = distancesToPointSetCenter[j];
				distancesToPointSetCenter[j] = distancesToPointSetCenter[j + 1];
				distancesToPointSetCenter[j + 1] = temp;

				temp2 = distanceRanks[j];
				distanceRanks[j] = distanceRanks[j + 1];
				distanceRanks[j + 1] = temp2;
			}
		}
	}

	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		//MITK_INFO << "Distance after sorting: " << distancesToPointSetCenter[i];
	}

	auto mitkSortedSingleSteelballCenterPointset = mitk::PointSet::New();
	for (int i = 0; i < numberOfActualSteelballs; i++)
	{
		mitkSortedSingleSteelballCenterPointset->InsertPoint(mitkSingleSteelballCenterPointset->GetPoint(distanceRanks[i]));
	}


	// add sorted steel ball centers
	auto nodeSortedSteelballCenters = mitk::DataNode::New();
	nodeSortedSteelballCenters->SetName("Steelball centers");
	// add new node
	nodeSortedSteelballCenters->SetData(mitkSortedSingleSteelballCenterPointset);
	GetDataStorage()->Add(nodeSortedSteelballCenters);

	return true;
}

void DentalAccuracy::ScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7])
{
	foundIDs[0] = 0;
	foundIDs[1] = 0;
	foundIDs[2] = 0;
	foundIDs[3] = 0;
	foundIDs[4] = 0;
	foundIDs[5] = 0;
	foundIDs[6] = 0;

	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());
	int inputPoinSetNum = inputPointSet->GetSize();
	auto steelballCenters = mitk::PointSet::New();

	int lengthOfFingerPrint = stdNeighborNum;
	int numOfTargetSteelballs = stdNeighborNum + 1;


	for (int q{ 0 }; q < numOfTargetSteelballs; q++)
	{
		double fingerPrint[6]
		{
			allBallFingerPrint[6 * q],
			allBallFingerPrint[6 * q + 1],
			allBallFingerPrint[6 * q + 2],
			allBallFingerPrint[6 * q + 3],
			allBallFingerPrint[6 * q + 4],
			allBallFingerPrint[6 * q + 5],
		};


		for (int i{ 0 }; i < inputPoinSetNum; i++)
		{
			int metric{ 0 };

			for (int j{ 0 }; j < lengthOfFingerPrint; j++)
			{
				bool metSingleRequirement = false;
				double standardDistance = fingerPrint[j];

				for (int k{ 0 }; k < inputPoinSetNum; k++)
				{
					double tmpDistance = GetPointDistance(inputPointSet->GetPoint(i), inputPointSet->GetPoint(k));
					if (fabs(tmpDistance - standardDistance) < 0.4)
					{
						metSingleRequirement = true;
						metric += 1;
						break;
					}
				}

				// if (metSingleRequirement == false)
				// {
				// 	break;
				// }

			}

			//m_Controls.textBrowser->append("metric: " + QString::number(metric));
			if (metric >= requiredNeighborNum)
			{

				// Add this point to the pointset
				steelballCenters->InsertPoint(inputPointSet->GetPoint(i));
				//break;
				foundIDs[q] = 1;
			}

		}

	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(steelballCenters);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();
}

void DentalAccuracy::IterativeScreenCoarseSteelballCenters(int requiredNeighborNum, int stdNeighborNum, int foundIDs[7])
{
	int oldNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
	int newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();

	// m_Controls.textBrowser->append("old: " + QString::number(oldNumOfCenters));
	// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
	//
	int limit{ 0 };

	while (newNumOfCenters != oldNumOfCenters)
	{
		// m_Controls.textBrowser->append("old: "+ QString::number(oldNumOfCenters));
		// m_Controls.textBrowser->append("new: " + QString::number(newNumOfCenters));
		oldNumOfCenters = newNumOfCenters;
		// m_Controls.textBrowser->append(QString::number(limit));

		ScreenCoarseSteelballCenters(requiredNeighborNum, stdNeighborNum, foundIDs);
		newNumOfCenters = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData())->GetSize();
		limit += 1;
		if (limit == 20)
		{
			m_Controls.textBrowser->append("--- Warning: Maximal screening iteration cycle has been reached ---");
			break;
		}
	}

	RemoveRedundantCenters();
}


void DentalAccuracy::RemoveRedundantCenters()
{
	auto inputPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto tmpPointSet = mitk::PointSet::New();

	for (int i{ 0 }; i < inputPointSet->GetSize(); i++)
	{
		bool unique = true;
		for (int j{ 0 }; j < tmpPointSet->GetSize(); j++)
		{
			if (GetPointDistance(tmpPointSet->GetPoint(j), inputPointSet->GetPoint(i)) < 0.05)
			{
				unique = false;
				break;
			}
		}

		if (unique == true)
		{
			tmpPointSet->InsertPoint(inputPointSet->GetPoint(i));
		}
	}


	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPointSet);
	GetDataStorage()->Add(tmpNode);
}

void DentalAccuracy::RearrangeSteelballs(int stdNeighborNum, int foundIDs[7])
{
	auto extractedPointSet = dynamic_cast<mitk::PointSet*>(GetDataStorage()->GetNamedNode("Steelball centers")->GetData());

	auto standartSteelballCenters = mitk::PointSet::New();
	int stdCenterNum{ 7 };
	

	for (int i{ 0 }; i < stdCenterNum; i++)
	{
		double tmpArray[3]
		{
			stdCenters[i * 3],
			stdCenters[i * 3 + 1],
			stdCenters[i * 3 + 2]
		};
		mitk::Point3D p(tmpArray);
		standartSteelballCenters->InsertPoint(p);
	}

	auto partialStdCenters = mitk::PointSet::New();
	for(int i{0}; i < stdCenterNum; i++)
	{
		if(foundIDs[i] == 1)
		{
			partialStdCenters->InsertPoint(standartSteelballCenters->GetPoint(i));
		}
	}

	// Todo: this array should be generated automatically
	// int a[] = { 0,1,2,3,4,5,6,7,8,9 }; // in case the result of iterative search has pointNum larger than 7

	std::vector<int> a;
	for(int n{0}; n < extractedPointSet->GetSize(); n++)
	{
		a.push_back(n);
	}

	std::sort(a.begin(), a.end());

	double errorSum = 50;
	int b[7]{ 0,1,2,3,4,5,6 };
	do
	{
		auto newPset = mitk::PointSet::New();


		for (int i{0}; i < extractedPointSet->GetSize(); i++)
		{
			// if (a[i] < (extractedPointSet->GetSize()))
			// {
			// 	newPset->InsertPoint(extractedPointSet->GetPoint(a[i]));
			// 	
			// }
			if ((newPset->GetSize()) < partialStdCenters->GetSize())
			{
				newPset->InsertPoint(extractedPointSet->GetPoint(a[i]));
			}
		}

		auto landmarkRegistrator = mitk::SurfaceRegistration::New();
		landmarkRegistrator->SetLandmarksSrc(newPset);
		landmarkRegistrator->SetLandmarksTarget(partialStdCenters);

		landmarkRegistrator->ComputeLandMarkResult();
		double maxError = landmarkRegistrator->GetmaxLandmarkError();
		double avgError = landmarkRegistrator->GetavgLandmarkError();
		double tmpError = maxError + avgError;

		MITK_INFO << "tmpError: " << tmpError;
		MITK_INFO << "errorSum: " << errorSum;

		if(tmpError < errorSum && tmpError > 0)
		{
			errorSum = tmpError;
			
			b[0] = a[0];
			b[1] = a[1];
			b[2] = a[2];
			b[3] = a[3];
			b[4] = a[4];
			b[5] = a[5];
			b[6] = a[6];
		}

		if(tmpError < 1)
		{
			break;
		}


	}while (std::next_permutation(a.begin(), a.end()));


	auto tmpPset = mitk::PointSet::New();

	for (int i{ 0 }; i < 7; i++)
	{
		if (i < partialStdCenters->GetSize())
		{
			tmpPset->InsertPoint(extractedPointSet->GetPoint(b[i]));
		}
	}

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Steelball centers"));

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("Steelball centers");
	tmpNode->SetData(tmpPset);
	GetDataStorage()->Add(tmpNode);
	RemoveRedundantCenters();
}
