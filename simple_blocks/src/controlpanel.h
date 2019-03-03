//
// Created by jim on 10/20/16.
//

#ifndef SUBVOL_CONTROLPANEL_H
#define SUBVOL_CONTROLPANEL_H

#include "renderer/blockrenderer.h"
#include "classificationtype.h"
#include "cmdline.h"
#include "messages/recipient.h"

#include <bd/io/indexfile/indexfile.h>

#include <QWidget>
//#include <QMutex>
#include <QReadWriteLock>

#include <map>

class QSlider;

class QLabel;

class QDoubleSpinBox;

class QCheckBox;

class QGroupBox;

class QProgressBar;

namespace subvol
{

//class LightingPanel : public QWidget
//{
//  Q_OBJECT
//
//public:
//  explicit LightingPanel(QWidget *parent = nullptr);
//  ~LightingPanel();
//
//signals:
//  void
//  lightPosChanged(float x, float y, float z);
//
//  void
//  materialChanged(float a, float d, float s);
//
//  void
//  nShineyChanged(float nShiney);
//
//
//
//public slots:
//  void
//  handle_X_posChanged(int sliderValue);
//
//  void
//  handle_Y_posChanged(int sliderValue);
//
//  void
//  handle_Z_posChanged(int sliderValue);
//
//private:
//
//};


class ClassificationPanel
    : public QWidget
{
Q_OBJECT

public:
  ClassificationPanel(QWidget *parent);


  ~ClassificationPanel() = default;


  void
  setMinMax(double min, double max);


signals:


  void
  minValueChanged(double rovMin);


  void
  maxValueChanged(double rovMax);


  void
  classificationTypeChanged(ClassificationType);


public slots:


  void
  slot_minSliderChanged(int minSliderValue);


  void
  slot_maxSliderChanged(int maxSliderValue);


  void
  slot_sliderPressed();


  void
  slot_sliderReleased();


  void
  slot_averageRadioClicked(bool);


  void
  slot_rovRadioClicked(bool);


  void
  slot_globalRangeChanged(double rmax, double rmin);


private:
  QGroupBox *m_groupBox;

  QSlider *m_minSlider;
  QSlider *m_maxSlider;
  QLabel *m_currentMin_Label;
  QLabel *m_currentMax_Label;

  double m_currentMinROVFloat;
  double m_currentMaxROVFloat;
  double m_globalMin;
  double m_globalMax;
  double m_incrementDelta;

  static const int m_numberOfSliderIncrements = 100000;

};

class StatsPanel : public QWidget, public subvol::Recipient
{
Q_OBJECT

public:
  StatsPanel(bd::Volume const &,
             size_t gpuCacheSize,
             size_t cpuCacheSize,
             QWidget *parent);


  ~StatsPanel() = default;


public:
  void
  slot_minRovValueChanged(double minrov);


  void
  slot_maxRovValueChanged(double maxrov);


  void
  slot_classificationTypeChanged(ClassificationType type);


  void
  handle_ShownBlocksMessage(ShownBlocksMessage &) override;


  void
  handle_BlockCacheStatsMessage(BlockCacheStatsMessage &) override;


  void
  handle_SliceSetChangedMessage(SliceSetChangedMessage &) override;


  void
  handle_BlockLoadedMessage(BlockLoadedMessage &) override;


signals:


  void
  updateStatsValues();


  void
  updateRenderStatsValues();


private:
  void
  updateShownBlocksLabels();


private slots:


  void
  setStatsValues();


  void
  setRenderStatsValues();


private:
  QLabel *m_blocksShownValueLabel;
  QLabel *m_blocksTotalValueLabel;
  QLabel *m_compressionValueLabel;

  QLabel *m_cpuCacheFilledValueLabel;
  QProgressBar *m_cpuCacheFilledBar;

  QLabel *m_gpuCacheFilledValueLabel;
  QProgressBar *m_gpuCacheFilledBar;

  QLabel *m_cpuLoadQueueValueLabel;
  QProgressBar *m_cpuLoadQueueValueBar;

  QLabel *m_gpuLoadQueueValueLabel;
  QProgressBar *m_gpuLoadQueueValueBar;

  QLabel *m_cpuBuffersAvailValueLabel;
  QProgressBar *m_cpuBuffersAvailValueBar;
  QLabel *m_gpuTexturesAvailValueLabel;
  QProgressBar *m_gpuTexturesAvailValueBar;

  size_t m_visibleBlocks;
  size_t m_currentGpuLoadQSize;

  size_t const m_totalBlocks;
  size_t const m_totalMainBlocks;
  size_t const m_totalGPUBlocks;

  QReadWriteLock m_blockCacheStatsRWLock;
  BlockCacheStatsMessage m_blockCacheStats;

  QReadWriteLock m_renderStatsMutex;
};

///////////////////////////////////////////////////////////////////////////////
class ControlPanel
    : public QWidget
{
Q_OBJECT

public:
  explicit ControlPanel(bd::Volume const &clo,
                        size_t gpuCacheSize,
                        size_t cpuCacheSize,
                        QWidget *parent = nullptr);


  ~ControlPanel();


  void
  setGlobalRovMinMax(double min, double max);


  void
  setcurrentMinMaxSliders(double min, double max);


signals:


  void
  globalRangeChanged(double, double);


public slots:


  void
  slot_classificationTypeChanged(ClassificationType type);


private:

  ClassificationPanel *m_classificationPanel;
  StatsPanel *m_statsPanel;

};

}

#endif //SUBVOL_CONTROLPANEL_H

