//	TwindyTabbedComponent.h - This is the original JUCE TabbedComponent.  The
//							  new one's fine for most things, but this one suits
//							  our purposes a bit better.
//
//	(obviously, all the code here was written by Jules - I just changed the name
//	 to avoid conflicts with the new one)
/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004 by Julian Storer.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "juce_amalgamated.h"

#include "TwindyTabbedComponent.h"


//==============================================================================
class TwindyTabHolderComponent  : public Component,
                            public ScrollBarListener
{
public:
    TwindyTabHolderComponent()
    {
    }

    ~TwindyTabHolderComponent()
    {
        deleteAllChildren();
    }

    void scrollBarMoved (ScrollBar *scrollBar, const double thing)
    {
        // scrollbar moved
        resized();
        getParentComponent()->resized();
    }

    void resized()
    {
        const TwindyTabbedComponent* const owner = dynamic_cast<const TwindyTabbedComponent*> (getParentComponent());

        const bool vertical = (owner->orientation == TwindyTabbedComponent::TabsAtLeft
                                || owner->orientation == TwindyTabbedComponent::TabsAtRight);

        const int overlap = owner->overlapPixels;

        int d = (int) -owner->scrollbar->getCurrentRangeStart();

        for (int i = 0; i < owner->tabList.size(); ++i)
        {
            Component* const c = (Component*)owner->tabList.getUnchecked(i);

            if (vertical)
            {
                c->setBounds (0, d, getWidth(), c->getHeight());
                d += c->getHeight() - overlap;
            }
            else
            {
                c->setBounds (d, 0, c->getWidth(), getHeight());
                d += c->getWidth() - overlap;
            }
        }
    }

    void paint (Graphics& g)
    {
        const TwindyTabbedComponent* const t = (const TwindyTabbedComponent*) getParentComponent();

        if (t != 0)
        {
            const Rectangle thisBounds (0, 0, getWidth(), getHeight());

            int i;
            for (i = 0; i < getNumChildComponents(); ++i)
            {
                TwindyTabbedComponentBasicTab* const c = (TwindyTabbedComponentBasicTab*) getChildComponent (i);

                if (c != (TwindyTabbedComponentBasicTab*)t->tabList [t->currentTab])
                {
                    const float indent = 2.0f;

                    float x1 = (float)c->getX();
                    float y1 = (float)c->getY();
                    float x2 = x1 + c->getWidth();
                    float y2 = y1 + c->getHeight();

                    if (c->getBounds().intersects (thisBounds))
                    {
                        if (t->orientation == TwindyTabbedComponent::TabsAtTop)
                        {
                            y1 += indent;
                            y2 += 100.0f;
                        }
                        else if (t->orientation == TwindyTabbedComponent::TabsAtBottom)
                        {
                            y2 -= indent;
                            y1 -= 100.0f;
                        }
                        else if (t->orientation == TwindyTabbedComponent::TabsAtLeft)
                        {
                            x1 += indent;
                            x2 += 100.0f;
                        }
                        else if (t->orientation == TwindyTabbedComponent::TabsAtRight)
                        {
                            x2 -= indent;
                            x1 -= 100.0f;
                        }

                        Path p;
                        p.addRoundedRectangle (x1 + t->normalOutlineThickness,
                                               y1 + t->normalOutlineThickness,
                                               x2 - x1 - t->normalOutlineThickness* 2.0f,
                                               y2 - y1 - t->normalOutlineThickness * 2.0f,
                                               t->cornerSize);

                        g.setColour (t->deselectedTabCol);
                        g.fillPath (p);

                        g.setColour (t->normalOutlineCol);
                        g.strokePath (p, PathStrokeType (t->normalOutlineThickness));
                    }
                }
            }

            AffineTransform trans;
            trans = trans.translated ((float)t->getScreenX() - getScreenX(),
                                      (float)t->getScreenY() - getScreenY());

            g.setColour (t->fillCol);
            g.fillPath (t->outlineShape, trans);

            g.setColour (t->selectedOutlineCol);
            g.fillPath (t->strokedOutline, trans);
        }
    }

    juce_UseDebuggingNewOperator

private:
    explicit TwindyTabHolderComponent (const TwindyTabHolderComponent&);
};


class TwindyTabEdgeDrawComponent  : public Component
{
public:
    TwindyTabEdgeDrawComponent()
    {
        setInterceptsMouseClicks (false, true);
    }

    ~TwindyTabEdgeDrawComponent() {}

    void paint (Graphics& g)
    {
        TwindyTabbedComponent* const t = (TwindyTabbedComponent*) getParentComponent();

        if (t != 0)
        {
            AffineTransform trans;
            trans = trans.translated ((float)t->getScreenX() - getScreenX(),
                                      (float)t->getScreenY() - getScreenY());

            g.setColour (t->fillCol);
            g.fillPath (t->outlineShape, trans);

            g.setColour (t->selectedOutlineCol);
            g.fillPath (t->strokedOutline, trans);
        }
    }

private:
    explicit TwindyTabEdgeDrawComponent (const TwindyTabEdgeDrawComponent&);
};


//==============================================================================
TwindyTabbedComponent::TwindyTabbedComponent (const String& name)
    : Component (name),
      orientation (TabsAtTop),
      currentTab (-1),
      tabAreaWidth (25),
      overlapPixels (4),
      xGap (4),
      yGap (4),
      leftTopInset (0),
      rightBottomInset (0),
      fillCol (Colours::grey),
      deselectedTabCol (Colours::darkgrey),
      textCol (Colours::black),
      selectedOutlineCol (Colours::white),
      normalOutlineCol (Colours::white),
      contents (0),
      cornerSize (7.0f),
      normalOutlineThickness (1.25f),
      selectedOutlineThickness (1.25f)
{
    addAndMakeVisible (tabs = new TwindyTabHolderComponent());

    addChildComponent (scrollbar = new ScrollBar (true));
    scrollbar->addListener ((TwindyTabHolderComponent*)tabs);

    for (int i = 6; --i >= 0;)
        addAndMakeVisible (outlineComps[i] = new TwindyTabEdgeDrawComponent());
}

TwindyTabbedComponent::~TwindyTabbedComponent()
{
    if (contents->isValidComponent())
        delete contents;

    deleteAllChildren();
}

void TwindyTabbedComponent::setTabOrientation (TabOrientation orientation_, int tabAreaSize, int leftRightGap, int topBottomGap)
{
    if (orientation != orientation_ || tabAreaSize != tabAreaWidth
        || xGap != leftRightGap || yGap != topBottomGap)
    {
        orientation = orientation_;
        tabAreaWidth = tabAreaSize;
        xGap = leftRightGap;
        yGap = topBottomGap;

        const bool vertical = orientation == TwindyTabbedComponent::TabsAtLeft
                                   || orientation == TwindyTabbedComponent::TabsAtRight;

        scrollbar->setOrientation (vertical);
        scrollbar->setSingleStepSize ((vertical) ? 18 : 40);

        resized();
    }
}

void TwindyTabbedComponent::setColours (const Colour& fillColour,
                                  const Colour& textColour,
                                  const Colour& selectedOutlineColour,
                                  const Colour& normalOutlineColour,
                                  const Colour& deselectedTabColour)
{
    if (fillCol != fillColour || deselectedTabCol != deselectedTabColour
        || textCol != textColour || selectedOutlineCol != selectedOutlineColour
        || normalOutlineCol != normalOutlineColour)
    {
        fillCol = fillColour;
        deselectedTabCol = deselectedTabColour;
        textCol = textColour;
        selectedOutlineCol = selectedOutlineColour;
        normalOutlineCol = normalOutlineColour;
        repaint();
    }
}

void TwindyTabbedComponent::setOverlap (int pixels)
{
    overlapPixels = pixels;
}

void TwindyTabbedComponent::setTabInsets (int spaceAtTopOrLeft, int spaceAtRightOrBottom)
{
    leftTopInset = spaceAtTopOrLeft;
    rightBottomInset = spaceAtRightOrBottom;
    resized();
}

void TwindyTabbedComponent::setOutlineStyle (float roundedCornerSize,
                                       float normalOutlineThickness_,
                                       float selectedOutlineThickness_)
{
    if (cornerSize != roundedCornerSize
         || normalOutlineThickness != normalOutlineThickness_
         || selectedOutlineThickness != selectedOutlineThickness_)
    {
        cornerSize = roundedCornerSize;
        normalOutlineThickness = normalOutlineThickness_;
        selectedOutlineThickness = selectedOutlineThickness_;
        resized();
    }
}

void TwindyTabbedComponent::addTab (int index, TwindyTabbedComponentBasicTab* tabToUse)
{
    tabs->addAndMakeVisible (tabToUse);
    tabList.insert (index, tabToUse);
    tabsChanged();
}

void TwindyTabbedComponent::removeTab (int index)
{
    Component* const t = tabs->getChildComponent (index);

    if (t != 0)
    {
        tabList.removeValue (t);
        delete t;
        tabsChanged();
    }
}

void TwindyTabbedComponent::removeTab (TwindyTabbedComponentBasicTab *tab)
{
	if(tab != 0)
	{
		tabList.removeValue(tab);
		delete tab;
		tabsChanged();
	}
}

void TwindyTabbedComponent::clearTabs()
{
    tabs->deleteAllChildren();
    tabList.clear();
    tabsChanged();
}

int TwindyTabbedComponent::getNumTabs() const
{
    return tabs->getNumChildComponents();
}

TwindyTabbedComponentBasicTab* TwindyTabbedComponent::getTabComponent (int index) const
{
    return static_cast<TwindyTabbedComponentBasicTab *>(tabList[index]);
}

void TwindyTabbedComponent::setCurrentlySelectedTab (int index)
{
    const bool changed = currentTab != index;

    if (changed)
    {
        currentTab = index;

        TwindyTabbedComponentBasicTab* const t = getTabComponent (index);
        if (t != 0)
            t->toFront (true);

        tabsChanged();
    }

    tabWasClicked (index, ! changed, ModifierKeys::getCurrentModifiers());
}

Component* TwindyTabbedComponent::setContentComponent (Component* newContentComp)
{
    if (newContentComp == contents)
        return 0;

    Component* const oldOne = contents;

    removeChildComponent (oldOne);
    addAndMakeVisible (contents = newContentComp);

    resized();

    return oldOne;
}

void TwindyTabbedComponent::tabsChanged()
{
    tabs->resized();

    int lo = 0, hi = 0;

    for (int i = tabs->getNumChildComponents(); --i >= 0;)
    {
        hi = jmax (hi, (scrollbar->isVertical()) ? tabs->getChildComponent(i)->getBottom()
                                                 : tabs->getChildComponent(i)->getRight());

        lo = jmin (lo, (scrollbar->isVertical()) ? tabs->getChildComponent(i)->getY()
                                                 : tabs->getChildComponent(i)->getX());
    }

    scrollbar->setRangeLimits (0, hi - lo);

    resized();
    repaint();
}

void TwindyTabbedComponent::paint (Graphics& g)
{
	const int lowerBorder = static_cast<int>(getHeight());
	int textHeight = 425;
	float textStretch = 8.0f;
	int textStart;
	Font tempFont(16.0f, Font::bold);

	g.setFont(tempFont);

	textHeight = (lowerBorder - 50);
	textStart = lowerBorder - 25;
	textStretch = (float)textHeight * (8.0f/425.0f);

	g.setColour(fillCol);
	g.drawTextAsPath(T("Twindy"),
					 AffineTransform::identity.scaled(textStretch, 6.0f).
					 followedBy(AffineTransform::identity.rotated(-1.570796327f).
					 followedBy(AffineTransform::identity.translated(100, textStart))));

    if (contents == 0 || !contents->isOpaque())
    {
        g.setColour (fillCol);
        g.fillRect (contentsPos);
    }
}

void TwindyTabbedComponent::resized()
{
    scrollbar->setCurrentRange (scrollbar->getCurrentRangeStart(),
                                (scrollbar->isVertical()) ? tabs->getHeight()
                                                          : tabs->getWidth());

    const int w = getWidth();
    const int h = getHeight();

    const int scrollbarSize = (scrollbar->isVisible()) ? 18 : 0;
    const int overlap = roundFloatToInt (cornerSize);
    const int overlap2 = overlap + 2;

    Rectangle mainArea, outlinePos;

    if (orientation == TabsAtTop)
    {
        const int width = w - (scrollbarSize * 2 + leftTopInset + rightBottomInset);

        tabs->setBounds (leftTopInset, 0, width, tabAreaWidth + overlap);
        scrollbar->setBounds (width + leftTopInset, 0, scrollbarSize * 2, tabAreaWidth);
        contentsPos.setBounds (0, tabAreaWidth, w, h - tabAreaWidth);
        mainArea.setBounds (0, tabAreaWidth + overlap, w, h - (tabAreaWidth + overlap));

        outlineComps[4]->setBounds (0, tabs->getHeight() - overlap2, tabs->getX(), overlap2);
        outlineComps[5]->setBounds (tabs->getRight(), tabs->getHeight() - overlap2, getWidth() - tabs->getRight(), overlap2);
    }
    else if (orientation == TabsAtBottom)
    {
        const int width = w - (scrollbarSize * 2 + leftTopInset + rightBottomInset);

        tabs->setBounds (leftTopInset, h - (tabAreaWidth + overlap), width, tabAreaWidth + overlap);
        scrollbar->setBounds (width + leftTopInset, h - tabAreaWidth, scrollbarSize * 2, tabAreaWidth);
        contentsPos.setBounds (0, 0, w, h - tabAreaWidth);
        mainArea.setBounds (0, 0, w, h - (tabAreaWidth + overlap));

        outlineComps[4]->setBounds (0, tabs->getY(), tabs->getX(), overlap2);
        outlineComps[5]->setBounds (tabs->getRight(), tabs->getY(), getWidth() - tabs->getRight(), overlap2);
    }
    else if (orientation == TabsAtLeft)
    {
        tabs->setBounds (scrollbarSize, leftTopInset, tabAreaWidth + overlap, h - (leftTopInset + rightBottomInset));
        scrollbar->setBounds (0, leftTopInset, scrollbarSize, h - (leftTopInset + rightBottomInset));

        contentsPos.setBounds (tabAreaWidth + scrollbarSize, 0, w - (tabAreaWidth + scrollbarSize), h);
        mainArea.setBounds (tabAreaWidth + scrollbarSize + overlap, 0, w - (tabAreaWidth + scrollbarSize + overlap), h);

        outlineComps[4]->setBounds (tabs->getRight() - overlap2, 0, overlap2, tabs->getY());
        outlineComps[5]->setBounds (tabs->getRight() - overlap2, tabs->getBottom(), overlap2, getHeight() - tabs->getBottom());
    }
    else if (orientation == TabsAtRight)
    {
        tabs->setBounds (w - (tabAreaWidth + scrollbarSize + overlap), leftTopInset,
                         tabAreaWidth + overlap, h - (leftTopInset + rightBottomInset));
        scrollbar->setBounds (tabs->getRight(), leftTopInset, scrollbarSize, h - (leftTopInset + rightBottomInset));

        contentsPos.setBounds (0, 0, w - (tabAreaWidth + scrollbarSize), h);
        mainArea.setBounds (0, 0, w - (tabAreaWidth + scrollbarSize + overlap), h);

        outlineComps[4]->setBounds (tabs->getX(), 0, overlap2, tabs->getY());
        outlineComps[5]->setBounds (tabs->getX(), tabs->getBottom(), overlap2, getHeight() - tabs->getBottom());
    }

    outlinePos = contentsPos;
	//****Twindy****
	//Re-wrote this bit, because it seemed to be broken?
	contentsPos.expand (-xGap/16,
                        -8 * yGap);
    if (contents != 0)
        contents->setBounds (contentsPos);

    outlineComps[0]->setBounds (mainArea.getX(), mainArea.getY(), mainArea.getWidth(), contentsPos.getY() - mainArea.getY());
    outlineComps[1]->setBounds (mainArea.getX(), contentsPos.getBottom(), mainArea.getWidth(), mainArea.getBottom() - contentsPos.getBottom());
    outlineComps[2]->setBounds (mainArea.getX(), contentsPos.getY(), contentsPos.getX() - mainArea.getX(), contentsPos.getHeight());
    outlineComps[3]->setBounds (contentsPos.getRight(), contentsPos.getY(), mainArea.getRight() - contentsPos.getRight(), contentsPos.getHeight());

    const Component* const selectedTab = (const Component*) tabList [currentTab];

    Path p;

    const float thickness = jmin (normalOutlineThickness,
                                   selectedOutlineThickness);

    // contents box size..
    float cx1 = outlinePos.getX() + thickness;
    float cy1 = outlinePos.getY() + thickness;
    float cx2 = outlinePos.getRight() - thickness;
    float cy2 = outlinePos.getBottom() - thickness;

    if (selectedTab != 0)
    {
        Rectangle selTabBounds (selectedTab->getBounds());
        selTabBounds.translate (tabs->getX(), tabs->getY());

        // tab size..
        const float tx1 = selTabBounds.getX() + thickness;
        const float ty1 = selTabBounds.getY() + thickness;
        const float tx2 = selTabBounds.getRight() - thickness;
        const float ty2 = selTabBounds.getBottom() - thickness;

        if (orientation == TwindyTabbedComponent::TabsAtLeft)
        {
            p.startNewSubPath (tx1, ty2);
            p.lineTo (tx1, ty1);
            p.lineTo (cx1, ty1);
            p.lineTo (cx1, cy1);
            p.lineTo (cx2, cy1);
            p.lineTo (cx2, cy2);
            p.lineTo (cx1, cy2);
            p.lineTo (cx1, ty2);
            p.closeSubPath();
        }
        else if (orientation == TwindyTabbedComponent::TabsAtRight)
        {
            p.startNewSubPath (tx2, ty1);
            p.lineTo (tx2, ty2);
            p.lineTo (cx2, ty2);
            p.lineTo (cx2, cy2);
            p.lineTo (cx1, cy2);
            p.lineTo (cx1, cy1);
            p.lineTo (cx2, cy1);
            p.lineTo (cx2, ty1);
            p.closeSubPath();
        }
        else if (orientation == TwindyTabbedComponent::TabsAtTop)
        {
            p.startNewSubPath (tx1, cy1);
            p.lineTo (tx1, ty1);
            p.lineTo (tx2, ty1);
            p.lineTo (tx2, cy1);
            p.lineTo (cx2, cy1);
            p.lineTo (cx2, cy2);
            p.lineTo (cx1, cy2);
            p.lineTo (cx1, cy1);
            p.closeSubPath();
        }
        else if (orientation == TwindyTabbedComponent::TabsAtBottom)
        {
            p.startNewSubPath (cx1, cy1);
            p.lineTo (cx2, cy1);
            p.lineTo (cx2, cy2);
            p.lineTo (tx2, cy2);
            p.lineTo (tx2, ty2);
            p.lineTo (tx1, ty2);
            p.lineTo (tx1, cy2);
            p.lineTo (cx1, cy2);
            p.closeSubPath();
        }
    }
    else
    {
        p.addRectangle (cx1, cy1, cx2 - cx1, cy2 - cy1);
    }

    outlineShape = p.createPathWithRoundedCorners (cornerSize);

    strokedOutline.clear();
    PathStrokeType strokeType ((selectedTab != 0) ? selectedOutlineThickness
                                                  : normalOutlineThickness);

    strokeType.createStrokedPath (strokedOutline, outlineShape);

    repaint();
}

//==============================================================================
TwindyTabbedComponentBasicTab::TwindyTabbedComponentBasicTab (const String& text)
    : Component (text)
{
    setRepaintsOnMouseActivity (true);
    setSize (50, 50);
}

TwindyTabbedComponentBasicTab::~TwindyTabbedComponentBasicTab()
{
}

void TwindyTabbedComponentBasicTab::activateThisTab()
{
    TwindyTabbedComponent* const t = (TwindyTabbedComponent*)(getParentComponent()->getParentComponent());

    if (t != 0)
        t->setCurrentlySelectedTab (t->tabList.indexOf (this));
}

void TwindyTabbedComponentBasicTab::paintTab (Graphics& g, bool isTheCurrentTab, const TwindyTabbedComponent* t)
{
    g.setColour ((isMouseOver()) ? Colour::contrasting (t->getTextColour(), t->getFillColour())
                                 : t->getTextColour());

    g.setFont (jmin (20.0f, getHeight() * 0.8f));

    g.drawText (getName(), 4, 2, getWidth() - 8, getHeight() - 4,
                Justification::centred, true);
}

void TwindyTabbedComponentBasicTab::paint (Graphics& g)
{
    const TwindyTabbedComponent* const t = (const TwindyTabbedComponent*)(getParentComponent()->getParentComponent());

    if (t != 0)
        paintTab (g, t->currentTab == t->tabList.indexOf (this), t);
}

void TwindyTabbedComponentBasicTab::mouseDown (const MouseEvent&)
{
    activateThisTab();
}

void TwindyTabbedComponentBasicTab::mouseWheelMove (const MouseEvent& e,
                                     float wheelIncrementX,
                                     float wheelIncrementY)
{
    const TwindyTabbedComponent* const t = (const TwindyTabbedComponent*)(getParentComponent()->getParentComponent());

    if (t != 0)
        t->scrollbar->mouseWheelMove (e, wheelIncrementX, wheelIncrementY);
}
