/***************************************************************************
 *   copyright       : (C) 2007-2017 by Pascal Brachet                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "spellerdialog.h"

#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>
#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>
#include <QTextCodec>
#include <QTextStream>
#include <QTextCharFormat>
#include <QFileInfo>
#include <QApplication>
#include "blockdata.h"

SpellerDialog::SpellerDialog(QWidget *parent,LatexEditor *ed,QString ignoredWords)
    :QDialog( parent)
{
editor=ed;

ui.setupUi(this);
setModal(true);

pChecker = editor->pChecker;
if (pChecker) spell_encoding=QString(pChecker->get_dic_encoding());

connect(ui.pushButtonIgnore, SIGNAL(clicked()), this, SLOT(slotIgnore()));
connect(ui.pushButtonAlwaysIgnore, SIGNAL(clicked()), this, SLOT(slotAlwaysIgnore()));
connect(ui.pushButtonReplace, SIGNAL(clicked()), this, SLOT(slotReplace()));
connect(ui.listWidget, SIGNAL(itemSelectionChanged()),this, SLOT(updateItem()));

if (!ignoredWords.isEmpty()) alwaysignoredwordList=ignoredWords.split(",");
else alwaysignoredwordList.clear();
ignoredwordList=alwaysignoredwordList;
QFile wordsfile(":/spell/spellignore.txt");
QString line;
if (wordsfile.open(QFile::ReadOnly))
    {
    while (!wordsfile.atEnd()) 
	    {
	    line = wordsfile.readLine();
	    if (!line.isEmpty()) hardignoredwordList.append(line.trimmed());
	    }
    }
ui.listWidget->setEnabled(false);
ui.lineEditNew->setEnabled(false);
ui.pushButtonIgnore->setEnabled(false);
ui.pushButtonAlwaysIgnore->setEnabled(false);
ui.pushButtonReplace->setEnabled(false);
ui.lineEditOriginal->setEnabled(false);
ui.lineEditOriginal->clear();
ui.listWidget->clear();
ui.lineEditNew->clear();
show();
c = editor->textCursor();
spellingInit();
//QFileInfo fi(SpellDic);
//if (fi.exists() && fi.isReadable()) spellingInit();
//else ui.labelMessage->setText("<b>"+tr("Error : Can't open the dictionary")+"</b>");
}

SpellerDialog::~SpellerDialog(){
//delete pChecker;
}

void SpellerDialog::closeEvent( QCloseEvent* ce )
{
QTextCursor cursor=editor->textCursor();
cursor.setPosition(startpos,QTextCursor::MoveAnchor);
editor->setTextCursor(cursor);
ce->accept();
}

void SpellerDialog::accept()
{
QTextCursor cursor=editor->textCursor();
cursor.setPosition(startpos,QTextCursor::MoveAnchor);
editor->setTextCursor(cursor);
QDialog::accept();
}

void SpellerDialog::updateItem()
{
int current=-1;
QList<QListWidgetItem *> items;
items = ui.listWidget->selectedItems();
if (items.count() > 0) 
	{
	ui.listWidget->setCurrentItem(items[0]);
	current=ui.listWidget->row(items[0]);
	}
if (current>=0) 
	{
	ui.lineEditNew->setText(ui.listWidget->currentItem()->text());
	}
}


void SpellerDialog::spellingInit()
{
deltacol=0;
go=true;
if (c.hasSelection()) 
	{
	ui.labelMessage->setText(tr("Check spelling selection..."));
	startpos=c.selectionStart();
	endpos=c.selectionEnd();
	c.setPosition(endpos,QTextCursor::MoveAnchor);
	c.setPosition(startpos,QTextCursor::MoveAnchor);
	SpellingNextWord();
	}
else
	{
//	c.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
	ui.labelMessage->setText(tr("Check spelling from cursor..."));
	startpos=c.position();
	c.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
	endpos=c.position();
	c.setPosition(startpos,QTextCursor::MoveAnchor);
	SpellingNextWord();
	}
}

void SpellerDialog::slotIgnore()
{
ignoredwordList.append(ui.lineEditOriginal->text());
ui.listWidget->setEnabled(false);
ui.lineEditNew->setEnabled(false);
ui.pushButtonIgnore->setEnabled(false);
ui.pushButtonAlwaysIgnore->setEnabled(false);
ui.pushButtonReplace->setEnabled(false);
ui.lineEditOriginal->setEnabled(false);
ui.lineEditOriginal->clear();
ui.listWidget->clear();
ui.lineEditNew->clear();
ui.labelMessage->setText("<b>"+tr("No more misspelled words")+"</b>");
SpellingNextWord();
}

void SpellerDialog::slotAlwaysIgnore()
{
alwaysignoredwordList.append(ui.lineEditOriginal->text());
ignoredwordList.append(ui.lineEditOriginal->text());
ui.listWidget->setEnabled(false);
ui.lineEditNew->setEnabled(false);
ui.pushButtonIgnore->setEnabled(false);
ui.pushButtonAlwaysIgnore->setEnabled(false);
ui.pushButtonReplace->setEnabled(false);
ui.lineEditOriginal->setEnabled(false);
ui.lineEditOriginal->clear();
ui.listWidget->clear();
ui.lineEditNew->clear();
ui.labelMessage->setText("<b>"+tr("No more misspelled words")+"</b>");
SpellingNextWord();
}

void SpellerDialog::slotReplace()
{
QString selectedword="";
QTextCursor cursor=editor->textCursor();
if (cursor.hasSelection()) selectedword=cursor.selectedText();
QString newword=ui.lineEditNew->text();
if (!newword.isEmpty()) 
	{
	deltacol=deltacol+newword.length()-selectedword.length();
	editor->replace(newword,false,"");
	}
ui.listWidget->setEnabled(false);
ui.lineEditNew->setEnabled(false);
ui.pushButtonIgnore->setEnabled(false);
ui.pushButtonAlwaysIgnore->setEnabled(false);
ui.pushButtonReplace->setEnabled(false);
ui.lineEditOriginal->setEnabled(false);
ui.lineEditOriginal->clear();
ui.listWidget->clear();
ui.lineEditNew->clear();
ui.labelMessage->setText("<b>"+tr("No more misspelled words")+"</b>");
SpellingNextWord();
}

void SpellerDialog::SpellingNextWord()
{
/*QApplication::setOverrideCursor(Qt::WaitCursor);
BlockData* data;
QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
QByteArray encodedString;
QTextBlock block;
QString text,word;
bool gonext=true;
QByteArray t;
int li,cols,cole,colstart,colend,check,ns;
char ** wlst;
QStringList suggWords;
while(gonext && c.position() < endpos+deltacol && go)
	{
	ui.listWidget->setEnabled(false);
	ui.lineEditNew->setEnabled(false);
	ui.pushButtonIgnore->setEnabled(false);
	ui.pushButtonAlwaysIgnore->setEnabled(false);
	ui.pushButtonReplace->setEnabled(false);
	ui.lineEditOriginal->setEnabled(false);
	ui.lineEditOriginal->clear();
	ui.listWidget->clear();
	ui.lineEditNew->clear();
	ui.labelMessage->setText("<b>"+tr("No more misspelled words")+"</b>");

//	c.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);
	c.movePosition(QTextCursor::StartOfWord,QTextCursor::MoveAnchor);
	data = (BlockData*)c.block().userData();
	li=c.blockNumber();
	block=c.block();
	colstart=c.position()-block.position();
	c.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
	block=c.block();
	colend=c.position()-block.position()-1;
	cols=colstart;
	text=c.selectedText();
	while ((cols<colend && cols<data->code.count() && data->code[cols]==1) || text.mid(cols-colstart,1)==" " || text.mid(cols-colstart,1)=="\t" )
		{
		cols++;
		}
	cole=colend;
	while (cole>colstart && cole<data->code.count() && data->code[cole]==1)
		{
		cole--;
		}
	if (text.length()>1 && cole>cols)
		{
		word=text.mid(cols-colstart,cole-cols+1);
		if (!ignoredwordList.contains(word) && !hardignoredwordList.contains(word))
			{
			encodedString = codec->fromUnicode(word);
			if (pChecker) check = pChecker->spell(encodedString.data());
			else check=true;
			if (!check)
				{
				editor->selectword(li,cols,word);
				ui.listWidget->setEnabled(true);
				ui.lineEditNew->setEnabled(true);
				ui.pushButtonIgnore->setEnabled(true);
				ui.pushButtonAlwaysIgnore->setEnabled(true);
				ui.pushButtonReplace->setEnabled(true);
				ui.lineEditOriginal->setEnabled(true);
				ui.lineEditOriginal->setText(word);
				ui.listWidget->clear();
				ui.lineEditNew->clear();
				ui.labelMessage->setText("");
				gonext=false;
				ns = pChecker->suggest(&wlst,encodedString.data());
				if (ns > 0)
					{
					suggWords.clear();
					for (int i=0; i < ns; i++) 
						{
						suggWords.append(codec->toUnicode(wlst[i]));
					//free(wlst[i]);
						} 
				//free(wlst);
					pChecker->free_list(&wlst, ns);
					if (!suggWords.isEmpty())
						{
						if (suggWords.contains(word)) gonext=true;
						else
							{
							ui.listWidget->addItems(suggWords);
							ui.lineEditNew->setText(suggWords.at(0));
							}
						}
					}
				}
			}
		}
	go=c.movePosition(QTextCursor::NextWord,QTextCursor::MoveAnchor);
	}
QApplication::restoreOverrideCursor();*/
QApplication::setOverrideCursor(Qt::WaitCursor);
QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
QTextBlock block;
QString text;
int i;
int check;
int offset ;
int ns;
char ** wlst;
QChar ch;
QString buffer;
QByteArray encodedString;
QStringList suggWords;
BlockData* blockData;
block=c.block();
bool gonext=true;
while (gonext && c.position() < endpos+deltacol && block.isValid() ) 
{
ui.listWidget->setEnabled(false);
ui.lineEditNew->setEnabled(false);
ui.pushButtonIgnore->setEnabled(false);
ui.pushButtonAlwaysIgnore->setEnabled(false);
ui.pushButtonReplace->setEnabled(false);
ui.lineEditOriginal->setEnabled(false);
ui.lineEditOriginal->clear();
ui.listWidget->clear();
ui.lineEditNew->clear();
ui.labelMessage->setText("<b>"+tr("No more misspelled words")+"</b>");
i=0;
text=block.text();
blockData= (BlockData*)block.userData();
while (gonext && i < text.length())
	{
	buffer = QString::null;
	ch = text.at( i );
	offset=0;
	while ((blockData->code[i]!=1) && (!isSpace(ch)))
	      {
	      buffer += ch;
	      i++;
	      if (i < text.length()) ch = text.at( i );
	      else break;
	      }
	while (buffer.startsWith('\''))
	  {
	  buffer=buffer.right(buffer.size()-1);
	  }
	while (buffer.endsWith('\''))
	  {
	  buffer.chop(1);
	  offset++;
	  }
	if ( (i - buffer.length()-offset+block.position() <= endpos+deltacol) && (i-offset+block.position()>=c.position()) && (buffer.length() > 1) && (!ignoredwordList.contains(buffer)) && (!hardignoredwordList.contains(buffer)))
	  {
	  encodedString = codec->fromUnicode(buffer);
	  if (pChecker) check = pChecker->spell(encodedString.data());
	  else check=true;
	  if (!check)
		  {
		  editor->selectword(block.blockNumber(),i - buffer.length()-offset,buffer);
		  ui.listWidget->setEnabled(true);
		  ui.lineEditNew->setEnabled(true);
		  ui.pushButtonIgnore->setEnabled(true);
		  ui.pushButtonAlwaysIgnore->setEnabled(true);
		  ui.pushButtonReplace->setEnabled(true);
		  ui.lineEditOriginal->setEnabled(true);
		  ui.lineEditOriginal->setText(buffer);
		  ui.listWidget->clear();
		  ui.lineEditNew->clear();
		  ui.labelMessage->setText("");
		  gonext=false;
		  ns = pChecker->suggest(&wlst,encodedString.data());
		  if (ns > 0)
			  {
			  suggWords.clear();
			  for (int i=0; i < ns; i++) 
				  {
				  suggWords.append(codec->toUnicode(wlst[i]));
			  //free(wlst[i]);
				  } 
		  //free(wlst);
			  pChecker->free_list(&wlst, ns);
			  if (!suggWords.isEmpty())
				  {
				  if (suggWords.contains(buffer)) gonext=true;
				  else
					  {
					  ui.listWidget->addItems(suggWords);
					  ui.lineEditNew->setText(suggWords.at(0));
					  }
				  }
			  }
		  }
	  }
	i++;
	}
block = block.next();
}
QApplication::restoreOverrideCursor();

}

bool SpellerDialog::isSpace(QChar c) const
{
    return c == QLatin1Char(' ')
        || c == QChar::Nbsp
        || c == QChar::LineSeparator
        || c == QLatin1Char('\t')
        ;
}
