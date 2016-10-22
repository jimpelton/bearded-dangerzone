//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QtWidgets/QAbstractSlider>
#include <cassert>

namespace subvol
{

ControlPanel::ControlPanel(QWidget *parent)
  : QWidget(parent)
  , m_numberOfSliderIncrements{ 100 }
  , m_minFloat{ 0.0 }
  , m_maxFloat{ 0.0 }
{
  m_minSlider = new QSlider(Qt::Orientation::Horizontal);
  m_maxSlider = new QSlider(Qt::Orientation::Horizontal);

  m_minSlider->setMinimum(0);
  m_minSlider->setMaximum(m_numberOfSliderIncrements);
  m_maxSlider->setMinimum(0);
  m_maxSlider->setMaximum(m_numberOfSliderIncrements);

  m_currentMin_Label = new QLabel("0");
  m_currentMax_Label = new QLabel("0");
  m_globalMin_Label = new QLabel("0");
  m_globalMax_Label = new QLabel("0");

  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->addWidget(m_minSlider, 0,0);
  gridLayout->addWidget(m_currentMin_Label, 0, 1);
  gridLayout->addWidget(m_maxSlider, 1,0);
  gridLayout->addWidget(m_currentMax_Label, 1, 1);


  setLayout(gridLayout);


  connect(m_minSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_valueChanged_min(int)));

  connect(m_maxSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_valueChanged_max(int)));

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
ControlPanel::setGlobalMin(double min)
{
  assert(false);
//  m_minSlider->setMinimum(static_cast<int>(min/m_numberOfSliderIncrements));
//  m_maxSlider->setMinimum(static_cast<int>(min/m_numberOfSliderIncrements));
//  m_globalMin_Label->setText(QString::number(min*m_numberOfSliderIncrements));
}


void
ControlPanel::setGlobalMax(double max)
{
  assert(false);
//  m_minSlider->setMaximum(static_cast<int>(max/m_numberOfSliderIncrements));
//  m_maxSlider->setMaximum(static_cast<int>(max/m_numberOfSliderIncrements));
//  m_globalMax_Label->setText(QString::number(max*m_numberOfSliderIncrements));
}


void
ControlPanel::handle_valueChanged_min(int minSliderValue)
{

  if (minSliderValue > m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue+1);
  }
  m_currentMin_Label->setText(QString::number(minSliderValue*m_numberOfSliderIncrements));

}

void
ControlPanel::handle_valueChanged_max(int maxSliderValue)
{
  if (maxSliderValue < m_minSlider->value()){
    m_minSlider->setValue(maxSliderValue-1);
  }
  m_currentMax_Label->setText(QString::number(maxSliderValue*m_numberOfSliderIncrements));
}


} // namespace subvol