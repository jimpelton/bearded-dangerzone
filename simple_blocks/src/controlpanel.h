//
// Created by jim on 10/20/16.
//

#ifndef SUBVOL_CONTROLPANEL_H
#define SUBVOL_CONTROLPANEL_H

#include "blockrenderer.h"

#include <QWidget>

class QSlider;
class QLabel;
class QDoubleSpinBox;

namespace subvol
{

class ControlPanel
    : public QWidget
{
  Q_OBJECT

public:
  explicit ControlPanel(BlockRenderer *renderer, QWidget *parent = nullptr);

  ~ControlPanel();

  void
  setGlobalRange(double, double newMax);

  void
  setMinMax(double min, double max);

signals:
  void
  min_valueChanged(int value);
  void
  max_valueChanged(int value);

public slots:
  void
  handle_valueChanged_min(int minSliderValue);
  void
  handle_valueChanged_max(int maxSliderValue);
//  void
//  handle_spinBox_valueChanged_min(double minValue);
//  void
//  handle_spinBox_valueChanged_max(double maxValue);
  void
  handle_sliderPressed();
  void
  handle_sliderReleased();

private:
  int
  floatToSliderValue(double value);

  double
  sliderValueToFloat(int value);

  int m_numberOfSliderIncrements;
  double m_incrementDelta;
  double m_currentMinFloat;
  double m_currentMaxFloat;
  double m_globalMin;
  double m_globalMax;

  QSlider *m_minSlider;
  QSlider *m_maxSlider;

//  QDoubleSpinBox *m_currentMin_SpinBox;
//  QDoubleSpinBox *m_currentMax_SpinBox;
  QLabel *m_currentMin_Label;
  QLabel *m_currentMax_Label;
  QLabel *m_globalMin_Label;
  QLabel *m_globalMax_Label;

  QLabel *m_blocksShownValueLabel,
    *m_blocksTotalValueLabel,
    *m_compressionValueLabel;


  BlockRenderer *m_renderer;

};

}

#endif //SUBVOL_CONTROLPANEL_H
