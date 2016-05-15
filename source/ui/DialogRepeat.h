
#ifndef DIALOG_REPEAT_H_
#define DIALOG_REPEAT_H_

#include <QDialog>
#include "ui_DialogRepeat.h"

class Document;

class DialogRepeat : public QDialog {
	Q_OBJECT
public:
	DialogRepeat(Document *forWindow, QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual ~DialogRepeat();
	
public Q_SLOTS:
	void setCommand(const QString &command);	
	void on_buttonBox_accepted();
	
private:
	bool doRepeatDialogAction();

private:
	Ui::DialogRepeat ui;
	QString lastCommand_;
	Document *forWindow_;
};

#endif
