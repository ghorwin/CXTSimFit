#include <QtGui>
#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
}

AboutDialog::~AboutDialog()
{
}


void AboutDialog::on_pushButtonAboutQt_clicked() {
	QMessageBox::aboutQt(this);
}
