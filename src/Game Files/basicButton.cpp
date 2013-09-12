#include "basicButton.h"

/** @function TBasicButton::TBasicButton - Constructor			Takes parameters to construct button with
 *		@param 		text				Initial text for mTextGraphic
 *		@param 		w					Width of the text
 *		@param 		h					Height of the text
 *		@param 		flags				Flags for mTextGraphic
 * 		@param 		fontFilename		Font to use
 * 		@param 		lineHeight			Size of the text
 * 		@param 		textColour			Colour of the text
 */
TBasicButton::TBasicButton(str text, uint32_t w, uint32_t h, uint32_t flags, 
						   str fontFilename, uint32_t lineHeight, const TColor& textColour) :
						   mTextGraphic(text, w, h, flags, fontFilename, lineHeight, textColour),
						   mCornerBias(-TReal(w/2), -TReal(h/2)),
						   mHeight(h)
						   {}

/** @function TBasicButton::Draw - Takes a position and new text and draws the button with these
 *		@param 		position			Position of the button
 *		@param 		text				New text for the button, defaults to "", so no change
 */
void TBasicButton::Draw(const TVec2& position, str text)
{
	if(text != str(""))
		mTextGraphic.SetText(text);

	TDrawSpec drawSpec(position);
	mImage->Draw(drawSpec);
	mTextGraphic.Draw(position + mCornerBias, mHeight);
}

/** @function TBasicButton::HitTest - Takes a position and new text and draws the button with these
 *		@param 		at					Position to check
 *		@param 		parentContext		Position context of the button
 *		
 *		@return		true if at was in the area of the button
 */
bool TBasicButton::HitTest(const TPoint& at, const TVec2& parentContext)
{
	if(mImage->HitTest(at, TDrawSpec(parentContext)))
		return true;
	else return false;
}