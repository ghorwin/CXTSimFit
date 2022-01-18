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

#include "qnanchartwidget.h"

#include <QtAlgorithms>
#include <QtXml>
#include <QStylePainter>
#include <QMouseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QStyleOptionComboBox>

#include <cmath>
#include <memory>

#include "qnanchartaxis.h"
#include "qnanabstractchartseries.h"
#include "qnandefaultchartseries.h"
#include "qnancharteditdialog.h"
#include "qnanserieseditdialog.h"
#include "qnanprintchartdialog.h"

QNANChartWidget::QNANChartWidget(QWidget * parent, Qt::WindowFlags f)
  : QWidget(parent,f),
	m_editChartAction(0),
	m_editSeriesAction(0),
	m_printAction(0),
	m_exportAction(0)
{
	m_doUpdate = false;

//	qDebug() << "[QNANChartWidget::QNANChartWidget]";
	// create chart axis
	m_x1Axis = new QNANChartAxis(this);
	m_x1Axis->setLabel(tr("Primary X-Axis"));

	m_x2Axis = new QNANChartAxis(this);
	m_x2Axis->setLabel(tr("Secondary X-Axis"));
	m_x2AxisVisible = false;

	m_y1Axis = new QNANChartAxis(this);
	m_y1Axis->setLabel(tr("Primary Y-Axis"));

	m_y2Axis = new QNANChartAxis(this);
	m_y2Axis->setLabel(tr("Secondary Y-Axis"));
	m_y2AxisVisible = false;

	// connect axisChanged signals with updateChart slot
	connect(m_x1Axis, SIGNAL(axisChanged()), this, SLOT(updateChart()));
	connect(m_x2Axis, SIGNAL(axisChanged()), this, SLOT(updateChart()));
	connect(m_y1Axis, SIGNAL(axisChanged()), this, SLOT(updateChart()));
	connect(m_y2Axis, SIGNAL(axisChanged()), this, SLOT(updateChart()));

	// set default margins
	m_marginTop = 30;
	m_marginBottom = 50;
	m_marginLeft = 50;
	m_marginRight = 40;

	// set basic colors
	setAutoFillBackground(true);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::StrongFocus);
	QPalette pal(palette());
	pal.setColor(QPalette::Window, Qt::white);
	setPalette(pal);

	// set the tick label font to the same as the widget font, but a little bit smaller
	QFont fnt = font();
	m_tickLabelFont = fnt;
	if (fnt.pointSize() < 10)
		fnt.setPointSize(10);
	setFont(fnt);
	m_tickLabelFont.setPointSizeF( fnt.pointSizeF()*0.8 );

	m_zoomIndicatorShown = false;
	m_zoomOption = BidirectionalZoom;
	m_panning = false;

	m_inDesigner = false;

	setMinimumSize( QSize(250,150) );

	m_penWidth = 0; // per default, 1 pixel wide lines

	// allow updates again
	m_doUpdate = true;
}

QNANChartWidget::~QNANChartWidget() {
//	delete dumpStream;
//	delete dumpFile;
}

QSize QNANChartWidget::minimumSizeHint() const {
	return QSize(300, 200);
}


QSize QNANChartWidget::sizeHint() const {
	return QSize(400, 300);
}

// *** Property functions ***

void QNANChartWidget::setTickLabelFont(const QFont& f) {
	m_tickLabelFont = f;
	updateChart();
}

void QNANChartWidget::setMarginTop(int m) {
	m_marginTop = m;
	// TODO : check for valid margins
	updateChart();
}

void QNANChartWidget::setMarginBottom(int m) {
	m_marginBottom = m;
	updateChart();
}

void QNANChartWidget::setMarginLeft(int m) {
	m_marginLeft = m;
	updateChart();
}

void QNANChartWidget::setMarginRight(int m) {
	m_marginRight = m;
	updateChart();
}

void QNANChartWidget::setZoomOption(ZoomOption option) {
	// TODO : cancel current zoom operation
	m_zoomOption = option;
}

void QNANChartWidget::setX2AxisVisible(bool b) {
	m_x2AxisVisible = b;
	if (b && m_marginTop < m_marginBottom) {
		setMarginTop(m_marginBottom);
	}
	updateChart();
}

void QNANChartWidget::setY2AxisVisible(bool b) {
	m_y2AxisVisible = b;
	if (b && m_marginRight < m_marginLeft) {
		setMarginRight(m_marginLeft);
	}
	updateChart();
}

QString QNANChartWidget::chartSettings() const {
	QDomDocument doc;
	QDomElement root = doc.createElement("QNANChartDocument");
	doc.appendChild(root);
	write(doc, root);
	return doc.toString();
}

void QNANChartWidget::setChartSettings(const QString& s) {
	// create a QDomDocument from the data and decode it to
	// restore the widget state
	QDomDocument doc;
	doc.setContent(s);
	QDomElement root = doc.documentElement();
	read(root);
}

// *** Chart series access functions ***

int QNANChartWidget::addSeries(QNANAbstractChartSeries * s) {
	// first check if ID is unique
	int id = 0;
	bool unique = true;
	for (QMap<int, QNANAbstractChartSeries *>::ConstIterator it = m_seriesData.constBegin();
		it != m_seriesData.constEnd(); ++it)
	{
		id = qMax(id, it.key());
		if (it.key() == s->id()) 	unique = false;
	}
	// if not unique, get new id by incrementing the max-ID found in the series list,
	// and set this ID in the series
	if (!unique)
		s->setId(++id);
	else
		id = s->id();
//	(*dumpStream) << "Add series with ID = " << id << endl;
	// now add the series to the data list
	m_seriesData[id] = s;
	// make the series owned by the chart
	s->setParent(this);
	// and update the other references
	if (s->xAxis() == 0)
		m_x1AxisSeries.append(id);
	else
		m_x2AxisSeries.append(id);
	if (s->yAxis() == 0)
		m_y1AxisSeries.append(id);
	else
		m_y2AxisSeries.append(id);
	// connect the changed signal
	connect(s, SIGNAL(axisChanged()), this, SLOT(seriesAxisChanged()));
	// finally return the (new) series id
	return id;
}

QNANAbstractChartSeries * QNANChartWidget::series(int ID) {
	if (m_seriesData.find(ID) == m_seriesData.end()) return NULL;
	else return m_seriesData[ID];
}

void QNANChartWidget::removeSeries(int ID) {
	delete takeSeries(ID);
}

QNANAbstractChartSeries * QNANChartWidget::takeSeries(int ID) {
	if (m_seriesData.find(ID) == m_seriesData.end()) return NULL;
	QNANAbstractChartSeries * s = m_seriesData.take(ID);
	s->setParent(0);
	// remove series from the data lists
	m_x1AxisSeries.removeAll(s->id());
	m_x2AxisSeries.removeAll(s->id());
	m_y1AxisSeries.removeAll(s->id());
	m_y2AxisSeries.removeAll(s->id());
	// disconnect chart from series
	disconnect(s, 0, this, 0);
	return s;
}

void QNANChartWidget::clear() {
	QList<QNANAbstractChartSeries *> seriesList = m_seriesData.values();
	qDeleteAll(seriesList);
	m_seriesData.clear();
	m_x1AxisSeries.clear();
	m_x2AxisSeries.clear();
	m_y1AxisSeries.clear();
	m_y2AxisSeries.clear();
}

void QNANChartWidget::read(QDomElement& root) {
	if (root.isNull() || root.tagName() != "QNANChartDocument") return;
	// get the QNANChart element
	QDomElement chartElement = root.firstChildElement("QNANChart");
	if (chartElement.isNull()) return;
	// read properties/attributes from chart node
	if (chartElement.hasAttribute("X2AxisVisible"))
		setX2AxisVisible( chartElement.attribute("X2AxisVisible").toInt() );
	if (chartElement.hasAttribute("Y2AxisVisible"))
		setY2AxisVisible( chartElement.attribute("Y2AxisVisible").toInt() );
	// read axis data
	QDomElement axisElement = chartElement.firstChildElement("QNANChartAxis");
	while (!axisElement.isNull()) {
		// we need the axis name to store the data in the correct
		// axis object
		if (axisElement.hasAttribute("Name")) {
			QString axisName = axisElement.attribute("Name");
			if (axisName == "X1 Axis")		m_x1Axis->read(axisElement);
			else if (axisName == "Y1 Axis")	m_y1Axis->read(axisElement);
			else if (axisName == "X2 Axis")	m_x2Axis->read(axisElement);
			else if (axisName == "Y2 Axis")	m_y2Axis->read(axisElement);
		}
		axisElement = axisElement.nextSiblingElement("QNANChartAxis");
	}
	// get list of series
	QDomElement seriesListElement = chartElement.firstChildElement("ChartSeriesList");
	if (!seriesListElement.isNull()) {
		QDomElement chartSeriesElement = seriesListElement.firstChildElement("QNANDefaultChartSeries");
		while (!chartSeriesElement.isNull()) {
			// create new default chart series
			QNANDefaultChartSeries * s = new QNANDefaultChartSeries(this);
			s->read(chartSeriesElement);
			if (m_inDesigner)	s->setRandomValues();
			addSeries(s);
			chartSeriesElement = chartSeriesElement.nextSiblingElement("QNANDefaultChartSeries");
		}
	}

	QDomElement marginsElement = chartElement.firstChildElement("Margins");
	if (!marginsElement.isNull()) {
		if (marginsElement.hasAttribute("Left"))	setMarginLeft( marginsElement.attribute("Left").toInt() );
		if (marginsElement.hasAttribute("Right"))	setMarginRight( marginsElement.attribute("Right").toInt() );
		if (marginsElement.hasAttribute("Top"))		setMarginTop( marginsElement.attribute("Top").toInt() );
		if (marginsElement.hasAttribute("Bottom"))	setMarginBottom( marginsElement.attribute("Bottom").toInt() );
	}
}

void QNANChartWidget::write(QDomDocument& doc, QDomElement& root) const {
	QDomElement chartNode = doc.createElement("QNANChart");
	root.appendChild(chartNode);
	// store some attributes of the chart
	chartNode.setAttribute ("X2AxisVisible", QString("%1").arg(m_x2AxisVisible));
	chartNode.setAttribute ("Y2AxisVisible", QString("%1").arg(m_y2AxisVisible));

	// store axis data
	m_x1Axis->write("X1 Axis", doc, chartNode);
	m_y1Axis->write("Y1 Axis", doc, chartNode);
	m_x2Axis->write("X2 Axis", doc, chartNode);
	m_y2Axis->write("Y2 Axis", doc, chartNode);

	// store all data series
	QDomElement seriesListNode = doc.createElement("ChartSeriesList");
	chartNode.appendChild(seriesListNode);
	for (QMap<int,QNANAbstractChartSeries*>::ConstIterator it = m_seriesData.constBegin();
		it != m_seriesData.constEnd(); ++it)
	{
		it.value()->write(doc, seriesListNode);
	}

	QDomElement fontNode = doc.createElement("AxisLabelFont");
	QDomText fontText = doc.createTextNode(font().toString());
	fontNode.appendChild(fontText);
	chartNode.appendChild(fontNode);

	fontNode = doc.createElement("TickLabelFont");
	fontText = doc.createTextNode(m_tickLabelFont.toString());
	fontNode.appendChild(fontText);
	chartNode.appendChild(fontNode);

	QDomElement marginsNode = doc.createElement("Margins");
	marginsNode.setAttribute ("Left", QString("%1").arg(m_marginLeft));
	marginsNode.setAttribute ("Right", QString("%1").arg(m_marginRight));
	marginsNode.setAttribute ("Top", QString("%1").arg(m_marginTop));
	marginsNode.setAttribute ("Bottom", QString("%1").arg(m_marginBottom));
	chartNode.appendChild(marginsNode);
}


// *** Protected (reimplemented) functions ***

void QNANChartWidget::showEvent(QShowEvent * /* event */) {
	updateChart();
}

void QNANChartWidget::paintEvent(QPaintEvent * /* event */) {
	QStyleOptionComboBox opt;
	opt.initFrom(this);
	QStylePainter p(this);
	p.drawPrimitive(QStyle::PE_FrameLineEdit, opt);

	// now draw the chart, including zoom rectangle if any
	drawAll(&p, true, true);

	// also draw the focus rectangle...
	if (hasFocus()) {
		QStyleOptionFocusRect o;
		o.initFrom(this);
		o.backgroundColor = palette().color(QPalette::Background);
		p.drawPrimitive(QStyle::PE_FrameFocusRect, o);
	}
}


void QNANChartWidget::resizeEvent(QResizeEvent * /* event */) {
	updateChart();
}


void QNANChartWidget::mousePressEvent(QMouseEvent * event) {
	// do we have a panning action going on?
	if (event->button() == Qt::MidButton) {
		m_panStartPos = event->pos();
		m_panning = true;
		setCursor(Qt::SizeAllCursor);
		return;
	}
	// if we are not allowed to zoom, or we are panning return
	if (m_zoomOption == NoZoom || m_panning) return;
	// check that the user clicked inside the chart for zooming
	QRectF chartRect(m_x[1], m_y[2], m_x[2] - m_x[1], m_y[3] - m_y[2]);
	if (event->button() == Qt::LeftButton) {
		if (chartRect.contains(event->pos())) {
			m_zoomIndicatorShown = true;
			m_zoomP1 = event->pos();
			m_zoomP2 = event->pos();
			setCursor(Qt::CrossCursor);
			updateZoomIndicator();
		}
	}
}

void QNANChartWidget::mouseMoveEvent(QMouseEvent * event) {
	if (m_panning) {
		QPoint newpos = event->pos();
		QPoint dist(newpos.x() - m_panStartPos.x(),
					newpos.y() - m_panStartPos.y());
		panChart(dist);
		m_panStartPos = newpos;
		return;
	}
	if (m_zoomIndicatorShown) {
		updateZoomIndicator();
		// limit the zoom position to inside the chart
		m_zoomP2 = event->pos();
		m_zoomP2.setX( qMax(int(m_x[1]), m_zoomP2.x() ));
		m_zoomP2.setX( qMin(int(m_x[2]), m_zoomP2.x() ));
		m_zoomP2.setY( qMax(int(m_y[2]), m_zoomP2.y() ));
		m_zoomP2.setY( qMin(int(m_y[3]), m_zoomP2.y() ));
		updateZoomIndicator();
	}
}

void QNANChartWidget::mouseReleaseEvent(QMouseEvent * event) {
	if (event->button() == Qt::MidButton && m_panning) {
		m_panning = false;
		unsetCursor();
		return;
	}
	if (event->button() == Qt::LeftButton && m_zoomIndicatorShown) {
		m_zoomIndicatorShown = false;
		updateZoomIndicator();
		unsetCursor();
		// zoom
		if (m_zoomOption == NoZoom)  return;
		// negative x-direction?
		if (m_zoomP2.x() < m_zoomP1.x() || m_zoomP2.y() < m_zoomP1.y()) {
			if ( (m_zoomOption == HorizontalZoom || m_zoomOption == BidirectionalZoom) ) {
				m_x1Axis->zoom(0,-1);
				m_x2Axis->zoom(0,-1);
			}
			if ( (m_zoomOption == VerticalZoom || m_zoomOption == BidirectionalZoom) ) {
				m_y1Axis->zoom(0,-1);
				m_y2Axis->zoom(0,-1);
			}
			return;
		}
		double x1_scale = (m_zoomP1.x() - m_x[1])/(m_x[2]-m_x[1]);
		double x2_scale = (m_zoomP2.x() - m_x[1])/(m_x[2]-m_x[1]);
		double y1_scale = (m_y[3] - m_zoomP1.y())/(m_y[3]-m_y[2]);
		double y2_scale = (m_y[3] - m_zoomP2.y())/(m_y[3]-m_y[2]);
		if ( (m_zoomOption == HorizontalZoom || m_zoomOption == BidirectionalZoom)
			 && fabs(x1_scale - x2_scale) > 1e-3)
		{
			double x_min = m_x1Axis->minVal();
			double x_max = m_x1Axis->maxVal();
			m_x1Axis->zoom((x_max-x_min)*x1_scale + x_min, (x_max-x_min)*x2_scale + x_min );

			if (m_x2AxisVisible) {
				double x_min = m_x2Axis->minVal();
				double x_max = m_x2Axis->maxVal();
				m_x2Axis->zoom( (x_max-x_min)*x1_scale + x_min, (x_max-x_min)*x2_scale + x_min );
			}
		}
		if ( (m_zoomOption == VerticalZoom || m_zoomOption == BidirectionalZoom)
			 && fabs(y1_scale-y2_scale) > 1e-3)
		{
			double y_min = m_y1Axis->minVal();
			double y_max = m_y1Axis->maxVal();
			m_y1Axis->zoom( (y_max-y_min)*y2_scale + y_min , (y_max-y_min)*y1_scale + y_min );
			if (m_y2AxisVisible) {
				double y_min = m_y2Axis->minVal();
				double y_max = m_y2Axis->maxVal();
				m_y2Axis->zoom( (y_max-y_min)*y2_scale + y_min, (y_max-y_min)*y1_scale + y_min );
			}
		}
	}
}

void QNANChartWidget::contextMenuEvent(QContextMenuEvent * event) {
	// TODO : check user flags to show only selected context menu entries
	QMenu menu(this);
	if (m_editChartAction == 0) {
		m_editChartAction = new QAction(tr("Edit &chart..."), this);
		//m_editChartAction->setShortcut(tr("Ctrl+N"));
		connect(m_editChartAction, SIGNAL(triggered()), this, SLOT(editChart()));
	}
	if (m_editSeriesAction == 0) {
		m_editSeriesAction = new QAction(tr("Edit &series..."), this);
		connect(m_editSeriesAction, SIGNAL(triggered()), this, SLOT(editDataSeries()));
	}
	if (m_printAction == 0) {
		m_printAction = new QAction(tr("Print chart..."), this);
		connect(m_printAction, SIGNAL(triggered()), this, SLOT(printChart()));
	}
	if (m_exportAction == 0) {
		m_exportAction = new QAction(tr("Export chart to image file..."), this);
		connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportChartAsPicture()));
	}
	menu.addAction(m_editChartAction);
	menu.addAction(m_editSeriesAction);
	menu.addAction(m_printAction);
	menu.addAction(m_exportAction);
	menu.exec(event->globalPos());
}

// *** Slot implementations ***

void QNANChartWidget::recalculateGeometry() {
	// calculate TickLabelSize and LabelSize using the font metrix
	QFontMetricsF LabelSizeMetrics( font() );
	QFontMetricsF TickLabelSizeMetrics( m_tickLabelFont );

	m_labelSize = LabelSizeMetrics.height();
	m_tickLabelHeight = TickLabelSizeMetrics.height();
	m_tickLabelWidth = TickLabelSizeMetrics.boundingRect("125.00").width();
	m_axisSpacing = 0.5*m_tickLabelHeight;

	// update the tick label width with the final axis tick labels and require a refresh
	// of the chart if the width doesn't match the projected width
	double oldTickLabelWidth;
	double leftMargin = m_marginLeft;
	double rightMargin = m_marginRight;
	int iter = 10;
	do {
		oldTickLabelWidth = m_tickLabelWidth;
		QFontMetricsF tickLabelFontMetrics(m_tickLabelFont);
		// TODO : add check for automatic margin adjustment flag
		if (m_tickLabelWidth + 2*tickLabelFontMetrics.height() > leftMargin) {
			leftMargin = m_tickLabelWidth + 2*tickLabelFontMetrics.height();
		}
		if (m_y2AxisVisible && m_tickLabelWidth + 2*tickLabelFontMetrics.height() > rightMargin) {
			rightMargin = m_tickLabelWidth + 2*tickLabelFontMetrics.height();
		}

		// calculate all the coordinates
		m_y[2] = m_marginTop;
		m_y[1] = m_y[2] - m_axisSpacing;
		m_y[0] = m_y[1] - m_tickLabelHeight;
		m_y[3] = height() - m_marginBottom;
		m_y[4] = m_y[3] + m_axisSpacing;
		m_y[5] = m_y[4] + m_tickLabelHeight;

		m_x[1] = leftMargin;
		m_x[0] = m_x[1] - m_tickLabelWidth;
		m_x[2] = width() - rightMargin;
		m_x[3] = m_x[2] + m_tickLabelWidth;

		// let dimension the y axis and get the tick label width
		double min_y1, max_y1;
		getDataMinMax(m_y1AxisSeries, min_y1, max_y1, false);
		m_y1Axis->updateAxisScaling(min_y1, max_y1, m_y[3]-m_y[2], tickLabelFontMetrics, false);
		if (m_y2AxisVisible) {
			// y2 axis
			double min_y2, max_y2;
			getDataMinMax(m_y2AxisSeries, min_y2, max_y2, false);
			m_y2Axis->updateAxisScaling(min_y2, max_y2, m_y[3]-m_y[2], tickLabelFontMetrics, false);
		}
		else {
			// we need to rescale the y1 axis, but this time also include the
			// series that are plotted on the y2 axis
			QList<int> allSeries = m_y1AxisSeries;
			allSeries += m_y2AxisSeries;
			getDataMinMax(allSeries, min_y1, max_y1, false);
			m_y1Axis->updateAxisScaling(min_y1, max_y1, m_y[3]-m_y[2], tickLabelFontMetrics, false);
			// and now copy the scaling from the y1 axis
			m_y2Axis->copyFrom(*m_y1Axis);
		}
		// get label width for y1 axis
		double y = m_y1Axis->niceMinVal() - m_y1Axis->minVal();
		double y1LabelWidth = m_tickLabelWidth;
		for (int i = 0; i<=m_y1Axis->numTicks(); ++i) {
			QString label = QString("%1").arg(y + m_y1Axis->minVal(), 0, m_y1Axis->numberFormat(), m_y1Axis->precision());
			y1LabelWidth = qMax(y1LabelWidth, tickLabelFontMetrics.boundingRect(label).width() + 1*tickLabelFontMetrics.height() );
			y += m_y1Axis->tickMarkDistance();
		}
		// get label width for y2 axis
		y = m_y2Axis->niceMinVal() - m_y2Axis->minVal();
		double y2LabelWidth = m_tickLabelWidth;
		for (int i = 0; i<=m_y2Axis->numTicks(); ++i) {
			QString label = QString("%1").arg(y + m_y2Axis->minVal(), 0, m_y2Axis->numberFormat(), m_y2Axis->precision());
			y2LabelWidth = qMax(y2LabelWidth, tickLabelFontMetrics.boundingRect(label).width() + 1*tickLabelFontMetrics.height() );
			y += m_y2Axis->tickMarkDistance();
		}
//		qDebug() << "y1=" << y1LabelWidth << " y2=" << y2LabelWidth << " tickMark=" << m_tickLabelWidth;
		m_tickLabelWidth = qMax(y1LabelWidth, y2LabelWidth);

	} while (m_tickLabelWidth != oldTickLabelWidth && --iter);

	// print warnings if we exceed the boundaries of the chart
	if (m_x[3] >= width())
		qDebug() << "Right chart margin is too small!";
}

void QNANChartWidget::updateChart() {
	if (!m_doUpdate) return;

	recalculateGeometry();

	// update the chart pixmap
	updateChartPixmap(1, NULL);
}

void QNANChartWidget::editChart() {
	QNANChartEditDialog dlg(this);
	dlg.exec();
	// let the designer know that we changed an option
	if (m_inDesigner) {
		setChartSettings( chartSettings() );
	}
	// also update the chart
	updateChart();
}

void QNANChartWidget::editDataSeries() {
	QNANSeriesEditDialog dlg(this);
	dlg.setupDialog();
	if (dlg.exec() == QDialog::Accepted) {
		dlg.storeData();
	}
}

void QNANChartWidget::printChart() {
	QNANPrintChartDialog dlg(this);
	dlg.setupDialog();
	dlg.exec();
}

void QNANChartWidget::exportChartAsPicture() {
	QString fname = QFileDialog::getSaveFileName(this, tr("Export chart as image"), QString(), tr("Images (*.png *.xpm *.bmp *.jpg )"));
	if (fname.isEmpty()) return;
	QImage img(width(), height(), QImage::Format_RGB32);
	QPainter p(&img);
	QPalette pal = palette();
	QBrush b(pal.color(QPalette::Window));
	p.fillRect(0, 0, width(), height(), b);
	drawAll(&p, true, false); // no zoom indicators, please
	img.save(fname);
}

void QNANChartWidget::seriesAxisChanged() {
	QNANAbstractChartSeries * s = dynamic_cast<QNANAbstractChartSeries *>(sender());
	if (s == NULL) {
		qDebug() << "Invalid sender in slot 'QNANChartWidget::seriesAxisChanged'";
		return;
	}
	// first remove the series from all axes
	m_x1AxisSeries.removeAll(s->id());
	m_x2AxisSeries.removeAll(s->id());
	m_y1AxisSeries.removeAll(s->id());
	m_y2AxisSeries.removeAll(s->id());
	// then add it to the corresponding axis again
	if (s->xAxis() == 0)	m_x1AxisSeries.append(s->id());
	else					m_x2AxisSeries.append(s->id());
	if (s->yAxis() == 0)	m_y1AxisSeries.append(s->id());
	else					m_y2AxisSeries.append(s->id());
	updateChart();
}

// *** Private member functions ***

void QNANChartWidget::updateChartPixmap(double scale, QPainter * p) {
	if (!m_doUpdate) return;
	bool pixmapDrawing = true;
	if (p != NULL) {
		pixmapDrawing = false;
	}

//	qDebug() << "[QNANChartWidget::updateChartPixmap]";
	if (!isVisible() && !m_inDesigner) return;
	// here we have already all the coordinates calculated and simply update the pixmap
	int w = static_cast<int>(m_x[3] - m_x[0]);
	int h = static_cast<int>(m_y[5] - m_y[0]);
	Q_ASSERT(w>0);
	Q_ASSERT(h>0);
	QSize s(w,h);
	if (pixmapDrawing) {
		m_chart = QPixmap(s);
		QPalette pal = palette();
		m_chart.fill(pal.color(QPalette::Window) );
	}
//	m_chart.fill(Qt::yellow);

	// get the size of the actual chart area
	QSize chart_size(static_cast<int>(m_x[2]-m_x[1]),
					 static_cast<int>(m_y[3] - m_y[2]) );

	// process all axis and update all min/max/tickmark values etc.

	QFontMetricsF tickLabelFontMetrics(m_tickLabelFont);

//	(*dumpStream) << "Update chart with " << m_seriesData.count() << " series " << endl;

	// y1 axis
	double min_y1, max_y1;
	getDataMinMax(m_y1AxisSeries, min_y1, max_y1, false);
	m_y1Axis->updateAxisScaling(min_y1, max_y1, m_y[3]-m_y[2], tickLabelFontMetrics, false);

	if (m_y2AxisVisible) {
		// y2 axis
		double min_y2, max_y2;
		getDataMinMax(m_y2AxisSeries, min_y2, max_y2, false);
		m_y2Axis->updateAxisScaling(min_y2, max_y2, m_y[3]-m_y[2], tickLabelFontMetrics, false);
	}
	else {
		// we need to rescale the x1 axis, but this time also include the
		// series that are plotted on the x2 axis
		QList<int> allSeries = m_y1AxisSeries;
		allSeries += m_y2AxisSeries;
		getDataMinMax(allSeries, min_y1, max_y1, false);
		m_y1Axis->updateAxisScaling(min_y1, max_y1, m_y[3]-m_y[2], tickLabelFontMetrics, false);
		// and now copy the scaling from the y1 axis
		m_y2Axis->copyFrom(*m_y1Axis);
	}

	// x1 axis
	double min_x1, max_x1;
	getDataMinMax(m_x1AxisSeries, min_x1, max_x1, true);
	m_x1Axis->updateAxisScaling(min_x1, max_x1, m_x[2]-m_x[1], tickLabelFontMetrics, true);


	if (m_x2AxisVisible) {
		// x2 axis
		double min_x2, max_x2;
		getDataMinMax(m_x2AxisSeries, min_x2, max_x2, true);
		m_x2Axis->updateAxisScaling(min_x2, max_x2, m_x[2]-m_x[1], tickLabelFontMetrics, true);
	}
	else {
		// we need to rescale the x1 axis, but this time also include the
		// series that are plotted on the x2 axis
		QList<int> allSeries = m_x1AxisSeries;
		allSeries += m_x2AxisSeries;
		getDataMinMax(allSeries, min_x1, max_x1, true);
		m_x1Axis->updateAxisScaling(min_x1, max_x1, m_x[2]-m_x[1], tickLabelFontMetrics, true);
		// and now copy the scaling from the x1 axis
		m_x2Axis->copyFrom(*m_x1Axis);
	}

	// create a painter
	std::unique_ptr<QPainter> p_ptr;
	if (pixmapDrawing) {
		p_ptr.reset( new QPainter(&m_chart) );
		p = p_ptr.get();
		p->initFrom(this);
	}

	QPen darkcolor(palette().dark().color().dark());
	darkcolor.setWidthF(m_penWidth);
	p->setPen(darkcolor);

	// for the drawing of the actual chart part, translate the painter
	double x1 = m_x[1] - m_x[0];
	double y1 = m_y[2] - m_y[0];
	p->save();
	p->translate(x1, y1);

	QSizeF chartAreaSize(m_x[2]-m_x[1], m_y[3] - m_y[2]);

	// draw each axis
	drawAxis(p, chartAreaSize, m_x1Axis, Bottom);
	drawAxis(p, chartAreaSize, m_x2Axis, Top);
	drawAxis(p, chartAreaSize, m_y1Axis, Left);
	drawAxis(p, chartAreaSize, m_y2Axis, Right);

	drawGrid(p, chartAreaSize);

	p->setClipping(true);
	p->setClipRegion( QRegion(0, 0, int(chartAreaSize.width()), int(chartAreaSize.height())) );
	// draw all the data series
	for (QMap<int, QNANAbstractChartSeries *>::ConstIterator it = m_seriesData.constBegin();
		it != m_seriesData.constEnd(); ++it)
	{
		// set extends depending on axis
		double min_x, max_x, min_y, max_y;
		if (it.value()->xAxis() == 0) {
			min_x = m_x1Axis->minVal();
			max_x = m_x1Axis->maxVal();
		}
		else {
			min_x = m_x2Axis->minVal();
			max_x = m_x2Axis->maxVal();
		}
		if (it.value()->yAxis() == 0) {
			min_y = m_y1Axis->minVal();
			max_y = m_y1Axis->maxVal();
		}
		else {
			min_y = m_y2Axis->minVal();
			max_y = m_y2Axis->maxVal();
		}
		QRectF extends(min_x, min_y, max_x-min_x, max_y-min_y);

		it.value()->paintSeries(p, chartAreaSize, extends);
	}
	p->setClipping(false);

	QPen pen(palette().dark().color().dark());
	pen.setStyle(Qt::SolidLine);
	pen.setWidthF(m_penWidth);
	p->setPen(pen);
	p->drawRect( QRectF(0.0, 0.0, chartAreaSize.width(), chartAreaSize.height()));

	p->restore(); // get original painter location back

	update();
}

void QNANChartWidget::updateZoomIndicator() {
	switch (m_zoomOption) {
		case NoZoom : return;
		case HorizontalZoom : {
			QPoint p1 = m_zoomP1;
			QPoint p2 = m_zoomP2;
			if (p1.x() > p2.x())
				qSwap(p1,p2);
			update(p1.x(), int(m_y[2]), 1, int(m_y[3] - m_y[2]));
			update(p2.x(), int(m_y[2]), 1, int(m_y[3] - m_y[2]));
		} break;
		case VerticalZoom : {
			QPoint p1 = m_zoomP1;
			QPoint p2 = m_zoomP2;
			if (p1.y() > p2.y())
				qSwap(p1,p2);
			update(int(m_x[1]), p1.y(), int(m_x[2] - m_x[1]), 1);
			update(int(m_x[1]), p2.y(), int(m_x[2] - m_x[1]), 1);
		} break;
		case BidirectionalZoom : {
			// update area
			QRect rect(m_zoomP1, m_zoomP2);
			rect = rect.normalized();
			update(rect.left(), rect.top(), rect.width(), 1);
			update(rect.left(), rect.top(), 1, rect.height());
			update(rect.left(), rect.bottom(), rect.width(), 1);
			update(rect.right(), rect.top(), 1, rect.height());
		}
	}
}

void QNANChartWidget::panChart(QPoint dist) {
	m_doUpdate = false;
	// set auto-scaling to false for all axis
	m_x1Axis->setAutomaticScaling(false);
	m_x2Axis->setAutomaticScaling(false);
	m_y1Axis->setAutomaticScaling(false);
	m_y2Axis->setAutomaticScaling(false);

	// now calculate the different pixel-coordinate ratios
	double x_min = m_x1Axis->minVal();
	double x_max = m_x1Axis->maxVal();
	int sx_min = static_cast<int>(m_x[1]);
	int sx_max = static_cast<int>(m_x[2]);
	double scale_x = (x_max - x_min)/(sx_max - sx_min);
	double x_move = scale_x * dist.x();
	m_x1Axis->setMinMaxValues(x_min - x_move, x_max - x_move);

	x_min = m_x2Axis->minVal();
	x_max = m_x2Axis->maxVal();
	scale_x = (x_max - x_min)/(sx_max - sx_min);
	x_move = scale_x * dist.x();
	m_x2Axis->setMinMaxValues(x_min - x_move, x_max - x_move);

	double y_min = m_y1Axis->minVal();
	double y_max = m_y1Axis->maxVal();
	int sy_min = static_cast<int>(m_y[2]);
	int sy_max = static_cast<int>(m_y[3]);
	double scale_y = (y_max - y_min)/(sy_max - sy_min);
	double y_move = scale_y * dist.y();
	m_y1Axis->setMinMaxValues(y_min + y_move, y_max + y_move);

	y_min = m_y2Axis->minVal();
	y_max = m_y2Axis->maxVal();
	scale_y = (y_max - y_min)/(sy_max - sy_min);
	y_move = scale_y * dist.y();
	m_y2Axis->setMinMaxValues(y_min + y_move, y_max + y_move);

	m_doUpdate = true;
	updateChart();
}

void QNANChartWidget::drawAxis(QPainter* p, QSizeF s, QNANChartAxis * axis, AxisLocation loc) {
	QFontMetricsF tickLabelFontMetrics(m_tickLabelFont);
	p->setFont(m_tickLabelFont);
	double text_height = tickLabelFontMetrics.height();

	QPen pen(p->pen());
	pen.setWidthF(m_penWidth);
	p->setPen(pen);

	double delta_minmax = axis->maxVal() - axis->minVal();
	Q_ASSERT(delta_minmax > 0);
	// TODO : the constants need to be resolution dependent
	const int TICK_LENGTH = 5;
	const double H_SPACING	= m_axisSpacing;
	const double V_SPACING	= m_axisSpacing;
	double representative_value = qMax(fabs(axis->minVal()), fabs(axis->maxVal()));
	switch (loc) {
		case Top 	: {
			double factor = s.width()/delta_minmax;
			double x = axis->niceMinVal() - axis->minVal();
			for (int i = 0; i<=axis->numTicks(); ++i) {
				p->drawLine( QLineF(factor*x, 0, factor*x, TICK_LENGTH) );
				if (m_x2AxisVisible) {
					QString label = QNANChartAxis::tickMarkLabel(x + axis->minVal(), representative_value, axis->numberFormat(), axis->precision());
					double labelwidth = tickLabelFontMetrics.boundingRect(label).width() + 0.5*tickLabelFontMetrics.height();
					p->drawText( QRectF(factor*x - labelwidth/2, - V_SPACING - text_height,
										labelwidth, text_height),
								Qt::AlignHCenter | Qt::AlignBottom, label);
				}
				x += axis->tickMarkDistance();
			}
		} break;
		case Bottom	: {
			double factor = s.width()/delta_minmax;
			double x = axis->niceMinVal() - axis->minVal();
			for (int i = 0; i<=axis->numTicks(); ++i) {
				p->drawLine( QLineF(factor*x, s.height()-TICK_LENGTH, factor*x, s.height()) );
				QString label = QNANChartAxis::tickMarkLabel(x + axis->minVal(), representative_value, axis->numberFormat(), axis->precision());
				double labelwidth = tickLabelFontMetrics.boundingRect(label).width() + 0.5*tickLabelFontMetrics.height();
				p->drawText( QRectF(factor*x - labelwidth/2, V_SPACING + s.height(),
									labelwidth, text_height),
							Qt::AlignHCenter | Qt::AlignTop, label);
				x += axis->tickMarkDistance();
			}
		} break;
		case Left 	: {
			double factor = s.height()/delta_minmax;
			double y = axis->niceMinVal() - axis->minVal();
			for (int i = 0; i<=axis->numTicks(); ++i) {
				p->drawLine( QLineF(0, s.height() -factor*y, TICK_LENGTH, s.height() -factor*y) );
				QString label = QNANChartAxis::tickMarkLabel(y + axis->minVal(), representative_value, axis->numberFormat(), axis->precision());
				double labelwidth = tickLabelFontMetrics.boundingRect(label).width() + 0.5*tickLabelFontMetrics.height();
				p->drawText( QRectF(- H_SPACING - labelwidth, s.height() - factor*y - text_height/2,
									labelwidth, text_height),
							Qt::AlignRight | Qt::AlignVCenter, label);
				y += axis->tickMarkDistance();
			}
		} break;
		case Right	: {
			double factor = s.height()/delta_minmax;
			double y = axis->niceMinVal() - axis->minVal();
			for (int i = 0; i<=axis->numTicks(); ++i) {
				p->drawLine( QLineF(s.width()-TICK_LENGTH, s.height() - factor*y, s.width(), s.height() -factor*y) );
				if (m_y2AxisVisible) {
					QString label = QNANChartAxis::tickMarkLabel(y + axis->minVal(), representative_value, axis->numberFormat(), axis->precision());
					double labelwidth = tickLabelFontMetrics.boundingRect(label).width() + 0.5*tickLabelFontMetrics.height();
					p->drawText( QRectF(H_SPACING + s.width(), s.height() - factor*y - text_height/2,
										labelwidth, text_height),
								Qt::AlignLeft | Qt::AlignVCenter, label);
				}
				y += axis->tickMarkDistance();
			}
		} break;
	}
}

void QNANChartWidget::drawGrid(QPainter* p, QSizeF s) {
	QPen pen(palette().dark().color().dark());
	pen.setStyle(Qt::DotLine);
	pen.setWidthF(m_penWidth);
	p->setPen(pen);
	// first vertical grid lines
	double delta_minmax = m_x1Axis->maxVal() - m_x1Axis->minVal();
	Q_ASSERT(delta_minmax > 0);
	double factor = s.width()/delta_minmax;
	double x = m_x1Axis->niceMinVal() - m_x1Axis->minVal();
	for (int i = 0; i <=m_x1Axis->numTicks(); ++i) {
		if (factor*x > 1 && factor*x < s.width()-1)
			p->drawLine( QLineF(factor*x, 0, factor*x, s.height()) );
		x += m_x1Axis->tickMarkDistance();
	}
	// then horizontal lines
	delta_minmax = m_y1Axis->maxVal() - m_y1Axis->minVal();
	Q_ASSERT(delta_minmax > 0);
	factor = s.height()/delta_minmax;
	double y = m_y1Axis->niceMinVal() - m_y1Axis->minVal();
	for (int i = 0; i <= m_y1Axis->numTicks(); ++i) {
		if (factor*y > 1 && factor*y < s.height()-1)
			p->drawLine( QLineF(0, s.height() - factor*y, s.width(), s.height() - factor*y) );
		y += m_y1Axis->tickMarkDistance();
	}
}

void QNANChartWidget::getDataMinMax(const QList<int>& seriesIDs, double & minval, double & maxval, bool xdir) {
	minval = 0;
	maxval = 1;
	// if the axis doesn't have any data assigned, leave the default values and return
	if (seriesIDs.isEmpty())  return;
//	(*dumpStream) << "Getting min/max, # of series = " << seriesIDs.count() << endl;
	// loop over all IDs until the first series is found that exists and use the min/max of this series
	// as initial min/max values
	int i=0;
	for (; i<seriesIDs.count(); ++i) {
		QNANAbstractChartSeries * s = series(seriesIDs[i]);
		if (s != NULL) {
			minval = xdir ? s->minX() : s->minY();
			maxval = xdir ? s->maxX() : s->maxY();
			break;
		}
	}
//	(*dumpStream) << "Initial min/max = " << minval << " .. " << maxval << "  of series " << seriesIDs[i] << endl;
	// now loop over the remaining series and get min/max
	for (; i<seriesIDs.count(); ++i) {
		QNANAbstractChartSeries * s = series(seriesIDs[i]);
		if (s != NULL) {
			minval = qMin(minval, (xdir ? s->minX() : s->minY()) );
			maxval = qMax(maxval, (xdir ? s->maxX() : s->maxY()) );
//			(*dumpStream) << "    min/max test = " << minval << " .. " << maxval << "  of series " << s->id() << endl;
		}
	}
}

void QNANChartWidget::drawAll(QPainter * p, bool drawPixmap, bool drawZoomIndicators) {
	if (drawPixmap)
		p->drawPixmap(static_cast<int>(m_x[0]), static_cast<int>(m_y[0]), m_chart);

	// now draw the axis
	QPen textPen(palette().color(QPalette::Text));
	p->setPen(textPen);
	// draw the axis labels
	//p->drawRect(QRectF(0, m_y[5], width(), height()-m_y[5]));
	p->drawText(QRectF(0, m_y[5], width(), height()-m_y[5]), Qt::AlignHCenter | Qt::AlignTop, m_x1Axis->label());
	if (m_x2AxisVisible) {
		//p->drawRect(QRectF(0, 0, width(), m_y[0]));
		p->drawText(QRectF(0, 0, width(), m_y[0]), Qt::AlignHCenter | Qt::AlignBottom, m_x2Axis->label());
	}
	// rotate the painter
	p->save();
	p->translate(0, m_y[5]);
	p->rotate(270);
	//p->drawRect(QRectF(0,0, m_y[5] - m_y[0], m_x[0]));
	p->drawText(QRectF(0,0, m_y[5] - m_y[0], m_x[0]), Qt::AlignHCenter | Qt::AlignBottom, m_y1Axis->label());
	p->restore();
	if (m_y2AxisVisible) {
		p->save();
		p->translate(m_x[3], m_y[5]);
		p->rotate(270);
		//p->drawRect(QRectF(0,0, m_y[5] - m_y[0], width()-m_x[3]));
		p->drawText(QRectF(0,0, m_y[5] - m_y[0], width()-m_x[3]), Qt::AlignHCenter | Qt::AlignTop, m_y2Axis->label());
		p->restore();
	}

	// finally draw zoom indicators
	if (m_zoomIndicatorShown) {
		QPen pen(palette().dark().color());
		pen.setStyle(Qt::DashDotLine);
		p->setPen(pen);
		switch (m_zoomOption) {
			case HorizontalZoom : {
				// draw vertical lines
				QPoint p1 = m_zoomP1;
				QPoint p2 = m_zoomP2;
				if (p1.x() > p2.x())
					qSwap(p1,p2);
				p->drawLine(p1.x(), int(m_y[2]), p1.x(), int(m_y[3]));
				p->drawLine(p2.x(), int(m_y[2]), p2.x(), int(m_y[3]));
			} break;

			case VerticalZoom : {
				// draw horizontal lines
				QPoint p1 = m_zoomP1;
				QPoint p2 = m_zoomP2;
				if (p1.y() > p2.y())
					qSwap(p1,p2);
				p->drawLine(int(m_x[1]), p1.y(), int(m_x[2]), p1.y());
				p->drawLine(int(m_x[1]), p2.y(), int(m_x[2]), p2.y());
			} break;

			case BidirectionalZoom : {
				QRect zoomIndicatorRect(m_zoomP1, m_zoomP2);
				p->drawRect(zoomIndicatorRect.normalized().adjusted(0,0,-1,-1));
			} break;

			case NoZoom :
			default	    : break;
		}
	}

}
