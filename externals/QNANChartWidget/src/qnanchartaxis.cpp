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

#include <QtGui>

#include "qnanchartaxis.h"
#include <cmath>

#include <QtXml>

QNANChartAxis::QNANChartAxis(QObject * parent) 
	: QObject(parent), m_automaticScaling(true), m_automaticTickMarkDistance(true), m_numberFormat('g'), m_precision(2)
{
//	qDebug() << "[QNANChartAxis::QNANChartAxis]";
}

void QNANChartAxis::setLabel(const QString& s) {
	m_label = s;
	emit axisChanged();
}

void QNANChartAxis::updateAxisScaling(double minval, double maxval, 
	double length, const QFontMetricsF& fm, bool aligned)
{
//	qDebug() << "[QNANChartAxis::updateAxisScaling]";
	if (m_automaticScaling) {
		m_minVal = minval;
		m_maxVal = maxval;
	}

	// we need to make sure that m_minVal < m_maxVal !!!
	if (m_minVal >= m_maxVal) {
		// lets first set some special cases
		if (m_minVal == 0) {
			m_maxVal = 1;
		}
		else {
			// determine order of magnitude
			double order = floor(log10(fabs(minval)));
			// determine the offset with one less order of magnitude
			double offset = pow(10, order-1);
			// set new min/max values
			m_minVal -= 0.5*offset;
			m_maxVal += 0.5*offset;
		}
	}

	// get nice min and max values
	
	// if we are using automatic scaling, the nice min and max values
	// are going to be bigger than the current min and max values
	// otherwise they will potentially  be smaller
	calculateNiceMinMaxValues(length, fm, aligned);	
//	qDebug() << "Axis " << m_label << " has new min/max : " << m_minVal << " / " << maxval;
}

void QNANChartAxis::copyFrom(const QNANChartAxis& axis) {
	if (m_automaticScaling) {
		m_minVal = axis.m_minVal;
		m_maxVal = axis.m_maxVal;
	}

	m_niceMinVal = axis.m_niceMinVal;
	m_niceMaxVal = axis.m_niceMaxVal;
	m_tickMarkDistance = axis.m_tickMarkDistance;
	m_numTicks = axis.m_numTicks;
}

void QNANChartAxis::setAutomaticScaling(bool autoscale) {
	if (m_automaticScaling != autoscale) {
		m_automaticScaling	= autoscale;
		emit axisChanged();
	}
}

void QNANChartAxis::setMinVal(double minval) {
	m_minVal = minval;
	if (!m_automaticScaling)	emit axisChanged();
}
	
void QNANChartAxis::setMaxVal(double maxval) {
	m_maxVal = maxval;
	if (!m_automaticScaling)	emit axisChanged();
}

void QNANChartAxis::setMinMaxValues(double minval, double maxval) {
	m_maxVal = maxval;
	m_minVal = minval;
	if (!m_automaticScaling)	emit axisChanged();
}

void QNANChartAxis::setTickMarkDistance(double step) {
	Q_ASSERT(step > 0);
	m_tickMarkDistance = step;
	if (!m_automaticScaling)	emit axisChanged();
}

void QNANChartAxis::setNumberFormat(char f) {
	if (f != m_numberFormat) {
		m_numberFormat = f;
		emit axisChanged();
	}
}

void QNANChartAxis::setAutomaticTickMarkDistance(bool on) {
	m_automaticTickMarkDistance = on;
	if (!m_automaticTickMarkDistance)  emit axisChanged();
}

void QNANChartAxis::setPrecision(int p) {
	if (p != m_precision) {
		m_precision = p;
		emit axisChanged();
	}
}

void QNANChartAxis::read(QDomElement in) {
	// read properties
	if (in.hasAttribute("AutomaticScaling"))
		m_automaticScaling = in.attribute("AutomaticScaling").toInt();
	if (in.hasAttribute("Min"))
		m_minVal = in.attribute("Min").toDouble();
	if (in.hasAttribute("Max"))
		m_maxVal = in.attribute("Max").toDouble();
	if (in.hasAttribute("TickStep")) {
		m_tickMarkDistance = in.attribute("TickStep").toDouble();
		m_automaticTickMarkDistance = false;
	}
	else {
		m_automaticTickMarkDistance = true;
	}
	if (in.hasAttribute("NumberFormat")) {
		std::string s = in.attribute("NumberFormat").toStdString();
		if (!s.empty())
			m_numberFormat = s[0];
	}
	if (in.hasAttribute("Precision"))
		m_precision = in.attribute("Precision").toInt();

	// read axis label
	QDomElement labelElement = in.firstChildElement("Label");
	if (!labelElement.isNull())
		m_label = labelElement.firstChild().nodeValue();
}

void QNANChartAxis::write(const QString& axisName, QDomDocument& doc, QDomElement& root) const {
	QDomElement chartNode = doc.createElement("QNANChartAxis");
	// store axis name as attribute
	root.appendChild(chartNode);
	chartNode.setAttribute ("Name", axisName);
	chartNode.setAttribute ("AutomaticScaling", QString("%1").arg(m_automaticScaling));
	if (!m_automaticScaling) {
		chartNode.setAttribute ("Min", QString("%1").arg(m_minVal));
		chartNode.setAttribute ("Max", QString("%1").arg(m_maxVal));
	}
	// we don't need to write the flag itself, because per definition a given
	// TickMark attribute means user defined tick mark distance
	if (!m_automaticTickMarkDistance)
		chartNode.setAttribute ("TickStep", QString("%1").arg(m_tickMarkDistance));
	chartNode.setAttribute ("NumberFormat", QString("%1").arg(m_numberFormat));
	chartNode.setAttribute ("Precision", QString("%1").arg(m_precision));

	// store axis label
	QDomElement label = doc.createElement("Label");
	QDomText labelText = doc.createTextNode(m_label);
	label.appendChild(labelText);
	chartNode.appendChild(label);	
}

void QNANChartAxis::zoom(double newMin, double newMax) {
	setAutomaticScaling(false);
	if (newMin >= newMax) {
		// zoom out to maximum
		setAutomaticScaling(true);
	}
	else {
		// zoom in
		setMinVal( newMin );
		setMaxVal( newMax );
	}
	emit zoomed(newMin, newMax);
}

void QNANChartAxis::pan(double dist) {
	emit panned(dist);
}


QString QNANChartAxis::tickMarkLabel(double val, double other_val, char f, int p) {
	// special treatment for values near but not quite zero
	// TODO : this algorithm can probably be improved
	if (val != 0 && other_val != 0 && f=='g') {
		double other_order = log10(fabs(other_val));
		double order = log10(fabs(val));
		if (other_order > 4 + order) {
			val = 0.0;
		}
	}
	QString label = QString("%1").arg(val, 0, f, p);
	return label;
}


// *** private member function implementations ***

void QNANChartAxis::calculateNiceMinMaxValues(double length, const QFontMetricsF& fm, bool aligned) {
	// different algoritm based on the automatic scaling flags
	
	// four different combinations:
	// 1. fully automatic axis scaling/labeling algorithm
	//    - m_minVal and m_maxVal are used 
	//    - m_tickMarkDistance, m_niceMinVal and m_niceMaxVal are determined
	//    - m_minVal and m_maxVal are set to m_niceMinVal and m_niceMaxVal
	// 2. automatic axis limits but fixed step
	//    - m_minVal and m_maxVal are used first
	//    - m_tickMarkDistance is used without modifications
	//    - m_niceMinVal and m_niceMaxVal are determined
	//    - m_minVal and m_maxVal are set to m_niceMinVal and m_niceMaxVal
	// 3. fixed axis limits but automatic step
	//    - m_minVal and m_maxVal are unchanged
	//    - m_tickMarkDistance, m_niceMinVal and m_niceMaxVal are determined
	// 4. completely user defined axis scaling/labeling
	//    - m_tickMarkDistance is used without modification
	//    - m_minVal and m_maxVal are unchanged
	//    - m_niceMinVal and m_niceMaxVal are determined
	
	double rounded_step;
	if (m_automaticTickMarkDistance || m_automaticScaling) {
		// aligned with the axis?
		if (aligned) {
			// we need to estimate the tick distance based on the label width
			double label_width = fm.width("10.123")*1.2;
			int ticks = static_cast<int>(length/label_width);
			double tickPixelDistance;
			do {
				double step = (m_maxVal - m_minVal) / ticks;
				if (m_automaticTickMarkDistance) {
					rounded_step = std::pow(10.0, std::floor(std::log10(step)));
//					qDebug() << "ticks = " << ticks << " step = " << step << " rounded step = " << rounded_step;
				
					if (5*rounded_step < step) {
						rounded_step *= 5;
					}
					else if (2*rounded_step < step) {
						rounded_step *= 2;
					}
				}
				else {
					rounded_step = m_tickMarkDistance;
					if (rounded_step <= 0 || int( (m_maxVal - m_minVal)/rounded_step) > 1000) {
//						qDebug() << "Invalid tick mark distance (aligned) = " << m_tickMarkDistance;
						rounded_step = std::pow(10.0, std::floor(std::log10(step)));
					}
				}
				m_numTicks = static_cast<int>(std::ceil( m_maxVal/rounded_step ) - std::floor(m_minVal/rounded_step));
				m_niceMinVal = std::floor(m_minVal/rounded_step) * rounded_step;
				m_niceMaxVal = std::ceil(m_maxVal/rounded_step) * rounded_step;
				m_tickMarkDistance = rounded_step;
				tickPixelDistance = m_tickMarkDistance/(m_maxVal - m_minVal)*length;
				if (m_automaticScaling) {
					m_minVal = m_niceMinVal;
					m_maxVal = m_niceMaxVal;
				}
				else {
					// make sure that our nice values are not outside the min/max values
					if (m_niceMaxVal - 1e-13*m_niceMaxVal > m_maxVal) {
						--m_numTicks;
						m_niceMaxVal -= m_tickMarkDistance;
					}
					if (m_niceMinVal + 1e-13*m_niceMinVal < m_minVal) {
						--m_numTicks;
						m_niceMinVal += m_tickMarkDistance;
					}
				}

				// now check if the labels are longer than the estimated ones
				double x = m_niceMinVal - m_minVal;
				label_width = 0;
				for (int i = 0; i<=m_numTicks; ++i) {
					QString label = QString("%1").arg(x + m_minVal, 0, m_numberFormat, m_precision);
					label_width = qMax(label_width, fm.width(label)*1.2);
					x += m_tickMarkDistance;
				}
				--ticks;
			} while (tickPixelDistance < label_width && m_numTicks > 1);
		}
		else {
			int ticks = static_cast<int>(length/(4.5*fm.height()));
			double step = (m_maxVal - m_minVal) / ticks;
			if (m_automaticTickMarkDistance) {
				// let's use the font height as indicator and have 4.5 times the font height
				rounded_step = std::pow(10.0, std::floor(std::log10(step)));
				
				if (5*rounded_step < step) {
					rounded_step *= 5;
				}
				else if (2*rounded_step < step) {
					rounded_step *= 2;
				}
			}
			else {
				rounded_step = m_tickMarkDistance;
				if (rounded_step <= 0 || int( (m_maxVal - m_minVal)/rounded_step) > 1000) {
					qDebug() << "Invalid tick mark distance = " << m_tickMarkDistance;
					rounded_step = std::pow(10.0, std::floor(std::log10(step)));
				}
			}
		}
		m_numTicks = static_cast<int>(std::ceil( m_maxVal/rounded_step ) - std::floor(m_minVal/rounded_step));
		m_niceMinVal = std::floor(m_minVal/rounded_step) * rounded_step;
		m_niceMaxVal = std::ceil(m_maxVal/rounded_step) * rounded_step;
		m_tickMarkDistance = rounded_step;
		if (m_automaticScaling) {
			m_minVal = m_niceMinVal;
			m_maxVal = m_niceMaxVal;
		}
		else {
			// make sure that our nice values are not outside the min/max values
			if (m_niceMaxVal - 1e-13*m_niceMaxVal > m_maxVal) {
				--m_numTicks;
				m_niceMaxVal -= m_tickMarkDistance;
			}
			if (m_niceMinVal + 1e-13*m_niceMinVal < m_minVal) {
				--m_numTicks;
				m_niceMinVal += m_tickMarkDistance;
			}
		}
	}
	else {
		// 4. completely user defined scaling
		m_numTicks = static_cast<int>(std::floor( m_maxVal/m_tickMarkDistance ) - std::ceil(m_minVal/m_tickMarkDistance));
		m_niceMinVal = std::ceil(m_minVal/m_tickMarkDistance) * m_tickMarkDistance;
		m_niceMaxVal = std::floor(m_maxVal/m_tickMarkDistance) * m_tickMarkDistance;
	}
	// final correction (why?)
//	if (m_numTicks*m_tickMarkDistance + m_niceMinVal < m_niceMaxVal - 1e-6) 
//		++m_numTicks;
}
