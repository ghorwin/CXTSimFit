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

#include "qnandefaultchartseries.h"

#include <QtGui>
#include <QtXml>

int QNANDefaultChartSeries::seriesCounter = 0;

// look at http://www.w3.org/TR/SVG/types.html#ColorKeywords for a complete list
QColor QNANDefaultChartSeries::seriesDefaultColors[NUM_DEFAULT_COLORS] =
	{
		"lightslategray",
		"blue",
		"forestgreen",
		"palevioletred",
		"olive",
		"cyan",
		"magenta",
		"goldenrod",
		"chocolate",
		"blueviolet",
		"teal",
		"darkred",
		"darksalmon",
		"gold",
		"grey",
		"khaki",
		"tomato",
		"purple",
		"mediumslateblue",
		"palegreen"
	};

QNANDefaultChartSeries::QNANDefaultChartSeries(QObject * parent)
	: QNANAbstractChartSeries(parent)
{
	m_seriesType = Line;
	m_lineStyle = Solid;
	m_markerStyle = Circle;
	m_markerSize = 5;
	m_color = seriesDefaultColors[seriesCounter % NUM_DEFAULT_COLORS];
	m_id = seriesCounter;
	m_name = QString("Data series %1").arg(++seriesCounter);
}

void QNANDefaultChartSeries::paintSeries(QPainter * p, QSizeF s, QRectF extends) {
	// the coordinate system is at the top left corner of the chart

	// if series is empty, return
	if (numPoints() < 2 && xData().count() < 2) return;
	// calculate scale factors
	double fx = s.width()/extends.width();
	double fy = s.height()/extends.height();
	// set options
	QPen pen;
	pen.setColor(m_color);
	// if we draw a line
	if (m_seriesType==Line || m_seriesType==LineAndMarker) {
		switch (m_lineStyle) {
			case Solid 		: pen.setStyle(Qt::SolidLine); break;
			case Dash 		: pen.setStyle(Qt::DashLine); break;
			case Dot 		: pen.setStyle(Qt::DotLine); break;
			case DashDot	: pen.setStyle(Qt::DashDotLine); break;
		}
		p->setPen(pen);
		// loop over the data and plot it
		if (numPoints() != 0) {
			// draw using the external data
			double * x = xDataPtr();
			double * y = yDataPtr();
			int nPoints = numPoints();
			for (int i=1; i<nPoints; ++i) {
				double x1 = x[i-1] - extends.left();
				double x2 = x[i] - extends.left();
				double y1 = extends.bottom() - y[i-1];
				double y2 = extends.bottom() - y[i];
				p->drawLine( QPointF(fx*x1, fy*y1), QPointF(fx*x2, fy*y2) );
			}
		}
		else {
			Q_ASSERT(xData().count() == yData().count());
			// draw using the internal data vectors
			for (int i=1; i<xData().count(); ++i) {
				double x1 = xData()[i-1] - extends.left();
				double x2 = xData()[i] - extends.left();
				double y1 = extends.bottom() - yData()[i-1];
				double y2 = extends.bottom() - yData()[i];
				Q_ASSERT(x1 > -1e200 && x1 < 1e200);
				Q_ASSERT(y1 > -1e200 && y1 < 1e200);
				Q_ASSERT(x2 > -1e200 && x2 < 1e200);
				Q_ASSERT(y2 > -1e200 && y2 < 1e200);
				p->drawLine( QPointF(fx*x1, fy*y1), QPointF(fx*x2, fy*y2) );
			}
		}
	}
	if (m_seriesType==Marker || m_seriesType==LineAndMarker) {
		pen.setStyle(Qt::SolidLine);
		p->setPen(pen);
		// loop over the data and plot it
		if (numPoints() != 0) {
			// draw using the external data
			double * x = xDataPtr();
			double * y = yDataPtr();
			int nPoints = numPoints();
			for (int i=0; i<nPoints; ++i) {
				double xp = x[i] - extends.left();
				double yp = extends.bottom() - y[i];
				paintMarker(p, fx*xp, fy*yp);
			}
		}
		else {
			Q_ASSERT(xData().count() == yData().count());
			// draw using the internal data vectors
			for (int i=0; i<xData().count(); ++i) {
				double xp = xData()[i] - extends.left();
				double yp = extends.bottom() - yData()[i];
				paintMarker(p, fx*xp, fy*yp);
			}
		}
	}
}

void QNANDefaultChartSeries::setSeriesType(SeriesType t) {
	m_seriesType = t;
	emit changed();
}

void QNANDefaultChartSeries::setLineStyle(LineStyle s) {
	m_lineStyle = s;
	emit changed();
}

void QNANDefaultChartSeries::setMarkerStyle(MarkerStyle s) {
	m_markerStyle = s;
	emit changed();
}

void QNANDefaultChartSeries::setMarkerSize(int s) {
	m_markerSize = s;
	emit changed();
}

void QNANDefaultChartSeries::setColor(QColor c) {
	m_color = c;
	emit changed();
}

bool QNANDefaultChartSeries::read(QDomElement in) {
	// read properties
	if (in.hasAttribute("SeriesType"))
		m_seriesType = static_cast<QNANDefaultChartSeries::SeriesType>(in.attribute("SeriesType").toInt() );
	if (in.hasAttribute("LineStyle"))
		m_lineStyle = static_cast<QNANDefaultChartSeries::LineStyle>(in.attribute("LineStyle").toInt() );
	if (in.hasAttribute("MarkerStyle"))
		m_markerStyle = static_cast<QNANDefaultChartSeries::MarkerStyle>(in.attribute("MarkerStyle").toInt() );
	if (in.hasAttribute("MarkerSize"))
		m_markerSize = in.attribute("MarkerSize").toInt();

	// read the base attributes of parent class
	QDomElement baseElement = in.firstChildElement("QNANAbstractChartSeries");
	QNANAbstractChartSeries::read(baseElement);

	// read color
	QDomElement colorElement = in.firstChildElement("Color");
	if (!colorElement.isNull()) {
		QString rgbColor = colorElement.firstChild().nodeValue();
		QTextStream strm(&rgbColor, QIODevice::ReadOnly);
		int r=0, g=0, b=0;
		strm >> r >> g >> b;
		m_color.setRgb(r, g, b);
	}
	return true;
}

void QNANDefaultChartSeries::write(QDomDocument& doc, QDomElement& root) const {
	QDomElement seriesNode = doc.createElement("QNANDefaultChartSeries");
	root.appendChild(seriesNode);

	// write the base attributes of parent class
	QNANAbstractChartSeries::write(doc, seriesNode);

	// set attributes
	seriesNode.setAttribute ("SeriesType", QString("%1").arg(m_seriesType));
	seriesNode.setAttribute ("LineStyle", QString("%1").arg(m_lineStyle));
	seriesNode.setAttribute ("MarkerStyle", QString("%1").arg(m_markerStyle));
	seriesNode.setAttribute ("MarkerSize", QString("%1").arg(m_markerSize));

	// store properties
	QDomElement colorNode = doc.createElement("Color");
	QDomText colorText = doc.createTextNode(QString("%1 %2 %3").arg(m_color.red()).arg(m_color.green()).arg(m_color.blue()) );
	colorNode.appendChild(colorText);
	seriesNode.appendChild(colorNode);
}


// *** Static member functions ***

QString QNANDefaultChartSeries::stringForSeriesType(SeriesType t) {
	switch (t) {
		case Line			: return tr("Line series");
		case Marker			: return tr("Markers");
		case LineAndMarker	: return tr("Line and markers");
		default				: return tr("<please select type>");
	}
}

QString QNANDefaultChartSeries::stringForLineStyle(LineStyle t) {
	switch (t) {
		case Solid			: return tr("Solid");
		case Dash			: return tr("Dash");
		case Dot			: return tr("Dot");
		case DashDot		: return tr("DashDot");
		default				: return tr("<please select type>");
	}

}

QString QNANDefaultChartSeries::stringForMarkerStyle(MarkerStyle t) {
	switch (t) {
		case Circle			: return tr("Circle");
		case Box			: return tr("Box");
		case Diamond		: return tr("Diamond");
		case Triangle		: return tr("Triangle");
		default				: return tr("<please select type>");
	}
}

// *** private member functions ***

void QNANDefaultChartSeries::paintMarker(QPainter * p, double x, double y) {
	int markersize = qMax(1, m_markerSize);
	switch (m_markerStyle) {
		case Circle :
			p->drawEllipse( QRectF(x-0.5*markersize, y-0.5*markersize, markersize, markersize) );
			break;
		case Box :
			p->drawRect( QRectF(x-0.5*markersize, y-0.5*markersize, markersize, markersize) );
			break;
		case Diamond : 
			{
				p->drawLine( QPointF(x-0.5*markersize, y), QPointF(x, y-0.5*markersize) );
				p->drawLine( QPointF(x, y-0.5*markersize), QPointF(x+0.5*markersize, y) );
				p->drawLine( QPointF(x+0.5*markersize, y), QPointF(x, y+0.5*markersize) );
				p->drawLine( QPointF(x, y+0.5*markersize), QPointF(x-0.5*markersize, y) );
			}
			break;
		case Triangle : 
			{
				double a_half = 1/1.7320508; // = 1/sqrt(3)
				p->drawLine( QPointF(x, y-0.5*markersize), QPointF(x+a_half*markersize, y+0.5*markersize) );
				p->drawLine( QPointF(x+a_half*markersize, y+0.5*markersize), QPointF(x-a_half*markersize, y+0.5*markersize) );
				p->drawLine( QPointF(x-a_half*markersize, y+0.5*markersize), QPointF(x, y-0.5*markersize) );
			}
			break;
	}
}


