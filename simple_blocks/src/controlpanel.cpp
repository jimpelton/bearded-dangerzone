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
#include <QGroupBox>
#include <QRadioButton>

namespace subvol
{

///////////////////////////////////////////////////////////////////////////////
//   ClassificatoinPanel Impl
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
ClassificationPanel::ClassificationPanel(QWidget *parent = nullptr)
    : QWidget(parent)
    , m_currentMinROVFloat{ 0.0 }
    , m_currentMaxROVFloat{ 0.0 }
    , m_globalMin{ 0.0 }
    , m_globalMax{ 0.0 }
    , m_incrementDelta{ 0 }
{
  m_groupBox = new QGroupBox("Classification Type");
  QRadioButton *averageRadio = new QRadioButton("Average");
  QRadioButton *rovRadio = new QRadioButton("ROV");

  rovRadio->setChecked(true);

  QVBoxLayout *vboxLayout = new QVBoxLayout;
  vboxLayout->addWidget(averageRadio);
  vboxLayout->addWidget(rovRadio);
  vboxLayout->addStretch(1);

  m_groupBox->setLayout(vboxLayout);
  QHBoxLayout *boxLayout = new QHBoxLayout;
  boxLayout->addWidget(m_groupBox);

  m_minSlider = new QSlider(Qt::Orientation::Horizontal);
  m_minSlider->setMinimum(0);
  m_minSlider->setMaximum(m_numberOfSliderIncrements);
  m_minSlider->setValue(0);

  m_maxSlider = new QSlider(Qt::Orientation::Horizontal);
  m_maxSlider->setMinimum(0);
  m_maxSlider->setMaximum(m_numberOfSliderIncrements);
  m_maxSlider->setValue(m_numberOfSliderIncrements);

  m_currentMin_Label = new QLabel("0");
  m_currentMax_Label = new QLabel("0");

  QWidget *gridWidget = new QWidget(this);
  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->addWidget(m_minSlider, 0,0);
  gridLayout->addWidget(m_currentMin_Label, 0, 1);
  gridLayout->addWidget(m_maxSlider, 1,0);
  gridLayout->addWidget(m_currentMax_Label, 1, 1);
  gridWidget->setLayout(gridLayout);

  boxLayout->addWidget(gridWidget);
  setLayout(boxLayout);

  connect(m_minSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_rovMinChanged(int)));

  connect(m_maxSlider, SIGNAL(valueChanged(int)),
          this, SLOT(handle_rovMaxChanged(int)));

  connect(m_minSlider, SIGNAL(sliderPressed()),
          this, SLOT(handle_sliderPressed()));

  connect(m_minSlider, SIGNAL(sliderReleased()),
          this, SLOT(handle_sliderReleased()));

  connect(m_maxSlider, SIGNAL(sliderPressed()),
          this, SLOT(handle_sliderPressed()));

  connect(m_maxSlider, SIGNAL(sliderReleased()),
          this, SLOT(handle_sliderReleased()));
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::setMinMax(double min, double max)
{
  auto floatToSliderValue = [this](double v) -> int {
    return static_cast<int>(v * this->m_incrementDelta);
  };


  m_currentMinROVFloat = min;
  m_currentMaxROVFloat = max;
  m_minSlider->setValue(floatToSliderValue(min));
  m_maxSlider->setValue(floatToSliderValue(max));
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::setGlobalRange(double newMin, double newMax)
{

  if(newMax < newMin) {
    return;
  }

  m_incrementDelta = (newMax - newMin) / m_numberOfSliderIncrements;

  m_globalMin = newMin;
  m_globalMax = newMax;
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::handle_rovMinChanged(int minSliderValue)
{
  m_currentMinROVFloat = m_globalMin + (minSliderValue * m_incrementDelta);


  if (minSliderValue > m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue + 1);
  }

  m_currentMin_Label->setText(QString::number(m_currentMinROVFloat));

  emit minRovValueChanged(m_currentMinROVFloat);

//  m_renderer->setROVRange(m_currentMinROVFloat, m_currentMaxROVFloat);

//  updateShownBlocksLabels();
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::handle_rovMaxChanged(int maxSliderValue)
{
  m_currentMaxROVFloat = m_globalMin + (maxSliderValue * m_incrementDelta);

  if (maxSliderValue < m_minSlider->value()) {
    m_minSlider->setValue(maxSliderValue - 1);
  }

  m_currentMax_Label->setText(QString::number(m_currentMaxROVFloat));

  emit maxRovValueChanged(m_currentMaxROVFloat);

//  m_renderer->setROVRange(m_currentMinROVFloat, m_currentMaxROVFloat);
//  updateShownBlocksLabels();

}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::handle_sliderPressed()
{
  emit rovChangingChanged(true);
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::handle_sliderReleased()
{
  emit rovChangingChanged(false);
}

///////////////////////////////////////////////////////////////////////////////
//   StatsPanel Impl
///////////////////////////////////////////////////////////////////////////////
StatsPanel::StatsPanel(QWidget *parent = nullptr)
{
  m_blocksShownValueLabel = new QLabel(QString::number(m_shownBlocks));
  m_blocksTotalValueLabel = new QLabel("/" + QString::number(m_totalBlocks));
  QLabel *compressionRateLabel = new QLabel("Compression: ");
  m_compressionValueLabel = new QLabel("100%");

  QHBoxLayout *blocksValueBoxLayout = new QHBoxLayout();
  blocksValueBoxLayout->addWidget(m_blocksShownValueLabel);
  blocksValueBoxLayout->addWidget(m_blocksTotalValueLabel);

  QLabel *blocksShownLabel = new QLabel("Blocks Rendered: ");

  QFormLayout *formLayout = new QFormLayout();
  formLayout->addRow(blocksShownLabel, blocksValueBoxLayout);
  formLayout->addRow(compressionRateLabel, m_compressionValueLabel);


  this->setLayout(formLayout);


}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::handle_shownBlocksChanged(size_t nblocks)
{
  m_shownBlocks = nblocks;
  updateShownBlocksLabels();
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::handle_totalBlocksChanged(size_t tblocks)
{
  m_totalBlocks = tblocks;
  updateShownBlocksLabels();
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::updateShownBlocksLabels()
{
  m_blocksShownValueLabel->setText(QString::number(m_shownBlocks));

  float p{ (1.0f - m_shownBlocks / float(m_totalBlocks)) * 100.0f };
  m_compressionValueLabel->setText(QString::asprintf("%f %%", p));
}




///////////////////////////////////////////////////////////////////////////////
//   ControlPanel Impl
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
ControlPanel::ControlPanel(BlockRenderer *renderer, QWidget *parent)
    : QWidget(parent)
    , m_totalBlocks{ 0 }
    , m_shownBlocks{ 0 }
    , m_globalMin{ 0.0 }
    , m_globalMax{ 0.0 }
    , m_renderer{ renderer }
{
  m_classificationPanel = new ClassificationPanel(this);
  m_statsPanel = new StatsPanel(this);

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(m_classificationPanel);
  layout->addWidget(m_statsPanel);

  this->setLayout(layout);


  connect(m_classificationPanel, SIGNAL(rovChangingChanged(bool)),
          this, SLOT(handle_rovChangeToggle(bool)));

}


///////////////////////////////////////////////////////////////////////////////
ControlPanel::~ControlPanel()
{
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::setGlobalRovMinMax(double min, double max)
{
  m_classificationPanel->setGlobalRange(min, max);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::setMinMax(double min, double max)
{
  m_classificationPanel->setMinMax(min, max);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::handle_rovChangeToggle(bool toggle)
{
  m_renderer->setROVChanging(toggle);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::handle_classificationChange(ClassificationType type)
{
  m_renderer->setClassificationType(type);
}


} // namespace subvol