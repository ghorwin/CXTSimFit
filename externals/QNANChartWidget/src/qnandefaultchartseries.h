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

#ifndef qnandefaultchartseries_h
#define qnandefaultchartseries_h

#include <QColor>

class QDomDocument;
class QDomElement;
class QDomNode;

#include "qnanabstractchartseries.h"

/// @brief Default implementation of a line/marker chart series.
/// @author Andreas Nicolai
///
///
class QNANDefaultChartSeries : public QNANAbstractChartSeries {
	Q_OBJECT
public:
	enum SeriesType {
		Line,
		Marker,
		LineAndMarker
	};

	enum LineStyle {
		Solid,
		Dash,
		Dot,
		DashDot
	};

	enum MarkerStyle {
		Circle,
		Box,
		Diamond,
		Triangle
	};

	QNANDefaultChartSeries(QObject * parent = 0);
	virtual ~QNANDefaultChartSeries() {}

	virtual void paintSeries(QPainter * p, QSizeF s, QRectF extends);

	SeriesType		seriesType() const { return m_seriesType; }
	void			setSeriesType(SeriesType t);
	LineStyle		lineStyle() const { return m_lineStyle; }
	void			setLineStyle(LineStyle s);
	MarkerStyle		markerStyle() const { return m_markerStyle; }
	void			setMarkerStyle(MarkerStyle s);
	int				markerSize() const { return m_markerSize; }
	void			setMarkerSize(int s);
	QColor			color() const { return m_color; }
	void			setColor(QColor c);

	/// @brief Loads the series data from an XML document
	virtual bool read(QDomElement in);
	/// @brief Saves the series data into an XML document
	virtual void write(QDomDocument& doc, QDomElement& root) const;

	static QString stringForSeriesType(SeriesType t);
	static QString stringForLineStyle(LineStyle t);
	static QString stringForMarkerStyle(MarkerStyle t);

	enum { NUM_DEFAULT_COLORS = 20 };
	static QColor seriesDefaultColors[NUM_DEFAULT_COLORS];

signals:
	void changed();

private:
	void paintMarker(QPainter * p, double x, double y);

	SeriesType		m_seriesType;
	LineStyle		m_lineStyle;
	MarkerStyle		m_markerStyle;
	int				m_markerSize;
	QColor			m_color;

	static int 		seriesCounter;

};

#endif // qnandefaultchartseries_h
