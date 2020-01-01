#include "qmessageboxweb.h"
// #ifdef __EMSCRIPTEN__
// #include <emscripten.h>
// #endif
void QMessageBoxWeb::warning(QWidget *parent, const QString &title, const QString &text) {
	QMessageBox *mbox = new QMessageBox(parent);
	mbox->setWindowTitle(title);
	mbox->setText(text);
	mbox->setIcon(QMessageBox::Warning);
	mbox->setAttribute(Qt::WA_DeleteOnClose);
	mbox->setModal(true);
	mbox->show();
}



int QMessageBoxWeb::warning(QWidget *parent, const QString &title, const QString &text, const QString &b1,  const QString &b2, int a1, int a2) {
	#ifdef __EMSCRIPTEN__
	return 0;
	#else
		return QMessageBox::warning(parent, title, text, b1, b2, a1, a2);
	#endif
}