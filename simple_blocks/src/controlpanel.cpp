//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"
#include "blockrenderer.h"

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QFormLayout>


namespace subvol
{

ControlPanel::ControlPanel(BlockRenderer *renderer, QWidget *parent)
    : QWidget(parent)
    , m_numberOfSliderIncrements{ 1000 }
    , m_totalBlocks{ 0 }
    , m_shownBlocks{ 0 }
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


  QLabel *blocksShownLabel = new QLabel("Blocks Rendered: ");
  m_totalBlocks = renderer->getNumBlocks();
  m_shownBlocks = renderer->getNumBlocksShown();
  m_blocksShownValueLabel = new QLabel(QString::number(m_shownBlocks));
  m_blocksTotalValueLabel = new QLabel("/" + QString::number(m_totalBlocks));
  QLabel *compressionRateLabel = new QLabel("Compression: ");
  m_compressionValueLabel = new QLabel(QString::number(m_shownBlocks/float(m_totalBlocks)) + "%");

//  m_globalMin_Label = new QLabel("0");
//  m_globalMax_Label = new QLabel("0");
  m_currentMin_Label = new QLabel("0");
  m_currentMax_Label = new QLabel("0");


  QHBoxLayout *blocksValueBoxLayout = new QHBoxLayout();
  blocksValueBoxLayout->addWidget(m_blocksShownValueLabel);
  blocksValueBoxLayout->addWidget(m_blocksTotalValueLabel);

  QFormLayout *formLayout = new QFormLayout();
  formLayout->addRow(blocksShownLabel, blocksValueBoxLayout);
  formLayout->addRow(compressionRateLabel, m_compressionValueLabel);

  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->addWidget(m_minSlider, 0,0);
  gridLayout->addWidget(m_currentMin_Label, 0, 1);
  gridLayout->addWidget(m_maxSlider, 1,0);
  gridLayout->addWidget(m_currentMax_Label, 1, 1);
  gridLayout->addItem(formLayout, 2, 0, 1, 2);

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

}

ControlPanel::~ControlPanel()
{
}



void
ControlPanel::setGlobalRange(double newMin, double newMax)
{

  if(newMax < newMin) {
    return;
  }

  m_incrementDelta = (newMax - newMin) / m_numberOfSliderIncrements;

  m_globalMin = newMin;
  m_globalMax = newMax;

//  m_globalMin_Label->setText(QString::number(newMin));
//  m_globalMax_Label->setText(QString::number(newMax));

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
  m_currentMinFloat = sliderValueToFloat(minSliderValue);

  if (minSliderValue > m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue + 1);
  }

  m_currentMin_Label->setText(QString::number(m_currentMinFloat));
  m_renderer->setROVRange(m_currentMinFloat, m_currentMaxFloat);

  updateShownBlocksLabels();
}


void
ControlPanel::handle_valueChanged_max(int maxSliderValue)
{
  m_currentMaxFloat = sliderValueToFloat(maxSliderValue);

  if (maxSliderValue < m_minSlider->value()) {
    m_minSlider->setValue(maxSliderValue - 1);
  }

  m_currentMax_Label->setText(QString::number(m_currentMaxFloat));
  m_renderer->setROVRange(m_currentMinFloat, m_currentMaxFloat);
  updateShownBlocksLabels();

}


void
ControlPanel::handle_sliderPressed()
{
  m_renderer->setROVChanging(true);
}


void
ControlPanel::handle_sliderReleased()
{
  std::cout << "\nMin ROV: " << m_currentMinFloat << " Max ROV: " << m_currentMaxFloat << std::endl;
  m_renderer->setROVChanging(false);

//TODO:  m_renderer->updateCache();
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


void
ControlPanel::updateShownBlocksLabels()
{
  m_shownBlocks = m_renderer->getNumBlocksShown();
  m_blocksShownValueLabel->setText(QString::number(m_shownBlocks));

  float p{ (1.0f - m_shownBlocks / float(m_totalBlocks)) * 100.0f };
  m_compressionValueLabel->setText(QString::asprintf("%f %%", p));
}

} // namespace subvol