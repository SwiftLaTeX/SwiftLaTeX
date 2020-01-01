#include <QMessageBox>

class QMessageBoxWeb {
public:
	static void warning(QWidget *parent, const QString &title, const QString &text);
	static int warningWithRet(QWidget *parent, const QString &title, const QString &text, const QString &b1,  const QString &b2, int a0, int a2);
};