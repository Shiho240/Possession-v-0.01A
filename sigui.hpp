/*
                     Simple Includable GUI

          Copyright (c) 2011   James John Hughes III

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#ifndef SIGUI_H_
#define SIGUI_H_

#include <SFML/Graphics.hpp>


enum GUIbevelStyle {bsNone, bsUp, bsDown};
enum GUIborderStyle {rectangle, rounded, flatRectangle, flatRounded, tab};
enum GUIorientation {orHorizontal, orVertical};
enum GUIalignment {atCenter, atTop, atBottom, atLeft, atRight, atUpperLeft, atLowerLeft, atUpperRight, atLowerRight};
enum GUIbuttonType {toggle, momentary};
enum GUIspecialSymbol {ssNone, ssLeftTriangle, ssRightTriangle, ssUpTriangle, ssDownTriangle, ssGripV, ssGripH,
                       ssMinimize, ssMaximize, ssNormal, ssClose, ssCheck, ssEllipsis};
enum GUIwindowState {wsNormal, wsMinimized, wsMaximized};
enum GUIalign {alNone, alLeft, alRight, alTop, alBottom, alClient};
//enum guiResize {resizeLeft, resizeRight, resizeTop, resizeBottom};

// the following is used for RTTI
enum GUIcomponentType {tguiWidget, tguiForm, tguiLabel, tguiPanel, tguiButton, tguiCheckBox, tguiRadioGroup, tguiScrollBar,
                       tguiTabPanel, tguiMemo, tguiEdit, tguiListBox, tguiRadioButton, tguiTrackBar, tguiMenu, tguiStatus,
                       tguiSpin, tguiStringGrid, tguiComboBox, tguiTreeView, tguiPopUp};

int GUIregisterSkin(sf::Image *pSkin);  // make an image available for use as a skin, returns the index (handle) of the image

bool GUIinitialize(sf::RenderWindow *AppName);
bool GUIprocessEvent(sf::Event); // inspects "Event", if it applies to the GUI, it is processed and true is returned (ie, no need to process in the main routine)
void GUIdraw();
void GUIclose(); // close (remove) all forms
void GUIhide(); // hide all forms
bool GUIterminate();


void GUIfocusChange(bool reverse); // change which component has focus for keyboard input.

class GUIwidget   // virtual base class for GUI components - a nonvisible rectangle
{
private:
    void WidgetInitialize();
    sf::Font font;
protected:
    bool canAlign;
    GUIalign align;
public:
    friend class GUIpanel;
    friend class GUIform;
    int siblingOrder; //
    GUIwidget(); // constructor
    GUIwidget(int Left, int Top); // constructor (left, top)
    GUIwidget(int Left, int Top, int Width, int Height); // constructor (left, top, width, height)
    GUIcomponentType thisType; // RTTI
    int left, top;  // relative to top left of parent, screen if no parent
    int width, height;
    int clLeft, clTop, clWidth, clHeight; // client area within widget
    struct
    {
        int minHeight;
        int maxHeight;
        int minWidth;
        int maxWidth;
    } Constraint;
    bool isVisible;
    bool tabStop; // if true, this item can receive keyboard focus (eg, by tabbing to it, or mouse clicking on it)
    GUIwidget * parent;                 // the parent (if any) widget
    std::vector<GUIwidget *> children; // list of child widgets (if any)
    int AddChild(GUIwidget * childWidget);
    sf::Color faceColor;
    sf::Color highlightColor;
    sf::Color faceTextColor;
    sf::Color highlightTextColor;
    sf::Color windowColor;
    sf::Color windowTextColor;
    bool useParentColors;
    bool useParentFont;
//    void Resize(int Width, int Height);
    void SetFont(sf::Font newFont);
    sf::Font& GetFont();
    void ReAlign(GUIalign newAlign); // assert the alignment to other panels/form
    void (*OnChange) (GUIwidget *);     // pointer to function to call when important change occurs (optional)
    void (*OnClick) (GUIwidget *);      // pointer to function to respond to a mouse click -or- keyboard "activation" command
    virtual void draw (void) =0;        // virtual function - makes this an abstract class
};

class GUIpanel: public GUIwidget
{
private:
    void panelInitialize(GUIwidget*);
protected:
    bool drawSpecialSymbol;     // ie. arrows on scroll bar buttons
    GUIspecialSymbol symbol;
    bool fixedBevel; // normally false, only set for special buttons that don't show bevel change when pressed
public:
    friend class GUIform;  // allow "GUIform" to use special symbols (on title bar buttons)
    friend class GUIcheckBox;
    friend class GUIradioButton;
    friend class GUIscrollBar;
    friend class GUItrackBar;
    friend class GUIcomboBox;
    friend class GUIspinner;
    GUIbevelStyle bevelInner, bevelOuter;
    int bevelWidth;
    int borderWidth;
    bool textItem; // draw face with text color if true
    GUIpanel();
    GUIpanel(int Left, int Top);
    GUIpanel(int Left, int Top, int Width, int Height);
    GUIpanel(GUIwidget*);
    GUIpanel(GUIwidget*, int Left, int Top);
    GUIpanel(GUIwidget*, int Left, int Top, int Width, int Height);
    bool translucent;
    sf::Sprite skin;
    void draw();
    void Resize(int Width, int Height);
    void ReAlign(GUIalign newAlign); // assert the alignment to other panels/form
};


class GUIlabel: public GUIwidget   // displays text. Anchor point is (left, top)
{
private:
    void LabelInitialize(GUIwidget*);
public:
    GUIlabel();
    GUIlabel(sf::Unicode::Text newText);
    GUIlabel(int Left, int Top); // constructor (left, top)
    GUIlabel(int Left, int Top, sf::Unicode::Text newText); // constructor (left, top)
    GUIlabel(GUIwidget*);
    GUIlabel(GUIwidget*, sf::Unicode::Text newText);
    GUIlabel(GUIwidget*, int Left, int Top); // constructor (left, top)
    GUIlabel(GUIwidget*, int Left, int Top, sf::Unicode::Text newText); // constructor (left, top)
    sf::String myText;
    GUIalignment textAlignment;     // alignment of the text with regard to anchor point
    void SetText(sf::Unicode::Text newText);
    void draw();
};

class GUIbutton: public GUIpanel
{
private:
    void btnInitialize(GUIwidget*);
    int autorepeatcountdown;
public:
    GUIlabel btnLabel;
    bool isTabButton;
    bool pressed;
    GUIbutton();
    GUIbutton(sf::Unicode::Text newText);
    GUIbutton(int Left, int Top);
    GUIbutton(int Left, int Top, sf::Unicode::Text newText);
    GUIbutton(int Left, int Top, int Width, int Height);
    GUIbutton(int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUIbutton(GUIwidget*);
    GUIbutton(GUIwidget*, sf::Unicode::Text newText);
    GUIbutton(GUIwidget*, int Left, int Top);
    GUIbutton(GUIwidget*, int Left, int Top, sf::Unicode::Text newText);
    GUIbutton(GUIwidget*, int Left, int Top, int Width, int Height);
    GUIbutton(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUIalignment glyphAlignment;
    GUIalignment textAlignment;
    bool flat;   // if false, draw bevel at all times, if true, draw bevel only when mouse over
    bool toggle; // if true, change state with each click. if false, active only while mouse held down.
    bool autorepeat; // if true, repeatedly call "OnClick" while in active state
    bool enabled;
    int glyphHorizontalInset;
    int glyphVerticalInset;
    sf::Sprite glyph;       // optional image to draw on button
    void SetText(sf::Unicode::Text newText);
    void draw();
    void Resize(int Width, int Height);
};

class GUIform: public GUIpanel
{
private:
    void FormInitialize();
    GUIwindowState windowState;
    GUIwindowState prevWindowState;
    GUIbutton formClose, formMinimize, formMaximize;
    sf::String caption; // text to display
    int titleBarHeight;
public:
    int oldLeft, oldWidth, oldTop, oldHeight; // used for restoring to normal window size
    std::vector<GUIwidget*>PopUps;
    sf::RenderWindow *parentApp;
    GUIform();
    GUIform(sf::Unicode::Text newText);
    GUIform(int Left, int Top);
    GUIform(int Left, int Top, sf::Unicode::Text newText);
    GUIform(int Left, int Top, int Width, int Height);
    GUIform(int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUIwidget * ActiveWidget; // which widget on the form has keyboard focus (or will have when form is active)
    bool resizable;     // whether the form can be resized by the user
    bool showTitleBar;      // whether to have a title bar (will include close button and border)
    bool borderBevel;   // whether to draw a beveled border (if titleBar is true)
    unsigned int resizeEdge;
    bool beingDragged;
    bool beingResized;
    void draw();
    void show();
    void show(bool state); // show if true, hide if false;
    void hide();
    void close();
    void minimize();
    void maximize();
    void SetCaption(sf::Unicode::Text newText);
    void AddPopUp(GUIwidget*);
    void RemovePopUp(GUIwidget* pWidget);
    void ClearPopUps();
    void Resize(int Width, int Height);
};

class GUIcheckBox: public GUIpanel
{
private:
    void CheckBoxInitialize(GUIwidget*);
    GUIpanel BoxPanel;
public:
    GUIcheckBox(); // constructor with default size
    GUIcheckBox(sf::Unicode::Text newText); // constructor with default size
    GUIcheckBox(int Left, int Top); // constructor with default size
    GUIcheckBox(int Left, int Top, sf::Unicode::Text newText); // constructor with default size
    GUIcheckBox(int Left, int Top, int Width, int Height); // constructor (left, top, width, height)
    GUIcheckBox(int Left, int Top, int Width, int Height, sf::Unicode::Text newText); // constructor (left, top, width, height)
    GUIcheckBox(GUIwidget*); // constructor with default size
    GUIcheckBox(GUIwidget*, sf::Unicode::Text newText); // constructor with default size
    GUIcheckBox(GUIwidget*, int Left, int Top); // constructor with default size
    GUIcheckBox(GUIwidget*, int Left, int Top, sf::Unicode::Text newText); // constructor with default size
    GUIcheckBox(GUIwidget*, int Left, int Top, int Width, int Height); // constructor (left, top, width, height)
    GUIcheckBox(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText); // constructor (left, top, width, height)
    GUIlabel btnText;
    bool checked;
    void Check(bool);
    bool enabled;
    void SetText(sf::Unicode::Text newText);
    void draw(); // override the draw function to modify based on status
};


class GUIradioButton: public GUIpanel
{
private:
    void radioButtonInitialize(GUIwidget*);
    GUIpanel BoxPanel;
public:
    GUIradioButton(); // constructor with default size
    GUIradioButton(sf::Unicode::Text newText); // constructor with default size
    GUIradioButton(int Left, int Top); // constructor with default size
    GUIradioButton(int Left, int Top, sf::Unicode::Text newText); // constructor with default size
    GUIradioButton(int Left, int Top, int Width, int Height); // constructor (left, top, width, height)
    GUIradioButton(int Left, int Top, int Width, int Height, sf::Unicode::Text newText); // constructor (left, top, width, height)
    GUIradioButton(GUIwidget*); // constructor with default size
    GUIradioButton(GUIwidget*, sf::Unicode::Text newText); // constructor with default size
    GUIradioButton(GUIwidget*, int Left, int Top); // constructor with default size
    GUIradioButton(GUIwidget*, int Left, int Top, sf::Unicode::Text newText); // constructor with default size
    GUIradioButton(GUIwidget*, int Left, int Top, int Width, int Height); // constructor (left, top, width, height)
    GUIradioButton(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText); // constructor (left, top, width, height)
    GUIlabel btnText;
    bool checked;
    bool enabled;
    void SetText(sf::Unicode::Text newText);
    void draw(); // override the draw function to modify based on status
};

class GUItabPanel: public GUIpanel
{
private:
    void TabPanelInitialize(GUIwidget*);
    int count;    // how mant tabs are there.
    int tabHeight;
    GUIalignment glyphAlignment;
    GUIalignment textAlignment;
public:
    GUItabPanel();
    GUItabPanel(int Count);
    GUItabPanel(int Left, int Top);
    GUItabPanel(int Left, int Top, int Count);
    GUItabPanel(int Left, int Top, int Width, int Height);
    GUItabPanel(int Left, int Top, int Width, int Height, int Count);
    GUItabPanel(GUIwidget*);
    GUItabPanel(GUIwidget*, int Count);
    GUItabPanel(GUIwidget*, int Left, int Top);
    GUItabPanel(GUIwidget*, int Left, int Top, int Count);
    GUItabPanel(GUIwidget*, int Left, int Top, int Width, int Height);
    GUItabPanel(GUIwidget*, int Left, int Top, int Width, int Height, int Count);
    ~GUItabPanel();
    int selected; // which tab is currently selected. Range = zero to count-1
    std::vector<GUIbutton *> tabs; // list of tabs for the selections.
    std::vector<GUIpanel *> pages; // list of panels for the selections
    void SetGlyphAlignment(GUIalignment newAlignment);
    void SetTabWidth(int newWidth);
    void SetTabHeight(int newHeight);
    void SetTabCount(int);
    void draw();
    void Resize(int Width, int Height);
};

class GUIprogressBar: public GUIpanel
{
private:
    void ProgressBarInitialize(GUIwidget*);
public:
    GUIprogressBar();
    GUIprogressBar(int Left, int Top);
    GUIprogressBar(int Left, int Top, int Width, int Height);
    GUIprogressBar(GUIwidget*);
    GUIprogressBar(GUIwidget*, int Left, int Top);
    GUIprogressBar(GUIwidget*, int Left, int Top, int Width, int Height);
    GUIorientation orientation;
    int max;
    int min;
    int position;
    void draw();
};

class GUIscrollBar: public GUIpanel
{
private:
    void ScrollBarInitialize(GUIwidget*);
    int oldPosition;
public:
    GUIscrollBar(); // vertical by default
    GUIscrollBar(int Left, int Top); // vertical by default
    GUIscrollBar(int Left, int Top, int Width, int Height); // orientation determined by width vs height
    GUIscrollBar(GUIwidget*); // vertical by default
    GUIscrollBar(GUIwidget*, int Left, int Top); // vertical by default
    GUIscrollBar(GUIwidget*, int Left, int Top, int Width, int Height); // orientation determined by width vs height
    GUIorientation orientation;
    GUIbutton btnDecrease;
    GUIbutton btnIncrease;
    GUIbutton btnSlider;
    bool beingDragged;      // for moving the slider with the mouse
    int AnchorX, AnchorY; // for moving the slider with the mouse
    int position; // indicates the relative position of the slider (not the pixel position)
    int stepSize;
    int min;
    int max; // "position" ranges from "min" to "max"
    float sliderPercent;    // specify how much of slider range should be occupied by the slider itself
    void draw();
    void Resize(int Width, int Height);
};

class GUItrackBar: public GUIpanel
{
private:
    void TrackBarInitialize(GUIwidget*);
    GUIbutton btnSlider;
    int oldPosition;
public:
    GUItrackBar();
    GUItrackBar(int Left, int Top);
    GUItrackBar(int Left, int Top, int Width, int Height);
    GUItrackBar(GUIwidget*);
    GUItrackBar(GUIwidget*, int Left, int Top);
    GUItrackBar(GUIwidget*, int Left, int Top, int Width, int Height);
    GUIorientation orientation;
    bool beingDragged;      // for moving the slider with the mouse
    int AnchorX, AnchorY; // for moving the slider with the mouse
    int position;
    int max;
    int min;
    void draw();
    void Resize(int Width, int Height);
};

class GUIedit: public GUIpanel
{
private:
    void EditInitialize(GUIwidget*);
    std::string oldCopy; // used internally to check if user action really changed the text.
    bool wasInEditMode; // used internally only.
    int viewLeft; // for tracking the position of text within the view region
public:
    GUIedit();
    GUIedit(GUIwidget*);
    GUIedit(int Left, int Top);
    GUIedit(GUIwidget*, int Left, int Top);
    GUIedit(int Left, int Top, sf::Unicode::Text newText);
    GUIedit(GUIwidget*, int Left, int Top, sf::Unicode::Text newText);
    GUIedit(int Left, int Top, int Width, int Height);
    GUIedit(GUIwidget*, int Left, int Top, int Width, int Height);
    GUIedit(int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUIedit(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    sf::String myText;
    int cursorPos;      // point where edit actions take place (insert, delete etc)
    int selectionStart; // follows cursorPos unless a "selected" string of text has been marked for edit actions
    bool editMode;
    void SetText(sf::Unicode::Text newText);
    sf::Unicode::Text GetText();
    void MouseCursor(int X, int Y); // position cursor based on mouse position
    void draw();
    bool wasChanged(); // reports if current content matches old content
};

class GUIlistBox: public GUIpanel
{
private:
    void ListBoxInitialize(GUIwidget*);
    int count;    // how mant items are there.
//    std::vector<sf::Unicode::Text *> items;
    sf::String myText; // template for size, color
    int ViewCount; // how many items can be viewed at one time
    bool isPopUp;
public:
struct ListEntry
{
    GUIspecialSymbol check;
    sf::String text;
    GUIspecialSymbol more;
    GUIwidget* NextItem;
};
    std::vector<ListEntry>Items;

    friend class GUIcomboBox;
    friend class GUIpopup;
    GUIscrollBar Scroll;        // vertical scroll bar for long lists
    int topSelection; // index of item which is drawn at top of box
    int selected; // which item is currently selected. Range = zero to count-1
    GUIlistBox();
    GUIlistBox(GUIwidget*);
    GUIlistBox(int Left, int Top);
    GUIlistBox(GUIwidget*, int Left, int Top);
    GUIlistBox(int Left, int Top, int Width, int Height);
    GUIlistBox(GUIwidget*, int Left, int Top, int Width, int Height);
    bool enableChecks; // displays check marks to left of text
    bool enableMore; // displays ellipsis with dialog link, or arrow with sublist link
    int GetCount(); // returns number of items in list
    sf::Unicode::Text GetItem(int index);
    void RemoveItem(int index);
    void ReplaceItem(int index, sf::Unicode::Text newText);
    int AppendItem(sf::Unicode::Text newText);
    void AppendItems(sf::Unicode::Text newText); // multiple entries - each separated by a coma in newText
    void LinkItem(int index, GUIform* pForm); // link to either a ListBox or a Form
    void LinkItem(int index, GUIlistBox* pList); // link to either a ListBox or a Form
    sf::Unicode::Text GetSelection();
    sf::Unicode::Text GetText(int index);
    void SetText(int index, sf::Unicode::Text newText);
    void SetSelection(int index);
    void SetCheck(int index);
    void SetCheck(int index, bool state);
    bool GetCheck(int index); // return true if index item is checked
    int GetIndex(); // returns index to current selection
    void draw();
    void Resize(int Width, int Height);
    void MouseSelect(int X, int Y); // deal with mouse click
};

class GUIpopup: public GUIlistBox
{
private:
    void PopUpInitialize();
public:
    GUIpopup();
    GUIpopup(GUIwidget*, unsigned int index); // create a popup with link to parent
    GUIpopup(sf::Unicode::Text newItems); // create a popup with items field populated
    GUIpopup(GUIwidget*, unsigned int index, sf::Unicode::Text newItems); // create a popup with link to parent and items field populated
    void LinkPopUp(unsigned int index, GUIpopup*); // link a sub-pop-up to this pop-up
    void draw();
};

class GUImenuBar: public GUIpanel
{
private:
    void menuBarInitialize(GUIwidget*);
    struct Entry {GUIbutton* btn; GUIpopup* pop;}; // each main menu entry contains a button and a popup pointer
//    std::vector<GUIbutton*> Items;  // used to reference buttons for the entries
    bool hidden;  // indicates if hidden - irrelevant if canHide is false
public:
    std::vector<Entry> items;
    bool canHide; // if true, the menu bar is only visible when mouse moves to top of form and subequently stays over menu
    GUImenuBar();
    GUImenuBar(GUIwidget*);
    GUImenuBar(sf::Unicode::Text newItems);
    GUImenuBar(GUIwidget*, sf::Unicode::Text newItems);
    void draw();
    int AddMenuItem(sf::Unicode::Text newItem); // returns tag (index) for this item
    void AddMenuItems(sf::Unicode::Text newItems); // multile entries separated by commas
//
//    void (*OnClick) (GUIwidget *);
//
    void MenuItemOnClick(int index, void (*OnClick) (GUIwidget *));
//    int AddMenuSubItem(int tag, sf::Unicode::Text newItem); // returns tag (index) for this item
    void Resize(int Width, int Height);
    void LinkPopUp(int index, GUIpopup* pPopUP); // link pPopUp to this menu at index
};

class GUIstatusBar: public GUIpanel
{
private:
    class GUIstatusItem
    {
    public:
        int Tag;
        int width;
        sf::Unicode::Text stat;
    };
    void statusBarInitialize(GUIwidget*);
    int LastTag;
    std::vector<GUIstatusItem*> leftItems;
    std::vector<GUIstatusItem*> rightItems;
    sf::String myText;
public:
    GUIstatusBar();
    GUIstatusBar(GUIwidget*);
    ~GUIstatusBar();
    void draw();
    int CreateItem(int thisWidth, GUIalignment align); // used to set the layout, returns a tag for future references
    int CreateItem(int thisWidth, GUIalignment align, sf::Unicode::Text newText); // used to set the layout, returns a tag for future references
    void DeleteItem(int tag); // used to remove the indicated item altogether, use the tag returned by the create routine
    void SetText(int tag, sf::Unicode::Text newText);
    void SetWidth(int tag, int newWidth);
};

class GUImemo: public GUIpanel
{
private:
    void MemoInitialize(GUIwidget*);
    std::vector<sf::String *> lines;
    sf::String memoStr;
    GUIscrollBar Scroll;        // vertical scroll bar for long text
public:
    GUImemo();
    GUImemo(GUIwidget*);
    GUImemo(sf::Unicode::Text newText);
    GUImemo(GUIwidget*, sf::Unicode::Text newText);
    GUImemo(sf::Unicode::Text newText, GUIalignment align);
    GUImemo(GUIwidget*, sf::Unicode::Text newText, GUIalignment align);
    GUImemo(int Left, int Top);
    GUImemo(GUIwidget*, int Left, int Top);
    GUImemo(int Left, int Top, sf::Unicode::Text newText);
    GUImemo(GUIwidget*, int Left, int Top, sf::Unicode::Text newText);
    GUImemo(int Left, int Top, sf::Unicode::Text newText, GUIalignment align);
    GUImemo(GUIwidget*, int Left, int Top, sf::Unicode::Text newText, GUIalignment align);
    GUImemo(int Left, int Top, int Width, int Height);
    GUImemo(GUIwidget*, int Left, int Top, int Width, int Height);
    GUImemo(int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUImemo(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText);
    GUImemo(int Left, int Top, int Width, int Height, sf::Unicode::Text newText, GUIalignment align);
    GUImemo(GUIwidget*, int Left, int Top, int Width, int Height, sf::Unicode::Text newText, GUIalignment align);
    void SetTextSize(int newSize);
    void SetTextColor(sf::Color newColor);
    void AddParagraph(sf::Unicode::Text newText);
    void AddParagraph(sf::Unicode::Text newText, GUIalignment align);
    void draw();
    void Resize(int Width, int Height);
};

class GUIspinner: public GUIpanel
{
private:
    void SpinnerInitialize(GUIwidget*);
    GUIedit SpinEdit;
    GUIbutton BtnUp;
    GUIbutton BtnDn;
public:
    GUIspinner();
    GUIspinner(GUIwidget*);
    GUIspinner(int Left, int Top);
    GUIspinner(GUIwidget*, int Left, int Top);
    GUIspinner(int Left, int Top, int Width, int Height);
    GUIspinner(GUIwidget*, int Left, int Top, int Width, int Height);
    int value;
    int stepSize;
    int max;
    int min;
    void draw();
    void Resize(int Width, int Height);
};


class GUIstringGrid: public GUIpanel
{
private:
    void StringGridInitialize(GUIwidget*);
    std::vector<std::vector<sf::String *> > items; // rows of columns
    sf::String myText; // model for all strings
    GUIpanel box;
    enum EditState {esNoSelection, esSelected, esEditing};
public:
    GUIedit Edit;
    GUIscrollBar VertScroll;
    GUIscrollBar HorzScroll;
    GUIstringGrid(GUIwidget* =NULL);
    GUIstringGrid(int Left, int Top);
    GUIstringGrid(GUIwidget*, int Left, int Top);
    GUIstringGrid(int Left, int Top, int Width, int Height);
    GUIstringGrid(GUIwidget*, int Left, int Top, int Width, int Height);
    int topRow;             // index of the first visible row in the scrollable region
    int leftCol;            // index of the first visible column in the scrollable region
    int colCount;           // number of columns, including fixed column if present.
    int rowCount;
    int selectedRow, selectedCol;
    bool showSelected;
    bool allowEdit;
    EditState SelectionState;
    int rowHeight;
    int defaultColWidth;
    std::vector<int> colWidths;
    bool fixedCol;          // if true, left column is always displayed (and has button face color)
    bool fixedRow;          // if true, the top row is always displayed (and has button face color)
    bool doFixedVertLine;   // draw vertical line between columns on fixed row
    bool doFixedHorzLine;   // draw horizontal line between rows on fixed column
    bool doVertLine;        // draw vertical lines between columns (except fixed column)
    bool doHorzLine;        // draw horizontal lines between rows (except fixed row)
    void draw();
    void SetText(int row, int col, sf::Unicode::Text newText);
    sf::Unicode::Text GetText(int row, int col);
    void SetColCount(int newCount);
    void SetRowCount(int newCount);
    void Resize(int Width, int Height);
    void SetLeftCol(int);
    void SetTopRow(int);
    void SetFixedRow(bool);
    void SetFixedCol(bool);
    void MouseSelect(int X, int Y);
};

class GUIcomboBox: public GUIpanel
{
private:
    void ComboBoxInitialize(GUIwidget*);
public:
    GUIedit Edit;
    GUIlistBox ListBox;
    GUIbutton DropBtn;
    GUIpanel Box;
    GUIcomboBox(GUIwidget* =NULL);
    GUIcomboBox(int left, int top);
    GUIcomboBox(GUIwidget*, int left, int top);
    GUIcomboBox(int left, int top, int width, int height);
    GUIcomboBox(GUIwidget*, int left, int top, int width, int height);
//    int selected; // which item is currently selected. Range = zero to count-1
    int dropDownCount; // maximum number of items to show in list - uses scroll bar if more items than this exist
    void AppendItem(sf::Unicode::Text newText);// add item to list box
    void draw();
    void Resize(int Width, int Height);
};

struct Node
{
    bool expand; // whether or not to show the children in the view
    sf::Sprite glyph; // image for the node
    sf::String myText; // text description
    std::vector<Node*> children;
    unsigned int childIndex;
    Node* parent;
};

class GUItree: public GUIpanel
{
private:
    void TreeInitialize(GUIwidget* pWidget);
    sf::String myText;
public:
    GUItree(GUIwidget* =NULL);
    GUItree(int left, int top);
    GUItree(GUIwidget*, int left, int top);
    GUItree(int left, int top, int width, int height);
    GUItree(GUIwidget*, int left, int top, int width, int height);
    Node Root;
    Node* CreateNode();
    Node* CreateNode(sf::Unicode::Text, Node* = NULL);
    Node* CreateNode(sf::Sprite, sf::Unicode::Text, Node* = NULL);
    void AddNode(Node* parentNode, Node* newNode);
    void draw();
};

extern GUIform GUImain;

#endif /*SIGUI_H_*/
