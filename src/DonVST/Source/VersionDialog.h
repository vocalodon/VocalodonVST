/**
 * @file VersionDialog.h
 * @brief バージョンダイアログ（未使用）
 * @author h.wakimoto
 * @date 2017/05/01
 */

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


//うまくいかないので未使用
class VersionDialog : public DialogWindow {
public:
	VersionDialog(const String &name, Colour backgroundColour, bool escapeKeyTriggersCloseButton = true, bool addToDesktop = true);
	~VersionDialog();
	//void resized() override;

private:
	ScopedPointer<Label> lbl;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VersionDialog)
};
