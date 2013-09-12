/**
 *	basicButton.h - Jan van der Kamp, 2011
 */
#ifndef BASICBUTTON_H_INCLUDED
#define BASICBUTTON_H_INCLUDED

#include <pf/pflib.h>
#include <pf/str.h>

/** @class TBasicButton - This class encapsulates functionality for a simple button, which is made up of a TSprite and
 *						  a TTextGraphic. This makes it easy to draw both a background image and a title by giving 
 *						  just one position.
 *		@property 		TTextGraphic			mTextGraphic			TTextGraphic used to draw the title of the button
 *		@property 		TSpriteRef				mImage					Background image for the button
 *		@property 		TVec2					mCornerBias				Position bias so that mTextGraphic can also be drawn by giving
 *																		the position of the centre of the button
 *		@property 		uint32_t				mHeight					Height of the text
 */
class TBasicButton
{
public:
	TBasicButton(str text, uint32_t w, uint32_t h, uint32_t flags=0, str fontFilename=str(""), uint32_t lineHeight=10, const TColor& textColour=TColor(0, 0, 0, 1));
	void Draw(const TVec2& position, str text=str(""));
	bool HitTest(const TPoint& at, const TVec2& parentContext4);
	str GetText() const { return mTextGraphic.GetText(); }
	void SetText(str text) { return mTextGraphic.SetText(text); }
	void SetImage(TSpriteRef image) { mImage=image; }
private:
	TTextGraphic mTextGraphic;
	TSpriteRef mImage;
	TVec2 mCornerBias;
	uint32_t mHeight;
};

#endif // BASICBUTTON_H_INCLUDED