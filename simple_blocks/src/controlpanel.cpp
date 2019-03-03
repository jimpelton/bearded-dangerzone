//
// Created by jim on 10/20/16.
//

#include "controlpanel.h"
#include "renderer/blockrenderer.h"
#include "renderhelp.h"
#include "messages/messagebroker.h"

#include <bd/io/indexfile/indexfile.h>

#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QProgressBar>

#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

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
  gridLayout->addWidget(m_minSlider, 0, 0);
  gridLayout->addWidget(m_currentMin_Label, 0, 1);
  gridLayout->addWidget(m_maxSlider, 1, 0);
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
    return static_cast<int>(v*this->m_incrementDelta);
  };

  m_currentMinROVFloat = min;
  m_currentMaxROVFloat = max;
  m_minSlider->setValue(floatToSliderValue(min));
  m_maxSlider->setValue(floatToSliderValue(max));
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_globalRangeChanged(double newMin, double newMax)
{

  if (newMax<newMin) {
    return;
  }

  m_incrementDelta = ( newMax-newMin )/m_numberOfSliderIncrements;

  m_globalMin = newMin;
  m_globalMax = newMax;

}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_minSliderChanged(int minSliderValue)
{
  m_currentMinROVFloat = m_globalMin+( minSliderValue*m_incrementDelta );

  if (minSliderValue>m_maxSlider->value()) {
    m_maxSlider->setValue(minSliderValue+1);
  }

  m_currentMin_Label->setText(QString::number(m_currentMinROVFloat));

  MinRangeChangedMessage *m{ new MinRangeChangedMessage };
  m->Min = m_currentMinROVFloat;
  Broker::send(m);

}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_maxSliderChanged(int maxSliderValue)
{
  m_currentMaxROVFloat = m_globalMin+( maxSliderValue*m_incrementDelta );

  if (maxSliderValue<m_minSlider->value()) {
    m_minSlider->setValue(maxSliderValue-1);
  }

  m_currentMax_Label->setText(QString::number(m_currentMaxROVFloat));

  MaxRangeChangedMessage *m{ new MaxRangeChangedMessage };
  m->Max = m_currentMaxROVFloat;
  Broker::send(m);

}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_sliderPressed()
{
  ROVChangingMessage *m{ new ROVChangingMessage };
  m->IsChanging = true;
  Broker::send(m);
}


///////////////////////////////////////////////////////////////////////////////
void
ClassificationPanel::slot_sliderReleased()
{
  ROVChangingMessage *m{ new ROVChangingMessage };
  m->IsChanging = false;
  Broker::send(m);
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
StatsPanel::StatsPanel(bd::Volume const &vol,
                       size_t gpuCacheSize,
                       size_t cpuCacheSize,
                       QWidget *parent = nullptr)
    : Recipient{ "StatsPanel" }
    , m_visibleBlocks{ 0 }
    , m_currentGpuLoadQSize{ 0 }
    , m_totalMainBlocks{ cpuCacheSize }
    , m_totalGPUBlocks{ gpuCacheSize }
    , m_totalBlocks{ vol.total_block_count() }
{

  QGridLayout *gridLayout = new QGridLayout();

  QLabel *blocksShownLabel = new QLabel("Blocks Rendered: ");
  m_blocksShownValueLabel = new QLabel(QString::number(m_visibleBlocks));
  m_blocksTotalValueLabel = new QLabel("/"+QString::number(m_totalBlocks));
  QHBoxLayout *blocksValueBoxLayout = new QHBoxLayout();
  blocksValueBoxLayout->addWidget(m_blocksShownValueLabel);
  blocksValueBoxLayout->addWidget(m_blocksTotalValueLabel);

  gridLayout->addWidget(blocksShownLabel, 0, 0);
  gridLayout->addLayout(blocksValueBoxLayout, 0, 1);

  QLabel *compressionRateLabel = new QLabel("Compression: ");
  m_compressionValueLabel = new QLabel("100%");
  gridLayout->addWidget(compressionRateLabel, 1, 0);
  gridLayout->addWidget(m_compressionValueLabel, 1, 1);

  QLabel *cpuCacheFilledLabel = new QLabel("Cpu cache blocks:");
  m_cpuCacheFilledValueLabel = new QLabel();
  m_cpuCacheFilledBar = new QProgressBar();
  m_cpuCacheFilledBar->setMaximum(100);
  m_cpuCacheFilledBar->setMinimum(0);
  gridLayout->addWidget(cpuCacheFilledLabel, 2, 0);
  gridLayout->addWidget(m_cpuCacheFilledValueLabel, 2, 1);
  gridLayout->addWidget(m_cpuCacheFilledBar, 2, 2);

  QLabel *gpuCacheFilledLabel = new QLabel("Gpu resident blocks:");
  m_gpuCacheFilledValueLabel = new QLabel();
  m_gpuCacheFilledBar = new QProgressBar();
  m_gpuCacheFilledBar->setMaximum(100);
  m_gpuCacheFilledBar->setMinimum(0);
  gridLayout->addWidget(gpuCacheFilledLabel, 3, 0);
  gridLayout->addWidget(m_gpuCacheFilledValueLabel, 3, 1);
  gridLayout->addWidget(m_gpuCacheFilledBar, 3, 2);

  QLabel *cpuLoadQueueLabel = new QLabel("Cpu load queue:");
  m_cpuLoadQueueValueLabel = new QLabel();
  m_cpuLoadQueueValueBar = new QProgressBar();
  m_cpuLoadQueueValueBar->setMaximum(100);
  m_cpuLoadQueueValueBar->setMinimum(0);
  gridLayout->addWidget(cpuLoadQueueLabel, 4, 0);
  gridLayout->addWidget(m_cpuLoadQueueValueLabel, 4, 1);
  gridLayout->addWidget(m_cpuLoadQueueValueBar, 4, 2);

  QLabel *gpuLoadQueueLabel = new QLabel("Gpu load queue:");
  m_gpuLoadQueueValueLabel = new QLabel();
  m_gpuLoadQueueValueBar = new QProgressBar();
  m_gpuLoadQueueValueBar->setMaximum(100);
  m_gpuLoadQueueValueBar->setMinimum(0);
  gridLayout->addWidget(gpuLoadQueueLabel, 5, 0);
  gridLayout->addWidget(m_gpuLoadQueueValueLabel, 5, 1);
  gridLayout->addWidget(m_gpuLoadQueueValueBar, 5, 2);

  QLabel *cpuBuffersAvailLabel = new QLabel("Avail cpu buffs:");
  m_cpuBuffersAvailValueLabel = new QLabel();
  m_cpuBuffersAvailValueBar = new QProgressBar();
  m_cpuBuffersAvailValueBar->setMaximum(100);
  m_cpuBuffersAvailValueBar->setMinimum(0);
  m_cpuBuffersAvailValueBar->setValue(100);
  gridLayout->addWidget(cpuBuffersAvailLabel, 6, 0);
  gridLayout->addWidget(m_cpuBuffersAvailValueLabel, 6, 1);
  gridLayout->addWidget(m_cpuBuffersAvailValueBar, 6, 2);

  QLabel *gpuTexturesAvailLabel = new QLabel("Gpu textures avail: ");
  m_gpuTexturesAvailValueLabel = new QLabel();
  m_gpuTexturesAvailValueBar = new QProgressBar();
  m_gpuTexturesAvailValueBar->setMaximum(100);
  m_gpuTexturesAvailValueBar->setMinimum(0);
  m_gpuTexturesAvailValueBar->setValue(100);
  gridLayout->addWidget(gpuTexturesAvailLabel, 7, 0);
  gridLayout->addWidget(m_gpuTexturesAvailValueLabel, 7, 1);
  gridLayout->addWidget(m_gpuTexturesAvailValueBar, 7, 2);

  this->setLayout(gridLayout);

  connect(this, SIGNAL(updateStatsValues()),
          this, SLOT(setStatsValues()), Qt::QueuedConnection);

  connect(this, SIGNAL(updateRenderStatsValues()),
          this, SLOT(setRenderStatsValues()), Qt::QueuedConnection);

  Broker::subscribeRecipient(this);

}


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

  float p{ 0.0 };
  if (m_totalBlocks!=0) {
    p = ( 1.0f-m_visibleBlocks/float(m_totalBlocks))*100.0f;
  }

  m_compressionValueLabel->setText(QString::asprintf("%f %%", p));
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::handle_ShownBlocksMessage(ShownBlocksMessage &m)
{
  m_visibleBlocks = m.ShownBlocks;
  updateShownBlocksLabels();
//  emit updateStatsValues();
}


///////////////////////////////////////////////////////////////////////////////
void
StatsPanel::handle_BlockCacheStatsMessage(BlockCacheStatsMessage &m)
{
  m_blockCacheStatsRWLock.lockForWrite();
  m_blockCacheStats = m;
  m_blockCacheStatsRWLock.unlock();
  emit updateStatsValues();
}


void
StatsPanel::handle_SliceSetChangedMessage(SliceSetChangedMessage &)
{

}


void
StatsPanel::handle_BlockLoadedMessage(BlockLoadedMessage &m)
{
  m_blockCacheStatsRWLock.lockForWrite();
  m_currentGpuLoadQSize = m.GpuLoadQueueSize;
  m_blockCacheStatsRWLock.unlock();
  emit updateStatsValues();
}

///////////////////////////////////////////////////////////////////////////////
//void 
//StatsPanel::handle_RenderStatsMessage(RenderStatsMessage &m)
//{
//  m_renderStatsMutex.lockForWrite();
//  m_renderStatsMessage = m;
//  m_renderStatsMutex.unlock();
//  emit updateRenderStatsValues();
//}

void
StatsPanel::setStatsValues()
{
  m_blockCacheStatsRWLock.lockForRead();
  BlockCacheStatsMessage m = m_blockCacheStats;
  size_t const gpuQSize{ m_currentGpuLoadQSize };
  m_blockCacheStatsRWLock.unlock();

//  m_totalBlocks = m.MaxCpuCacheSize;
//  m_blocksTotalValueLabel->setText(QString::number(m_totalBlocks));
//  updateShownBlocksLabels();

  int const cpuCashFilledPerc = int(100*( m.CpuCacheSize/float(m_totalMainBlocks)));

  int const gpuCashFilledPerc = int(100*( m.GpuCacheSize/float(m_totalGPUBlocks)));

  m_cpuCacheFilledValueLabel->setText(QString::number(m.CpuCacheSize));
  m_cpuCacheFilledBar->setValue(cpuCashFilledPerc);

  m_gpuCacheFilledValueLabel->setText(QString::number(m.GpuCacheSize));
  m_gpuCacheFilledBar->setValue(gpuCashFilledPerc);

  m_cpuLoadQueueValueLabel->setText(QString::number(m.CpuLoadQueueSize));
  m_gpuLoadQueueValueLabel->setText(QString::number(gpuQSize));


//  m_cpuBuffersAvailValueLabel->setText(QString::number(m.CpuBuffersAvailable));
//  m_cpuBuffersAvailValueBar->setValue(100 - cpuCashFilledPerc);

//  m_gpuTexturesAvailValueLabel->setText(QString::number(m.GpuTexturesAvailable));
//  m_gpuTexturesAvailValueBar->setValue(100 - gpuCashFilledPerc);
}


void
StatsPanel::setRenderStatsValues()
{

}



///////////////////////////////////////////////////////////////////////////////
//   ControlPanel Impl
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
ControlPanel::ControlPanel(bd::Volume const &vol, size_t gpuCacheSize,
                           size_t cpuCacheSize, QWidget *parent)
    : QWidget(parent)
{
  m_classificationPanel = new ClassificationPanel(this);
  m_statsPanel = new StatsPanel(vol, gpuCacheSize, cpuCacheSize, this);

  QVBoxLayout *layout = new QVBoxLayout;

  layout->addWidget(m_classificationPanel);
  layout->addWidget(m_statsPanel);

  this->setLayout(layout);

  connect(m_classificationPanel, SIGNAL(classificationTypeChanged(ClassificationType)),
          this, SLOT(slot_classificationTypeChanged(ClassificationType)));

  connect(this, SIGNAL(globalRangeChanged(double, double)),
          m_classificationPanel, SLOT(slot_globalRangeChanged(double, double)));
}


///////////////////////////////////////////////////////////////////////////////
ControlPanel::~ControlPanel()
{
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::setGlobalRovMinMax(double min, double max)
{
  emit globalRangeChanged(min, max);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::setcurrentMinMaxSliders(double min, double max)
{
  m_classificationPanel->setMinMax(min, max);
}


///////////////////////////////////////////////////////////////////////////////
void
ControlPanel::slot_classificationTypeChanged(ClassificationType type)
{

//  auto avgCompare = [](bd::FileBlock const &lhs, bd::FileBlock const &rhs) {
//    return lhs.avg_val < rhs.avg_val;
//  };
//
//  auto rovCompare = [](bd::FileBlock const &lhs, bd::FileBlock const &rhs) {
//    return lhs.rov < rhs.rov;
//  };
//
//  double min = -1.0;
//  double max = -1.0;
//
//  switch(type) {
//    case ClassificationType::Rov: {
//      auto r = std::minmax_element(this->m_index->getFileBlocks().cbegin(),
//                                   this->m_index->getFileBlocks().cend(),
//                                   rovCompare);
//      min = r.first->rov;
//      max = r.second->rov;
//    }
//      break;
//    case ClassificationType::Avg: {
//      auto r = std::minmax_element(this->m_index->getFileBlocks().cbegin(),
//                                   this->m_index->getFileBlocks().cend(),
//                                   avgCompare);
//      min = r.first->avg_val;
//      max = r.second->avg_val;
//    }
//      break;
//    default:
//      break;
//  }
//
//  emit globalRangeChanged(min, max);

}


} // namespace subvol