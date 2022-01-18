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

#include "qnanabstractchartseries.h"

#include <QtGui>
#include <QtXml>

#include <cmath>
#include <algorithm>
#include <stdexcept>

QNANAbstractChartSeries::QNANAbstractChartSeries(QObject * parent)
	: QObject(parent), m_xAxis(0), m_yAxis(0),
	  m_numPoints(0), m_xDataPtr(NULL), m_yDataPtr(NULL)
{
	updateMinMaxValues();
}

void QNANAbstractChartSeries::setId(int id) {
	if (id == m_id) return;
	m_id = id;
	emit changed();
}

void QNANAbstractChartSeries::setName(const QString& name) {
	if (m_name == name) return;
	m_name = name;
	emit changed();
}

void QNANAbstractChartSeries::setXAxis(int axis) {
	if (m_xAxis == axis) return;
	m_xAxis = axis;
	emit axisChanged();
}

void QNANAbstractChartSeries::setYAxis(int axis) {
	if (m_yAxis == axis) return;
	m_yAxis = axis;
	emit axisChanged();
}

void QNANAbstractChartSeries::clear() {
	m_xDataPtr = NULL; 
	m_yDataPtr = NULL; 
	m_numPoints = 0;
	m_xData.clear();
	m_yData.clear();
}

void QNANAbstractChartSeries::setData(const DataVec& x_points, const DataVec& y_points) {
	m_xDataPtr = NULL; m_yDataPtr = NULL; m_numPoints = 0;
	m_xData = x_points; m_yData = y_points;
	if (!checkData()) {
		m_xData.clear();
		m_yData.clear();
	}
	updateMinMaxValues();
}

void QNANAbstractChartSeries::setData(int num, double * x_ptr, double * y_ptr) {
	Q_ASSERT(num > 0);
	m_xData.clear(); m_yData.clear();
	m_numPoints = num;
	m_xDataPtr = x_ptr; m_yDataPtr = y_ptr;
	if (!checkData()) {
		m_numPoints = 0;
		m_xDataPtr = m_yDataPtr = NULL;
	}
	updateMinMaxValues();
}

void QNANAbstractChartSeries::setRandomValues() {
	// create 25 random points
	m_xData.clear();
	m_yData.clear();
	double slope;
	double last_y = 15 - 10.0*rand()/RAND_MAX;
	for (int i=0; i<=25; ++i) {
		if (i % 5 == 0)
			slope = 2.0-(4.0*rand())/RAND_MAX;
		double x = i*0.04;
		double y = last_y + slope + 0.5-(1.0*rand())/RAND_MAX;
		m_xData.append(x);
		m_yData.append(y);
		last_y = y;
	}
	updateMinMaxValues();
}

bool QNANAbstractChartSeries::read(QDomElement in) {
	// read properties
	if (in.hasAttribute("ID"))
		m_id = in.attribute("ID").toInt();
	if (in.hasAttribute("XAxis"))
		m_xAxis = in.attribute("XAxis").toInt();
	if (in.hasAttribute("YAxis"))
		m_yAxis = in.attribute("YAxis").toInt();

	// read the base attributes of parent class
	//AbstractChartSeries::write(doc, seriesNode);

	// read axis label
	QDomElement nameElement = in.firstChildElement("Name");
	if (!nameElement.isNull())
		m_name = nameElement.firstChild().nodeValue();
	return true;
}

void QNANAbstractChartSeries::write(QDomDocument& doc, QDomElement& root) const {
	QDomElement seriesNode = doc.createElement("QNANAbstractChartSeries");
	root.appendChild(seriesNode);
	// set attributes
	seriesNode.setAttribute ("ID", QString("%1").arg(m_id));
	seriesNode.setAttribute ("XAxis", QString("%1").arg(m_xAxis));
	seriesNode.setAttribute ("YAxis", QString("%1").arg(m_yAxis));

	// store series name
	QDomElement name = doc.createElement("Name");
	QDomText nameText = doc.createTextNode(m_name);
	name.appendChild(nameText);
	seriesNode.appendChild(name);
}

void QNANAbstractChartSeries::updateMinMaxValues() {
	// if we don't have any data, set 0 and 1 for both axes
	if (m_numPoints == 0 && m_xData.isEmpty()) {
		m_minX = 0;
		m_maxX = 1;
		m_minY = 0;
		m_maxY = 1;
	}
	else {
		if (m_numPoints != 0) {
			m_minX = *std::min_element(m_xDataPtr, m_xDataPtr+m_numPoints);
			m_maxX = *std::max_element(m_xDataPtr, m_xDataPtr+m_numPoints);
			m_minY = *std::min_element(m_yDataPtr, m_yDataPtr+m_numPoints);
			m_maxY = *std::max_element(m_yDataPtr, m_yDataPtr+m_numPoints);
		}
		else {
			m_minX = *std::min_element(m_xData.constBegin(), m_xData.constEnd());
			m_maxX = *std::max_element(m_xData.constBegin(), m_xData.constEnd());
			m_minY = *std::min_element(m_yData.constBegin(), m_yData.constEnd());
			m_maxY = *std::max_element(m_yData.constBegin(), m_yData.constEnd());
		}
	}
}

bool QNANAbstractChartSeries::checkData() {
    bool dataok = true;
    double minval = 1e200;
    double maxval = -1e200;
    int n;
    if (m_xDataPtr != 0) {
        n = m_numPoints;
        //qDebug() << "Pointer based data";
    }
    else {
        n = m_xData.count();
        //qDebug() << "Value based data";
    }
    for (int k=0; k<n; ++k) {
        double x,y;
        if (m_xDataPtr != 0) {
            x = m_xDataPtr[k];
            y = m_yDataPtr[k];
        }
        else {
            x = m_xData[k];
            y = m_yData[k];
        }
        minval = std::min(minval, y);
        maxval = std::max(maxval, y);
        // d==d is false for NaN
        if (!(x == x)) {
            qDebug() << QString("Invalid x data at index %1").arg(k);
            dataok = false;
            break;
        }
        if (!(y == y)) {
            qDebug() << QString("Invalid y data at index %1").arg(k);
            dataok = false;
            break;
        }
    }
    //qDebug() << minval << " .. " << maxval;
    return dataok;
}
