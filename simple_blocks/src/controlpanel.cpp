//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QDoubleSpinBox>

//#include <cassert>

namespace subvol
{

ControlPanel::ControlPanel(BlockRenderer *renderer, QWidget *parent)
    : QWidget(parent)
    , m_numberOfSliderIncrements{ 1000 }
    , m_incrementDelta{ 0.0 }
    , m_currentMinFloat{ 0.0 }
    , m_currentMaxFloat{ 0.0 }
    , m_globalMin{ 0.0 }
    , m_globalMax{ 0.0 }
    , m_renderer{ renderer }
{

  m_minSlider = new QSlider(Qt::Orientation::Horizontal);
  m_minSlider->setMinimum(0);
  m_minSlider->setMaximum(m_numberOfSliderIncrements);
  m_minSlider->setValue(0);

  m_maxSlider = new QSlider(Qt::Orientation::Horizontal);
  m_maxSlider->setMinimum(0);
  m_maxSlider->setMaximum(m_numberOfSliderIncrements);
  m_maxSlider->setValue(m_numberOfSliderIncrements);

//  m_currentMin_SpinBox = new QDoubleSpinBox();
//  m_currentMin_SpinBox->setMinimum(0);
//  m_currentMin_SpinBox->setMaximum(m_numberOfSliderIncrements);
//  m_currentMin_SpinBox->setValue(0);
//
//  m_currentMin_SpinBox = new QDoubleSpinBox();
//  m_currentMin_SpinBox->setMinimum(m_currentMinFloat);
//  m_currentMin_SpinBox->setMaximum(m_currentMaxFloat);
//  m_currentMin_SpinBox->setSingleStep((m_currentMaxFloat - m_currentMinFloat) / m_incrementDelta);
//  m_currentMin_SpinBox->setValue(m_currentMinFloat);
//
//  m_currentMax_SpinBox = new QDoubleSpinBox();
//  m_currentMax_SpinBox->setMinimum(m_currentMinFloat);
//  m_currentMax_SpinBox->setMaximum(m_currentMaxFloat);
//  m_currentMax_SpinBox->setSingleStep((m_currentMaxFloat - m_currentMinFloat) / m_incrementDelta);
//  m_currentMax_SpinBox->setValue(m_currentMaxFloat);

  m_globalMin_Label = new QLabel("0");

  m_globalMax_Label = new QLabel("0");
  m_currentMin_Label = new QLabel("0");
  m_currentMax_Label = new QLabel("0");

  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->addWidget(m_minSlider, 0,0);
  gridLayout->addWidget(m_currentMin_Label, 0, 1);
  gridLayout->addWidget(m_maxSlider, 1,0);
  gridLayout->addWidget(m_currentMax_Label, 1, 1);


  setLayout(gridLayout);


  connect(m_minSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_valueChanged_min(int)));

  connect(m_minSlider, SIGNAL(sliderPressed()),
  this, SLOT(handle_sliderPressed()));

  connect(m_minSlider, SIGNAL(sliderReleased()),
          this, SLOT(handle_sliderReleased()));

  connect(m_maxSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_valueChanged_max(int)));

  connect(m_maxSlider, SIGNAL(sliderPressed()),
          this, SLOT(handle_sliderPressed()));

  connect(m_maxSlider, SIGNAL(sliderReleased()),
          this, SLOT(handle_sliderReleased()));

//  connect(m_currentMin_SpinBox, SIGNAL(valueChanged(int)),
//          this, SLOT(handle_valueChanged_min(int)));
//
//  connect(m_currentMax_SpinBox, SIGNAL(valueChanged(int)),
//          this, SLOT(handle_valueChanged_max(int)));

}

ControlPanel::~ControlPanel()
{
}

//void
//ControlPanel::min_valueChanged(int value)
//{
//
//void
//ControlPanel::max_valueChanged(int value)
//{
//
//}

void
ControlPanel::setGlobalRange(double newMin, double newMax)
{

  if(newMax < newMin) {
    return;
  }

  m_incrementDelta = (newMax - newMin) / m_numberOfSliderIncrements;

  m_globalMin = newMin;
  m_globalMax = newMax;

//  m_currentMinFloat = newMin;
//  m_currentMaxFloat = newMax;

//  m_minSlider->setValue(floatToSliderValue(newMin));
//  m_maxSlider->setValue(floatToSliderValue(newMax));


  m_globalMin_Label->setText(QString::number(newMin));
  m_globalMax_Label->setText(QString::number(newMax));


//  m_currentMin_SpinBox->setRange(newMin, newMax);
//  m_currentMin_SpinBox->setValue(newMin);
//
//  m_currentMax_SpinBox->setRange(newMin, newMax);
//  m_currentMax_SpinBox->setValue(newMax);


}


void
ControlPanel::setMinMax(double min, double max)
{
  m_currentMinFloat = min;
  m_currentMaxFloat = max;
  m_minSlider->setValue(floatToSliderValue(min));
  m_maxSlider->setValue(floatToSliderValue(max));
}


void
ControlPanel::handle_valueChanged_min(int minSliderValue)
{
//  if (m_minSlider->value() != minSliderValue) {
    m_currentMinFloat = sliderValueToFloat(minSliderValue);

    if (minSliderValue > m_maxSlider->value()) {
      m_maxSlider->setValue(minSliderValue + 1);
    }
    m_currentMin_Label->setText(QString::number(m_currentMinFloat));
//    m_currentMin_SpinBox->setValue(m_currentMinFloat);
//  }
  //TODO: send as min ROV to renderer
}


void
ControlPanel::handle_valueChanged_max(int maxSliderValue)
{
//  if (m_maxSlider->value() != maxSliderValue) {
    m_currentMaxFloat = sliderValueToFloat(maxSliderValue);

    if (maxSliderValue < m_minSlider->value()) {
      m_minSlider->setValue(maxSliderValue - 1);
    }

  m_currentMax_Label->setText(QString::number(m_currentMaxFloat));

//    m_currentMax_SpinBox->setValue(maxSliderValue);
//  }
  //TODO: send as max ROV to renderer.
}


//void
//ControlPanel::handle_spinBox_valueChanged_min(double minValue)
//{
////  if (m_currentMin_SpinBox->value() != minValue) {
//    m_currentMinFloat = minValue;
//    double maxSpinBoxValue = m_currentMax_SpinBox->value();
//    if (minValue > maxSpinBoxValue) {
//      m_currentMax_SpinBox->setValue(maxSpinBoxValue + m_incrementDelta);
//    }
//    m_minSlider->setValue(floatToSliderValue(minValue));
////  }
//}


//void
//ControlPanel::handle_spinBox_valueChanged_max(double maxValue)
//{
////  if (m_currentMax_SpinBox->value() != maxValue) {
//    m_currentMaxFloat = maxValue;
//    double minSpinBoxValue = m_currentMin_SpinBox->value();
//    if (maxValue < minSpinBoxValue) {
//      m_currentMin_SpinBox->setValue(minSpinBoxValue - m_incrementDelta);
//    }
//    m_maxSlider->setValue(floatToSliderValue(maxValue));
////  }
//}


void
ControlPanel::handle_sliderPressed()
{
  //TODO: disable textures, show bboxes
  m_renderer->setDrawNonEmptySlices(false);
  m_renderer->setDrawNonEmptyBoundingBoxes(true);
}


void
ControlPanel::handle_sliderReleased()
{
  //TODO: reenable textures, disable bboxes
  std::cout << "\nMin ROV: " << m_currentMinFloat << " Max ROV: " << m_currentMaxFloat << std::endl;
  m_renderer->setROVRange(m_currentMinFloat, m_currentMaxFloat);
  m_renderer->setDrawNonEmptySlices(true);
  m_renderer->setDrawNonEmptyBoundingBoxes(false);
}

int
ControlPanel::floatToSliderValue(double value)
{
  return static_cast<int>(value * m_incrementDelta);
}


double
ControlPanel::sliderValueToFloat(int value)
{
  return m_globalMin + (value * m_incrementDelta);
}

} // namespace subvol