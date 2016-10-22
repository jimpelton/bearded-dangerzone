//
// Created by jim on 10/20/16.
//

#ifndef SUBVOL_CONTROLPANEL_H
#define SUBVOL_CONTROLPANEL_H

#include <QWidget>

class QSlider;
class QLabel;

namespace subvol
{

class ControlPanel
    : public QWidget
{
  Q_OBJECT

public:
  explicit ControlPanel(QWidget *parent = 0);


  ~ControlPanel();

  void
  setGlobalMin(double);
  void
  setGlobalMax(double);

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

private:
  int m_numberOfSliderIncrements;
  double m_minFloat;
  double m_maxFloat;

  QSlider *m_minSlider;
  QSlider *m_maxSlider;

  QLabel *m_globalMin_Label;
  QLabel *m_globalMax_Label;
  QLabel *m_currentMin_Label;
  QLabel *m_currentMax_Label;


};

}

#endif //SUBVOL_CONTROLPANEL_H
