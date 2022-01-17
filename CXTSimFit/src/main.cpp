#include <QtGui/QApplication>
#include "cxtsimfit.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CXTSimFit w;
	w.resize(1000,600);
	w.show();
	return a.exec();
}
