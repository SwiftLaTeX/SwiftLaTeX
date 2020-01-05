#include "qmessageboxweb.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
void QMessageBoxWeb::warning(QWidget *parent, const QString &title, const QString &text) {
	QMessageBox *mbox = new QMessageBox(parent);
	mbox->setWindowTitle(title);
	mbox->setText(text);
	mbox->setIcon(QMessageBox::Warning);
	mbox->setAttribute(Qt::WA_DeleteOnClose);
	mbox->setModal(true);
	mbox->show();
}



extern "C" {
  extern int javascriptconfirm(const char *text);
  extern int javascriptprompt(const char *text);
}

int QMessageBoxWeb::warningWith3Buttons(QWidget *parent, const QString &title, const QString &text, const QString &b1,  const QString &b2, const QString &b3, int a1, int a2) {
	#ifdef __EMSCRIPTEN__
	if(b3 == QString::null) {
		return javascriptconfirm(text.toStdString().c_str());
	} else {
		return javascriptprompt(text.toStdString().c_str());
	}
	#else
		return QMessageBox::warning(parent, title, text, b1, b2, b3, a1, a2);
	#endif
}