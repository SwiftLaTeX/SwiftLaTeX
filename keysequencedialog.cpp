/***************************************************************************
 *   copyright       : (C) 2003-2017 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keysequencedialog.h"
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QContextMenuEvent>




KeySequenceDialog::KeySequenceDialog(QWidget *parent)
    : QDialog( parent), m_num(0)
{
setModal(true);
ui.setupUi(this);
ui.lineEdit->installEventFilter(this);
ui.lineEdit->setReadOnly(true);
ui.lineEdit->setFocusProxy(this);
setFocusPolicy(ui.lineEdit->focusPolicy());
setAttribute(Qt::WA_InputMethodEnabled);
connect( ui.pushButtonClear, SIGNAL( clicked() ), this, SLOT( slotClearShortcut() ) );
}
KeySequenceDialog::~KeySequenceDialog()
{
}

bool KeySequenceDialog::eventFilter(QObject *o, QEvent *e)
{
if (o == ui.lineEdit && e->type() == QEvent::ContextMenu) {
    QContextMenuEvent *c = static_cast<QContextMenuEvent *>(e);
    QMenu *menu = new QMenu(ui.lineEdit);
    QAction *clearAction = new QAction(tr("Clear"), menu);
    menu->addAction(clearAction);
    clearAction->setEnabled(!m_keySequence.isEmpty());
    connect(clearAction, SIGNAL(triggered()), this, SLOT(slotClearShortcut()));
    menu->exec(c->globalPos());
    delete menu;
    e->accept();
    return true;
}

return QWidget::eventFilter(o, e);
}

void KeySequenceDialog::slotClearShortcut()
{
    if (m_keySequence.isEmpty())
        return;
    setKeySequence(QKeySequence());
    emit keySequenceChanged(m_keySequence);
}

void KeySequenceDialog::handleKeyEvent(QKeyEvent *e)
{
    int nextKey = e->key();
    if (nextKey == Qt::Key_Control || nextKey == Qt::Key_Shift ||
            nextKey == Qt::Key_Meta || nextKey == Qt::Key_Alt ||
            nextKey == Qt::Key_Super_L || nextKey == Qt::Key_AltGr)
        return;

    nextKey |= translateModifiers(e->modifiers(), e->text());
    int k0 = m_keySequence[0];
    int k1 = m_keySequence[1];
    int k2 = m_keySequence[2];
    int k3 = m_keySequence[3];
    switch (m_num) {
        case 0: k0 = nextKey; k1 = 0; k2 = 0; k3 = 0; break;
        case 1: k1 = nextKey; k2 = 0; k3 = 0; break;
        case 2: k2 = nextKey; k3 = 0; break;
        case 3: k3 = nextKey; break;
        default: break;
    }
    ++m_num;
    if (m_num > 3)
        m_num = 0;
    m_keySequence = QKeySequence(k0, k1, k2, k3);
    ui.lineEdit->setText(m_keySequence.toString(QKeySequence::PortableText));
    e->accept();
    emit keySequenceChanged(m_keySequence);
}

void KeySequenceDialog::setKeySequence(const QKeySequence &sequence)
{
    if (sequence == m_keySequence)
        return;
    m_num = 0;
    m_keySequence = sequence;
    ui.lineEdit->setText(m_keySequence.toString(QKeySequence::PortableText));
}

QKeySequence KeySequenceDialog::keySequence() const
{
    return m_keySequence;
}

int KeySequenceDialog::translateModifiers(Qt::KeyboardModifiers state, const QString &text) const
{
    int result = 0;
    if ((state & Qt::ShiftModifier) && (text.size() == 0 || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
        result |= Qt::SHIFT;
    if (state & Qt::ControlModifier)
        result |= Qt::CTRL;
    if (state & Qt::MetaModifier)
        result |= Qt::META;
    if (state & Qt::AltModifier)
        result |= Qt::ALT;
    return result;
}

void KeySequenceDialog::focusInEvent(QFocusEvent *e)
{
    ui.lineEdit->event(e);
    ui.lineEdit->selectAll();
    QWidget::focusInEvent(e);
}

void KeySequenceDialog::focusOutEvent(QFocusEvent *e)
{
    m_num = 0;
    ui.lineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void KeySequenceDialog::keyPressEvent(QKeyEvent *e)
{
    handleKeyEvent(e);
    e->accept();
}

void KeySequenceDialog::keyReleaseEvent(QKeyEvent *e)
{
    ui.lineEdit->event(e);
}

bool KeySequenceDialog::event(QEvent *e)
{
    if (e->type() == QEvent::Shortcut ||
            e->type() == QEvent::ShortcutOverride  ||
            e->type() == QEvent::KeyRelease) {
        e->accept();
        return true;
    }
    return QWidget::event(e);
}
