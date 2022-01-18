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

#ifndef qnanabstractchartseries_h
#define qnanabstractchartseries_h

#include <QSize>
#include <QRect>
#include <QList>
#include <QString>
#include <QObject>

class QPainter;
class QDomDocument;
class QDomElement;
class QDomNode;

/// @brief Abstract base class for all chart series used to plot curves.
///
/// The chart widget can handle abstract chart series and all derived series.
/// Derive from QNANAbstractChartSeries and reimplement your own chart series
/// class to support additional drawing capabilities (like error bars, bands etc).
///
/// A chart series needs X-Y data points to be plotted. The QNANAbstractChartSeries
/// can handle this in two ways. You can pass the values which are then copied into
/// the data series and stored there or you can pass pointers to memory arrays
/// containg the x and y data arrays. For this purpose two overloads of the function
/// setData() are provided.
///
/// If the data that is used to plot the chart is no longer available, make sure to
/// call clear() first.
class QNANAbstractChartSeries : public QObject {
	Q_OBJECT
public:
	/// @brief Datatype for the storage member.
	typedef QList<double>	DataVec;

	/// @brief Default constructor.
	QNANAbstractChartSeries(QObject * parent=0);
	/// @brief Virtual destructor, so that destructors of child classes will be called.
	virtual ~QNANAbstractChartSeries() {}

	/// @brief Returns the series ID. 
	/// Every chart series has a unique ID, which is also shown in the chart series
	/// edit dialog.
	int id() const { return m_id; }
	/// Sets the series ID.
	/// @see id()
	void setId(int id);
	/// @brief Returns the name of the series.
	/// The name of a chart series is used to describe its data and can be
	/// used in a legend.
	QString name() const { return m_name; }
	/// Sets the name of the sereis.
	void setName(const QString& name);

	/// @brief Returns which x-axis the series uses.
	int xAxis() const { return m_xAxis; }
	/// Sets which x-axis the series uses.
	/// @param axis Either 0 for x1 or 1 for x2 axis.
	void setXAxis(int axis);
	
	/// @brief Returns which y-axis the series uses.
	int	yAxis() const { return m_yAxis; }
	/// @brief Sets which y-axis the series uses.
	/// @param axis Either 0 for y1 or 1 for y2 axis.
	void setYAxis(int axis);

	/// Empties the data of the chart series.
	void clear();

	/// @brief Sets the series data.
	/// This function stores the data in the interal storage of the
	/// chart series. Both vectors must have the same length and must 
	/// contain at least two values each. If they don't, the series
	/// data will be cleared so that the series won't contain any data.
	/// The data will checked using checkData() for invalid values like
	/// QINF or QNAN which would potentially crash the chart engine.
	/// If invalid values are found the chart data will not be accepted.
	void setData(const DataVec& x_points, const DataVec& y_points);
	/// @brief Sets the series data.
	/// This is an overloaded function that supports the alternative
	/// data storage for the series data. The series remembers the
	/// pointers to the data, but that data does not become owned
	/// by the series (so the user must clean up that memory itself).
	/// The data will checked using checkData() for invalid values like
	/// QINF or QNAN which would potentially crash the chart engine.
	/// If invalid values are found the chart data will not be accepted.
	/// @param num Number of data points, must be at least 2.
	/// @param x_ptr Vector to a memory location with the x values and at least num values.
	/// @param y_ptr Vector to a memory location with the y values and at least num values.
	void setData(int num, double * x_ptr, double * y_ptr);

	/// @brief Returns minimum X value of the data set.
	/// This value gets updated whenever the data in the series changes due to a call
	/// to set data or directly by calling updateMinMaxValues().
	double minX() const { return m_minX; }
	/// @brief Returns maximum X value of the data set.
	/// @see minX()
	double maxX() const { return m_maxX; }
	/// @brief Returns minumum Y value of the data set.
	/// @see minX()
	double minY() const { return m_minY; }
	/// @brief Returns maximum Y value of the data set.
	/// @see minX()
	double maxY() const { return m_maxY; }

	/// @brief Updates the minimum/maximum values using the current data.
	/// This function recalculates the minimum and maximum x and y values 
	/// to be used for axis scaling.
	void updateMinMaxValues();

	/// @brief Returns the number of points in case external data storage is used.
	/// @return If the data was set using external pointers, this function returns the size
	/// of this data. If the data was stored internally, this function returns 0.
	int numPoints() const { return m_numPoints; }
	/// @brief Returns a pointer to the x value memory location in case 
	///        external data storage is used or a NULL pointer if not.
	double *xDataPtr() const { return m_xDataPtr; }
	/// @brief Returns a pointer to the y value memory location in case 
	///		   external data storage is used or a NULL pointer if not.
	double *yDataPtr() const { return m_yDataPtr; }
	/// @brief Returns the x values stored in the data series (can be empty if external data storage is used).
	const DataVec& xData() const { return m_xData; }
	/// @brief Returns the y values stored in the data series (can be empty if external data storage is used).
	const DataVec& yData() const { return m_yData; }

	/// @brief Paints the series onto the chart.
	///
	/// This function needs to be reimplemented by all child classes.
	///	The calling function needs to provide the correct chart extends
	/// for the axis to be used.
	/// @param p		The painter to be used for drawing the series.
	/// @param s		The size of the chart in pixels.
	/// @param extends	The extends of the chart in value units.
	virtual void paintSeries(QPainter * p, QSizeF s, QRectF extends) = 0;

	/// @brief Populates the series with some random data.
	/// This function is used in the chart edit dialog during design time 
	/// to give a visual impression of the chart series appearance when plotted.
	/// The random data is stored in the storage
	void setRandomValues();

    /// @brief Loads the series data from an XML document.
	virtual bool read(QDomElement in);
    /// @brief Saves the series data into an XML document.
    virtual void write(QDomDocument& doc, QDomElement& root) const;

signals:
	/// @brief Signal is emitted whenever a value changed in the function.
	void changed();
	/// @brief Signal is emitted whenever the series was assigned to a different axis.
	void axisChanged();

protected:
	int			m_id;			///< Unique series ID value;
	QString		m_name;			///< Series name.

private:

    /// @brief Tests if the data contains any invalid points (like inf or nan).
    bool checkData();

	int			m_xAxis;		///< X Axis ID: 0 indicates the X1 axis, 1 indicates the X2 axis.
	int			m_yAxis;		///< Y Axis ID: 0 indicates the Y1 axis, 1 indicates the Y2 axis.

	int			m_numPoints;	///< Number of points for external data storage.
	double	   *m_xDataPtr;		///< The pointer to a linear array of X data points.
	double	   *m_yDataPtr;		///< The pointer to a linear array of Y data points corresponding to the X data points.

	DataVec		m_xData;		///< The X coordinates.
	DataVec		m_yData;		///< The Y coordinates.

	// parameters updated whenever the data set changes
	// (we use overeager evaluation and cache the result for later access).
	double 		m_minX;			///< The minimum X value.
	double 		m_maxX;			///< The maximum X value.
	double 		m_minY;			///< The minimum Y value.
	double 		m_maxY;			///< The maximum Y value.
};


#endif // qnanabstractchartseries_h
