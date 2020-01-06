#include "qmessageboxweb.h"
extern "C" {
  extern int javascriptconfirm(const char *text, const char*b1, const char*b2);
  extern int javascriptprompt(const char *text, const char*b1, const char*b2, const char*b3);
}

void QMessageBoxWeb::warning(QWidget *parent, const QString &title, const QString &text) {
	QMessageBox *mbox = new QMessageBox(parent);
	mbox->setWindowTitle(title);
	mbox->setText(text);
	mbox->setIcon(QMessageBox::Warning);
	mbox->setAttribute(Qt::WA_DeleteOnClose);
	mbox->setModal(true);
	mbox->show();
}

int QMessageBoxWeb::warningWith3Buttons(QWidget *parent, const QString &title, const QString &text, const QString &b1,  const QString &b2, const QString &b3, int a1, int a2) {
	#ifdef __EMSCRIPTEN__
	const char *textStr = text.toUtf8().constData();
	const char *btn1Str = b1.toUtf8().constData();
	const char *btn2Str = b2.toUtf8().constData();
	if(b3 == QString::null) {
		return javascriptconfirm(textStr, btn1Str, btn2Str);
	} else {
		const char *btn3str = b3.toUtf8().constData();
		return javascriptprompt(textStr, btn1Str, btn2Str, btn3str);
	}
	#else
		return QMessageBox::warning(parent, title, text, b1, b2, b3, a1, a2);
	#endif
}