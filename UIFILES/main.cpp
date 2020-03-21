#include "ApplicationGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ApplicationGUI w;
	w.show();
	return a.exec();
}
