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

#ifndef TWINDYTABBEDCOMPONENT_H_
#define TWINDYTABBEDCOMPONENT_H_

#include "../juce_amalgamated.h"

//==============================================================================
class TwindyTabbedComponentBasicTab;

//==============================================================================
/**
    A tabbed box that holds a set of other components.

    xxx TODO need to document (and probably change a lot of) this class.
*/
class TwindyTabbedComponent  : public Component
{
public:
    //==============================================================================
    /** Creates a tabbed component.
    */
    TwindyTabbedComponent (const String& name);

    /** Destructor. */
    ~TwindyTabbedComponent();

    //==============================================================================
    enum TabOrientation
    {
        TabsAtTop,
        TabsAtBottom,
        TabsAtLeft,
        TabsAtRight
    };

    // set the position and size of the tab area.
    // If the tabs are on the left/right, tabAreaSize is the width of this area,
    // otherwise it dictates the height of the bar at the top/bottom.
    // The gaps are the number of pixels that will be left around the edges
    // of the component for drawing a border.
    void setTabOrientation (TabOrientation orientation,
                            int tabAreaSize,
                            int leftRightGap,
                            int topBottomGap);

    void setColours (const Colour& fillColour,
                     const Colour& textColour,
                     const Colour& selectedOutlineColour,
                     const Colour& normalOutlineColour,
                     const Colour& deselectedTabColour);

    const Colour& getFillColour() const             { return fillCol; }
    const Colour& getTextColour() const             { return textCol; }
    const Colour& getSelectedOutlineColour() const  { return selectedOutlineCol; }
    const Colour& getNormalOutlineColour() const    { return normalOutlineCol; }
    const Colour& getDeselectedTabColour() const    { return deselectedTabCol; }

    void setOutlineStyle (float roundedCornerSize,
                          float normalOutlineThickness,
                          float selectedOutlineThickness);

    // sets how much space should be left at either end of the tab section - default is 0
    void setTabInsets (int spaceAtTopOrLeft, int spaceAtRightOrBottom);

    // sets the amount by which adjacent tabs should overlap
    void setOverlap (int pixels);

    //==============================================================================
    // adds a tab, which will be deleted later when no longer needed. To add a tab at the
    // end of the list, use -1 as the index.
    void addTab (int index, TwindyTabbedComponentBasicTab* tabToUse);

    // removes (and deletes the component for) a tab
    void removeTab (int index);
    void removeTab (TwindyTabbedComponentBasicTab *tab);

    void clearTabs();

    int getNumTabs() const;
    TwindyTabbedComponentBasicTab* getTabComponent (int index) const;

    // pass a number < 0 to deselect all
    void setCurrentlySelectedTab (int index);

    // may be < 0 if none is selected
    int getCurrentlySelectedTab() const     { return currentTab; }

    //==============================================================================
    // this must be overridden to update the content component when the
    // selected tab changes - the implementation should use
    // the setContentComponent() method to so this.
    // This function is called even if the currently selected tab was clicked,
    // so use clickedTabWasAlreadySelected to decide if you want to take action or not.
    // Modifier values are those used in Component::mouseDown()
    virtual void tabWasClicked (int newSelectedIndex,
                                bool clickedTabWasAlreadySelected,
                                const ModifierKeys& currentModifiers) = 0;

    // call this to set the component that will be used as the main panel
    // of the tabbed box. The old content component will be returned so that
    // it can be deleted if necessary (0 may also be returned). When the TabbedComponent
    // object is deleted, it will also delete its current content component, so
    // if you don't want this to happen, call setContentComponent (0) in your
    // destructor.
    Component* setContentComponent (Component* newContentComp);

    Component* getContentComponent() const { return contents; }

    //==============================================================================
    juce_UseDebuggingNewOperator

protected:
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void resized();

private:
    //==============================================================================
    friend class TwindyTabHolderComponent;
    friend class TwindyTabbedComponentBasicTab;
    friend class TwindyTabEdgeDrawComponent;

    TabOrientation orientation;
    int currentTab, tabAreaWidth, overlapPixels, xGap, yGap, leftTopInset, rightBottomInset;
    Colour fillCol, deselectedTabCol, textCol, selectedOutlineCol, normalOutlineCol;
    Component* contents;
    Component* tabs;
    VoidArray tabList;
    ScrollBar* scrollbar;
    Rectangle contentsPos;
    Component* outlineComps[6];
    Path outlineShape, strokedOutline;
    float cornerSize, normalOutlineThickness, selectedOutlineThickness;

    void tabsChanged();

    void layoutTabComponents();

    explicit TwindyTabbedComponent (const TwindyTabbedComponent&);
};


//==============================================================================
// one of these, or a subclass of it, is used for each tab in a TabbedComponent
class TwindyTabbedComponentBasicTab  : public Component
{
public:
    /** Creates a tab.
    */
    TwindyTabbedComponentBasicTab (const String& text = String::empty);

    /** Destructor. */
    ~TwindyTabbedComponentBasicTab();

    // this should be called by the subclass when it wants to select this
    // tab, e.g. when clicked.
    void activateThisTab();

    // use this for custom designs
    virtual void paintTab (Graphics& g, bool isTheCurrentTab, const TwindyTabbedComponent* ownerTabbedComp);

    void mouseDown (const MouseEvent& e);
    void mouseWheelMove (const MouseEvent& e,
                                     float wheelIncrementX,
                                     float wheelIncrementY);

private:
    void paint (Graphics& g);

    explicit TwindyTabbedComponentBasicTab (const TwindyTabbedComponentBasicTab&);
};


#endif
