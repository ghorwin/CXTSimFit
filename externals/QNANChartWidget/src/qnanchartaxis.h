/*******************************************************************************

QNANWidgets - Easy-to-use widgets for scientific and engineering applications
Copyright (C) 2007 Andreas Nicolai

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*******************************************************************************/

#ifndef qnanchartaxis_h
#define qnanchartaxis_h

#include <QObject>
#include <QString>
#include <QFontMetricsF>
class QDomDocument;
class QDomElement;
class QDomNode;

/// @brief Encapsulates all data of a single chart axis.
/// @author Andreas Nicolai
///
/// This class encapsulates all properties and functionality of one of the four chart axes.
/// Most useful for the user are the functions setLabel(), setMinVal() and setMaxVal().
/// The example source code below shows how a chart axis can be best customized.
/// @code
/// // customize the left y axis
/// chart->y1Axis()->setLabel(tr("Temperature [C]"));
/// chart->y1Axis()->setMinMaxValues(-10, 100);
/// // customize the right y axis, first we need to make it visible
/// chart->setY2AxisVisible(true);
/// chart->y2Axis()->setLabel(tr("Temperature [F]"));
/// chart->y2Axis()->setMinMaxValues(14, 212);
/// @endcode
///
/// If any of the axis properties are changed, the signal axisChanged() is emitted. Normally
/// the chart widget connects to this signal and triggers an update of the chart.
///
/// The axis has the ability to automatically select suitable min and max values, as well as
/// tick mark distances based on the data shown by the axes. This feature can be controlled
/// via the automaticScaling property.
///
/// Zooming and panning of the chart is implemented via zooming and panning of the axis
/// distances. See zoom() and pan() for details. These actions also emit the corresponding
/// signals so that users can connect to these signals, perhaps for synchronizing charts
/// as shown in the example below.
/// @code
/// // first connect to the axis zoom signal of the two charts
/// connect(chart1->x1Axis(), SIGNAL(zoomed(double, double)), this, SLOT(xAxisZoomed(double,double)));
/// connect(chart2->x1Axis(), SIGNAL(zoomed(double, double)), this, SLOT(xAxisZoomed(double,double)));
/// // ...
/// // now the implementation of the slot
/// void MyClass::xAxisZoomed(double newMin, double newMax) {
///     // first disconnect the signals of the charts to avoid feedback
///     disconnect(chart1->x1Axis(), SIGNAL(zoomed(double, double)));
///     disconnect(chart2->x1Axis(), SIGNAL(zoomed(double, double)));
///     // now set the zoom for the appropriate other axis
///     chart1->x1Axis()->zoom(newMin, newMax);
///     chart2->x1Axis()->zoom(newMin, newMax);
///     // and connect the signals again
///     connect(chart1->x1Axis(), SIGNAL(zoomed(double, double)), this, SLOT(xAxisZoomed(double,double)));
///     connect(chart2->x1Axis(), SIGNAL(zoomed(double, double)), this, SLOT(xAxisZoomed(double,double)));
/// }
/// @endcode
class QNANChartAxis : public QObject {
	Q_OBJECT
public:
	/// @brief Constructor (creates a chart axis as child object of the chart widget).
	QNANChartAxis(QObject * parent);
	/// @brief Destructor.
	virtual ~QNANChartAxis() {}

	/// @brief Returns the current axis label.
	virtual QString label() const { return m_label; }
	/// @brief Sets the axis label.
	virtual void setLabel(const QString& s);

	/// @brief Recalculates the axis scaling (tick mark distances etc).
	///
	/// This function is a wrapper function for the calculateNiceMinMaxValues() function
	/// that contains the implements the automatic axis scaling algorithm.
	/// @param minval 		The minimum value in plot units.
	/// @param maxval 		The maximum value in plot units.
	/// @param length		The axis length in pixels.
	/// @param fm 			The font metrics for the tick label font.
	/// @param aligned		The alignment of the labels with the axis
	///                     (true - if aligned with the axis, like for a horizontal axis, false - if perpendicular to the axis)
	virtual void updateAxisScaling(double minval, double maxval, double length,
		const QFontMetricsF& fm, bool aligned);

	/// @brief Copies the properties and scaling data from another axis.
	/// This function is used for synchronizing the ticks of the secondary x or y axis to the ticks of the primary axes.
	void copyFrom(const QNANChartAxis& axis);

	/// @brief Returns whether this axis uses automatic axis scaling.
	bool automaticScaling() const { return m_automaticScaling; }
	/// @brief Sets the automatic axis scaling property of the axis.
	void setAutomaticScaling(bool autoscale);
	/// @brief Returns the number of ticks of the axis.
	int numTicks() const { return m_numTicks; }
	/// @brief Returns the minimum value of the axis.
	double minVal() const { return m_minVal; }
	/// @brief Sets the minimum value.
	/// If autoscaling is turned off, this function emits the axisChanged()
	/// signal which triggers an update of the chart.
	void setMinVal(double minval);
	/// @brief Returns the maximum value.
	double maxVal() const { return m_maxVal; }
	/// @brief Sets the maximum value.
	/// If autoscaling is turned off, this function emits the axisChanged()
	/// signal which triggers an update of the chart.
	/// @see setMinMaxValues()
	void setMaxVal(double maxval);
	/// @brief Sets the minimum and maximum values at the same time.
	/// This function adjusts the axis limits and if autoscaling is turned off,
	/// it emits the axisChanged() signal which triggers an
	/// update of the chart. When the minima and maxima of the axis should be
	/// set simultaneously, using this function is more efficient than using
	/// the setMaxVal() and setMinVal() functions separately because only
	/// one update is done.
	/// @see setMinVal()
	/// @see setMaxVal()
	void setMinMaxValues(double minval, double maxval);
	/// @brief Returns the tick mark distance in plot units.
	double tickMarkDistance() const { return m_tickMarkDistance; }
	/// @brief Sets the tick mark distance in plot units.
	/// If the data to be plotted is in the range of 2 to 10 meters, the tick mark distance could,
	/// for example, be set to 0.5 meters. The user defined tick mark distance is only used
	/// if the automaticTickMarkDistance property is off.
	void setTickMarkDistance(double step);
	/// @brief Returns, wether the tick mark distance is automatically calculated or the
	/// user defined tick mark distance is used.
	/// @see setTickMarkDistance()
	bool automaticTickMarkDistance() const { return m_automaticTickMarkDistance; }
	/// @brief Turns the automatic tick mark distance calculation function on/off.
	void setAutomaticTickMarkDistance(bool on);

	/// @brief Returns the 'nice' minimum value.
	/// The nice minimum value can differ from the minimum value if automatic scaling is turned off
	/// and the user defined min value is not a 'nice' value. For instance, the user defined min
	/// value could be 0.127 and the calculated nice min value could be 0.13 or 0.15, depending on
	/// the value range.
	double niceMinVal() const { return m_niceMinVal; }
	/// @brief Returns the 'nice' maximum value.
	/// See niceMinVal() for an explanation of the meaning of a 'nice' value.
	double niceMaxVal() const { return m_niceMaxVal; }

	/// @brief Returns the number format to be used for the axis.
	/// @see QString::arg()
	char numberFormat() const { return m_numberFormat; }
	/// @brief Sets the number format to be used for the axis.
	/// @see QString::arg()
	void setNumberFormat(char f);
	/// @brief Returns the precision to be used for the axis.
	/// @see QString::arg()
	int precision() const { return m_precision; }
	/// @brief Sets the precision to be used for the axis.
	/// @see QString::arg()
	void setPrecision(int p);
	/// @brief Loads the chart axis data from an XML document.
	void read(QDomElement in);
	/// @brief Saves the chart axis data into an XML document.
	void write(const QString& axisName, QDomDocument& doc, QDomElement& root) const;

	/// @brief Zooms the axis to the new given min and max values.
	/// If the new min value is larger than the new max value, the function will zoom out
	/// to the automatically calculated maximum values (also, the axis scaling property
	/// will be set to maximum).
	/// If the axis is zoomed in (newMin less than newMax), the automatic axis scaling property is set
	/// to false.
	/// This function emits the signal zoomed().
	/// @see zoomed()
	void zoom(double newMin, double newMax);

	/// @brief Moves the axis by the distance dist.
	/// This function sets the automatic axis scaling property to false and adjusts the min and max values
	/// according to the distance 'dist'.
	/// This function emits the signal panned().
	/// @see panned()
	void pan(double dist);

	/// Creates a tick mark label.
	/// @param val 			The value to be written as tick mark.
	/// @param other_val 	A differnt value of the usual order of magnitude (use for instance a maximum value)
	/// @param f			The format, can be 'g', 'e', or 'f'.
	/// @param p			The precision.
	static QString tickMarkLabel(double val, double other_val, char f, int p);

signals:
	/// @brief This signal is emitted whenever a property of the axis has been changed.
	void axisChanged();

	/// @brief This signal is emitted whenever the axis was changed due to a call to zoom().
	/// @see zoom()
	void zoomed(double newMin, double newMax);

	/// @brief This signal is emitted whenever the axis was changed due to a call to pan().
	/// @see pan()
	void panned(double dist);

protected:
	/// @brief Calculates nice min/max values.
	///
	/// The nice min/max values are calculated based on the current values
	/// in m_minVal and m_maxVal and the flag m_automaticScaling.
	/// Overload this function in a derived class to implement a user defined scaling engine.
	/// @param length		The available length of the axis.
	/// @param fm 			The font metrics for the tick label font.
	/// @param alignment	The alignment of the labels with the axis
	///                     (true - if aligned with the axis, like for an horizontal axis, false - if perpendicular to the axis)
	virtual void calculateNiceMinMaxValues(double length, const QFontMetricsF& fm, bool aligned);

	QString	m_label;						///< The current axis label.

	bool	m_automaticScaling;				///< Automatically determine min/max values.
	bool	m_automaticTickMarkDistance;	///< If true, the tick mark distance is automatically determined.

	double 	m_minVal;						///< Minimum value in plot units.
	double 	m_maxVal;						///< Maximum value in plot units.

	char	m_numberFormat;					///< Number format for axis tick mark labels.
	int		m_precision;					///< Precision for axis tick mark labels.

	double 	m_niceMinVal;					///< Nice minimum value in plot units.
	double 	m_niceMaxVal;					///< Nice maximum value in plot units.
	double	m_tickMarkDistance;				///< Tick mark distance in plot units.
	int		m_numTicks;						///< Number of ticks to be drawn on the axis.
};

#endif // qnanchartaxis_h
