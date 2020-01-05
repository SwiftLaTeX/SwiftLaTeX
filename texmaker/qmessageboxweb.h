#include <QMessageBox>

class QMessageBoxWeb {
public:
	static void warning(QWidget *parent, const QString &title, const QString &text);
	static int warningWith3Buttons(QWidget *parent, const QString &title, const QString &text, const QString &b1,  const QString &b2, const QString &b3, int a0, int a2);
};