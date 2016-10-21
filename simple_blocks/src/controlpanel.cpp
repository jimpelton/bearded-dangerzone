//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

namespace subvol
{

ControlPanel::ControlPanel(QWidget *parent)
  : QWidget(parent)
  , m_scale{ 0.1 }
  , m_minFloat{ 0.0 }
  , m_maxFloat{ 0.0 }
{
  m_minSlider = new QSlider(Qt::Orientation::Horizontal);
  m_maxSlider = new QSlider(Qt::Orientation::Horizontal);

  m_minSlider->setMinimum(0);
  m_minSlider->setMaximum(0);
  m_maxSlider->setMinimum(0);
  m_maxSlider->setMaximum(0);

  m_currentMin_Label = new QLabel("0");
  m_currentMax_Label = new QLabel("0");
  m_globalMin_Label = new QLabel("0");
  m_globalMax_Label = new QLabel("0");

  QVBoxLayout *vboxLayout = new QVBoxLayout();
  vboxLayout->addWidget(m_minSlider);
  vboxLayout->addWidget(m_maxSlider);

  setLayout(vboxLayout);

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
  m_minSlider->setMinimum(static_cast<int>(min/m_scale));
  m_maxSlider->setMinimum(static_cast<int>(min/m_scale));
  m_globalMin_Label->setText(QString::number(min*m_scale));
}


void
ControlPanel::setGlobalMax(double max)
{
  m_minSlider->setMaximum(static_cast<int>(max/m_scale));
  m_maxSlider->setMaximum(static_cast<int>(max/m_scale));
  m_globalMax_Label->setText(QString::number(max*m_scale));
}


void
ControlPanel::handle_valueChanged_min(int minSliderValue)
{

  if (minSliderValue > m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue+1);
  }
  m_currentMin_Label->setText(QString::number(minSliderValue*m_scale));

}

void
ControlPanel::handle_valueChanged_max(int maxSliderValue)
{
  if (maxSliderValue < m_minSlider->value()){
    m_minSlider->setValue(maxSliderValue-1);
  }
  m_currentMax_Label->setText(QString::number(maxSliderValue*m_scale));
}


} // namespace subvol