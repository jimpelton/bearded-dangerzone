//
// Created by jim on 10/20/16.
//

#ifndef SUBVOL_CONTROLPANEL_H
#define SUBVOL_CONTROLPANEL_H

#include "blockrenderer.h"
#include "classificationtype.h"

#include <bd/io/indexfile.h>

#include <QWidget>

class QSlider;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;

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


class ClassificationPanel : public QWidget
{
  Q_OBJECT


public:
  ClassificationPanel(QWidget *parent);
  ~ClassificationPanel() = default;


  void
  setMinMax(double min, double max);

  void
  setGlobalRange(double rmax, double rmin);

signals:
  void minValueChanged(double rovMin);
  void maxValueChanged(double rovMax);
  void rovChangingChanged(bool toggle);
  void classificationTypeChanged(ClassificationType);

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


class StatsPanel : public QWidget
{
  Q_OBJECT

public:
  StatsPanel(QWidget *parent);
  ~StatsPanel() = default;

//signals:

public slots:
  void
  slot_visibleBlocksChanged(size_t numblk);

  void
  slot_totalBlocksChanged(size_t t);

  void
  slot_minRovValueChanged(double minrov);

  void
  slot_maxRovValueChanged(double maxrov);

  void
  slot_classificationTypeChanged(ClassificationType type);

private:
  void
  updateShownBlocksLabels();

  QLabel *m_blocksShownValueLabel;
  QLabel *m_blocksTotalValueLabel;
  QLabel *m_compressionValueLabel;


  size_t m_shownBlocks;
  size_t m_totalBlocks;


};


///////////////////////////////////////////////////////////////////////////////
class ControlPanel : public QWidget
{
  Q_OBJECT

public:
  explicit ControlPanel(BlockRenderer *renderer,
                        BlockCollection *collection,
                        std::shared_ptr<const bd::IndexFile> indexFile,
                        QWidget *parent = nullptr);

  ~ControlPanel();


  void
  setGlobalRovMinMax(double min, double max);


  void
  setMinMax(double min, double max);

  void
  setVisibleBlocks(size_t visibleBlocks);

signals:


public slots:
  void
  slot_rovChangingChanged(bool toggle);

  void
  slot_classificationTypeChanged(ClassificationType type);

  void
  slot_minMaxChange(double min, double max);


private:

  unsigned long long m_totalBlocks;
  unsigned long long m_shownBlocks;
  double m_globalMin;
  double m_globalMax;


//  QDoubleSpinBox *m_currentMin_SpinBox;
//  QDoubleSpinBox *m_currentMax_SpinBox;
//  QLabel *m_globalMin_Label;
//  QLabel *m_globalMax_Label;



  ClassificationPanel *m_classificationPanel;
  StatsPanel *m_statsPanel;

  BlockRenderer *m_renderer;
  std::shared_ptr<const bd::IndexFile> m_index;

};

}

#endif //SUBVOL_CONTROLPANEL_H
