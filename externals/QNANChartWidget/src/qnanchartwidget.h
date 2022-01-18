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

#ifndef qnanchartwidget_h
#define qnanchartwidget_h

#include <QWidget>
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariantList>
#include <QFile>
#include <QTextStream>
#include <QPainter>

class QDomDocument;
class QDomElement;
class QDomNode;

class QNANChartAxis;
class QNANAbstractChartSeries;
class QNANChartWidgetPlugin;

/// @brief The main chart widget class.
/// @author Andreas Nicolai <andreas.nicolai-at-gmx.net>
/// @version 1.0.0
/// @date    07/25/2007
///
/// If you want to use the QNANChartWidget without the
/// designer plugin, take a look at the code example below.
/// @code
/// #include <QtGui>
///
/// // include master include file for the chart widget
/// #include <QNANChartWidget>
///
/// #include <cmath>
///
/// int main(int argc, char ** argv) {
/// 	QApplication app( argc, argv );
///
/// 	// create chart widget just like
/// 	QNANChartWidget w(NULL, Qt::WindowMinMaxButtonsHint);
///
/// 	// create two data series
/// 	QNANDefaultChartSeries * s1 = new QNANDefaultChartSeries(&w);
/// 	QNANDefaultChartSeries * s2 = new QNANDefaultChartSeries(&w);
///
/// 	// add them to the chart
/// 	w.addSeries(s1);
/// 	w.addSeries(s2);
///
/// 	// create some test data
/// 	QList<double> x,y1,y2;
/// 	for (int i=0; i<1000; ++i)
/// 	{
/// 		x.append(i*0.001);
/// 		y1.append(std::sin(i/100.0 * 3.14157)*std::exp(-i/500.0));
/// 		y2.append(std::sin(i/200.0 * 3.14157)*std::exp(-i/250.0));
/// 	}
/// 	// set the data in the series
/// 	s1->setData(x,y1);
/// 	s2->setData(x,y2);
///
/// 	// show the widget and start the application
/// 	w.show();
/// 	return app.exec();
/// }
/// @endcode
class QNANChartWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(QFont tickLabelFont				READ tickLabelFont			WRITE setTickLabelFont)
	Q_PROPERTY(bool x2AxisVisible				READ x2AxisVisible			WRITE setX2AxisVisible	STORED false)
	Q_PROPERTY(bool y2AxisVisible				READ y2AxisVisible			WRITE setY2AxisVisible	STORED false)
	Q_PROPERTY(int marginTop					READ marginTop				WRITE setMarginTop		STORED false)
	Q_PROPERTY(int marginBottom					READ marginBottom			WRITE setMarginBottom	STORED false)
	Q_PROPERTY(int marginLeft					READ marginLeft				WRITE setMarginLeft		STORED false)
	Q_PROPERTY(int marginRight					READ marginRight			WRITE setMarginRight	STORED false)
	Q_PROPERTY(ZoomOption zoomOption			READ zoomOption 			WRITE setZoomOption 	STORED true)
	Q_ENUMS(ZoomOption)
	Q_PROPERTY(QString chartSettings			READ chartSettings			WRITE setChartSettings	STORED true  DESIGNABLE false)

public:
	/// The different zoom options to be used for the zooming of the chart.
	/// @see setZoomOption()
	/// @see zoomOption()
	enum ZoomOption {
		NoZoom,				///< No zooming permitted.
		HorizontalZoom,		///< Only the horizontal range can be zoomed.
		VerticalZoom,		///< Only the vertical range can be zoomed.
		BidirectionalZoom	///< Zooming is possible in all directions.
	};

	/// Constructor (typical widget constructor).
	QNANChartWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
	/// Destructor (handles all cleanup).
	virtual ~QNANChartWidget();

	/// Returns a size of the chart that is a meaningful minimum (300, 200).
	QSize minimumSizeHint() const;
	/// Returns the proposed size of the chart (400, 300).
	QSize sizeHint() const;

	/// The font currently used to draw tick labels.
	const QFont& tickLabelFont() const { return m_tickLabelFont; }
	/// Set the font to be used to draw tick labels.
	void setTickLabelFont(const QFont& f);

	/// The distance from the top to the top border line of the chart itself.
	/// The tick marks, tick labels and axis caption are drawn within this distance.
	/// @see setMarginTop
	int marginTop() const { return m_marginTop; }
	/// Sets the distance from the top to the top border line of the chart itself.
	/// The tick marks, tick labels and axis caption are drawn within this distance.
	/// @see marginTop
	void setMarginTop(int m);
	/// Returns the distance from the bottom chart line of the chart to the bottom of the widget.
	int marginBottom() const { return m_marginBottom; }
	/// Sets the distance in pixel from the bottom chart line of the chart to the bottom of the widget.
	void setMarginBottom(int m);
	/// Returns the distance from the left chart line of the chart to the left of the widget.
	int marginLeft() const { return m_marginLeft; }
	/// Sets the distance in pixel from the left chart line of the chart to the left of the widget.
	void setMarginLeft(int m);
	/// Returns the distance from the right chart line of the chart to the right of the widget.
	int marginRight() const { return m_marginRight; }
	/// Sets the distance in pixel from the right chart line of the chart to the right of the widget.
	void setMarginRight(int m);

	/// Returns the current zoom option.
	/// @see ZoomOption
	/// @see setZoomOption
	ZoomOption zoomOption() const { return m_zoomOption; }
	void setZoomOption(ZoomOption option);

	/// Returns a pointer to the bottom axis used in the chart.
	QNANChartAxis * x1Axis() const { return m_x1Axis; }
	/// Returns a pointer to the top axis used in the chart.
	QNANChartAxis * x2Axis() const { return m_x2Axis; }
	/// Returns a pointer to the left axis used in the chart.
	QNANChartAxis * y1Axis() const { return m_y1Axis; }
	/// Returns a pointer to the right axis used in the chart.
	QNANChartAxis * y2Axis() const { return m_y2Axis; }

	/// Returns whether the top x axis is visible or not.
	bool x2AxisVisible() const { return m_x2AxisVisible; }
	/// Shows/hides the top x axis.
	/// If this additional axis is not visible, the tick marks shown at the top of the chart
	/// are the same as for the bottom axis.
	void setX2AxisVisible(bool b);
	/// Returns whether the right x axis is visible or not.
	bool y2AxisVisible() const { return m_y2AxisVisible; }
	/// Shows/hides the right y axis.
	/// If this additional axis is not visible, the tick marks shown at the right of the chart
	/// are the same as for the left axis.
	void setY2AxisVisible(bool b);

	/// Returns an XML encoded string containing the chart configuration.
	QString chartSettings() const;
	/// Sets the chart configuration from an XML encoded string.
	void setChartSettings(const QString& s);

	// chart series handling

	/// @brief Adds a data series to the chart.
	///
	/// The function adds a data series to the chart. The chart takes ownership
	/// of the chart object and takes care of deleting allocated memory.
	///
	/// The ID stored in the chart series must be unique within the chart. Therefore,
	/// if the ID conflicts with an existing ID, the series will be assigned a new ID
	/// which will also be returned by the function. In practice it is useful to use
	/// a code like in the following example:
	/// @code
	/// QNANDefaultChartSeries * series = new QNANDefaultChartSeries(chartwidget);
	/// series->setName("Coding time - coffee consumption dependency");
	/// // other customizations
	/// int series_id = chartwidget->addSeries(series);
	/// // store ID for future reference
	/// @endcode
	int addSeries(QNANAbstractChartSeries * s);

	/// @brief Returns a data series.
	///
	/// Use this function to obtain access to a data series, so that it can be modified.
	/// @warning Never change the ID of data series that has been already added to the chart!
	/// @return Returns a pointer to the chart series denoted by ID, or NULL if the ID is invalid.
	QNANAbstractChartSeries * series(int ID);

	/// @brief Removes a data series from the chart and frees allocated memory.
	///
	/// Use this function to get rid of a data series completely. If you intend to temporarily
	/// remove the series from the chart and add it later again (possibly to some other chart)
	/// use the takeSeries() function. If the ID is invalid, nothing will happen.
	void removeSeries(int ID);

	/// @brief Returns a data series from the chart for later use somewhere else.
	///
	/// In contrast to the removeSeries() function, this function will remove a series from
	/// the chart and return a pointer to the series data to the user. The series will
	/// be unparented and no longer be owned by the chart. Therefore the user will be
	/// responsible for deleting the object and free up any memory allocated.
	/// If the ID is invalid, a NULL pointer is returned.
	QNANAbstractChartSeries * takeSeries(int ID);

	/// @brief Removes all chart series from the chart.
	void clear();

	/// @brief Returns a reference to the series data stored in the class
	const QMap<int,QNANAbstractChartSeries *>&	seriesData() const { return m_seriesData; }

//	QFile * dumpFile;
//	QTextStream * dumpStream;

	/// @brief Loads the chart data from an XML document
	void read(QDomElement& root);
	/// @brief Saves the chart data into an XML document
	void write(QDomDocument& doc, QDomElement& root) const;

	/// @brief Returns true if the widget is being edited in Qt designer.
	bool inDesigner() const { return m_inDesigner; }

	/// @brief Public function to redirect drawing of the actual chart pixmap to a custom painter.
	/// The chart will be drawn with the same coordinates as they appear on screen.
	void drawChart(double scale, QPainter * p) { updateChartPixmap(scale, p); }

	/// @brief Draws the chart pixmap on the painter, and the axes and labels.
	/// This function encapsulates the drawing functionality. It first draws the cached
	/// pixmap onto the painter if drawPixmap is true. Then it draws the axis. And
	/// finally it overlays the chart with zoom indicators, if drawZoomIndicators is true.
	void drawAll(QPainter * p, bool drawPixmap, bool drawZoomIndicators);

public slots:
	/// @brief Causes a complete recalculation of the chart.
	void updateChart();

	/// @brief Context menu item "Edit chart...".
	void editChart();
	/// @brief Context menu item "Edit chart series...".
	void editDataSeries();
	/// @brief Context menu item "Print chart...".
	void printChart();
	/// @brief Context menu item "Export chart...".
	void exportChartAsPicture();

	/// @brief This function is used by QNANAbstractChartSeries objects to
	///        notify the chart of an axis change.
	void seriesAxisChanged();

protected:
	void showEvent(QShowEvent * event);
	void paintEvent(QPaintEvent * event);
	void resizeEvent(QResizeEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void contextMenuEvent(QContextMenuEvent * event);

private:
	/// Locations for each chart axis.
	enum AxisLocation {
		Top,
		Bottom,
		Left,
		Right
	};

	/// @brief Recalculates the geometry (positioning lines).
	void recalculateGeometry();

	/// @brief Updates the pixmap with the chart using the given size and scaling factor.
	/// Basically, this function draws the chart as on screen (where scale = 1), to
	/// the pixmap with larger scale. If a pointer to a painter object is given, the
	/// drawing is done on the painter instead.
	void updateChartPixmap(double scale, QPainter * p);

	/// @brief Updates the zoom indicator
	void updateZoomIndicator();

	/// @brief Pans the chart by some distance given in pixels
	void panChart(QPoint dist);

	/// @brief Draws the axis (ticks and tick labels) using the painter p.
	void drawAxis(QPainter* p, QSizeF s, QNANChartAxis * axis, AxisLocation loc);

	/// Draws the grid lines.
	void drawGrid(QPainter* p, QSizeF s);

	/// Obtains the minimum and maximum values of all the assigned chart series.
	void getDataMinMax(const QList<int>& seriesIDs, double & minval, double & maxval, bool xdir);

	// *** general properties of the chart ***
	QNANChartAxis	*	m_x1Axis;
	QNANChartAxis	*	m_x2Axis;
	QNANChartAxis	*	m_y1Axis;
	QNANChartAxis	*	m_y2Axis;
	bool				m_x2AxisVisible;
	bool				m_y2AxisVisible;

	QPixmap				m_chart;	///< The pixmap holding the actual chart inclusive tick labels.

	/// @brief The pen-width to be used for drawing lines in the chart.
	/// For print output this value should be > 2;
	double				m_penWidth;

	// *** chart series variables ***
	QMap<int,QNANAbstractChartSeries *>	m_seriesData;
	QList<int>							m_x1AxisSeries;
	QList<int>							m_x2AxisSeries;
	QList<int>							m_y1AxisSeries;
	QList<int>							m_y2AxisSeries;

	// *** appearance properties ***

	// the widget font is used for the labels and
	// the widget palette is used for drawing the background
	QFont			m_tickLabelFont;

	int				m_marginTop;
	int				m_marginBottom;
	int				m_marginLeft;
	int				m_marginRight;

	ZoomOption		m_zoomOption;
	QPoint			m_zoomP1;
	QPoint			m_zoomP2;
	bool			m_zoomIndicatorShown;

	QPoint			m_panStartPos;
	bool			m_panning;

	// *** actions for context menu ***

	QAction			*m_editChartAction;
	QAction			*m_editSeriesAction;
	QAction			*m_printAction;
	QAction			*m_exportAction;

	// *** calculated variables for drawing the chart ***

	// optimization variables

	/// If false, the chart won't update automatically when a
	/// relevant property such as the axis scaling changes.
	bool			m_doUpdate;

	// coordinates needed for painting the chart
	double			m_tickLabelHeight;
	double			m_tickLabelWidth;
	double			m_labelSize;
	double			m_axisSpacing;

	double			m_y[6];	// vertial guiding lines
	double			m_x[4];	// horizontal guiding lines

	bool			m_inDesigner;	///< True, if open in Qt designer.

	friend class QNANChartWidgetPlugin;
	friend class QNANPrintChartDialog;
};

/// @mainpage QNANChartWidget
/// @section intro_sec Introduction
/// The QNANChartWidget is a Qt designer widget for plotting X-Y data sets. It's usage is very simple
/// as explained in the usage description below.
///
/// QNANChartWidget is a simple xy-charting class. It is simple and it was designed as
/// such. The key motivation was to create charting class for displaying arbitrary 2D dataset, but
/// leave the really fancy stuff to the professional toolkits. As a result, QNANChartWidget has
/// some nice properties:
/// <ul>
/// <li>it is lightweight, just a coupled of source code files</li>
/// <li>there are no dependencies other than Qt itself (so just adding the chart source files to your
///     project will get you started very quickly)</li>
/// <li>it is <b>fast</b>, actually much faster than other charting widgets (as demonstrated in the examples)</li>
/// <li>it comes with handy Qt designer plugins, that allow usage of the chart just like a push button</li>
/// </ul>
/// In the sections below the procedure on how to use the chart widget in Qt designer is shown. For
/// more detailed instructions and information, browse the documented files (start reading with the
/// documentation of QNANChartWidget itself)!
///
/// @section design Using the chart widget as plugin
/// After building and installing the widget plugin Qt designer will have a new category called
/// QNANWidgets.
///
/// @image HTML component_palette.png
///
/// Just like any other widget you can drag the chart widget onto your widget/dialog/main form etc.
/// and you will first see an empty chart widget with some grid. Note that the grid is already
/// automatically scaled when resizing the widget.
///
/// @image HTML new_chart_widget.png
///
/// Now the chart widget can be customized and all without writing a single line of code. Simply right
/// click on the chart to open the context menu
/// @image HTML chart_context_menu.png
/// and select one of the circled menu options. The chart appearance and the data series can be
/// individually modified.
///
/// @subsection appearance Modifying the chart appearance
/// The chart appearance dialog shown below allows to setup the visual aspects of the chart that cannot
/// directly set as the chart widget properties, for instance font and chart palette (background etc.).
/// Specifically the axis options can be edited. Changed in the options will be directly shown in the
/// chart widget so playing around with the options will give you an idea quickly about what each property does.
///
/// @image HTML appearance_edit_dialog.png
///
/// @subsection series Editing the data series
/// In the data series edit dialog you can add and remove data series and set the series options. Pay
/// attention to the unique series identifier in the first column (circled in the screenshot below).
/// You will need these identifiers later to access the data series and set the plot data.
///
/// @image HTML series_edit_dialog.png
///
/// In order to illustrate the appearance of your chart including the data series, random values will
/// be set for each data series to give an impression on how the chart will finally look. For example,
/// in the screenshot below
/// @image HTML chart_widget_with_series.png
/// a chart with visible second y axis and several data series with different formats can be seen.
///
/// After designing the chart and the data series, the only thing to remember for later coding is
/// the name of the chart and the IDs of the created data series.
///
/// @section docover QNANWidget library
///
/// The widget is part of the widget library named QNANWidgets. The prefix stems from the fact, that
/// Qnan or Qinf values are often obtained in scientific/engineering code as a result of some
/// numerical calculation problem. The widgets can be found on the
/// sourceforge project <a href="http://qnanwidgets.sourceforge.net">qnanwidgets.sourceforge.net</a>.
///
///


#endif // qnanchartwidget_h
