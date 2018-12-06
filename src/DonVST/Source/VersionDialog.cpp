/**
* @file VersionDialog.cpp
* @brief バージョンダイアログ（未使用）
* @author h.wakimoto
* @date 2017/05/01
*/

#include "VersionDialog.h"

//うまくいかないので未使用
VersionDialog::VersionDialog(const String &name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, bool addToDesktop) 
	: DialogWindow(name, backgroundColour, escapeKeyTriggersCloseButton, addToDesktop)
{
	setAlwaysOnTop(true);
	//addAndMakeVisible( lbl = new Label(String(), TRANS("test")));
	lbl = new Label(String(), TRANS("LABEL"));
	lbl->setJustificationType(Justification::right);
	lbl->setEditable(false, false, false);
	lbl->setColour(Label::textColourId, Colours::silver);
	lbl->setBounds(10, 10, 50, 50);
	lbl->setVisible(true);

	setSize(300, 300);
}

VersionDialog::~VersionDialog(){
	//
	delete lbl;
}



//void VersionDialog::resized() {
//	lbl->setBounds(10, 10, 50, 50);

//}
