//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"
#include "blockrenderer.h"

#include <bd/io/indexfile.h>

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
          this, SLOT(slot_minSliderChanged(int)));

  connect(m_maxSlider, SIGNAL(valueChanged(int)),
          this, SLOT(slot_maxSliderChanged(int)));

  connect(m_minSlider, SIGNAL(sliderPressed()),
          this, SLOT(slot_sliderPressed()));

  connect(m_minSlider, SIGNAL(sliderReleased()),
          this, SLOT(slot_sliderReleased()));

  connect(m_maxSlider, SIGNAL(sliderPressed()),
          this, SLOT(slot_sliderPressed()));

  connect(m_maxSlider, SIGNAL(sliderReleased()),
          this, SLOT(slot_sliderReleased()));

  connect(averageRadio, SIGNAL(clicked(bool)),
          this, SLOT(slot_averageRadioClicked(bool)));

  connect(rovRadio, SIGNAL(clicked(bool)),
          this, SLOT(slot_rovRadioClicked(bool)));
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
ClassificationPanel::slot_minSliderChanged(int minSliderValue)
{
  m_currentMinROVFloat = m_globalMin + (minSliderValue * m_incrementDelta);


  if (minSliderValue > m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue + 1);
  }

  m_currentMin_Label->setText(QString::number(m_currentMinROVFloat));

  emit minValueChanged(m_currentMinROVFloat);

//  m_renderer->setROVRange(m_currentMinROVFloat, m_currentMaxROVFloat);

//  updateShownBlocksLabels();
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_maxSliderChanged(int maxSliderValue)
{
  m_currentMaxROVFloat = m_globalMin + (maxSliderValue * m_incrementDelta);

  if (maxSliderValue < m_minSlider->value()) {
    m_minSlider->setValue(maxSliderValue - 1);
  }

  m_currentMax_Label->setText(QString::number(m_currentMaxROVFloat));

  emit maxValueChanged(m_currentMaxROVFloat);

//  m_renderer->setROVRange(m_currentMinROVFloat, m_currentMaxROVFloat);
//  updateShownBlocksLabels();

}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_sliderPressed()
{
  emit rovChangingChanged(true);
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_sliderReleased()
{
  emit rovChangingChanged(false);
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_averageRadioClicked(bool)
{
  emit classificationTypeChanged(ClassificationType::Avg);
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_rovRadioClicked(bool)
{
  emit classificationTypeChanged(ClassificationType::Rov);
}


///////////////////////////////////////////////////////////////////////////////
//   StatsPanel Impl
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
StatsPanel::StatsPanel(size_t totalBlocks,
                       QWidget *parent = nullptr )
    : m_totalBlocks{ totalBlocks }
{
  m_blocksShownValueLabel = new QLabel(QString::number(m_visibleBlocks));
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
StatsPanel::slot_visibleBlocksChanged(unsigned int numblk)
{
  m_visibleBlocks = numblk;
  updateShownBlocksLabels();
}


///////////////////////////////////////////////////////////////////////////////
//void
//StatsPanel::slot_totalBlocksChanged(size_t t)
//{
//  m_totalBlocks = t;
//  updateShownBlocksLabels();
//}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::slot_minRovValueChanged(double minrov)
{
  std::cout << __PRETTY_FUNCTION__ << " " << minrov << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::slot_maxRovValueChanged(double maxrov)
{
  std::cout << __PRETTY_FUNCTION__ << " " << maxrov << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::slot_classificationTypeChanged(ClassificationType type)
{
  std::cout << __PRETTY_FUNCTION__ << " " << type << std::endl;
}
///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::updateShownBlocksLabels()
{
  m_blocksShownValueLabel->setText(QString::number(m_visibleBlocks));

  float p{ (1.0f - m_visibleBlocks / float(m_totalBlocks)) * 100.0f };
  m_compressionValueLabel->setText(QString::asprintf("%f %%", p));
}



///////////////////////////////////////////////////////////////////////////////
//   ControlPanel Impl
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
ControlPanel::ControlPanel(BlockRenderer *renderer,
                           BlockCollection *collection,
                           std::shared_ptr<const bd::IndexFile> indexFile,
                           QWidget *parent)
    : QWidget(parent)
    , m_totalBlocks{ 0 }
    , m_shownBlocks{ 0 }
    , m_globalMin{ 0.0 }
    , m_globalMax{ 0.0 }
    , m_renderer{ renderer }
    , m_collection{ collection }
    , m_index{ std::move(indexFile) }
{
  // oh my god! such hack!
  std::function<void(size_t)> vb(
      [this](size_t b) -> void {
        emit shownBlocksChanged((unsigned int)b);
      });
  collection->setVisibleBlocksCallback(vb);

  m_classificationPanel = new ClassificationPanel(this);
  m_statsPanel = new StatsPanel(collection->getTotalNumBlocks(), this);

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(m_classificationPanel);
  layout->addWidget(m_statsPanel);

  this->setLayout(layout);


  connect(m_classificationPanel, SIGNAL(rovChangingChanged(bool)),
          this, SLOT(slot_rovChangingChanged(bool)));

  connect(m_classificationPanel, SIGNAL(classificationTypeChanged(ClassificationType)),
          this, SLOT(slot_classificationTypeChanged(ClassificationType)));

  connect(m_classificationPanel, SIGNAL(minValueChanged(double)),
          this, SLOT(slot_minValueChanged(double)));

  connect(m_classificationPanel, SIGNAL(maxValueChanged(double)),
          this, SLOT(slot_maxValueChanged(double)));

  connect(this, SIGNAL(shownBlocksChanged(unsigned int)),
          m_statsPanel, SLOT(slot_visibleBlocksChanged(unsigned int)));


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
ControlPanel::setcurrentMinMaxSliders(double min, double max)
{
  m_classificationPanel->setMinMax(min, max);
}


///////////////////////////////////////////////////////////////////////////////
//void
//ControlPanel::setVisibleBlocks(size_t visibleBlocks)
//{
//  m_statsPanel->slot_visibleBlocksChanged(visibleBlocks);
//}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::slot_rovChangingChanged(bool toggle)
{
  m_renderer->setROVChanging(toggle);
}



///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::slot_classificationTypeChanged(ClassificationType type)
{
  m_collection->changeClassificationType(type);

  auto avgCompare = [](bd::FileBlock const &lhs, bd::FileBlock const &rhs) {
    return lhs.avg_val < rhs.avg_val;
  };

  auto rovCompare = [](bd::FileBlock const &lhs, bd::FileBlock const &rhs) {
    return lhs.rov < rhs.rov;
  };

  double min = -1.0;
  double max = -1.0;

  switch(type) {
    case ClassificationType::Rov: {
      auto r = std::minmax_element(this->m_index->getFileBlocks().cbegin(),
                                   this->m_index->getFileBlocks().cend(),
                                   rovCompare);
      min = r.first->rov;
      max = r.second->rov;
    }
      break;
    case ClassificationType::Avg: {
      auto r = std::minmax_element(this->m_index->getFileBlocks().cbegin(),
                                   this->m_index->getFileBlocks().cend(),
                                   avgCompare);
      min = r.first->avg_val;
      max = r.second->avg_val;
    }
      break;
    default:
      break;
  }

  m_classificationPanel->setGlobalRange(min, max);

}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::slot_minValueChanged(double min)
{
  m_collection->setRangeMin(min);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::slot_maxValueChanged(double max)
{
  m_collection->setRangeMax(max);
}

} // namespace subvol