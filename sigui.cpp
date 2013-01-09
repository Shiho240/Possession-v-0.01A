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

#include "sigui.hpp"
//#include <string>
#include <cstdio>
#include <iostream>

#include <list>
#define PI 3.14159265

// default values for GUI components
const int GUIdefaultPanelWidth   = 200;
const int GUIdefaultPanelHeight  = 200;
const int GUIdefaultButtonWidth  =  80;
const int GUIdefaultButtonHeight =  30;
const int GUIdefaultBevelWidth   =   4;
const int GUIdefaultRadius       =  10; // for the corners of rounded rectangles
const int GUIdefaultTextSize     =  16;
const int GUIformTitleBarHeight  = 22;
const float GUIrepeatTime        = 0.150; // seconds between autorepeats of so-designated buttons
const int GUIrepeatDelay         = 10;   // delay count before autorepeat active (should be about 1.5 to 2 sec)


const unsigned int GUIresizeLeft   = 1;
const unsigned int GUIresizeRight  = 2;
const unsigned int GUIresizeTop    = 4;
const unsigned int GUIresizeBottom = 8;

//GUIborderStyle defaultBorderStyle = rectangle;
//GUIborderStyle defaultBorderStyle = rounded;
//GUIborderStyle defaultBorderStyle = flatRounded;
//GUIborderStyle defaultBorderStyle = flatRectangle;

sf::Color GUIdefaultFaceColor          = sf::Color(200, 200, 200);
sf::Color GUIdefaultFaceTextColor      = sf::Color(  0,   0,   0);
sf::Color GUIdefaultHighlightColor     = sf::Color(100, 100, 255);
sf::Color GUIdefaultHighlightTextColor = sf::Color(255, 255, 255);
sf::Color GUIdefaultWindowColor        = sf::Color(255, 255, 255);
sf::Color GUIdefaultWindowTextColor    = sf::Color(  0,   0,   0);


sf::Clock GUIcursorClock;   // clock used to blink the edit cursor
sf::Clock GUIrepeatClock;  // use for autorepeating buttons
bool GUIperformRepeat;      // evaluated at each draw loop
bool GUIdrawPopUps;

GUIwidget* pWidgetInFocus;

sf::RenderWindow *GUIapp;      // this is the opengl "render context" from the main program.
const sf::Input *GUImenuInput; // this is the SFML input system for the active render window
sf::Font GUIdefaultFont;

int GUIdragAnchorX, GUIdragAnchorY; // point where mouse grabbed widget for dragging
//std::list<GUIform*>  GUIvisibleForms; // tracks which forms are visible and the order to draw them. The last form is "on top"
//std::list<GUIform*>::iterator GUIvisibleFormsIterator;
const int GUIformMaxCount = 100; // maximum number of forms that can be visible at one time
GUIform* GUIformVisible[GUIformMaxCount]; // forms to display, last item will be on "top"
int GUIformCount; // current number of forms which are being displayed

GUIform GUImain; // refers to the main window of the application

bool GUIinitialize(sf::RenderWindow *AppName)
{
    // this routine should be called after the main program sets up the opengl render window.
    // this allows the menu routines to access the sfml routines related to the render window
    GUIapp = AppName;
    GUImenuInput = &GUIapp->GetInput();
    pWidgetInFocus = NULL; // no widget currently has focused - we just started up!
    GUIdefaultFont= sf::Font::GetDefaultFont();
    GUIdragAnchorX = 0; GUIdragAnchorY = 0;
    GUIrepeatClock.Reset();
    GUIcursorClock.Reset();
    for (int i=0; i<GUIformMaxCount; i++) GUIformVisible[i] = NULL;
    GUIformCount = 0;
    GUImain.isVisible = true;
    GUImain.Resize(GUIapp->GetWidth(), GUIapp->GetHeight());
    GUImain.showTitleBar = false;
    GUImain.useParentColors = false;
    GUImain.SetFont(GUIdefaultFont);
    GUImain.show(); // by default, guimain will be active
    GUImain.parentApp = AppName;
    GUImain.borderBevel = false;
    return true;
}

void GUItempBtnOnClick(GUIwidget *pWidget) // the button was clicked
{
//    static_cast<GUItempBtn*>(pWidget)->pressed = !static_cast<GUItempBtn*>(pWidget)->pressed;
}

void FindSceenOfs(GUIwidget* thisComponent, int &xOfs, int &yOfs)
{
    xOfs = thisComponent->left;
    yOfs = thisComponent->top;
    while (thisComponent->parent != NULL)
    {
        xOfs = xOfs + thisComponent->parent->left;
        yOfs  = yOfs  + thisComponent->parent->top;
        thisComponent = thisComponent->parent;
    }
}

void GUIformOnClick(GUIwidget *pWidget)
{
    GUIform* thisForm;
    thisForm = static_cast<GUIform*>(pWidget);
    // was title bar clicked? if so, start drag mode
    int mouseX = GUImenuInput->GetMouseX();
    int mouseY = GUImenuInput->GetMouseY();
    int L = pWidget->left;
    int R = L + pWidget->width;
    int T = pWidget->top;
    int B = T + pWidget->height;
    thisForm->beingDragged = false;
    thisForm->beingResized = false;
    thisForm->resizeEdge = 0; //clear
    if (thisForm->showTitleBar) // can only resize or drag if title bar/border visible
    {
        if (thisForm->resizable&&(L <= mouseX)&&(mouseX <= (L+2))&&(T <= mouseY)&&(mouseY <= B))
        {   // left edge clicked
            thisForm->beingResized = true;
            thisForm->resizeEdge += GUIresizeLeft;
            if (mouseY > (B-20)) thisForm->resizeEdge += GUIresizeBottom; // corner - resixe bottom also
        }

        if (thisForm->resizable&&((R-2) <= mouseX)&&(mouseX <= R)&&(T <= mouseY)&&(mouseY <= B))
        {   // right edge clicked
            thisForm->beingResized = true;
            thisForm->resizeEdge += GUIresizeRight;
            if (mouseY > (B-20)) thisForm->resizeEdge += GUIresizeBottom; // corner - resixe bottom also
        }

        if (thisForm->resizable&&(L <= mouseX)&&(mouseX <= R)&&((B-2) <= mouseY)&&(mouseY <= B))
        {   // bottom edge clicked
            thisForm->beingResized = true;
            thisForm->resizeEdge += GUIresizeBottom;
            if (mouseX < (L+20)) thisForm->resizeEdge += GUIresizeLeft; // corner - resixe bottom also
            if (mouseX > (R-20)) thisForm->resizeEdge += GUIresizeRight; // corner - resixe bottom also
        }

        if (thisForm->resizable&&(L <= mouseX)&&(mouseX <= R)&&(T <= mouseY)&&(mouseY <= (T+2)))
        {   // top edge clicked
            thisForm->beingResized = true;
            thisForm->resizeEdge += GUIresizeTop;
        }
        else
        {
            if ((L <= mouseX)&&(mouseX <= R)&&(T <= mouseY)&&(mouseY <= (T+GUIformTitleBarHeight)))
            {
                thisForm->beingDragged = true;
                GUIdragAnchorX = mouseX - pWidget->left;
                GUIdragAnchorY = mouseY - pWidget->top;
            }
        }
        if (thisForm->beingResized)
        {
            thisForm->oldLeft = thisForm->left; thisForm->oldTop = thisForm->top;
            thisForm->oldWidth = thisForm->width; thisForm->oldHeight = thisForm->height;
        }
    }
}

bool GUIisTabStop(GUIwidget* thisPtr)
{
    if (thisPtr->thisType == tguiButton)
    {
        if (!static_cast<GUIbutton*>(thisPtr)->enabled) return false;
    }
    return thisPtr->tabStop;
}

void GUIfocusChange(bool reverse)
{
  GUIwidget *pTemp, *pStart;
  GUIform* pThisForm = NULL;
  // determine form with focus
  if (GUIformCount > 0)
  {
      pThisForm = GUIformVisible[GUIformCount-1];
      pStart = pThisForm->ActiveWidget;
  }
  else pStart = NULL;
  // pStart points to the active widget of the current form
  if (pThisForm->children.size() == 0) pStart = NULL; // if the form has no children, nothing to do

  if (pStart != NULL)
  {
    pTemp = pStart; // point to current
    bool bDone = false;
    while (!bDone)
    {
      if (pTemp->children.size() > 0)
      { // has children
        // move to first/last child
        if (reverse)  pTemp = pTemp->children.back(); // move to last child
        else pTemp = pTemp->children[0]; // move to first child

        if (GUIisTabStop(pTemp)||(pStart == pTemp)) // found a new one or came back to original
        {
            pThisForm->ActiveWidget = pTemp; // the child becomes the new focus
           pWidgetInFocus = pTemp;
           bDone = true;
           return;
        }
      }
      else
      {  // no child
          bool bbDone = false;
          while (!bbDone)
          {
              // is there another sibling?
              if (pTemp->parent != NULL)
              {
                  if(reverse) // reverse direction, check for sibling to left
                  {
                    if (pTemp->siblingOrder >0)
                    {  // there is another sibling
                        pTemp = pTemp->parent->children[pTemp->siblingOrder-1]; // move to prev sibling
                        bbDone = true;
                    }
                  }
                  else // forward direction, check for sibling to right
                  {
                    if (pTemp->siblingOrder < (int(pTemp->parent->children.size())-1))
                    {  // there is another sibling
                        pTemp = pTemp->parent->children[pTemp->siblingOrder+1]; // advance to next sibling
                        bbDone = true;
                    }
                  }
              }

              if (!bbDone)
              {  // not another sibling
                  if (pTemp->parent == NULL)
                  { // at root
                      bbDone = true;
                  }
                  else
                  {  // not another sibling, not at root
                      pTemp = pTemp->parent; // ascend
                  }

              }

              if (bbDone)
              {
                  if (GUIisTabStop(pTemp)||(pStart == pTemp)) // found a new one or came back to original
                  {
                     pThisForm->ActiveWidget = pTemp; // the sibling becomes the new focus
                     pWidgetInFocus = pTemp;
                     return;
                  }
              }
          }
      }
    }
    pWidgetInFocus = pThisForm->ActiveWidget;
  }
}


bool GUIprocessEvent(sf::Event thisEvent) // inspects "Event", if it applies to the GUI, it is processed and true is returned (ie, no need to process in the main routine)
{
  bool WidgetWasClicked = false;
  GUIwidget* thisWidget;
  thisWidget = NULL;
  GUIform* thisForm = NULL;
  bool PopUpClicked = false;
  if (GUIformCount > 0)
  {
      int formIndex = GUIformCount-1; // start at last ("top") form
      thisForm = GUIformVisible[formIndex]; // start at last ("top") form
      if (thisEvent.Type == sf::Event::MouseButtonPressed)
      {
          int mouseX = GUImenuInput->GetMouseX();
          int mouseY = GUImenuInput->GetMouseY();
          // is the mouse over any active form?
          bool MouseIsOver = false;
          while ((formIndex >= 0)&& !MouseIsOver)
          {
              thisForm = GUIformVisible[formIndex];
              if ((thisForm->left <= mouseX)&&((thisForm->left+thisForm->width)  >= mouseX)&&
                  (thisForm->top  <= mouseY)&&((thisForm->top +thisForm->height) >= mouseY))
              {
                  MouseIsOver = true;
              }
              else formIndex--;
          }
          if (MouseIsOver) // mouse press occured over a GUI form
          { // move it to the back so it will be on "top"
              for (int i=formIndex; i<GUIformCount; i++)
              {
                  GUIformVisible[i] = GUIformVisible[i+1];
              }
              GUIformVisible[GUIformCount-1] = thisForm;
              pWidgetInFocus = thisForm->ActiveWidget;

              // close popups on all other forms
              for (int i=0; i<GUIformCount; i++)
              {
                  if (i != formIndex)
                  GUIformVisible[i]->ClearPopUps();
              }

              int thisLeft, thisRight, thisTop, thisBottom;

              thisWidget = thisForm;
              // If this form has any active popups. inspect them first
              if (thisForm->PopUps.size()>0)
              {

                  unsigned int index = 0;
                  while ((index < thisForm->PopUps.size())&&(!WidgetWasClicked))
                  {
                      thisWidget = thisForm->PopUps[index];
                      FindSceenOfs(thisWidget, thisLeft, thisTop);
                      thisRight = thisLeft + thisWidget->width;
                      thisBottom = thisTop + thisWidget->height;
                      if ((thisWidget->isVisible)&&(thisLeft <= mouseX)&&(thisRight >= mouseX)&&(thisTop <= mouseY)&&(thisBottom >= mouseY))
                      { // this popup was hit by mouse - reassign it to the pointer of interest
                          thisWidget = thisWidget;
                          WidgetWasClicked = true;
                          PopUpClicked = true;
                      }
                      index++;
                  }
                  if (!WidgetWasClicked)
                  {     // pop ups were active but mouse clicked somewhere other than on a popup -
                        // remove them
                      thisForm->ClearPopUps();
                  }
              }

              /////// finished inspecting popups

              if (!WidgetWasClicked)
              {
                thisWidget = thisForm;
              }

              {
                  // step through children looking for a hit
//                  thisWidget = thisForm;
                  int childPointer = 0;
                  while (childPointer < int(thisWidget->children.size()))
                  {
                    FindSceenOfs(thisWidget->children[childPointer], thisLeft, thisTop);
                    thisRight = thisLeft + thisWidget->children[childPointer]->width;
                    thisBottom = thisTop + thisWidget->children[childPointer]->height;
                    if ((thisWidget->children[childPointer]->isVisible)&&(thisLeft <= mouseX)&&(thisRight >= mouseX)&&(thisTop <= mouseY)&&(thisBottom >= mouseY))
                    { // this child was hit by mouse - reassign it to the pointer of interest
                        thisWidget = thisWidget->children[childPointer];
                        childPointer = 0;
                    }
                    else childPointer++; // look at next child
                  }
                  // "thisWidget" now points to the component actually being clicked
                  WidgetWasClicked = true;
                  if ((thisWidget->thisType == tguiListBox)&&(PopUpClicked))
                  {
                     thisForm->ClearPopUps();
                  }
              }
          }
          else
          {   // click did not occur on any form - close all popups
              for (int i=0; i<GUIformCount; i++)
              {
                  GUIformVisible[i]->ClearPopUps();
              }
          }
      } // end of "Mouse Pressed"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (thisEvent.Type == sf::Event::MouseMoved)
      {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if ((thisWidget->thisType == tguiEdit)&&(GUImenuInput->IsMouseButtonDown(sf::Mouse::Left)))
                { // mouse is dragging over edit box
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    int keepSelectionStart = pEdit->selectionStart;
                    pEdit->MouseCursor(GUImenuInput->GetMouseX(), GUImenuInput->GetMouseY()); // reposition cursor base on current mouse position
                    pEdit->selectionStart = keepSelectionStart;
                    return true;
                }
                //WidgetWasClicked = true;
            }
      }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (thisEvent.Type == sf::Event::KeyPressed)
      {
         if(thisEvent.Key.Code == sf::Key::Tab)
         {
            GUIfocusChange(thisEvent.Key.Shift);
            return true;
         }
         if(thisEvent.Key.Code == sf::Key::Space)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiButton)
                {  // check for momentary button already pressed - dont press it again
                    if (static_cast<GUIbutton*>(thisWidget)->toggle ||
                       !static_cast<GUIbutton*>(thisWidget)->pressed)   WidgetWasClicked = true; else return true;
                }
                if (thisWidget->thisType == tguiEdit)
                {
                    return true;
                }
                WidgetWasClicked = true;
            }
         }
         if (thisEvent.Key.Code == sf::Key::Return)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    pEdit->selectionStart = pEdit->cursorPos;
                    if (pEdit->wasChanged())
                    {
                        if (thisWidget->OnChange != NULL) thisWidget->OnChange(thisWidget);
                    }
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::Left)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    if (pEdit->cursorPos > 0) pEdit->cursorPos--;
                    if (!thisEvent.Key.Shift) pEdit->selectionStart = pEdit->cursorPos;
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::Right)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    std::string str = pEdit->myText.GetText();
                    if (pEdit->cursorPos < int(str.length())) pEdit->cursorPos++;
                    if (!thisEvent.Key.Shift) pEdit->selectionStart = pEdit->cursorPos;
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::Back)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    std::string str = pEdit->myText.GetText();
                    if (pEdit->selectionStart != pEdit->cursorPos)
                    {  // delete selection
                        int selectionLeft = pEdit->cursorPos;
                        int selectionRight = pEdit->cursorPos;
                        if (pEdit->selectionStart < selectionLeft) selectionLeft = pEdit->selectionStart;
                        if (pEdit->selectionStart > selectionRight) selectionRight = pEdit->selectionStart;
                        str.erase(selectionLeft, selectionRight-selectionLeft);
                        pEdit->cursorPos = selectionLeft;
                    }
                    else
                    {  // delete character to left of cursor
                        if (pEdit->cursorPos > 0)
                        {
                            pEdit->cursorPos--;
                            str.erase(pEdit->cursorPos, 1);
                        }
                    }
                    pEdit->myText.SetText(str);
                    pEdit->selectionStart = pEdit->cursorPos;
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::Delete)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    std::string str = pEdit->myText.GetText();
                    if (pEdit->selectionStart != pEdit->cursorPos)
                    {  // delete selection
                        int selectionLeft = pEdit->cursorPos;
                        int selectionRight = pEdit->cursorPos;
                        if (pEdit->selectionStart < selectionLeft) selectionLeft = pEdit->selectionStart;
                        if (pEdit->selectionStart > selectionRight) selectionRight = pEdit->selectionStart;
                        str.erase(selectionLeft, selectionRight-selectionLeft);
                        pEdit->cursorPos = selectionLeft;
                    }
                    else
                    {  // delete character to right of cursor
                        if (pEdit->cursorPos < int(str.length()))
                        {
                            str.erase(pEdit->cursorPos,1);
                        }

                    }
                    pEdit->myText.SetText(str);
                    pEdit->selectionStart = pEdit->cursorPos;
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::Home)
         {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    if (thisEvent.Key.Shift)
                    {
                      if (pEdit->selectionStart < pEdit->cursorPos) pEdit->selectionStart = pEdit->cursorPos;
                    }
                    else pEdit->selectionStart = 0;
                    pEdit->cursorPos = 0;
                    return true;
                }
            }
         }
         if (thisEvent.Key.Code == sf::Key::End)
         {
            if (thisForm->ActiveWidget != NULL)
//            if ()
            {
                thisWidget = thisForm->ActiveWidget;
                if (thisWidget->thisType == tguiEdit)
                {
                    GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
                    std::string str = pEdit->myText.GetText();
                    if (thisEvent.Key.Shift)
                    {
                      if (pEdit->selectionStart > pEdit->cursorPos) pEdit->selectionStart = pEdit->cursorPos;
                    }
                    else pEdit->selectionStart = str.length();
                    pEdit->cursorPos = str.length();
                    return true;
                }
            }
         }
         //
//return false;
         if (!WidgetWasClicked) return false;
      } // end of "Key Pressed"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (thisEvent.Type == sf::Event::TextEntered) // Event.Text.Unicode
      {
            if (thisForm->ActiveWidget != NULL)
            {
                thisWidget = thisForm->ActiveWidget;
                if ((thisWidget->thisType == tguiEdit)&&(thisEvent.Text.Unicode != 8))
                {
                    GUIedit* pEdit;
                    pEdit = static_cast<GUIedit*>(thisWidget);
                    std::string str = pEdit->myText.GetText();
                    std::string str3 = sf::Unicode::Text::Text(&thisEvent.Text.Unicode);
                    // delete selection if any...
                    if (pEdit->selectionStart < pEdit->cursorPos)
                    {
                        str.erase(pEdit->selectionStart, pEdit->cursorPos-pEdit->selectionStart);
                        pEdit->cursorPos = pEdit->selectionStart;
                    }
                    if (pEdit->selectionStart > pEdit->cursorPos)
                    {
                        str.erase(pEdit->cursorPos, pEdit->selectionStart-pEdit->cursorPos);
                        pEdit->selectionStart = pEdit->cursorPos;
                    }

                    str.insert(pEdit->cursorPos, str3, 0, 1); // now insert new text
                    pEdit->myText.SetText(str);
                    pEdit->cursorPos++;
                    pEdit->selectionStart = pEdit->cursorPos;
                    return true;
                }
            }
      }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

  }

  if (WidgetWasClicked)
  {

      if (thisWidget->thisType == tguiButton)
      {
          if (static_cast<GUIbutton*>(thisWidget)->enabled)
          {
            if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
            pWidgetInFocus = thisForm->ActiveWidget;
            static_cast<GUIbutton*>(thisWidget)->pressed = !static_cast<GUIbutton*>(thisWidget)->pressed;
            if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
          }
          return true;
      }
      if (thisWidget->thisType == tguiCheckBox)
      {
          if (static_cast<GUIcheckBox*>(thisWidget)->enabled)
          {
            if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
            pWidgetInFocus = thisForm->ActiveWidget;
            static_cast<GUIcheckBox*>(thisWidget)->checked = !static_cast<GUIcheckBox*>(thisWidget)->checked;
            if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
          }
          return true;
      }
      if (thisWidget->thisType == tguiRadioButton)
      {
          if (static_cast<GUIradioButton*>(thisWidget)->enabled)
          {
            if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
            pWidgetInFocus = thisForm->ActiveWidget;
            static_cast<GUIradioButton*>(thisWidget)->checked = !static_cast<GUIradioButton*>(thisWidget)->checked;
            if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
            // uncheck any other radio buttons which are siblings
            if (thisWidget->parent != NULL)
            {
                GUIwidget* parentPtr = thisWidget->parent;
                for (unsigned int i=0; i<parentPtr->children.size(); i++)
                {  // step through parent's children
                    if (parentPtr->children[i]->thisType == tguiRadioButton)
                    {
                        if (parentPtr->children[i] != thisWidget)
                        {
                            static_cast<GUIradioButton*>(parentPtr->children[i])->checked = false;
                        }
                    }
                }
            }
          }
          return true;
      }

      if (thisWidget->thisType == tguiEdit)
      {
          if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
          pWidgetInFocus = thisForm->ActiveWidget;
          GUIedit* pEdit = static_cast<GUIedit*>(thisWidget);
          int thisX = GUImenuInput->GetMouseX();
          int thisY = GUImenuInput->GetMouseY();
          pEdit->MouseCursor(thisX, thisY);
          return true;
      }

      if (thisWidget->thisType == tguiListBox)
      {
          if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
          pWidgetInFocus = thisForm->ActiveWidget;
          GUIlistBox* pList = static_cast<GUIlistBox*>(thisWidget);
//          // was a child
//          if(thisWidget == pList->b)
          int thisX = GUImenuInput->GetMouseX();
          int thisY = GUImenuInput->GetMouseY();
          int oldSelection = pList->selected;
          pList->MouseSelect(thisX, thisY);
          if (pList->selected != oldSelection)
          {
              if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
          }
          return true;
      }

      if (thisWidget->thisType == tguiStringGrid)
      {
          if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
          pWidgetInFocus = thisForm->ActiveWidget;
          GUIstringGrid* pGrid = static_cast<GUIstringGrid*>(thisWidget);
          int thisX = GUImenuInput->GetMouseX();
          int thisY = GUImenuInput->GetMouseY();
//          int oldSelectedRow = pGrid->selectedRow;
//          int oldSelectedCol = pGrid->selectedCol;
          pGrid->MouseSelect(thisX, thisY);
/*
          if ((pGrid->selectedRow != oldSelectedRow)||(pGrid->selectedCol != oldSelectedCol))
          {
              if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
          }
*/
          return true;
      }

      if (thisWidget->tabStop) thisForm->ActiveWidget = thisWidget;
      pWidgetInFocus = thisForm->ActiveWidget;
      if (thisWidget->OnClick != NULL) thisWidget->OnClick(thisWidget);
      return true;
  }

  return false;
}



void GUIdraw()
{
  GUIdrawPopUps = false;

  GUIform* pForm = NULL;
  if (GUIrepeatClock.GetElapsedTime() > GUIrepeatTime)
  {
     GUIperformRepeat = true;
     GUIrepeatClock.Reset();
  }
  else GUIperformRepeat = false;

  if (GUIcursorClock.GetElapsedTime() > 3600) GUIcursorClock.Reset(); // reset every hour

  // make sure GUImain (if present) is not on top of other forms
  int index;
  bool outofplace = false;
  for (int i=1; i<GUIformCount; i++)
  {
      if (GUIformVisible[i] == &GUImain)
      {
          index = i;
          outofplace = true;
      }
  }
  if (outofplace)
  {
      for (int i=index; i>0; i--)
      {
          GUIformVisible[i] = GUIformVisible[i-1];
      }
      GUIformVisible[0] = &GUImain;
  }


  for (int i=0; i<GUIformCount; i++) // draws forms from back to front
  {
      // check for popups: allow for guimain and for top form only
      if ((i<(GUIformCount-1))&&(GUIformVisible[i] != &GUImain))
      {
          GUIformVisible[i]->ClearPopUps();
      }
      GUIformVisible[i]->draw();
  }

  if (GUIformCount > 0)
  {
      pForm = GUIformVisible[GUIformCount-1];
      if (pForm->ActiveWidget == NULL)
      {
          GUIfocusChange(false);
      }
      else pWidgetInFocus = pForm->ActiveWidget;

      // draw pop-ups on the top form
      GUIdrawPopUps = true;
      for (unsigned int i=0; i < pForm->PopUps.size(); i++)
      {
          (pForm->PopUps[i])->draw();
      }
      GUIdrawPopUps = false;
  }
}

void GUIclose()  // close all visible forms
{
    for(int i=0; i<GUIformCount; i++)
    {
        GUIformVisible[i]->isVisible = false;
        GUIformVisible[i] = NULL;
    }
    GUIformCount = 0;
}

void CopyParentColors(GUIwidget* thisWidget)
{
    if ((thisWidget != NULL)&&(thisWidget->parent != NULL))
    {
      thisWidget->faceColor           = thisWidget->parent->faceColor;
      thisWidget->highlightColor      = thisWidget->parent->highlightColor;
      thisWidget->faceTextColor       = thisWidget->parent->faceTextColor;
      thisWidget->highlightTextColor  = thisWidget->parent->highlightTextColor;
      thisWidget->windowColor         = thisWidget->parent->windowColor;
      thisWidget->windowTextColor     = thisWidget->parent->windowTextColor;
    }
}

void GUIwidget::WidgetInitialize()
{
    canAlign = false;
    isVisible = true;
    tabStop = false;
    Constraint.minHeight = 0;
    Constraint.minWidth = 0;
    Constraint.maxHeight = 0;
    Constraint.maxWidth = 0;
    clLeft = clTop = 0;
    clWidth = width;
    clHeight = height;
    parent = NULL;
    children.clear();
    OnChange = NULL;
    OnClick = NULL;
    thisType = tguiWidget;
    useParentFont = true;
    font = GUIdefaultFont;
    useParentColors = true;
    siblingOrder = 0;
    faceColor = GUIdefaultFaceColor;
    highlightColor = GUIdefaultHighlightColor;
    faceTextColor = GUIdefaultFaceTextColor;
    highlightTextColor = GUIdefaultHighlightTextColor;
    windowColor = GUIdefaultWindowColor;
    windowTextColor = GUIdefaultWindowTextColor;
}

void GUIwidget::ReAlign(GUIalign newAlign)
{
    // default routine does nothing, will be overridden by components where applicable
}

GUIwidget::GUIwidget()   // default constructor - sets position to upper left corner and size to zero (not typically used)
{
    left = 0;
    top = 0;
    width = 0;
    height = 0;
    WidgetInitialize();
}

GUIwidget::GUIwidget(int Left, int Top)   // another constructor - user sets position, but has size of zero (not typically used)
{
    left = Left;
    top = Top;
    width = 0;
    height = 0;
    WidgetInitialize();
}

GUIwidget::GUIwidget(int Left, int Top, int Width, int Height)   // typical constructor - user sets position and size
{
    left = Left;
    top = Top;
    // size must be non-negative
    if (Width > 0) width = Width;
    else width = 0;
    if (Height > 0) height = Height;
    else height = 0;
    WidgetInitialize();
}

void GUIwidget::SetFont(sf::Font newFont)
{
    font = newFont;
}


sf::Font& GUIwidget::GetFont()
{
    return font;
}

int GUIwidget::AddChild(GUIwidget * childWidget)
{
    childWidget->parent = this;
    childWidget->siblingOrder = this->children.size();
    children.push_back(childWidget); // insert a pointer to the child widget into the "children" vector
    return 0;
}

void GUIbutton::btnInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    bevelWidth = GUIdefaultBevelWidth;
    isTabButton = false;
    autorepeatcountdown = GUIrepeatDelay;
    pressed = false;
    thisType = tguiButton;
    OnClick = &GUItempBtnOnClick;
    btnLabel.parent = this;
    btnLabel.isVisible = true;
    tabStop = true;
    flat = false;
    toggle = true;
    enabled = true;
    autorepeat = false;
    btnLabel.textAlignment = atCenter;
    glyphAlignment = atLeft;
    textAlignment = atCenter;
    glyphHorizontalInset = bevelWidth;
    glyphVerticalInset = bevelWidth;
    if (pWidget != NULL) pWidget->AddChild(this);
}

void GUIbutton::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
}

GUIbutton::GUIbutton() : GUIpanel()
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText("Button");
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(sf::Unicode::Text newText) : GUIpanel()
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText(newText);
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(int Left, int Top) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText("Button");
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(int Left, int Top, sf::Unicode::Text newText) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText(newText);
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    btnLabel.SetText("Button");
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(int Left, int Top, int Width, int Height, sf::Unicode::Text newText) : GUIpanel(Left, Top, Width, Height)
{
    btnLabel.SetText(newText);
    btnInitialize(NULL);
}

GUIbutton::GUIbutton(GUIwidget* pWidget) : GUIpanel()
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText("Button");
    btnInitialize(pWidget);
}

GUIbutton::GUIbutton(GUIwidget* pWidget, sf::Unicode::Text newText) : GUIpanel()
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText(newText);
    btnInitialize(pWidget);
}

GUIbutton::GUIbutton(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText("Button");
    btnInitialize(pWidget);
}

GUIbutton::GUIbutton(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth; height = GUIdefaultButtonHeight;
    btnLabel.SetText(newText);
    btnInitialize(pWidget);
}

GUIbutton::GUIbutton(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    btnLabel.SetText("Button");
    btnInitialize(pWidget);
}

GUIbutton::GUIbutton(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText) : GUIpanel(Left, Top, Width, Height)
{
    btnLabel.SetText(newText);
    btnInitialize(pWidget);
}

void GUIbutton::SetText(sf::Unicode::Text newText)
{
    btnLabel.SetText(newText);
}

void GUIbutton::draw()
{
  if (pressed) bevelOuter = bsDown; else bevelOuter = bsUp;
  if (flat) bevelOuter = bsNone; // if "flat", draw without bevel unless mouse over
  if (enabled) skin.SetColor(sf::Color(255, 255, 255, 255));
  else skin.SetColor(sf::Color(128, 128, 128));

  if (useParentColors) CopyParentColors(this);

  if (isVisible) GUIpanel::draw();
  btnLabel.left = width/2;
  btnLabel.top = height/2;
  btnLabel.textAlignment = textAlignment; // copy button text alignment to label object
  if (!drawSpecialSymbol)
  {
    if (glyph.GetImage() != NULL)
    {
        ///////////////// location where text can go..
        int textLeft = bevelWidth;
        int textTop = bevelWidth;
        int textRight = width-bevelWidth;
        int textBottom = height-bevelWidth;
        sf::Rect<int> spriteRect = glyph.GetSubRect();
        int glyphWidth = spriteRect.GetWidth();
        int glyphHeight = spriteRect.GetHeight();

        switch (glyphAlignment)
        {
            case atLeft:   textLeft += glyphWidth+glyphHorizontalInset*2; break;
            case atRight:  textRight -= glyphWidth+glyphHorizontalInset*2; break;
            case atTop:    textTop += glyphHeight+glyphVerticalInset*2; break;
            case atBottom: textBottom -= glyphHeight+glyphVerticalInset*2; break;
            default: break;
        }
        switch (btnLabel.textAlignment)
        {
            case atLeft:   btnLabel.left = textLeft; btnLabel.top = (textTop+textBottom)/2; break;
            case atRight:  btnLabel.left = textRight; btnLabel.top = (textTop+textBottom)/2; break;
            case atTop:    btnLabel.left = (textLeft+textRight)/2; btnLabel.top = textTop; break;
            case atBottom: btnLabel.left = (textLeft+textRight)/2; btnLabel.top = textBottom; break;
            case atUpperLeft: btnLabel.left = textLeft; btnLabel.top = textTop; break;
            case atUpperRight: btnLabel.left = textRight; btnLabel.top = textTop; break;
            case atLowerLeft: btnLabel.left = textLeft; btnLabel.top = textBottom; break;
            case atLowerRight: btnLabel.left = textRight; btnLabel.top = textBottom; break;
            case atCenter: btnLabel.left = (textLeft+textRight)/2; btnLabel.top = (textTop+textBottom)/2; break;
            default: break;
        }
    }

    if (enabled)
    {
//        if (pWidgetInFocus == this)
//        {
//            btnLabel.myText.SetColor(highlightTextColor);
//        }
//        else
        btnLabel.myText.SetColor(faceTextColor);
    }
    else btnLabel.myText.SetColor(sf::Color((faceColor.r+faceTextColor.r)/2, (faceColor.g+faceTextColor.g)/2, (faceColor.b+faceTextColor.b)/2));
    btnLabel.draw();
  }
  ///////////////////////////// Check for auto-repeat
  if (pressed && autorepeat && GUIperformRepeat)
  {
      if (autorepeatcountdown> 0) autorepeatcountdown--;
      if ((autorepeatcountdown == 0)&&(OnClick != NULL)) OnClick(this);
  }
  if (!pressed) autorepeatcountdown = GUIrepeatDelay;
}

void GUIpanel::panelInitialize(GUIwidget* pWidget)
{
    align = alNone;
    canAlign = true;
    bevelInner = bsNone;
    bevelOuter = bsUp;
    bevelWidth = 2;
    fixedBevel = false;
    thisType = tguiPanel;
    borderWidth = 2; // spacing between outer and inner bevels
    drawSpecialSymbol = false;
    symbol = ssGripV;
    translucent = false;
    textItem = false;
    if (pWidget != NULL) pWidget->AddChild(this);
    Resize(width, height);
}

void GUIpanel::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    clLeft = 0;
    if (bevelInner != bsNone) clLeft += bevelWidth;
    if (bevelOuter != bsNone) clLeft += bevelWidth;
    if ((bevelInner != bsNone)&&(bevelOuter != bsNone)) clLeft += borderWidth;
    clTop = clLeft;
    clWidth = width-clLeft*2;
    clHeight = height-clTop*2;
}

void GUIpanel::ReAlign(GUIalign newAlign)
{
    align = newAlign;
    canAlign = true;
    // determine client limits
    int exLeft = 0;
    int exRight = width;
    int exTop = 0;
    int exBottom = height;
    if (parent != NULL)
    {   // search siblings for client limits
        exLeft = parent->clLeft;
        exTop = parent->clTop;
        exRight = parent->clLeft+parent->clWidth;
        exBottom = parent->clTop+parent->clHeight;
        GUIwidget* pWidget;
        for (unsigned int siblingIndex = 0; siblingIndex < parent->children.size(); siblingIndex++)
        {
            pWidget = parent->children[siblingIndex];
            if ((int(siblingIndex) != siblingOrder)&&(pWidget->canAlign))
            {
                int sibLeft = pWidget->left;
                int sibRight = sibLeft+pWidget->width;
                int sibTop = pWidget->top;
                int sibBottom = sibTop+ pWidget->height;

                int right = left+width;
                int bottom = top+height;

                if ((right < sibLeft)&&(sibLeft <= exRight)) exRight = sibLeft-1;
                if ((exLeft <= sibRight)&&(sibRight < left)) exLeft = sibRight+1;
                if ((exTop < sibBottom)&&(sibBottom <= top)) exTop = sibBottom+1;
                if ((bottom <= sibTop)&&(sibTop < exBottom)) exBottom = sibTop-1;
            }
        }
    }

    //adjust left, top, width and height to match new alignment
    switch (align)
    {
        case alNone: break;
        case alLeft: width += left-exLeft; left = exLeft; height = exBottom-exTop; top = exTop; break;
        case alRight: width = exRight-left; height = exBottom-exTop; top = exTop; break;
        case alTop: width = exRight-exLeft; left = exLeft; height += top-exTop; top = exTop; break;
        case alBottom: width = exRight-exLeft; left = exLeft; height = exBottom-top; break;
        case alClient: width = exRight-exLeft; left = exLeft; height = exBottom-exTop; top = exTop; break;
    }

    Resize(width, height);
}

GUIpanel::GUIpanel() : GUIwidget()
{
    width = GUIdefaultPanelWidth; height = GUIdefaultPanelHeight;
    panelInitialize(NULL);
}

GUIpanel::GUIpanel(int Left, int Top) : GUIwidget(Left, Top)
{
    width = GUIdefaultPanelWidth; height = GUIdefaultPanelHeight;
    panelInitialize(NULL);
}

GUIpanel::GUIpanel(int Left, int Top, int Width, int Height) : GUIwidget(Left, Top, Width, Height)
{
    panelInitialize(NULL);
}

GUIpanel::GUIpanel(GUIwidget* pWidget) : GUIwidget()
{
    width = GUIdefaultPanelWidth; height = GUIdefaultPanelHeight;
    panelInitialize(pWidget);
}

GUIpanel::GUIpanel(GUIwidget* pWidget, int Left, int Top) : GUIwidget(Left, Top)
{
    width = GUIdefaultPanelWidth; height = GUIdefaultPanelHeight;
    panelInitialize(pWidget);
}

GUIpanel::GUIpanel(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIwidget(Left, Top, Width, Height)
{
    panelInitialize(pWidget);
}

void GUIpanel::draw()   // draw a beveled panel
{
  if ((useParentColors)&&(parent != NULL)) CopyParentColors(this);

  int screenLeft, screenTop;
  FindSceenOfs(this, screenLeft, screenTop);
  sf::Color shineColor;
  sf::Color shadowColor;
     shineColor = sf::Color((faceColor.r+255)/2, (faceColor.g+255)/2, (faceColor.b+255)/2);  // halfway between face color and pure white
     shadowColor = sf::Color((faceColor.r)/2, (faceColor.g)/2, (faceColor.b)/2);  // halfway between face color and black

  // set up 2D drawing context
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0, GUIapp->GetWidth(), 0, GUIapp->GetHeight());
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  if (translucent)
  {
    shineColor.a = faceColor.a = shadowColor.a = 150; // set alphas
  }
  else shineColor.a = faceColor.a = shadowColor.a = 255;

  int screenHeight = GUIapp->GetHeight();
////////////////////////////////////////////////////////////////////////////////////////
//
//                  DRAW RECTANGLE AS THE BACKGROUND
//
sf::Color FaceColor = faceColor;
sf::Color ShineColor = shineColor;
sf::Color ShadowColor = shadowColor;
sf::Color HighlightColor = highlightColor;
sf::Color WindowColor = windowColor;

glDisable(GL_BLEND);
//WindowColor = sf::Color::White;

int radius = 6;
sf::Shape TabPoly;
sf::Shape BackGroundRect = sf::Shape::Rectangle(screenLeft, screenTop, screenLeft+width, screenTop+height, sf::Color::White, 1, HighlightColor);
BackGroundRect.EnableOutline(false);
if (textItem) BackGroundRect.SetColor(WindowColor); else BackGroundRect.SetColor(FaceColor);
if ((thisType == tguiButton)&&(static_cast<GUIbutton*>(this)->isTabButton))
{
    TabPoly.AddPoint(screenLeft+radius, screenTop, FaceColor);
    TabPoly.AddPoint(screenLeft, screenTop+radius, FaceColor);
    TabPoly.AddPoint(screenLeft, screenTop+height, FaceColor);
    TabPoly.AddPoint(screenLeft+width, screenTop+height, FaceColor);
    TabPoly.AddPoint(screenLeft+width, screenTop+radius, FaceColor);
    TabPoly.AddPoint(screenLeft+width-radius, screenTop, FaceColor);
    GUIapp->Draw(TabPoly);
}
else
{
    GUIapp->Draw(BackGroundRect);

}
faceColor.a = 255;

//////////////////////////////////////////////////////////////////////////////
//
//              DETERMINE IF MOUSE OR KEYBOARD HIGHLIGHTS APPLY
//
  bool doKeyBoardHighlight = false;
  if (pWidgetInFocus == this)  doKeyBoardHighlight = true; // draw a dashed line around the edge to indicate keyboard focus

  bool doMouseHighlight = false;
  bool enabledWidget = true;
  if (thisType == tguiButton)
  {
      if (!static_cast<GUIbutton*>(this)->enabled) enabledWidget = false;
  }
  if (enabledWidget && tabStop)      // draw an outline in highlight color if mouse is over a component which can receive focus
  {
    int mouseX = GUImenuInput->GetMouseX();
    int mouseY = GUImenuInput->GetMouseY();
    if ((screenLeft<mouseX)&&(mouseX <(screenLeft+width))&&(screenTop < mouseY)&&((screenTop+height)>mouseY))
    {
        doMouseHighlight = true;
    }
  }

/////////////////////////////////////////////////////////////////////////////////////////
//
//                  CHECK FOR RELEASE OF MOMENTARY BUTTONS
//

  if (this->thisType == tguiButton)
  {
      GUIbutton* thisPtr;
      thisPtr = static_cast<GUIbutton*>(this);

      if (thisPtr->pressed && !thisPtr->toggle)   // a momentary button is pressed (down)
      {
          thisPtr->pressed = false; // assume it is released
          if (doKeyBoardHighlight && GUImenuInput->IsKeyDown(sf::Key::Space)) thisPtr->pressed = true;
          if (doMouseHighlight && GUImenuInput->IsMouseButtonDown(sf::Mouse::Left)) thisPtr->pressed = true;
      }
  }




///////////////////////////////////////////////////////////////////////////////////
//
//                      DRAW BEVELS
//
  bool thisBevelUp;
  if (bevelOuter == bsUp) thisBevelUp = true; else thisBevelUp = false;
  if (fixedBevel) thisBevelUp = true;

  if (this->thisType == tguiButton) // was this a button?
  {
      if (doMouseHighlight)
      {      // if mouse over button, always show bevel (even for "flat" buttons)
          if (static_cast<GUIbutton*>(this)->pressed) bevelOuter = bsDown;  else bevelOuter = bsUp;
      }
      if (static_cast<GUIbutton*>(this)->flat) doMouseHighlight = false;  // don't draw mouse rectangle for flat buttons
//      if (static_cast<GUIbutton*>(this)->isTabButton) thisBevelUp = !thisBevelUp;
  }

  if (bevelOuter != bsNone)
  {
      sf::Shape BevelPolygon;
      if ((thisType == tguiButton)&&(static_cast<GUIbutton*>(this)->isTabButton))
      {
          int Selected = static_cast<GUItabPanel*>(this->parent)->selected;
          if (static_cast<GUItabPanel*>(this->parent)->tabs[Selected] == this)
          {
              BevelPolygon.SetColor(ShineColor);
              BevelPolygon.AddPoint(screenLeft+width-radius, screenTop);
              BevelPolygon.AddPoint(screenLeft+width-radius+bevelWidth, screenTop-bevelWidth);
              BevelPolygon.AddPoint(screenLeft+radius-bevelWidth, screenTop-bevelWidth);
              BevelPolygon.AddPoint(screenLeft+radius, screenTop);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(0, screenLeft, screenTop+radius);
              BevelPolygon.SetPointPosition(1, screenLeft-bevelWidth, screenTop+radius-bevelWidth);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(2, screenLeft-bevelWidth, screenTop+height+bevelWidth);
              BevelPolygon.SetPointPosition(3, screenLeft, screenTop+height+bevelWidth);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetColor(ShadowColor);
              BevelPolygon.SetPointPosition(0, screenLeft+width, screenTop+height+bevelWidth);
              BevelPolygon.SetPointPosition(1, screenLeft+width+bevelWidth, screenTop+height+bevelWidth);
              BevelPolygon.SetPointPosition(2, screenLeft+width+bevelWidth, screenTop+radius-bevelWidth);
              BevelPolygon.SetPointPosition(3, screenLeft+width, screenTop+radius);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(0, screenLeft+width-radius, screenTop);
              BevelPolygon.SetPointPosition(1, screenLeft+width-radius+bevelWidth, screenTop-bevelWidth);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetColor(FaceColor);
              BevelPolygon.SetPointPosition(0, screenLeft, screenTop+height+bevelWidth);
              BevelPolygon.SetPointPosition(1, screenLeft, screenTop+height);
              BevelPolygon.SetPointPosition(2, screenLeft+width, screenTop+height);
              BevelPolygon.SetPointPosition(3, screenLeft+width, screenTop+height+bevelWidth);
              GUIapp->Draw(BevelPolygon);
          }
          else
          {
              BevelPolygon.SetColor(ShineColor);
              BevelPolygon.AddPoint(screenLeft+width-radius-bevelWidth, screenTop+bevelWidth);
              BevelPolygon.AddPoint(screenLeft+width-radius, screenTop);
              BevelPolygon.AddPoint(screenLeft+radius, screenTop);
              BevelPolygon.AddPoint(screenLeft+radius+bevelWidth, screenTop+bevelWidth);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(0, screenLeft+bevelWidth, screenTop+radius+bevelWidth);
              BevelPolygon.SetPointPosition(1, screenLeft, screenTop+radius);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(2, screenLeft, screenTop+height);
              BevelPolygon.SetPointPosition(3, screenLeft+bevelWidth, screenTop+height);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetColor(ShadowColor);
              BevelPolygon.SetPointPosition(0, screenLeft+width-bevelWidth, screenTop+height);
              BevelPolygon.SetPointPosition(1, screenLeft+width, screenTop+height);
              BevelPolygon.SetPointPosition(2, screenLeft+width, screenTop+radius);
              BevelPolygon.SetPointPosition(3, screenLeft+width-bevelWidth, screenTop+bevelWidth+radius);
              GUIapp->Draw(BevelPolygon);
              //
              BevelPolygon.SetPointPosition(0, screenLeft+width-radius-bevelWidth, screenTop+bevelWidth);
              BevelPolygon.SetPointPosition(1, screenLeft+width-radius, screenTop);
              GUIapp->Draw(BevelPolygon);
          }
      }
      else
      {     // not a tab button
        if (bevelOuter == bsUp) BevelPolygon.SetColor(ShineColor); else BevelPolygon.SetColor(ShadowColor);
        // draw top bevel
        BevelPolygon.AddPoint(screenLeft, screenTop);
        BevelPolygon.AddPoint(screenLeft+bevelWidth, screenTop+bevelWidth);
        BevelPolygon.AddPoint(screenLeft+width-bevelWidth, screenTop+bevelWidth);
        BevelPolygon.AddPoint(screenLeft+width, screenTop);
        GUIapp->Draw(BevelPolygon);
        // draw left bevel
        BevelPolygon.SetPointPosition(0, screenLeft, screenTop);
        BevelPolygon.SetPointPosition(1, screenLeft, screenTop+height);
        BevelPolygon.SetPointPosition(2, screenLeft+bevelWidth, screenTop+height-bevelWidth);
        BevelPolygon.SetPointPosition(3, screenLeft+bevelWidth, screenTop+bevelWidth);
        GUIapp->Draw(BevelPolygon);

        if (bevelOuter == bsUp) BevelPolygon.SetColor(ShadowColor); else BevelPolygon.SetColor(ShineColor);
        // draw bottom bevel
        BevelPolygon.SetPointPosition(0, screenLeft, screenTop+height);
        BevelPolygon.SetPointPosition(1, screenLeft+width, screenTop+height);
        BevelPolygon.SetPointPosition(2, screenLeft+width-bevelWidth, screenTop+height-bevelWidth);
        BevelPolygon.SetPointPosition(3, screenLeft+bevelWidth, screenTop+height-bevelWidth);
        GUIapp->Draw(BevelPolygon);
        // draw left bevel
        BevelPolygon.SetPointPosition(0, screenLeft+width, screenTop);
        BevelPolygon.SetPointPosition(1, screenLeft+width-bevelWidth, screenTop+bevelWidth);
        BevelPolygon.SetPointPosition(2, screenLeft+width-bevelWidth, screenTop+height-bevelWidth);
        BevelPolygon.SetPointPosition(3, screenLeft+width, screenTop+height);
        GUIapp->Draw(BevelPolygon);
      }
  }
  if (bevelInner != bsNone)
  {
      sf::Shape iBevelPolygon;
      int iLeft = screenLeft+borderWidth;
      int iRight = screenLeft+width-borderWidth;
      int iTop = screenTop+borderWidth;
      int iBottom = screenTop+height-borderWidth;
      if (bevelOuter != bsNone)
      {
          iLeft += bevelWidth;
          iRight -= bevelWidth;
          iTop += bevelWidth;
          iBottom -= bevelWidth;
      }
        if (bevelInner == bsUp) iBevelPolygon.SetColor(ShineColor); else iBevelPolygon.SetColor(ShadowColor);
        // draw top bevel
        iBevelPolygon.AddPoint(iLeft, iTop);
        iBevelPolygon.AddPoint(iLeft+bevelWidth, iTop+bevelWidth);
        iBevelPolygon.AddPoint(iRight-bevelWidth, iTop+bevelWidth);
        iBevelPolygon.AddPoint(iRight, iTop);
        GUIapp->Draw(iBevelPolygon);
        // draw left bevel
        iBevelPolygon.SetPointPosition(0, iLeft, iTop);
        iBevelPolygon.SetPointPosition(1, iLeft, iBottom);
        iBevelPolygon.SetPointPosition(2, iLeft+bevelWidth, iBottom-bevelWidth);
        iBevelPolygon.SetPointPosition(3, iLeft+bevelWidth, iTop+bevelWidth);
        GUIapp->Draw(iBevelPolygon);

        if (bevelInner == bsUp) iBevelPolygon.SetColor(ShadowColor); else iBevelPolygon.SetColor(ShineColor);
        // draw bottom bevel
        iBevelPolygon.SetPointPosition(0, iLeft, iBottom);
        iBevelPolygon.SetPointPosition(1, iRight, iBottom);
        iBevelPolygon.SetPointPosition(2, iRight-bevelWidth, iBottom-bevelWidth);
        iBevelPolygon.SetPointPosition(3, iLeft+bevelWidth, iBottom-bevelWidth);
        GUIapp->Draw(iBevelPolygon);
        // draw left bevel
        iBevelPolygon.SetPointPosition(0, iRight, iTop);
        iBevelPolygon.SetPointPosition(1, iRight-bevelWidth, iTop+bevelWidth);
        iBevelPolygon.SetPointPosition(2, iRight-bevelWidth, iBottom-bevelWidth);
        iBevelPolygon.SetPointPosition(3, iRight, iBottom);
        GUIapp->Draw(iBevelPolygon);
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                          DRAW SKIN IMAGE
//
  if (skin.GetImage() != NULL)
  {

    bool showBevels = true;
    if (showBevels)
    {
      skin.Resize(width, height);
      skin.SetPosition(screenLeft, screenTop);
    }
    else
    {
      skin.Resize(width-bevelWidth*2, height-bevelWidth*2);
      skin.SetPosition(screenLeft+bevelWidth, screenTop+bevelWidth);
    }


    GUIapp->Draw(skin);
  }

////////////////////////////////////////////////////////////////////////////////
//
//                      DRAW KEYBOARD HIGHLIGHT

  // set up 2D drawing context
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0, GUIapp->GetWidth(), 0, GUIapp->GetHeight());
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);



  if (doKeyBoardHighlight) // draw a dashed line around the edge to indicate keyboard focus
  {
      glLineStipple(1, 0x0F0F);
      glEnable(GL_LINE_STIPPLE);
      glColor3f(0.5, 0.5, 0.5);
      glBegin(GL_LINE_STRIP);
        glVertex2i(screenLeft+bevelWidth+3,         screenHeight-screenTop-height+bevelWidth+3);
        glVertex2i(screenLeft+bevelWidth+3,         screenHeight-screenTop-bevelWidth-2);
        glVertex2i(screenLeft+width-bevelWidth-2,   screenHeight-screenTop-bevelWidth-2);
        glVertex2i(screenLeft+width-bevelWidth-2,   screenHeight-screenTop-height+bevelWidth+3);
        glVertex2i(screenLeft+bevelWidth+2,         screenHeight-screenTop-height+bevelWidth+3);
      glEnd();
      glDisable(GL_LINE_STIPPLE);
  }

////////////////////////////////////////////////////////////////////////////////
//
//                      DRAW MOUSE HIGHLIGHT
  if (doMouseHighlight)
  {
      BackGroundRect.SetColor(HighlightColor);
      BackGroundRect.SetPointPosition(0, screenLeft+bevelWidth+2,         screenTop+height-bevelWidth-2);
      BackGroundRect.SetPointPosition(1, screenLeft+bevelWidth+2,         screenTop+bevelWidth+2);
      BackGroundRect.SetPointPosition(2, screenLeft+width-bevelWidth-2,   screenTop+bevelWidth+2);
      BackGroundRect.SetPointPosition(3, screenLeft+width-bevelWidth-2,   screenTop+height-bevelWidth-2);
      BackGroundRect.EnableFill(false);
      BackGroundRect.EnableOutline(true);
      BackGroundRect.SetOutlineWidth(1);
      GUIapp->Draw(BackGroundRect);
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                          DRAW BUTTON IMAGE
//

if (thisType == tguiButton)
{
  GUIbutton* thisPtr;
  thisPtr =  static_cast<GUIbutton*>(this);
//  if (thisPtr->pGlyph != NULL)
  if (thisPtr->glyph.GetImage() != NULL)
  {
    sf::Rect<int> spriteRect = thisPtr->glyph.GetSubRect();
    int glyphWidth = spriteRect.GetWidth();
    int glyphHeight = spriteRect.GetHeight();
    switch (thisPtr->glyphAlignment)
    {
        case atLeft:
            {
                thisPtr->glyph.SetPosition(screenLeft+bevelWidth+thisPtr->glyphHorizontalInset, screenTop+height/2-glyphHeight/2);
                break;
            }
        case atRight:
            {
                thisPtr->glyph.SetPosition(screenLeft+width-glyphWidth-bevelWidth-thisPtr->glyphHorizontalInset, screenTop+height/2-glyphHeight/2);
                break;
            }
        case atTop:
            {
                thisPtr->glyph.SetPosition(screenLeft+width/2-glyphWidth/2, screenTop+bevelWidth+thisPtr->glyphVerticalInset);
                break;
            }
        case atBottom:
            {
                thisPtr->glyph.SetPosition(screenLeft+width/2-glyphWidth/2, screenTop+height+glyphHeight-bevelWidth-thisPtr->glyphVerticalInset);
                break;
            }
        default:
            {
                thisPtr->glyph.SetPosition(screenLeft+width/2-glyphWidth/2, screenTop+height/2-glyphHeight/2);
                break;
            }
    }
    thisPtr->glyph.SetBlendMode(sf::Blend::None);
    if (thisPtr->enabled) thisPtr->glyph.SetColor(sf::Color::White);
    else thisPtr->glyph.SetColor(sf::Color((thisPtr->faceColor.r+thisPtr->faceTextColor.r)/2, (thisPtr->faceColor.g+thisPtr->faceTextColor.g)/2,(thisPtr->faceColor.b+thisPtr->faceTextColor.b)/2, 255));

    GUIapp->Draw(thisPtr->glyph);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0, GUIapp->GetWidth(), 0, GUIapp->GetHeight());


  if(drawSpecialSymbol)
  {
      int size;
      int sLeft, sRight, sTop, sBottom;
      size = height-bevelWidth*2-2;
      if (size < 6) size = 6;
      if (size > 20) size = 20;
      if (height > width) size = width-bevelWidth*2-6;
      sLeft = screenLeft+width/2-size/2;
      sRight = sLeft + size;
      sTop = screenHeight-screenTop-height/2+size/2;
      sBottom = sTop - size+1;
      glLineWidth(1);

      switch (symbol)
      {
          case ssNone: break;
          case ssGripH:
                 glBegin(GL_LINES);
                   size = height-bevelWidth*2-6;
                   for (int i= -size/2; i < size/2; i += 4)
                   {
                      glColor3f(float(shadowColor.r)/255, float(shadowColor.g)/255, float(shadowColor.b)/255);
                      glVertex2i(screenLeft+width/2+i, screenHeight-screenTop-height/2+size/2);
                      glVertex2i(screenLeft+width/2+i, screenHeight-screenTop-height/2-size/2);
                      glColor3f(float(shineColor.r)/255, float(shineColor.g)/255, float(shineColor.b)/255);
                      glVertex2i(screenLeft+width/2+i+1, screenHeight-screenTop-height/2+size/2);
                      glVertex2i(screenLeft+width/2+i+1, screenHeight-screenTop-height/2-size/2);
                   }
                 glEnd();
               break;
          case ssGripV:
                 glBegin(GL_LINES);
                   size = width-bevelWidth*2-6;
                   for (int i= -size/2; i < size/2; i += 4)
                   {
                      glColor3f(float(shadowColor.r)/255, float(shadowColor.g)/255, float(shadowColor.b)/255);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2+i);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2+i);
                      glColor3f(float(shineColor.r)/255, float(shineColor.g)/255, float(shineColor.b)/255);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2+i+1);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2+i+1);
                   }
                 glEnd();
               break;
          case ssCheck:
                 size = height-bevelWidth*2;
                 glLineWidth(3);
                 glColor3f(float(windowTextColor.r)/255, float(windowTextColor.g)/255, float(windowTextColor.b)/255);
                 glBegin(GL_LINE_STRIP);
                      glVertex2i(screenLeft+bevelWidth+2,       screenHeight-screenTop-height/2);
                      glVertex2i(screenLeft+width/2,          screenHeight-screenTop-height+bevelWidth+1);
                      glVertex2i(screenLeft+width-bevelWidth-2, screenHeight-screenTop-bevelWidth-1);
                 glEnd();
                 glLineWidth(1);
               break;
          case ssUpTriangle:
                 size = height-bevelWidth*2-6;
                 glBegin(GL_TRIANGLES);
//                      glColor3f(shadowColor.r/255, shadowColor.g/255, shadowColor.b/255);
                      glColor3f(faceTextColor.r/255, faceTextColor.g/255, faceTextColor.b/255);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2-size/2);
                      glVertex2i(screenLeft+width/2,        screenHeight-screenTop-height/2+size/2);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2-size/2);
                 glEnd();
               break;
          case ssDownTriangle:
                 size = height-bevelWidth*2-6;
                 size -=4;
                 if (size < 6) size = 6;
                 if (size > 20) size = 20;
                 glBegin(GL_TRIANGLES);
//                      glColor3f(shadowColor.r/255, shadowColor.g/255, shadowColor.b/255);
                      glColor3f(faceTextColor.r/255, faceTextColor.g/255, faceTextColor.b/255);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2+size/2);
                      glVertex2i(screenLeft+width/2,        screenHeight-screenTop-height/2-size/2);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2+size/2);
                 glEnd();
               break;
          case ssLeftTriangle:
                 size = height-bevelWidth*2-6;
                 size -=4;
                 if (size < 6) size = 6;
                 if (size > 20) size = 20;
                 glBegin(GL_TRIANGLES);
//                      glColor3f(shadowColor.r/255, shadowColor.g/255, shadowColor.b/255);
                      glColor3f(faceTextColor.r/255, faceTextColor.g/255, faceTextColor.b/255);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2-size/2);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2+size/2);
                 glEnd();
               break;
          case ssRightTriangle:
                 size = height-bevelWidth*2-6;
                 size -=4;
                 if (size < 6) size = 6;
                 if (size > 20) size = 20;
                 glBegin(GL_TRIANGLES);
//                      glColor3f(shadowColor.r/255, shadowColor.g/255, shadowColor.b/255);
                      glColor3f(faceTextColor.r/255, faceTextColor.g/255, faceTextColor.b/255);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2-size/2);
                      glVertex2i(screenLeft+width/2+size/2, screenHeight-screenTop-height/2);
                      glVertex2i(screenLeft+width/2-size/2, screenHeight-screenTop-height/2+size/2);
                 glEnd();
               break;
          case ssMinimize:
                 glBegin(GL_LINE_STRIP);
                      glColor3f(float(highlightTextColor.r)/255, float(highlightTextColor.g)/255, float(highlightTextColor.b)/255);
                      glVertex2i(sLeft, sBottom);
                      glVertex2i(sLeft, sTop);
                      glVertex2i(sRight, sTop);
                      glVertex2i(sRight, sBottom);
                      glVertex2i(sLeft-1, sBottom);
                 glEnd();
                 glBegin(GL_LINES);
                      glVertex2i(sLeft+2, sBottom+4);
                      glVertex2i(sLeft+size/2, sBottom+4);
                      glVertex2i(sLeft+2, sBottom+3);
                      glVertex2i(sLeft+size/2, sBottom+3);
                 glEnd();
               break;
          case ssMaximize:
                 glBegin(GL_LINE_STRIP);
                      glColor3f(float(highlightTextColor.r)/255, float(highlightTextColor.g)/255, float(highlightTextColor.b)/255);
                      glVertex2i(sLeft, sBottom);
                      glVertex2i(sLeft, sTop);
                      glVertex2i(sRight, sTop);
                      glVertex2i(sRight, sBottom);
                      glVertex2i(sLeft-1, sBottom);
                 glEnd();
                 glBegin(GL_LINE_STRIP);
                      glVertex2i(sLeft+3, sBottom+3);
                      glVertex2i(sLeft+3, sTop-3);
                      glVertex2i(sRight-3, sTop-3);
                      glVertex2i(sLeft+3, sTop-4);
                      glVertex2i(sRight-3, sTop-4);
                      glVertex2i(sRight-3, sBottom+3);
                      glVertex2i(sLeft+2, sBottom+3);
                 glEnd();
               break;
          case ssNormal:
                 glBegin(GL_LINE_STRIP);
                      glColor3f(float(highlightTextColor.r)/255, float(highlightTextColor.g)/255, float(highlightTextColor.b)/255);
                      glVertex2i(sLeft, sBottom);
                      glVertex2i(sLeft, sTop);
                      glVertex2i(sRight, sTop);
                      glVertex2i(sRight, sBottom);
                      glVertex2i(sLeft-1, sBottom);
                 glEnd();
                 glBegin(GL_LINE_STRIP);
                      glVertex2i(sLeft+2, sBottom+2);
                      glVertex2i(sLeft+2, sBottom+size/2+2);
                      glVertex2i(sLeft+size/2+2, sBottom+size/2+2);
                      glVertex2i(sLeft+2, sBottom+size/2+1);
                      glVertex2i(sLeft+size/2+2, sBottom+size/2+1);
                      glVertex2i(sLeft+size/2+2, sBottom+2);
                      glVertex2i(sLeft+1, sBottom+2);
                 glEnd();
                 glBegin(GL_LINE_STRIP);
                      glVertex2i(sRight-size/2-2, sBottom+size/2+2);
                      glVertex2i(sRight-size/2-2, sTop-3);
                      glVertex2i(sRight-2, sTop-3);
                      glVertex2i(sRight-size/2-2, sTop-4);
                      glVertex2i(sRight-2, sTop-4);
                      glVertex2i(sRight-2, sTop-size/2-3);
                      glVertex2i(sLeft+size/2+2, sTop-size/2-3);
                 glEnd();
               break;
          case ssClose:
                 size +=4;
                 if (size < 6) size = 6;
                 if (size > 20) size = 20;
                 glBegin(GL_LINE_STRIP);
                      glColor3f(float(highlightTextColor.r)/255, float(highlightTextColor.g)/255, float(highlightTextColor.b)/255);
                      glVertex2i(sLeft, sBottom);
                      glVertex2i(sLeft, sTop);
                      glVertex2i(sRight, sTop);
                      glVertex2i(sRight, sBottom);
                      glVertex2i(sLeft-1, sBottom);
                 glEnd();
                 glLineWidth(2);
                 glBegin(GL_LINES);
                      glVertex2i(sLeft+3, sBottom+3);
                      glVertex2i(sRight-3, sTop-3);
                      glVertex2i(sLeft+3, sTop-3);
                      glVertex2i(sRight-3, sBottom+3);
                 glEnd();
                 glLineWidth(1);
               break;
          case ssEllipsis:
                 int X = sLeft;
                 int Y = sBottom;
                 int R = 3;
                 sf::Shape dot;
                 for (int i=0; i<3; i++)
                 {
                     dot  = sf::Shape::Circle(X, Y, R, windowTextColor);
                     GUIapp->Draw(dot);
                     X += 6;
                 }
               break;
      }
  }
  glLineWidth(1);

    // draw children
    for (int i=0; i < int(children.size()); i++)
    {
        children[i]->draw();
    }
}

void GUIformClose (GUIwidget *pWidget)
{
  // find parent form
  while (pWidget->parent != NULL) pWidget = pWidget->parent;
  if (pWidget->thisType == tguiForm) static_cast<GUIform*>(pWidget)->close();
}

void GUIformMinimize (GUIwidget *pWidget)
{
  // find parent form
  while (pWidget->parent != NULL) pWidget = pWidget->parent;
  if (pWidget->thisType == tguiForm) static_cast<GUIform*>(pWidget)->minimize();
}

void GUIformMaximize (GUIwidget *pWidget)
{
  // find parent form
  while (pWidget->parent != NULL) pWidget = pWidget->parent;
  if (pWidget->thisType == tguiForm) static_cast<GUIform*>(pWidget)->maximize();
}

void GUIform::FormInitialize()
{
    Constraint.minWidth = 40;
    Constraint.minHeight = 40;
    canAlign = false;
    parentApp = GUIapp;
    windowState = wsNormal;
    resizable = true;
    showTitleBar = true;
    titleBarHeight = GUIformTitleBarHeight;
    borderBevel = true;
    beingDragged = false;
    beingResized = false;
    formClose.top = 1; formMaximize.top = 1; formMinimize.top = 1;
    formClose.width = GUIformTitleBarHeight-2;
    formMaximize.width = GUIformTitleBarHeight-2;
    formMinimize.width = GUIformTitleBarHeight-2;
    formClose.height = GUIformTitleBarHeight-2;
    formMaximize.height = GUIformTitleBarHeight-2;
    formMinimize.height = GUIformTitleBarHeight-2;
    formClose.bevelWidth = 0; formMaximize.bevelWidth = 0; formMinimize.bevelWidth = 0;
    formClose.drawSpecialSymbol = true; formMaximize.drawSpecialSymbol = true; formMinimize.drawSpecialSymbol = true;
    formClose.symbol = ssClose; formMaximize.symbol = ssMaximize; formMinimize.symbol = ssMinimize;
    formClose.useParentColors = false; formMaximize.useParentColors = false; formMinimize.useParentColors = false;
    formClose.tabStop = false; formMaximize.tabStop = false; formMinimize.tabStop = false;
    formClose.OnClick = &GUIformClose;
    formMaximize.OnClick = &GUIformMaximize;
    formMinimize.OnClick = &GUIformMinimize;
    AddChild(&formClose);
    AddChild(&formMaximize);
    AddChild(&formMinimize);
//    formClose.parent = this;
//    formMaximize.parent = this;
//    formMinimize.parent = this;

    ActiveWidget = NULL;
    thisType = tguiForm;
    caption.SetSize(GUIdefaultTextSize); // font size
    caption.SetColor(highlightTextColor);
    OnClick = &GUIformOnClick;
//    SetFont(GUIdefaultFont);
    Resize(width, height);
}

void GUIform::Resize(int Width, int Height)
{
    width = Width; height = Height;
    if (showTitleBar)
    {
        clTop = titleBarHeight;
        clLeft = 3; clWidth = width-6; clHeight = height-clTop-3;
    }
    else
    {
        clTop = 0; clLeft = 0; clWidth = width; clHeight = height;
    }

    // scan children for resizable components
    for (unsigned int i = 0; i < children.size(); i++)
    {
        switch (children[i]->thisType)
        {
            case tguiTreeView:
            case tguiMemo:
            case tguiPanel: static_cast<GUIpanel*>(children[i])->ReAlign(children[i]->align); break;
            default: ;
        }
    }
}

GUIform::GUIform() : GUIpanel()   // default constructor - sets position to upper left corner and size to zero (not typically used)
{
    caption.SetText("");
    FormInitialize();
}

GUIform::GUIform(sf::Unicode::Text newText) : GUIpanel()   // default constructor - sets position to upper left corner and size to zero (not typically used)
{
    caption.SetText(newText);
    FormInitialize();
}

GUIform::GUIform(int Left, int Top) : GUIpanel(Left, Top)   // another constructor - user sets position, but has size of zero (not typically used)
{
    caption.SetText("");
    FormInitialize();
}

GUIform::GUIform(int Left, int Top, sf::Unicode::Text newText) : GUIpanel(Left, Top)   // another constructor - user sets position, but has size of zero (not typically used)
{
    caption.SetText(newText);
    FormInitialize();
}

GUIform::GUIform(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)   // typical constructor - user sets position and size
{
    caption.SetText("");
    FormInitialize();
}

GUIform::GUIform(int Left, int Top, int Width, int Height, sf::Unicode::Text newText) : GUIpanel(Left, Top, Width, Height)
{
    caption.SetText(newText);
    FormInitialize();
}

void GUIform::show()
{
    if (GUIformCount < GUIformMaxCount)
    {
        GUIformVisible[GUIformCount] = this;
        GUIformCount++;
        this->isVisible = true;
    }
}
void GUIform::show(bool state) // show if true, hide if false;
{
    if (state) this->show(); else this->hide();
}

void GUIform::hide()
{
    this->isVisible = false;
}

void GUIform::close()
{
  if (GUIformCount > 0)
  {
    int i = 0;
    // find the instance of "this"
    while ((GUIformVisible[i] != this)&&(i<GUIformCount))
    {
        i++;
    }
    if (GUIformVisible[i] == this) // found it
    {
        while (i<GUIformCount)
        {
            GUIformVisible[i] = GUIformVisible[i+1];
            i++;
        }
        GUIformVisible[GUIformCount-1] = NULL;
        GUIformCount--;
    }
  }
  this->isVisible = false;
}

void GUIform::minimize()
{
  prevWindowState = windowState;
  if (windowState == wsMaximized) formMaximize.symbol = ssMaximize;
  else
  {
      oldLeft = left; oldTop = top; oldWidth = width; oldHeight = height;
      formMaximize.symbol = ssNormal;
  }
  width = 160; height = 22;
  top = GUIapp->GetHeight() - height;
  windowState = wsMinimized;
}

void GUIform::maximize()
{

  switch (windowState)
  {
      case wsNormal:
        {
            oldLeft = left; oldTop = top; oldWidth = width; oldHeight = height;
            prevWindowState = wsNormal;
            left = 0; top = 0; width = GUIapp->GetWidth(); height = GUIapp->GetHeight();
            formMaximize.symbol = ssNormal;
            windowState = wsMaximized;
            break;
        }
      case wsMinimized:
        {
            if (prevWindowState == wsNormal)
            {
                left = oldLeft; top = oldTop; width = oldWidth; height = oldHeight;
                formMaximize.symbol = ssMaximize;
                windowState = wsNormal;
            }
            else
            {  // was minimized from maximized, return to maximized
                windowState = wsMaximized;
                left = 0; top = 0; width = GUIapp->GetWidth(); height = GUIapp->GetHeight();
                formMaximize.symbol = ssNormal;
                windowState = wsMaximized;
            }
            prevWindowState = wsMinimized;
            break;
        }
      case wsMaximized:
        {
            windowState = wsNormal;
            left = oldLeft; top = oldTop; width = oldWidth; height = oldHeight;
            if ((width == int(GUIapp->GetWidth()))&&(height == int(GUIapp->GetHeight())))
            { // still at max size, force a smaller size
               width = (7*width)/8; height = (7*height)/8;
               left = width/16; top = height/16;
            }
            formMaximize.symbol = ssMaximize;
            prevWindowState = wsMaximized;
            break;
        }
  }

}

void GUIform::SetCaption(sf::Unicode::Text newText)
{
    caption.SetText(newText);
}

void GUIform::AddPopUp(GUIwidget* pWidget)
{
pWidget->isVisible = true;
    PopUps.push_back(pWidget);
}

void GUIform::RemovePopUp(GUIwidget* pWidget) // search for and remove indicated widget from popup list
{
    std::vector<GUIwidget*>::iterator it;
    for(it=PopUps.begin(); it < PopUps.end(); it++)
    {
        if (*it == pWidget)
        {
            (*it)->isVisible = false;
            PopUps.erase(it);
        }
    }
}

void GUIform::ClearPopUps()
{
    for (unsigned int j=0; j < PopUps.size(); j++)
    {
        if (PopUps[j]->thisType == tguiListBox)
        PopUps[j]->isVisible = false;
    }
    PopUps.clear();
}

void GUIform::draw()
{
  if (this != &GUImain)
  {
//      GUIpanel::draw();

      if (!GUImenuInput->IsMouseButtonDown(sf::Mouse::Left)) {beingDragged = false; beingResized = false;}
      if (windowState == wsMaximized)  {beingDragged = false; beingResized = false;}

      if (beingDragged)
      {
          left = GUImenuInput->GetMouseX()-GUIdragAnchorX;
          top  = GUImenuInput->GetMouseY()-GUIdragAnchorY;
      }

      if (beingResized)
      {
          int newX = GUImenuInput->GetMouseX();
          int newY = GUImenuInput->GetMouseY();
          int newWidth, newHeight;
          if (resizeEdge & GUIresizeLeft)
          {
              newWidth = oldLeft - newX + oldWidth;
              if (newWidth < Constraint.minWidth)
              {
                  if ((Constraint.minWidth-newWidth)>20) beingResized = false; // mouse moved too far beyond limit
                  newWidth = Constraint.minWidth;
              }
              if ((Constraint.maxWidth > 0)&&(newWidth > Constraint.maxWidth)) newWidth = Constraint.maxWidth;
              left = oldLeft+oldWidth-newWidth; width = newWidth;
          }
          if (resizeEdge & GUIresizeRight)
          {
              newWidth = newX - left;
              if (newWidth < Constraint.minWidth)
              {
                  if ((Constraint.minWidth-newWidth)>20) beingResized = false; // mouse moved too far beyond limit
                  newWidth = Constraint.minWidth;
              }
              if ((Constraint.maxWidth > 0)&&(newWidth > Constraint.maxWidth)) newWidth = Constraint.maxWidth;
              width = newWidth;
          }
          if (resizeEdge & GUIresizeTop)
          {
              newHeight = oldTop - newY + oldHeight;
              if (newHeight < Constraint.minHeight)
              {
                  if ((Constraint.minHeight-newHeight) > 20) beingResized = false; // mouse moved too far beyond limit
                  newHeight = Constraint.minHeight;
              }
              if ((Constraint.maxHeight > 0)&&(newHeight > Constraint.maxHeight)) newHeight = Constraint.maxHeight;
              top = oldTop + oldHeight - newHeight;
              height = newHeight;
          }
          if (resizeEdge & GUIresizeBottom)
          {
              newHeight = newY - top;
              if (newHeight < Constraint.minHeight)
              {
                  if ((Constraint.minHeight-newHeight) > 20) beingResized = false; // mouse moved too far beyond limit
                  newHeight = Constraint.minHeight;
              }
              if ((Constraint.maxHeight > 0)&&(newHeight > Constraint.maxHeight)) newHeight = Constraint.maxHeight;
              height = newHeight;
          }

          Resize(width, height);
      }
      GUIpanel::draw();
  }

  sf::Shape line;
  if (showTitleBar)  // display border and title bar
  {

      sf::Color highlightShineColor = sf::Color((highlightColor.r+255)/2, (highlightColor.g+255)/2, (highlightColor.b+255)/2);
      sf::Color highlightShadowColor = sf::Color((highlightColor.r)/2, (highlightColor.g)/2, (highlightColor.b)/2);

      if (borderBevel)
      {  // top
        line = sf::Shape::Line(left, top, left+width, top, 1, highlightShineColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+1, top+1, left+width-1, top+1, 1, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+2, top+2, left+width-2, top+2, 1, highlightShadowColor);
        GUIapp->Draw(line);
        // right
        line = sf::Shape::Line(left+width-2, top+2, left+width-2, top+height-2, 1, highlightShineColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width-1, top+1, left+width-1, top+height-1, 1, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width, top, left+width, top+height, 1, highlightShadowColor);
        GUIapp->Draw(line);
        // bottom
        line = sf::Shape::Line(left+width-2, top+height-2, left+1, top+height-2, 1, highlightShineColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width-1, top+height-1, left+0, top+height-1, 1, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width, top+height, left-1, top+height, 1, highlightShadowColor);
        GUIapp->Draw(line);
        // left
        line = sf::Shape::Line(left, top+height, left, top, 1, highlightShineColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+1, top+height-1, left+1, top+1, 1, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+2, top+height-2, left+2, top+2, 1, highlightShadowColor);
        GUIapp->Draw(line);
      }
      else // no border bevel
      {
        line = sf::Shape::Line(left+1, top+1, left+width-1, top+1, 3, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width-1, top+1, left+width-1, top+height-1, 3, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width+1, top+height-1, left+1, top+height-1, 3, highlightColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+1, top+height-1, left+1, top+1, 3, highlightColor);
        GUIapp->Draw(line);
      }

    if (resizable)
    {
        line = sf::Shape::Line(left, top+height-20, left+2, top+height-20, 1, highlightTextColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+20, top+height-2, left+20, top+height, 1, highlightTextColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width, top+height-20, left+width-2, top+height-20, 1, highlightTextColor);
        GUIapp->Draw(line);
        line = sf::Shape::Line(left+width-20, top+height, left+width-20, top+height-2, 1, highlightTextColor);
        GUIapp->Draw(line);
    }

    int titleBarHeight = GUIformTitleBarHeight;
    sf::Shape rect = sf::Shape::Rectangle(left+1, top+1, left+width-1, top+titleBarHeight, highlightColor);
    GUIapp->Draw(rect);
    if (borderBevel)
    {
        line = sf::Shape::Line(left+2, top+titleBarHeight, left+width-2, top+titleBarHeight, 1, highlightShadowColor);
        GUIapp->Draw(line);
    }

    formClose.faceColor = highlightColor;
    formMaximize.faceColor = highlightColor;
    formMinimize.faceColor = highlightColor;

    formClose.isVisible = true;
    formMaximize.isVisible = true;
    formMinimize.isVisible = true;
    formClose.left = width-formClose.width-1;
    formMaximize.left = formClose.left-formMaximize.width-2;
    formMinimize.left = formMaximize.left-formMinimize.width-2;

    caption.SetColor(highlightTextColor);
    caption.SetPosition(left+4, top+0);

    glDisable(GL_LIGHTING);

    GUIapp->Draw(caption);

  }
  else
  {  // no title bar, no borders
     windowState = wsNormal;
     formClose.isVisible = false;
     formMaximize.isVisible = false;
     formMinimize.isVisible = false;
  }

    if (windowState == wsMinimized)
    {
        formClose.draw();
        formMaximize.isVisible = true;
        formMaximize.draw();
        // don't draw children
    }
    else
    {  // not minimized
        if (resizable)
        {
            if(showTitleBar) formMaximize.isVisible = true;
            formMaximize.left = formClose.left-formMaximize.width-2;
            formMinimize.left = formMaximize.left-formMinimize.width-2;

        }
        else
        {  // not resizable
            formMinimize.left = formClose.left-formMinimize.width-2;
            formMaximize.isVisible = false;
        }
        // draw children
        for (int i=0; i < int(children.size()); i++)
        {
            children[i]->draw();
        }
    }


}

void GUIlabel::LabelInitialize(GUIwidget* pWidget)
{
    myText.SetSize(GUIdefaultTextSize); // default size
    myText.SetColor(faceTextColor);
    textAlignment = atUpperLeft;
    if (pWidget != NULL) pWidget->AddChild(this);
}

GUIlabel::GUIlabel() : GUIwidget()
{
    myText.SetText("Label");
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(NULL);
}

GUIlabel::GUIlabel(sf::Unicode::Text newText) : GUIwidget()
{
    myText.SetText(newText);
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(NULL);
}

GUIlabel::GUIlabel(int Left, int Top) : GUIwidget(Left, Top)
{
    myText.SetText("Label");
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(NULL);
}

GUIlabel::GUIlabel(int Left, int Top, sf::Unicode::Text newText) : GUIwidget(Left, Top)
{
    myText.SetText(newText);
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(NULL);
}

GUIlabel::GUIlabel(GUIwidget* pWidget) : GUIwidget()
{
    myText.SetText("Label");
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(pWidget);
}

GUIlabel::GUIlabel(GUIwidget* pWidget, sf::Unicode::Text newText) : GUIwidget()
{
    myText.SetText(newText);
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(pWidget);
}

GUIlabel::GUIlabel(GUIwidget* pWidget, int Left, int Top) : GUIwidget(Left, Top)
{
    myText.SetText("Label");
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(pWidget);
}

GUIlabel::GUIlabel(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText) : GUIwidget(Left, Top)
{
    myText.SetText(newText);
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
    LabelInitialize(pWidget);
}

void GUIlabel::SetText(sf::Unicode::Text newText)
{
    myText.SetText(newText);
    width = myText.GetRect().Right - myText.GetRect().Left;
    height = abs(myText.GetRect().Top - myText.GetRect().Bottom);
}

void GUIlabel::draw()
{
    int screenLeft, screenTop; // top-left position on screen to draw this component at
    if (isVisible)
    {
        // ascend the parent chain to account for ancestor's screen position.
        FindSceenOfs(this, screenLeft, screenTop);
        GUIwidget* lookWidget;
        lookWidget = this;
        while (lookWidget->useParentFont && (lookWidget->parent != NULL)) lookWidget = lookWidget->parent;
//        myText.SetFont(lookWidget->font);
        myText.SetFont(lookWidget->GetFont());
        width = myText.GetRect().Right - myText.GetRect().Left;
        height = myText.GetRect().Bottom - myText.GetRect().Top;

        switch (textAlignment) // the widget's position indicates the anchor point.
        {
            case atUpperLeft: myText.SetPosition(screenLeft, screenTop); break;
            case atLowerLeft: myText.SetPosition(screenLeft, screenTop-height); break;
            case atUpperRight: myText.SetPosition(screenLeft-width, screenTop); break;
            case atLowerRight: myText.SetPosition(screenLeft-width, screenTop-height); break;
            case atCenter: myText.SetPosition(screenLeft-width/2, screenTop-height/2-2); break;
            case atLeft: myText.SetPosition(screenLeft, screenTop-height/2-2); break;
            case atRight: myText.SetPosition(screenLeft-width, screenTop-height/2-2); break;
            case atTop: myText.SetPosition(screenLeft-width/2, screenTop-2); break;
            case atBottom: myText.SetPosition(screenLeft-width/2, screenTop-height-2); break;
            default: break;
        }

        if ((this->thisType != tguiLabel)&&(this->parent != NULL))
        {   // check for parent beign a widget in focus
            GUIwidget* pWidget = this->parent;

            if (pWidget == pWidgetInFocus)
            {
                int tLeft = myText.GetPosition().x;
                int tTop = myText.GetPosition().y;
                int tRight = myText.GetRect().GetWidth()+tLeft;
                int tBottom = myText.GetRect().GetHeight()+tTop;
                sf::Shape rect = sf::Shape::Rectangle(tLeft, tTop, tRight, tBottom, pWidget->highlightColor);
                GUIapp->Draw(rect);
                myText.SetColor(pWidget->highlightTextColor);
            }
        }
        GUIapp->Draw(myText);
    }
}


void GUIcheckBox::CheckBoxInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    thisType = tguiCheckBox;
    checked = false;
    enabled = true;
    bevelOuter = bsUp;
    tabStop = true;
    btnText.parent = this;
    btnText.left = height+2;
    btnText.top = 1;
    BoxPanel.parent = this;
    BoxPanel.bevelOuter = bsDown;
    BoxPanel.textItem = true;
    BoxPanel.top = 3;
    BoxPanel.left = 3;
    BoxPanel.bevelWidth=3;
    bevelWidth = 0;
    BoxPanel.height = height-6;
    BoxPanel.width = BoxPanel.height;
    BoxPanel.drawSpecialSymbol = true;
    BoxPanel.symbol = ssCheck;
    if (pWidget != NULL) pWidget->AddChild(this);

}

GUIcheckBox::GUIcheckBox() :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(sf::Unicode::Text newText) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(int Left, int Top) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(int Left, int Top, sf::Unicode::Text newText) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(int Left, int Top, int Width, int Height) :  GUIpanel(Left, Top, Width, Height)
{
    btnText.SetText("Check Box");
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(int Left, int Top, int Width, int Height, sf::Unicode::Text newText) :  GUIpanel(Left, Top, Width, Height)
{
    this->SetText(newText);
    CheckBoxInitialize(NULL);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    CheckBoxInitialize(pWidget);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget, sf::Unicode::Text newText) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    CheckBoxInitialize(pWidget);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget, int Left, int Top) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    CheckBoxInitialize(pWidget);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    CheckBoxInitialize(pWidget);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget, int Left, int Top, int Width, int Height) :  GUIpanel(Left, Top, Width, Height)
{
    btnText.SetText("Check Box");
    CheckBoxInitialize(pWidget);
}

GUIcheckBox::GUIcheckBox(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText) :  GUIpanel(Left, Top, Width, Height)
{
    this->SetText(newText);
    CheckBoxInitialize(pWidget);
}

void GUIcheckBox::Check(bool newState)
{
    checked = newState;
}

void GUIcheckBox::SetText(sf::Unicode::Text newText)
{
    btnText.SetText(newText);
//    btnText.left = (width - btnText.width)/2;
    btnText.left = height;
    btnText.top = -2+(height - btnText.height)/2;
}

void GUIcheckBox::draw()
{
    btnText.left = height+2;
    BoxPanel.height = height-6;
    BoxPanel.width = BoxPanel.height;
    btnText.top = -2+(height - btnText.height)/2;
    btnText.left = height+2;

    if (isVisible)
    {
        GUIpanel::draw();
        if (checked) BoxPanel.symbol = ssCheck; else BoxPanel.symbol = ssNone;
        BoxPanel.draw();

        if (useParentColors)
        {
            CopyParentColors(this);
        }
        if (enabled) btnText.myText.SetColor(faceTextColor);
        else btnText.myText.SetColor(sf::Color((faceColor.r+faceTextColor.r)/2, (faceColor.g+faceTextColor.g)/2, (faceColor.b+faceTextColor.b)/2));

        btnText.draw();
    }
}


void GUIradioButton::radioButtonInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    thisType = tguiRadioButton;
    checked = false;
    enabled = true;
    bevelOuter = bsUp;
    tabStop = true;
    btnText.parent = this;
    btnText.left = height+2;
    btnText.top = 1;
    BoxPanel.parent = this;
    BoxPanel.bevelOuter = bsDown;
    BoxPanel.textItem = true;
    BoxPanel.top = 3;
    BoxPanel.left = 3;
    BoxPanel.bevelWidth=3;
    bevelWidth = 0;
    BoxPanel.height = height-6;
    BoxPanel.width = BoxPanel.height;
    BoxPanel.drawSpecialSymbol = true;
    BoxPanel.symbol = ssLeftTriangle;
    if (pWidget != NULL) pWidget->AddChild(this);
}

GUIradioButton::GUIradioButton() :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(sf::Unicode::Text newText) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(int Left, int Top) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(int Left, int Top, sf::Unicode::Text newText) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(int Left, int Top, int Width, int Height) :  GUIpanel(Left, Top, Width, Height)
{
    btnText.SetText("Check Box");
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(int Left, int Top, int Width, int Height, sf::Unicode::Text newText) :  GUIpanel(Left, Top, Width, Height)
{
    this->SetText(newText);
    radioButtonInitialize(NULL);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    radioButtonInitialize(pWidget);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget, sf::Unicode::Text newText) :  GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    radioButtonInitialize(pWidget);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget, int Left, int Top) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    btnText.SetText("Check Box");
    radioButtonInitialize(pWidget);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText) :  GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    this->SetText(newText);
    radioButtonInitialize(pWidget);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget, int Left, int Top, int Width, int Height) :  GUIpanel(Left, Top, Width, Height)
{
    btnText.SetText("Check Box");
    radioButtonInitialize(pWidget);
}

GUIradioButton::GUIradioButton(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText) :  GUIpanel(Left, Top, Width, Height)
{
    this->SetText(newText);
    radioButtonInitialize(pWidget);
}

void GUIradioButton::SetText(sf::Unicode::Text newText)
{
    btnText.SetText(newText);
//    btnText.left = (width - btnText.width)/2;
    btnText.left = height;
    btnText.top = -2+(height - btnText.height)/2;
}

void GUIradioButton::draw()
{
    btnText.left = height+2;
    BoxPanel.height = height-6;
    BoxPanel.width = BoxPanel.height;
    btnText.left = height+2;
    btnText.top = -2+(height - btnText.height)/2;

    if (isVisible)
    {
        GUIpanel::draw();

        if (checked) BoxPanel.symbol = ssLeftTriangle; else BoxPanel.symbol = ssRightTriangle;

//        BoxPanel.draw();

      if ((useParentColors)&&(parent != NULL))  CopyParentColors(this)        ;

      int screenLeft, screenTop;
      FindSceenOfs(this, screenLeft, screenTop);
      sf::Color shineColor;
      sf::Color shadowColor;
      for (int i = 0; i < 3; i++)
      {
         shineColor = sf::Color((faceColor.r+255)/2, (faceColor.g+255)/2, (faceColor.b+255)/2);  // halfway between face color and pure white
         shadowColor = sf::Color((faceColor.r)/2, (faceColor.g)/2, (faceColor.b)/2);  // half brightness of face color
      }

        int X = screenLeft+height/2;
        int Y = screenTop+height/2;
        int R = BoxPanel.height/2 - 4;
        sf::Color Face = faceColor;
        sf::Color Shine = shineColor;
        sf::Color Shadow = shadowColor;
        sf::Color Window = windowColor;
        sf::Color WindowText = windowTextColor;
        sf::Shape Circle1 = sf::Shape::Circle(X-2, Y-2, R, Shadow);
        sf::Shape Circle2 = sf::Shape::Circle(X+2, Y+2, R, Shine);
        sf::Shape Circle3 = sf::Shape::Circle(X, Y, R-0, Window);
        sf::Shape Circle4 = sf::Shape::Circle(X, Y, R-2, WindowText);
        GUIapp->Draw(Circle1);
        GUIapp->Draw(Circle2);
        GUIapp->Draw(Circle3);
        if (checked) GUIapp->Draw(Circle4);




        if (useParentColors) CopyParentColors(this);
        if (enabled) btnText.myText.SetColor(faceTextColor);
        else btnText.myText.SetColor(sf::Color((faceColor.r+faceTextColor.r)/2, (faceColor.g+faceTextColor.g)/2, (faceColor.b+faceTextColor.b)/2));

        btnText.draw();
    }
}

void GUItabPanel::TabPanelInitialize(GUIwidget* pWidget)
{
    bevelWidth = 2;
    thisType = tguiTabPanel;
    bevelOuter = bsDown;
    selected = 0;
    tabHeight = 80;
    glyphAlignment = atLeft;
    textAlignment = atCenter;
    char buffer[20];
    tabs.clear();
    pages.clear();
    int desiredCount = count;
    count = 0; // none actually exist yet
    SetTabCount(desiredCount);

    for (int i=0; i<count; i++)
    {
        sprintf(buffer, "Tab %d", i+1);
        tabs[i]->SetText(buffer);
        tabs[i]->glyphAlignment = glyphAlignment;
        tabs[i]->textAlignment = textAlignment;
        tabs[i]->bevelWidth = 2;
        pages[i]->bevelWidth = 2;
    }
    tabs.at(selected)->pressed = true;
    if (pWidget != NULL) pWidget->AddChild(this);

}

void GUItabPanel::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    int tabWidth = (width-bevelWidth*6-4)/count;
    std::vector<GUIbutton *>::iterator it; // tabs
    SetTabWidth(tabWidth);
    for (it=tabs.begin(); it<tabs.end(); it++)
    {
        (*it)->Resize(tabWidth, tabHeight);
    }

    std::vector<GUIpanel *>::iterator itp; // pages; // list of panels for the selections
    for (itp=pages.begin(); itp<pages.end(); itp++)
    {
        (*itp)->top = tabHeight+ bevelWidth*2+4;
        (*itp)->Resize(width-bevelWidth*2, height-tabHeight-bevelWidth*2-6);
    }

}

GUItabPanel::~GUItabPanel()
{
    SetTabCount(0); // free all tabs and pages that were dynamically allocated
}

void GUItabPanel::SetTabCount(int newCount)
{
  if (newCount < 0) newCount = 0;
  if (newCount > count)
  {
      // create new tabs and pages
      int thisTop = bevelWidth*2+4;
      int tabWidth = (width-bevelWidth*6-4)/newCount;
      tabs.resize(newCount);
      pages.resize(newCount);
      for (int i=count; i < newCount; i++)
      {
          GUIbutton* newButton = new GUIbutton(tabWidth*i+bevelWidth*2+4, thisTop, tabWidth, tabHeight);
          GUIpanel* newPanel = new GUIpanel(bevelWidth*1+0, thisTop+tabHeight, width-bevelWidth*2-0, height-tabHeight-bevelWidth*2-6);
          newButton->parent = this;
          newPanel->parent = this;
          newButton->bevelWidth = 2;
          newButton->isTabButton = true;
          tabs[i] = newButton;
          AddChild(tabs[i]);
          pages[i] = newPanel;
          AddChild(pages[i]);
      }

  }
  else
  {

      if (newCount < count)
      {
          std::vector<GUIwidget*>::iterator it;

          // free unused tabs and pages
          for (int i=count-1; i>=newCount; i--)
          {

              for ( it=children.begin() ; it < children.end(); it++ )
              {
                  if (*it ==  tabs[i]) children.erase(it);
                  if (*it == pages[i]) children.erase(it);
              }
          }
      }
  }
  count = newCount;
}


GUItabPanel::GUItabPanel(): GUIpanel()
{
    left = 0;
    top = 0;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    count    = 2;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(int Count): GUIpanel()
{
    left = 0;
    top = 0;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    if (Count >= 2) count = Count;
    else count = 2;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(int Left, int Top): GUIpanel(Left, Top)
{
    count    = 2;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(int Left, int Top, int Count): GUIpanel(Left, Top)
{
    if (Count >= 2) count = Count;
    else count = 2;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
    count    = 2;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(int Left, int Top, int Width, int Height, int Count): GUIpanel(Left, Top, Width, Height)
{
    if (Count >= 2) count = Count;
    else count = 2;
    TabPanelInitialize(NULL);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget): GUIpanel()
{
    left = 0;
    top = 0;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    count    = 2;
    TabPanelInitialize(pWidget);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget, int Count): GUIpanel()
{
    left = 0;
    top = 0;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    if (Count >= 2) count = Count;
    else count = 2;
    TabPanelInitialize(pWidget);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget, int Left, int Top): GUIpanel(Left, Top)
{
    count    = 2;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    TabPanelInitialize(pWidget);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget, int Left, int Top, int Count): GUIpanel(Left, Top)
{
    if (Count >= 2) count = Count;
    else count = 2;
    width = GUIdefaultPanelWidth;
    height = GUIdefaultPanelHeight;
    TabPanelInitialize(pWidget);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget, int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
    count    = 2;
    TabPanelInitialize(pWidget);
}

GUItabPanel::GUItabPanel(GUIwidget* pWidget, int Left, int Top, int Width, int Height, int Count): GUIpanel(Left, Top, Width, Height)
{
    if (Count >= 2) count = Count;
    else count = 2;
    TabPanelInitialize(pWidget);
}

void GUItabPanel::SetGlyphAlignment(GUIalignment newAlignment)
{
    for (int i=0; i<count; i++) tabs.at(i)->glyphAlignment = newAlignment;
}

void GUItabPanel::SetTabWidth(int newWidth)
{
    if (newWidth > (bevelWidth*2+6))
    {
        for (int i=0; i<count; i++)
        {
            tabs.at(i)->width = newWidth;
            tabs.at(i)->left = bevelWidth*2+4 + i*newWidth;
        }
    }
}

void GUItabPanel::SetTabHeight(int newHeight)
{
    if (newHeight > (bevelWidth*2+6))
    {
        for (int i=0; i<count; i++)
        {
            tabs.at(i)->height = newHeight;
            pages.at(i)->top = bevelWidth*2+4+newHeight;
            pages.at(i)->height = height-newHeight-bevelWidth*2-8;
        }
    }
}

void GUItabPanel::draw()
{

    GUIpanel::draw();

    for (int i=0; i<count; i++)
    {
        if ((tabs.at(i)->pressed)&&(i != selected))
        {
            selected = i;
            for (int j=0; j < count; j++)  tabs.at(j)->pressed = false;
            tabs.at(selected)->pressed = true;
        }
    }

    for (int i=0; i<count; i++)
    {
        if (i != selected) tabs.at(i)->draw();
        pages[i]->isVisible = false;
    }

    pages.at(selected)->draw();
    pages[selected]->isVisible = true;
    tabs.at(selected)->draw();

}

void GUIprogressBar::ProgressBarInitialize(GUIwidget* pWidget)
{
  orientation = orHorizontal;
  bevelOuter = bsDown;
  bevelWidth = 2;
  min = 0;
  max = 100;
  position = 0;
  if (pWidget != NULL) pWidget->AddChild(this);
}

GUIprogressBar::GUIprogressBar(): GUIpanel()
{
  width = 120; height = 18;
  ProgressBarInitialize(NULL);
}

GUIprogressBar::GUIprogressBar(int Left, int Top): GUIpanel(Left, Top)
{
  width = 120; height = 18;
  ProgressBarInitialize(NULL);
}

GUIprogressBar::GUIprogressBar(int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
  ProgressBarInitialize(NULL);
}

GUIprogressBar::GUIprogressBar(GUIwidget* pWidget): GUIpanel()
{
  width = 120; height = 18;
  ProgressBarInitialize(pWidget);
}

GUIprogressBar::GUIprogressBar(GUIwidget* pWidget, int Left, int Top): GUIpanel(Left, Top)
{
  width = 120; height = 18;
  ProgressBarInitialize(pWidget);
}

GUIprogressBar::GUIprogressBar(GUIwidget* pWidget, int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
  ProgressBarInitialize(pWidget);
}

void GUIprogressBar::draw()
{
    if (isVisible)
    {
      GUIpanel::draw();

      if ((useParentColors)&&(parent != NULL)) CopyParentColors(this);

      int screenLeft, screenTop;
      FindSceenOfs(this, screenLeft, screenTop);

        sf::Color Face = faceColor;
        sf::Color Shine = sf::Color((255+faceColor.r)/2, (255+faceColor.g)/2, (255+faceColor.b)/2);
        sf::Color Shadow = sf::Color(faceColor.r/2, faceColor.g/2, faceColor.b/2);
        sf::Color Window = windowColor;
        sf::Color WindowText = windowTextColor;
        sf::Color Highlight = highlightColor;
    int L = screenLeft+bevelWidth*2;
    int T = screenTop+bevelWidth*2;
    int R = screenLeft+width-bevelWidth*2;
    int B = screenTop+height-bevelWidth*2;
    if (position > max) position = max;
    if (position < min) position = min;
    if (orientation == orHorizontal)
    {
        R = L + (R-L)*(position-min)/(max-min);
    }
    else
    {  // vertical
        T = B - (B-T)*(position-min)/(max-min);
    }
    sf::Shape bar= sf::Shape::Rectangle(L, T, R, B, Highlight);
    GUIapp->Draw(bar);
  }
}

void GUIscrollDecrement(GUIwidget *pWidget)
{
    GUIscrollBar* pScroll;
    pScroll = static_cast<GUIscrollBar*>(pWidget->parent);
    int stepsize = pScroll->stepSize;
    if (pScroll->position > pScroll->min) pScroll->position-=stepsize;
    if (pScroll->position < pScroll->min) pScroll->position = pScroll->min;
}


void GUIscrollIncrement(GUIwidget *pWidget)
{
    GUIscrollBar* pScroll;
    pScroll = static_cast<GUIscrollBar*>(pWidget->parent);
    int stepsize = pScroll->stepSize;
    if (pScroll->position < pScroll->max) pScroll->position+=stepsize;
    if (pScroll->position > pScroll->max) pScroll->position = pScroll->max;
}

void GUIscrollSliderOnClick(GUIwidget *pWidget)
{
    GUIscrollBar* thisScroll;
    thisScroll = static_cast<GUIscrollBar*>(pWidget->parent);
    int mouseX = GUImenuInput->GetMouseX();
    int mouseY = GUImenuInput->GetMouseY();
    int screenLeft, screenTop;
    FindSceenOfs(pWidget, screenLeft, screenTop);
    thisScroll->AnchorX = mouseX - screenLeft;
    thisScroll->AnchorY = mouseY - GUIapp->GetHeight() - screenTop;
    thisScroll->beingDragged = true;
}

void GUItrackSliderOnClick(GUIwidget *pWidget)
{
    GUItrackBar* thisScroll;
    thisScroll = static_cast<GUItrackBar*>(pWidget->parent);
    int mouseX = GUImenuInput->GetMouseX();
    int mouseY = GUImenuInput->GetMouseY();
    int screenLeft, screenTop;
    FindSceenOfs(pWidget, screenLeft, screenTop);
    thisScroll->AnchorX = mouseX - screenLeft;
    thisScroll->AnchorY = mouseY - GUIapp->GetHeight() - screenTop;
    thisScroll->beingDragged = true;
}



void GUIscrollBar::ScrollBarInitialize(GUIwidget* pWidget)
{
    bevelOuter = bsDown;
    AnchorX = 0;
    AnchorY = 0;
    beingDragged = false; // applies to slider
    thisType = tguiScrollBar;
    stepSize = 1;
    min = 0;
    max = 100;
    position = 20;
    oldPosition = position;
    bevelWidth = 2;
    btnDecrease.bevelWidth = 2;
    btnIncrease.bevelWidth = 2;
    btnSlider.bevelWidth = 2;

    btnDecrease.toggle = false;
    btnIncrease.toggle = false;
    btnSlider.toggle = false;
    btnDecrease.autorepeat = true;
    btnIncrease.autorepeat = true;


    btnDecrease.drawSpecialSymbol = true;
    btnIncrease.drawSpecialSymbol = true;
    btnSlider.drawSpecialSymbol = true;
//    btnSlider.symbol = ssGrip;

    btnDecrease.parent = this;
    btnIncrease.parent = this;
    btnSlider.parent = this;

    this->AddChild(&btnDecrease);
    this->AddChild(&btnIncrease);
    this->AddChild(&btnSlider);

    btnDecrease.OnClick = &GUIscrollDecrement;
    btnIncrease.OnClick = &GUIscrollIncrement;
    btnSlider.OnClick = &GUIscrollSliderOnClick;
    sliderPercent = 0.2;

    if (pWidget != NULL) pWidget->AddChild(this);

    Resize(width, height);
}

void GUIscrollBar::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    int markerSize;

    if (orientation == orHorizontal)
    {
        btnSlider.symbol = ssGripH;
        markerSize = height-4;
        // make sure the scroll bar is wide enough to display all parts
        if (width < (3*markerSize+4)) width = 3*markerSize+4;

        btnDecrease.height = markerSize;
        btnDecrease.width = markerSize;
        btnDecrease.top = 2;
        btnDecrease.left = 2;
        btnDecrease.symbol = ssLeftTriangle;

        btnIncrease.height = markerSize;
        btnIncrease.width = markerSize;
        btnIncrease.top = 2;
        btnIncrease.left = width-2-markerSize;
        btnIncrease.symbol = ssRightTriangle;

        btnSlider.top = 2;
        btnSlider.height = markerSize;
        if (width < (5*markerSize+4)) btnSlider.width = 2*markerSize;
        else btnSlider.width = markerSize;
        btnSlider.left = width/2;

        // btnSlider.left will be determined at draw time
    }
    else // orientation = vertical
    {
        btnSlider.symbol = ssGripV;
        markerSize = width-4;
        // make sure the scroll bar is tall enough to display all parts
        if (height < (3*markerSize+4)) height = 3*markerSize+4;

        btnDecrease.height = markerSize;
        btnDecrease.width = markerSize;
        btnDecrease.top = 2;
        btnDecrease.left = 2;
        btnDecrease.symbol = ssUpTriangle;

        btnIncrease.height = markerSize;
        btnIncrease.width = markerSize;
        btnIncrease.left = 2;
        btnIncrease.top = height-2-markerSize;
        btnIncrease.symbol = ssDownTriangle;

        btnSlider.left = 2;
        btnSlider.width = markerSize;
        if (height < (5*markerSize+4)) btnSlider.height = 2*markerSize;
        else btnSlider.height = markerSize;
        // btnSlider.top will be determined at draw time

    }

}

GUIscrollBar::GUIscrollBar() : GUIpanel()
{
    orientation = orVertical;
    left = 0; top = 0;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    ScrollBarInitialize(NULL);
}

GUIscrollBar::GUIscrollBar(int Left, int Top) : GUIpanel(Left, Top)
{
    orientation = orVertical;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    ScrollBarInitialize(NULL);
}

GUIscrollBar::GUIscrollBar(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    if (width >= height) orientation = orHorizontal;
    else orientation = orVertical;
    ScrollBarInitialize(NULL);
}

GUIscrollBar::GUIscrollBar(GUIwidget* pWidget) : GUIpanel()
{
    orientation = orVertical;
    left = 0; top = 0;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    ScrollBarInitialize(pWidget);
}

GUIscrollBar::GUIscrollBar(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    orientation = orVertical;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    ScrollBarInitialize(pWidget);
}

GUIscrollBar::GUIscrollBar(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    if (width >= height) orientation = orHorizontal;
    else orientation = orVertical;
    ScrollBarInitialize(pWidget);
}

void GUIscrollBar::draw()
{
   if (isVisible)
   {
      if (!GUImenuInput->IsMouseButtonDown(sf::Mouse::Left)) beingDragged = false;

      int newLeft, newTop;
      if (beingDragged)
      {
          int screenLeft, screenTop;
          FindSceenOfs(this, screenLeft, screenTop);

          newLeft = GUImenuInput->GetMouseX()-screenLeft-AnchorX;
          newTop  = GUImenuInput->GetMouseY()-GUIapp->GetHeight()-screenTop-AnchorY;
      }

        if (orientation == orHorizontal)
        {
            btnSlider.width =  sliderPercent*(width-btnIncrease.width-btnDecrease.width-4);
            int availableWidth = width-btnIncrease.width-btnDecrease.width-btnSlider.width-4;
            if (beingDragged)
            {
                if (newLeft < (btnDecrease.width+2)) newLeft = btnDecrease.width+2;
                if (newLeft > (width-btnIncrease.width-btnSlider.width-2)) newLeft = width-btnIncrease.width-btnSlider.width-2;
                position = min + float((newLeft-2-btnDecrease.width)*(max-min))/float(availableWidth);
            }
            btnSlider.left = 2+btnDecrease.width + float((position-min)*availableWidth)/float(max-min);
        }
        else
        {
            btnSlider.height =  sliderPercent*(height-btnIncrease.height-btnDecrease.height-4);
            int availableHeight = height-btnIncrease.height-btnDecrease.height-btnSlider.height-4;
            if (beingDragged)
            {
                if (newTop < (btnDecrease.height+2)) newTop = btnDecrease.height+2;
                if (newTop > (height-btnIncrease.height-btnSlider.height-2)) newTop = height-btnIncrease.height-btnSlider.height-2;
                position = min + float((newTop-2-btnDecrease.height)*(max-min))/float(availableHeight);
            }
            btnSlider.top = 2+btnDecrease.height + float((position-min)*availableHeight)/float(max-min);
        }
        if ((position != oldPosition) && (OnChange != NULL)) OnChange(this);
        GUIpanel::draw();
        oldPosition = position;
   }
}

void GUItrackBar::TrackBarInitialize(GUIwidget* pWidget)
{
    bevelOuter = bsDown;
    AnchorX = 0;
    AnchorY = 0;
    beingDragged = false; // applies to slider
    thisType = tguiTrackBar;
    min = 0;
    max = 100;
    position = 20;
    oldPosition = position;
    bevelWidth = 2;
    btnSlider.bevelWidth = 2;
    btnSlider.toggle = false;
    btnSlider.drawSpecialSymbol = true;
//    btnSlider.symbol = ssGrip;

    Resize(width, height);
    btnSlider.parent = this;

    this->AddChild(&btnSlider);

    btnSlider.OnClick = &GUItrackSliderOnClick;
    if (pWidget != NULL) pWidget->AddChild(this);

}

void GUItrackBar::Resize(int Width, int Height)
{
    width = Width;
    height = Height;

    if (orientation == orHorizontal)
    {
        btnSlider.top = 0;
        btnSlider.height = height;
        btnSlider.width = height/3;
        if (btnSlider.width < 8) btnSlider.width = 8;
        btnSlider.left = btnSlider.width/2;
        // btnSlider.left will be determined at draw time
    }
    else // orientation = vertical
    {
        btnSlider.left = 0;
        btnSlider.width = width;
        btnSlider.height = width/3;
        if (btnSlider.height < 8) btnSlider.height = 8;
        // btnSlider.top will be determined at draw time
    }
}

GUItrackBar::GUItrackBar() : GUIpanel()
{
    orientation = orVertical;
    left = 0; top = 0;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    TrackBarInitialize(NULL);
}

GUItrackBar::GUItrackBar(int Left, int Top) : GUIpanel(Left, Top)
{
    orientation = orVertical;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    TrackBarInitialize(NULL);
}

GUItrackBar::GUItrackBar(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    if (width >= height) orientation = orHorizontal;
    else orientation = orVertical;
    TrackBarInitialize(NULL);
}

GUItrackBar::GUItrackBar(GUIwidget* pWidget) : GUIpanel()
{
    orientation = orVertical;
    left = 0; top = 0;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    TrackBarInitialize(pWidget);
}

GUItrackBar::GUItrackBar(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    orientation = orVertical;
    width = GUIdefaultButtonHeight;
    height = 3*GUIdefaultButtonHeight;
    TrackBarInitialize(pWidget);
}

GUItrackBar::GUItrackBar(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    if (width >= height) orientation = orHorizontal;
    else orientation = orVertical;
    TrackBarInitialize(pWidget);
}

void GUItrackBar::draw()
{
  if (!GUImenuInput->IsMouseButtonDown(sf::Mouse::Left)) beingDragged = false;

  int newLeft, newTop;
  if (beingDragged)
  {
      int screenLeft, screenTop;
      FindSceenOfs(this, screenLeft, screenTop);

      newLeft = GUImenuInput->GetMouseX()-screenLeft-AnchorX;
      newTop  = GUImenuInput->GetMouseY()-GUIapp->GetHeight()-screenTop-AnchorY;
  }

    if (orientation == orHorizontal)
    {
        int availableWidth = width-btnSlider.width-0;
        if (beingDragged)
        {
            if (newLeft < (0)) newLeft = 0;
            if (newLeft > (width-btnSlider.width-0)) newLeft = width-btnSlider.width-0;
            position = min + float((newLeft-0)*(max-min))/float(availableWidth);
        }
        btnSlider.left = 0 + float((position-min)*availableWidth)/float(max-min);
    }
    else
    {
        int availableHeight = height-btnSlider.height-0;
        if (beingDragged)
        {
            if (newTop < (0)) newTop = 0;
            if (newTop > (height-btnSlider.height-0)) newTop = height-btnSlider.height-0;
            position = min + float((newTop-0)*(max-min))/float(availableHeight);
        }
        btnSlider.top = 0 + float((position-min)*availableHeight)/float(max-min);
    }
    if ((position != oldPosition) && (OnChange != NULL)) OnChange(this);
    oldPosition = position;




    if (orientation == orHorizontal)
    {
        int keepTop = top;
        int keepHeight = height;
        top = keepTop + (height)/3;
        height = height/3;
        btnSlider.top = keepTop-top;
        GUIpanel::draw();
        top = keepTop;
        btnSlider.top = 0;
        height = keepHeight;
    }
    else
    {
        int keepLeft = left;
        int keepWidth = width;
        left = keepLeft + (width)/3;
        width = width/3;
        btnSlider.left = keepLeft-left;
        GUIpanel::draw();
        left = keepLeft;
        btnSlider.left = 0;
        width = keepWidth;
    }
}

void GUIedit::EditInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    bevelOuter = bsDown;
    textItem = true;
    thisType = tguiEdit;
    myText.SetSize(GUIdefaultTextSize);
    tabStop = true;
    cursorPos = 0;
    selectionStart = 0;
    editMode = false;
    viewLeft = 0;
    if (pWidget != NULL) pWidget->AddChild(this);
}

GUIedit::GUIedit(): GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(NULL);
    myText.SetText("Edit");
}

GUIedit::GUIedit(GUIwidget* pWidget): GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(pWidget);
    myText.SetText("Edit");
}

GUIedit::GUIedit(int Left, int Top): GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(NULL);
    myText.SetText("Edit");
}

GUIedit::GUIedit(GUIwidget* pWidget, int Left, int Top): GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(pWidget);
    myText.SetText("Edit");
}

GUIedit::GUIedit(int Left, int Top, sf::Unicode::Text newText): GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(NULL);
    myText.SetText(newText);
}

GUIedit::GUIedit(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText): GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    EditInitialize(pWidget);
    myText.SetText(newText);
}

GUIedit::GUIedit(int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
    EditInitialize(NULL);
    myText.SetText("Edit");
}

GUIedit::GUIedit(GUIwidget* pWidget, int Left, int Top, int Width, int Height): GUIpanel(Left, Top, Width, Height)
{
    EditInitialize(pWidget);
    myText.SetText("Edit");
}
GUIedit::GUIedit(int Left, int Top, int Width, int Height, sf::Unicode::Text newText): GUIpanel(Left, Top, Width, Height)
{
    EditInitialize(NULL);
    myText.SetText(newText);
}

GUIedit::GUIedit(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText): GUIpanel(Left, Top, Width, Height)
{
    EditInitialize(pWidget);
    myText.SetText(newText);
}

void GUIedit::MouseCursor(int X, int Y) // position cursor based on mouse position
{
    int charCount = 0;
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);

    int textLeft;
    textLeft = screenLeft+bevelWidth+2;
    int cursorSpacing = X - textLeft; // how far the mouse click was from the left edge of the text field
    int charRight = 0; // right edge of character
    int charLeft = 0;  // left edge of character
    std::string str = myText.GetText();
    bool bDone =false;

    while (!bDone)
    {
        if ((cursorSpacing < ((charRight+charLeft)/2))||(charCount > int(str.length())))
        {
            bDone = true;
            if (charCount <= int(str.length())) charCount--;
        }
        else
        {
            charCount++; // look at next character
            charLeft = charRight;
            charRight = myText.GetCharacterPos(charCount).x;
        }
    }
    if (charCount < 0)  charCount = 0;
    cursorPos = charCount;
    if (cursorPos > int(str.length())) cursorPos = str.length();
    selectionStart = cursorPos;
}

void GUIedit::SetText(sf::Unicode::Text newText)
{
    myText.SetText(newText);
    std::string aStr = newText;
//    cursorPos = aStr.length();
    cursorPos = 0;
    viewLeft = 0;
    selectionStart = cursorPos;
}

sf::Unicode::Text GUIedit::GetText()
{
    return myText.GetText();
}

bool GUIedit::wasChanged()
{
    std::string astr;
    astr = myText.GetText();
    if (oldCopy.compare(astr) == 0) return false;// no change
    else return true;
}

void GUIedit::draw()
{
  if (isVisible)
  {
    GUIpanel::draw();

    int screenLeft, screenTop;
    int textLeft;
    FindSceenOfs(this, screenLeft, screenTop);
    textLeft = screenLeft+bevelWidth+2;
    myText.SetPosition(screenLeft+bevelWidth+2, screenTop+bevelWidth);
    myText.SetColor(windowTextColor);
    glDisable(GL_LIGHTING);
    sf::String tempText;
    tempText = myText;
    std::string astr, bstr;
    if (cursorPos < viewLeft) viewLeft = cursorPos; // move view to left if cursor has moved left out of view
    int viewRight = viewLeft; // character positin at right edge of view region
    int viewWidth = width-bevelWidth*2-4; // width of edit box available for displaying text
    int charWidth = 0;
    astr = myText.GetText();

    while ((charWidth < viewWidth)&&(viewRight < int(astr.length())))
    {
        viewRight++;
        charWidth = myText.GetCharacterPos(viewRight).x - myText.GetCharacterPos(viewLeft).x;
    }
    if (charWidth > viewWidth) viewRight--;
    if (viewRight < viewLeft) viewRight = viewLeft;

    if (cursorPos > viewRight)
    {  // have to update viewLeft so that character at cursor position (at right edge) will be within view
        viewRight = cursorPos;
        charWidth = 0;
        viewLeft = viewRight;
        while ((charWidth < viewWidth)&&(viewLeft > 0))
        {
            viewLeft--;
            charWidth = myText.GetCharacterPos(viewRight).x - myText.GetCharacterPos(viewLeft).x;
        }
        if (charWidth > viewWidth) viewLeft++;
    }

    int selectionLeft = cursorPos;
    int selectionRight = cursorPos;
    if (selectionStart < selectionLeft) selectionLeft = selectionStart;
    if (selectionStart > selectionRight) selectionRight = selectionStart;

    astr = myText.GetText();
    if (selectionLeft > viewLeft)
    {
        // draw text up to selection region
        bstr = astr.substr(viewLeft, selectionLeft-viewLeft);
        tempText.SetText(bstr);
        GUIapp->Draw(tempText);
    }

    // draw text in the selection region
    if (selectionLeft < selectionRight)
    {
      tempText.SetColor(highlightTextColor);
//        tempText.SetColor(sf::Color::Magenta);

      int subLeft = selectionLeft;
      int subRight = selectionRight;
      if (viewLeft > subLeft) subLeft = viewLeft;
      if (viewRight < subRight) subRight = viewRight;

      bstr = astr.substr(subLeft, subRight-subLeft);
      tempText.SetText(bstr);
      int leftX = textLeft+myText.GetCharacterPos(subLeft).x - myText.GetCharacterPos(viewLeft).x;
      int rightX = leftX + tempText.GetCharacterPos(subRight-subLeft).x;
      int topY = myText.GetPosition().y;
      int bottomY = topY + height-bevelWidth*2;
      sf::Shape highlightBack = sf::Shape::Rectangle(leftX, topY, rightX, bottomY, highlightColor);
//      sf::Shape highlightBack = sf::Shape::Rectangle(leftX, topY, rightX, bottomY, sf::Color::Blue);
      tempText.SetPosition(leftX, myText.GetPosition().y);
      GUIapp->Draw(highlightBack);
      GUIapp->Draw(tempText);
    }

    // draw text after the selection region
    if (selectionRight < viewRight)
    {
      tempText.SetColor(windowTextColor);
      tempText.SetPosition(myText.GetCharacterPos(selectionRight).x-myText.GetCharacterPos(viewLeft).x+myText.GetPosition().x, myText.GetPosition().y);
      bstr = astr.substr(selectionRight, viewRight-selectionRight);
      tempText.SetText(bstr);
      GUIapp->Draw(tempText);
    }


    std::string str = myText.GetText();
    bool nowInEditMode = (pWidgetInFocus == this);
    if (nowInEditMode && !editMode) // just entered edit mode
    {
      oldCopy = myText.GetText();
    }
    editMode = nowInEditMode;
    if ((editMode)&&(fmod(GUIcursorClock.GetElapsedTime(), 1.0) > 0.5)) // blinking cursor
    {
        // draw cursor
        int cursorX = textLeft+ myText.GetCharacterPos(cursorPos).x-myText.GetCharacterPos(viewLeft).x;
        sf::Shape Line   = sf::Shape::Line(cursorX, screenTop+bevelWidth+2, cursorX, screenTop+height-bevelWidth-2, 1, sf::Color::Black);
        GUIapp->Draw(Line);
    }
  }
}

void GUImenuBar::menuBarInitialize(GUIwidget* pWidget)
{
    align = alTop;
    height = GUIformTitleBarHeight;
    thisType = tguiMenu;
    bevelInner = bsNone;
    bevelOuter = bsNone;
    borderWidth = 0;
    bevelWidth = 0;
    top = GUIformTitleBarHeight;
    left = 2;
    canHide = false;
    hidden = false;
    Resize(width, height);
    if (pWidget != NULL) pWidget->AddChild(this);
}

void GUImenuBar::Resize(int Width, int Height)
{

    width = Width;
    height = Height;
//    std::vector<GUIbutton*>::iterator it;
    std::vector<Entry>::iterator it;
    for (it=items.begin(); it < items.end(); it++)
    {
        (*it).btn->height = height;
    }
    if ((parent != NULL)&&(static_cast<GUIform*>(parent)->showTitleBar))
    {
        top = GUIformTitleBarHeight;
    }
    else top = 0;
    if (parent != NULL) width = parent->width-8;

}

void MainMenuClick(GUIwidget* pWidget) // handle clicking of main menu button
{
    GUIwidget* pKeep = pWidget;
    if ((pWidget != NULL)&&(pWidget->parent != NULL)&&(pWidget->parent->thisType == tguiMenu))
    {
        GUImenuBar* pMenu = static_cast<GUImenuBar*>(pWidget->parent);
        // scan main menu to locate this button (pwidget) and then get the popup pointer
        unsigned int index;
        bool found = false;
        for (unsigned int i=0; i < pMenu->items.size(); i++)
        {
            if (pMenu->items[i].btn == pWidget)
            {
                found = true;
                index = i;
            }
            else
            {
                pMenu->items[i].btn->pressed = false;
            }
        }
        if (found)
        {
            GUIpopup* pPop = pMenu->items[index].pop;

            pPop->isVisible = pMenu->items[index].btn->pressed;

            while (pWidget->parent != NULL) pWidget = pWidget->parent; // find uber parent
            if (pWidget->thisType == tguiForm)
            {
                GUIform* pForm = static_cast<GUIform*>(pWidget);
                pWidget = pPop;
                if (pPop->isVisible)
                {
                    pForm->ClearPopUps();
                    pForm->AddPopUp(static_cast<GUIwidget*>(pWidget));
                    pForm->AddPopUp(static_cast<GUIwidget*>(pKeep));
                }
                else
                {
                    pForm->ClearPopUps();
                }
            }
        }

    }
}

void GUImenuBar::LinkPopUp(int index, GUIpopup* pPopUP)
{
    if (index < int(items.size()))
    {
        items[index].btn->OnClick = &MainMenuClick;
        items[index].pop = pPopUP;
        pPopUP->parent = items[index].btn;
    }
}

void GUImenuBar::MenuItemOnClick(int index, void (*OnClick) (GUIwidget *))
{
    if (index < int(items.size()))
    items[index].btn->OnClick =  OnClick;
}

GUImenuBar::GUImenuBar(): GUIpanel()
{
    menuBarInitialize(NULL);
}


GUImenuBar::GUImenuBar(GUIwidget* pWidget): GUIpanel()
{
    menuBarInitialize(pWidget);
}

GUImenuBar::GUImenuBar(sf::Unicode::Text newItems)
{
    menuBarInitialize(NULL);
    AddMenuItems(newItems);
}

GUImenuBar::GUImenuBar(GUIwidget* pWidget, sf::Unicode::Text newItems)
{
    menuBarInitialize(pWidget);
    AddMenuItems(newItems);
}

int GUImenuBar::AddMenuItem(sf::Unicode::Text newItem)
{
  GUIbutton* thisItem = new GUIbutton(newItem);
  Entry thisEntry;
  thisEntry.btn = thisItem;
//  thisEntry.btn->toggle = false;
  thisEntry.pop = NULL;
  items.push_back(thisEntry);
  this->AddChild(thisItem);
  thisItem->flat = true;
  thisItem->height = height;
  thisItem->bevelWidth=1;
  return items.size()-1; // return the index of this new item
}

void GUImenuBar::AddMenuItems(sf::Unicode::Text newItems) // multile entries separated by commas
{
    std::string astr, bstr;
    astr = newItems;
    size_t commaAt, firstChar;
    while (astr.length() > 0)
    {
        commaAt = astr.find(",");   // find next comma (if any)
        if (commaAt != std::string::npos)
        {   // a comma was found
            if (commaAt>0) bstr = astr.substr(0, commaAt);
            else bstr = "";
            // bstr is now the string from astr up to but not including the comma
            if (astr.length() > (commaAt+1)) astr = astr.substr(commaAt+1);
            else astr = "";
            // astr is now the remainder of the string after the comma
        }
        else    // there were no more commas, make this the last entry
        {
            bstr = astr;
            astr = "";
        }
        firstChar = bstr.find_first_not_of(" "); // locate first character after spaces
        if (firstChar != std::string::npos) bstr = bstr.substr(firstChar);
        AddMenuItem(bstr);
    }

}

void GUImenuBar::draw()
{
    if ((parent != NULL)&&(static_cast<GUIform*>(parent)->showTitleBar))
    {
        top = GUIformTitleBarHeight;
    }
    else top = 0;

  int screenLeft, screenTop;
  FindSceenOfs(this, screenLeft, screenTop);
  if (parent != NULL) width = parent->width-4;

  if (canHide)
  {
      int mouseX = GUImenuInput->GetMouseX();
      int mouseY = GUImenuInput->GetMouseY();
      if ((screenLeft <= mouseX)&&(mouseX <= (screenLeft+width))
        &&(screenTop <= mouseY)&&(mouseY <= (screenTop+GUIformTitleBarHeight))
//        &&(screenTop <= mouseY)&&(mouseY <= (screenTop+height))
          )
      { // mouse is in region to be occupied by menu bar
        hidden = false;
      }
      else
      { // need to know if any menu items active - if so, don't hide
          hidden = true;
      }

  }
  else hidden = false;

  if (!hidden)
  {
      if (parent != NULL)
      {
//          top = GUIformTitleBarHeight;
//          left = 2;
          if (static_cast<GUIform*>(parent)->borderBevel) width = parent->width-6;
          else width = parent->width;
      }

      int thisLeft = 4;
      int thisWidth;
      for (unsigned int i=0; i<items.size(); i++)
      {
          items[i].btn->left = thisLeft;
          thisWidth = items[i].btn->btnLabel.myText.GetRect().GetWidth();
          items[i].btn->width = thisWidth;
          thisLeft += thisWidth + GUIformTitleBarHeight/2; // add spacing to right of button
      }

      GUIpanel::draw();
//      sf::Color Shine = sf::Color((255+faceColor.r)/2, (255+faceColor.g)/2, (255+faceColor.b)/2);
      sf::Color Shadow = sf::Color(faceColor.r/2, faceColor.g/2, faceColor.b/2);
      sf::Shape line = sf::Shape::Line(screenLeft, screenTop+GUIformTitleBarHeight,
                                       screenLeft+width, screenTop+GUIformTitleBarHeight, 2, Shadow);
      GUIapp->Draw(line);
  }
}


void GUIstatusBar::statusBarInitialize(GUIwidget* pWidget)
{
    align = alBottom;
    height = GUIformTitleBarHeight;
    thisType = tguiStatus;
    bevelInner = bsDown;
    borderWidth = 1;
    bevelWidth = 1;
    LastTag = 0;
    myText.SetSize(GUIdefaultTextSize);
    if (pWidget != NULL) pWidget->AddChild(this);
}

GUIstatusBar::GUIstatusBar(): GUIpanel()
{
    statusBarInitialize(NULL);
}

GUIstatusBar::GUIstatusBar(GUIwidget* pWidget): GUIpanel()
{
    statusBarInitialize(pWidget);
}

int GUIstatusBar::CreateItem(int thisWidth, GUIalignment align)
{
    LastTag++;
    GUIstatusItem* newItem = new GUIstatusItem;
    newItem->width = thisWidth;
    newItem->Tag = LastTag;
    newItem->stat = "";
    if (align == atLeft)
    {
        leftItems.push_back(newItem);
    }
    else  // at right
    {
        rightItems.push_back(newItem);
    }
    return LastTag;
}


int GUIstatusBar::CreateItem(int thisWidth, GUIalignment align, sf::Unicode::Text newText)
{
    LastTag++;
    GUIstatusItem* newItem = new GUIstatusItem;
    newItem->width = thisWidth;
    newItem->Tag = LastTag;
    newItem->stat = newText;
    if (align == atLeft)
    {
        leftItems.push_back(newItem);
    }
    else  // at right
    {
        rightItems.push_back(newItem);
    }
    return LastTag;
}

void GUIstatusBar::DeleteItem(int tag)
{
    std::vector<GUIstatusItem*>::iterator it;
    // search left items...
    for ( it=leftItems.begin(); it < leftItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            delete *it;
            leftItems.erase(it);
        }
    }
    // search right items...
    for ( it=rightItems.begin(); it < rightItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            delete *it;
            rightItems.erase(it);
        }
    }
}

void GUIstatusBar::SetWidth(int tag, int newWidth)
{
    std::vector<GUIstatusItem*>::iterator it;
    // search left items...
    for ( it=leftItems.begin(); it < leftItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            (*it)->width = newWidth;
        }
    }
    // search right items...
    for ( it=rightItems.begin(); it < rightItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            (*it)->width = newWidth;
        }
    }
}

void GUIstatusBar::SetText(int tag, sf::Unicode::Text newText)
{
    std::vector<GUIstatusItem*>::iterator it;
    // search left items...
    for ( it=leftItems.begin(); it < leftItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            (*it)->stat = newText;
        }
    }
    // search right items...
    for ( it=rightItems.begin(); it < rightItems.end(); it++ )
    {
        if ((*it)->Tag == tag)
        {
            (*it)->stat = newText;
        }
    }
}

GUIstatusBar::~GUIstatusBar()
{
    std::vector<GUIstatusItem*>::iterator it;
    // search left items...
    for ( it=leftItems.begin(); it < leftItems.end(); it++ )
    {
        delete *it;
        leftItems.erase(it);
    }
    // search right items...
    for ( it=rightItems.begin(); it < rightItems.end(); it++ )
    {
        delete *it;
        rightItems.erase(it);
    }
}

void GUIstatusBar::draw()
{
  if (parent != NULL)
  {
      top = parent->height-height-2;
      left = 2;
      width = parent->width-4;
  }
  GUIpanel::draw();

    if (useParentColors) CopyParentColors(this);
    myText.SetColor(faceTextColor);
    sf::Color shineColor = sf::Color((faceColor.r+255)/2, (faceColor.g+255)/2, (faceColor.b+255)/2);
    sf::Color shadowColor = sf::Color(faceColor.r/2, faceColor.g/2, faceColor.b/2);
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);
    int Dtop = screenTop+bevelWidth+borderWidth;
    sf::Shape DividerL = sf::Shape::Rectangle(0, 0, bevelWidth, height-bevelWidth*2-borderWidth*2, shineColor);
    sf::Shape DividerM = sf::Shape::Rectangle(0, 0, borderWidth, height-bevelWidth*2-borderWidth*2, faceColor);
    sf::Shape DividerR = sf::Shape::Rectangle(0, 0, bevelWidth, height-bevelWidth*2-borderWidth*2, shadowColor);

    std::vector<GUIstatusItem*>::iterator it;
    int thisLeft = screenLeft;
    // search left items...
    for ( it=leftItems.begin(); it < leftItems.end(); it++ )
    {
        myText.SetText((*it)->stat);
        myText.SetPosition(thisLeft+bevelWidth+borderWidth+3, screenTop+bevelWidth+borderWidth);
        thisLeft += (*it)->width;
        GUIapp->Draw(myText);
        DividerL.SetPosition(thisLeft-1, Dtop);
        GUIapp->Draw(DividerL);
        DividerM.SetPosition(thisLeft-0, Dtop);
        GUIapp->Draw(DividerM);
        DividerR.SetPosition(thisLeft+1, Dtop);
        GUIapp->Draw(DividerR);
    }
    // search right items...
    thisLeft = screenLeft + width;
    for ( it=rightItems.begin(); it < rightItems.end(); it++ )
    {
        thisLeft -= (*it)->width;
        myText.SetText((*it)->stat);
        myText.SetPosition(thisLeft+bevelWidth+borderWidth+3, screenTop+bevelWidth+borderWidth);
        GUIapp->Draw(myText);
        DividerL.SetPosition(thisLeft-1, Dtop);
        GUIapp->Draw(DividerL);
        DividerM.SetPosition(thisLeft-0, Dtop);
        GUIapp->Draw(DividerM);
        DividerR.SetPosition(thisLeft+1, Dtop);
        GUIapp->Draw(DividerR);
    }
}

void GUImemo::MemoInitialize(GUIwidget* pWidget)
{
    memoStr.SetSize(GUIdefaultTextSize);
    bevelOuter = bsDown;
    textItem = true;
    memoStr.SetColor(sf::Color::Black);
    Scroll.isVisible = false;
    this->AddChild(&Scroll);
    Scroll.width = 20;
    Resize(width, height);
    if (pWidget != NULL) pWidget->AddChild(this);
}

void GUImemo::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    Scroll.left = Width - Scroll.width;
    Scroll.Resize(Scroll.width, Height);
}

GUImemo::GUImemo() : GUIpanel()
{
    MemoInitialize(NULL);
}

GUImemo::GUImemo(sf::Unicode::Text newText) : GUIpanel()
{
    MemoInitialize(NULL);
    AddParagraph(newText);
}

GUImemo::GUImemo(sf::Unicode::Text newText, GUIalignment align) : GUIpanel()
{
    MemoInitialize(NULL);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(int Left, int Top) : GUIpanel(Left, Top)
{
    MemoInitialize(NULL);
}

GUImemo::GUImemo(int Left, int Top, sf::Unicode::Text newText) : GUIpanel(Left, Top)
{
    MemoInitialize(NULL);
    AddParagraph(newText);
}

GUImemo::GUImemo(int Left, int Top, sf::Unicode::Text newText, GUIalignment align) : GUIpanel(Left, Top)
{
    MemoInitialize(NULL);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(NULL);
}

GUImemo::GUImemo(int Left, int Top, int Width, int Height, sf::Unicode::Text newText, GUIalignment align) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(NULL);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(int Left, int Top, int Width, int Height, sf::Unicode::Text newText) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(NULL);
    AddParagraph(newText);
}

GUImemo::GUImemo(GUIwidget* pWidget) : GUIpanel()
{
    MemoInitialize(pWidget);
}

GUImemo::GUImemo(GUIwidget* pWidget, sf::Unicode::Text newText) : GUIpanel()
{
    MemoInitialize(pWidget);
    AddParagraph(newText);
}

GUImemo::GUImemo(GUIwidget* pWidget, sf::Unicode::Text newText, GUIalignment align) : GUIpanel()
{
    MemoInitialize(pWidget);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    MemoInitialize(pWidget);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText) : GUIpanel(Left, Top)
{
    MemoInitialize(pWidget);
    AddParagraph(newText);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top, sf::Unicode::Text newText, GUIalignment align) : GUIpanel(Left, Top)
{
    MemoInitialize(pWidget);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(pWidget);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText, GUIalignment align) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(pWidget);
    AddParagraph(newText, align);
}

GUImemo::GUImemo(GUIwidget* pWidget, int Left, int Top, int Width, int Height, sf::Unicode::Text newText) : GUIpanel(Left, Top, Width, Height)
{
    MemoInitialize(pWidget);
    AddParagraph(newText);
}

void GUImemo::SetTextSize(int newSize)
{
    if (newSize != int(memoStr.GetSize()))
    {
        memoStr.SetSize(newSize);
        // reformat all paragraphs

    }

}

void GUImemo::SetTextColor(sf::Color newColor)
{
    memoStr.SetColor(newColor);
}

void GUImemo::AddParagraph(sf::Unicode::Text newText)
{
    this->AddParagraph(newText, atLeft);
}

void GUImemo::AddParagraph(sf::Unicode::Text newText, GUIalignment align)
{

    bool bDone = false;
    memoStr.SetText(newText);
    std::string astr, bstr;
    // chop paragraph into lines
    astr = memoStr.GetText();
    int viewWidth = width-bevelWidth*2-4; // width of memo field available for displaying text
    while (!bDone)
    {
        // scan until end of paragraph or largest string that will fit horizontally
        int index = 0;
        int charWidth = 0;
//        int lastWordEnd = 0; // index to end of last whole word
        //int thisWordBegin = 0; // index to first character of current work

        while ((charWidth < viewWidth)&&(index < int(astr.length())))
        {
            index++;
            charWidth = memoStr.GetCharacterPos(index).x;

        }
        if (charWidth > viewWidth) index--;
        bstr = astr.substr(0, index); // this is the text that will fit on the line
        memoStr.SetText(bstr);
        sf::String* Aline = new sf::String;

        charWidth = memoStr.GetRect().GetWidth();
        switch (align)
        {
            case atLeft: memoStr.SetPosition(bevelWidth+2, 0); break;
            case atRight: memoStr.SetPosition(width-bevelWidth-2-charWidth, 0); break;
            case atCenter: memoStr.SetPosition(bevelWidth+2+(viewWidth-charWidth)/2, 0); break;
            default: memoStr.SetPosition(bevelWidth+2, 0); // same as atLeft
        }

        *Aline = memoStr;
        lines.push_back(Aline);
        astr = astr.substr(index, astr.length()-index); // remainder of text
        memoStr.SetText(astr);
        if (astr.length() == 0) bDone = true;
    }
}

void GUImemo::draw()
{
    GUIpanel::draw();
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);

    int thisTop = bevelWidth+2;
    for (unsigned int i=0; i < lines.size(); i++)
    {
        memoStr = *lines[i];
        memoStr.SetPosition(screenLeft+lines[i]->GetPosition().x, screenTop+thisTop);
        GUIapp->Draw(memoStr);
        thisTop += memoStr.GetSize();
    }
}

void GUIspinDecrement(GUIwidget *pWidget)
{
    GUIspinner* pSpin;
    pSpin = static_cast<GUIspinner*>(pWidget->parent);
    int stepsize = pSpin->stepSize;
    if (pSpin->value > pSpin->min) pSpin->value-=stepsize;
    if (pSpin->value < pSpin->min) pSpin->value = pSpin->min;
    if (pSpin->OnChange != NULL) pSpin->OnChange(pSpin);
}


void GUIspinIncrement(GUIwidget *pWidget)
{
    GUIspinner* pSpin;
    pSpin = static_cast<GUIspinner*>(pWidget->parent);
    int stepsize = pSpin->stepSize;
    if (pSpin->value < pSpin->max) pSpin->value+=stepsize;
    if (pSpin->value > pSpin->max) pSpin->value = pSpin->max;
    if (pSpin->OnChange != NULL) pSpin->OnChange(pSpin);
}


void GUIspinner::SpinnerInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    min = 0;
    max = 100;
    value = 0;
    stepSize = 1;
    bevelWidth = 1;
    bevelOuter = bsDown;
    thisType = tguiSpin;

    BtnUp.top = 1;
    BtnUp.drawSpecialSymbol = true;
    BtnDn.drawSpecialSymbol = true;
    BtnUp.symbol = ssUpTriangle;
    BtnDn.symbol = ssDownTriangle;
    BtnUp.bevelWidth = bevelWidth;
    BtnDn.bevelWidth = bevelWidth;
    BtnUp.toggle = false;
    BtnDn.toggle = false;
    BtnUp.OnClick = &GUIspinIncrement;
    BtnDn.OnClick = &GUIspinDecrement;
    BtnUp.autorepeat = true;
    BtnDn.autorepeat = true;

    SpinEdit.left = bevelWidth;
    SpinEdit.top = bevelWidth;
    SpinEdit.bevelWidth = bevelWidth;


    this->AddChild(&BtnUp);
    this->AddChild(&BtnDn);
    this->AddChild(&SpinEdit);
    if (pWidget != NULL) pWidget->AddChild(this);

    Resize(width, height);
}

void GUIspinner::Resize(int Width, int Height)
{
    width = Width;
    height = Height;

    BtnUp.height = height/2 - bevelWidth;
    BtnDn.height = height-BtnUp.height-2*bevelWidth;
    BtnUp.width = height;
    BtnDn.width = BtnUp.width;
    BtnDn.top = BtnUp.height+bevelWidth;
    BtnUp.left = width-BtnUp.width;
    BtnDn.left = BtnUp.left;
    SpinEdit.height = height-bevelWidth*2;
    SpinEdit.width = width - BtnUp.width-bevelWidth*2;
}

GUIspinner::GUIspinner() : GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    SpinnerInitialize(NULL);
}

GUIspinner::GUIspinner(GUIwidget* pWidget) : GUIpanel()
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    SpinnerInitialize(pWidget);
}

GUIspinner::GUIspinner(int Left, int Top) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    SpinnerInitialize(NULL);
}

GUIspinner::GUIspinner(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    width = GUIdefaultButtonWidth;
    height = GUIdefaultButtonHeight;
    SpinnerInitialize(pWidget);
}

GUIspinner::GUIspinner(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    SpinnerInitialize(NULL);
}

GUIspinner::GUIspinner(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    SpinnerInitialize(pWidget);
}

void GUIspinner::draw()
{
  char calc[20];
  sprintf(calc, "%d", value);
  SpinEdit.myText.SetText(calc);
  GUIpanel::draw();
}

////////////////////////////////////////////////////////////////////////////////////////////////

void ListBoxScrollChange(GUIwidget *pWidget)
{
  if (pWidget->parent != NULL);
  GUIlistBox* pList;
  pList = static_cast<GUIlistBox*>(pWidget->parent);
  pList->topSelection = pList->Scroll.position;
}

void GUIlistBox::ListBoxInitialize(GUIwidget* pWidget)
{
    isPopUp = false;
    enableChecks = false;
    enableMore = false;
    topSelection = 0;
    count = 0;
    selected = 0;
    bevelOuter = bsDown;
    textItem = true;
    thisType = tguiListBox;
    Scroll.top = 0;
    Scroll.height = height;
    Scroll.orientation = orVertical;
    Scroll.width = 20;
    Scroll.left = width-Scroll.width;
    this->AddChild(&Scroll);
    myText.SetSize(GUIdefaultTextSize);
    myText.SetColor(windowTextColor);
    Resize(width, height);
    Scroll.OnChange = &ListBoxScrollChange;
    if (pWidget != NULL) pWidget->AddChild(this);
}

void GUIlistBox::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    Scroll.left = width-Scroll.width;
    Scroll.Resize(Scroll.width, height);
    // if selection no longer visible, reposition "topSelection"
    ViewCount = (height-bevelWidth*2) / myText.GetSize();
    if ((selected-topSelection)>ViewCount) topSelection = selected-ViewCount;

    Scroll.max = Items.size()-ViewCount;
    Scroll.position = topSelection;
    Scroll.sliderPercent = float(ViewCount)/float(Items.size());
    if (Scroll.sliderPercent > 1)
    {
        Scroll.sliderPercent = 1;
        Scroll.isVisible = false;
    }
    else Scroll.isVisible = true;
}

GUIlistBox::GUIlistBox() : GUIpanel()
{
    ListBoxInitialize(NULL);
}

GUIlistBox::GUIlistBox(GUIwidget* pWidget) : GUIpanel()
{
    ListBoxInitialize(pWidget);
}

GUIlistBox::GUIlistBox(int Left, int Top) : GUIpanel(Left, Top)
{
    ListBoxInitialize(NULL);
}

GUIlistBox::GUIlistBox(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    ListBoxInitialize(pWidget);
}

GUIlistBox::GUIlistBox(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    ListBoxInitialize(NULL);
}

GUIlistBox::GUIlistBox(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    ListBoxInitialize(pWidget);
}

void GUIlistBox::SetCheck(int index)
{
    if ((index >=0)&&(index < int(Items.size())))
        {
            Items[index].check = ssCheck;
        }
}

void GUIlistBox::SetCheck(int index, bool state)
{
    if ((index >=0)&&(index < int(Items.size())))
    {
        if (state) Items[index].check = ssCheck;
        else  Items[index].check = ssNone;
    }
}

bool GUIlistBox::GetCheck(int index)
{
    if ((index >=0)&&(index < int(Items.size())))
    {
        if (Items[index].check == ssCheck) return true;
    }
    return false;
}

int GUIlistBox::GetCount() // returns number of items in list
{
    return Items.size();
}

sf::Unicode::Text GUIlistBox::GetItem(int index)
{
    if ((int(Items.size()) > index)&&(index >= 0)) return (Items[index]).text.GetText();
    else return "";
}

void GUIlistBox::RemoveItem(int index) // remove item at "index", subsequent items are moved to fill the gap
{
  if ((index < int(Items.size()))&&(index >= 0))
  {
      Items.erase(Items.begin()+index);
      count = Items.size();
      Resize(width, height);
  }
}

void GUIlistBox::ReplaceItem(int index, sf::Unicode::Text newText)
{
  if ((index < int(Items.size()))&&(index >= 0))
  {
/*
      sf::String* newString = new sf::String;
      newString->SetText(newText);
      newString->SetSize(myText.GetSize());
      newString->SetColor(myText.GetColor());
 //     Items.insert(Items.begin()+index, newString);
*/
      ListEntry thisEntry;
      thisEntry.check = ssNone;
      thisEntry.more = ssNone;
      thisEntry.NextItem = NULL;
      thisEntry.text.SetText(newText);
      thisEntry.text.SetSize(myText.GetSize());
      thisEntry.text.SetColor(myText.GetColor());
      Items.insert(Items.begin()+index, thisEntry);

      count = Items.size();
      Resize(width, height);
  }
}

int GUIlistBox::AppendItem(sf::Unicode::Text newText)
{
    ListEntry thisEntry;
    thisEntry.check = ssNone;
    thisEntry.more = ssNone;
    thisEntry.NextItem = NULL;
    thisEntry.text.SetText(newText);
    thisEntry.text.SetSize(myText.GetSize());
    thisEntry.text.SetColor(myText.GetColor());
    Items.push_back(thisEntry);

    count = Items.size();
    Resize(width, height);
    return Items.size()-1;
}

void GUIlistBox::AppendItems(sf::Unicode::Text newText)
{
    std::string astr, bstr;
    astr = newText;
    size_t commaAt, firstChar;
    while (astr.length() > 0)
    {
        commaAt = astr.find(",");   // find next comma (if any)
        if (commaAt != std::string::npos)
        {   // a comma was found
            if (commaAt>0) bstr = astr.substr(0, commaAt);
            else bstr = "";
            // bstr is now the string from astr up to but not including the comma
            if (astr.length() > (commaAt+1)) astr = astr.substr(commaAt+1);
            else astr = "";
            // astr is now the remainder of the string after the comma
        }
        else    // there were no more commas, make this the last entry
        {
            bstr = astr;
            astr = "";
        }
        firstChar = bstr.find_first_not_of(" "); // locate first character after spaces
        if (firstChar != std::string::npos) bstr = bstr.substr(firstChar);
        AppendItem(bstr);
    }
}

void GUIlistBox::LinkItem(int index, GUIform* pForm) // link to either a ListBox or a Form
{
    Items[index].more = ssNone;
    if ((index >= 0)&&(index < int(Items.size())))
    {
        if (pForm != NULL)
        {
            Items[index].more = ssEllipsis;
        }
        Items[index].NextItem = static_cast<GUIwidget*>(pForm);
    }
}

void GUIlistBox::LinkItem(int index, GUIlistBox* pList) // link to either a ListBox or a Form
{
    Items[index].more = ssNone;
    if ((index >= 0)&&(index < int(Items.size())))
    {
        if (pList != NULL)
        {
            pList->isPopUp = true;
            Items[index].more = ssRightTriangle;
        }
        Items[index].NextItem = static_cast<GUIwidget*>(pList);
    }
}

sf::Unicode::Text GUIlistBox::GetSelection()
{
    if (int(Items.size()) > selected) return (Items[selected]).text.GetText();
    else return "";
}

void GUIlistBox::SetSelection(int index)
{
  if ((index < int(Items.size()))&&(index >= 0)) selected = index;
}

int GUIlistBox::GetIndex() // returns index to current selection
{
    return selected;
}

sf::Unicode::Text GUIlistBox::GetText(int index)
{
    if ((index >= 0)&&(index < int(Items.size()))) return (Items[index]).text.GetText();
    else return "";
}

void GUIlistBox::SetText(int index, sf::Unicode::Text newText)
{
    if ((index  >= 0)&&(index < int(Items.size()))) (Items[index]).text.SetText(newText);
}

void GUIlistBox::MouseSelect(int X, int Y) // deal with mouse click at (X, Y)
{
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);
    int thisSelect = topSelection + (Y - screenTop)/myText.GetSize();
    if (thisSelect != selected)
    {
        selected = thisSelect;
    }
}

void GUIlistBox::draw()
{
    if (isVisible &&(isPopUp == GUIdrawPopUps))
    {
        if (useParentColors) CopyParentColors(this);
        GUIpanel::draw();
        int screenLeft, screenTop;
        FindSceenOfs(this, screenLeft, screenTop);

        for (int i = topSelection; (i < int(Items.size()))&&((i-topSelection)<ViewCount); i++)
        {
            int rowheight = myText.GetSize();
            myText.SetText((Items[i].text.GetText()));
            int L = screenLeft+bevelWidth;
            int T = screenTop+bevelWidth+(i-topSelection)*rowheight;
            int B = T + myText.GetSize();
            int R = screenLeft+width-bevelWidth;
            if (Scroll.isVisible) R = R - Scroll.width + bevelWidth;
            if (i == selected)
            {
                sf::Shape back = sf::Shape::Rectangle(L, T, R, B, highlightColor);
                GUIapp->Draw(back);
                myText.SetColor(highlightTextColor);
            }

            if (enableChecks) // draw check mark to left of text
            {
                int M = L + rowheight;
                L += bevelWidth;
                T += bevelWidth;
                if (Items[i].check == ssCheck)
                {
                    sf::Shape line = sf::Shape::Line(L, (B+T)/2, (L+M)/2, B, 2, myText.GetColor());
                    GUIapp->Draw(line);
                    line = sf::Shape::Line((L+M)/2, B, M, T, 2, myText.GetColor());
                    GUIapp->Draw(line);
                }
                T -= bevelWidth;
                L = M;
            }
            myText.SetPosition(L+bevelWidth, T-bevelWidth);
            GUIapp->Draw(myText);
            L = R- rowheight;
            if (enableMore) // draw ellipsis or triangle to right of text, depending on nature of
            {
                switch (Items[i].more)
                {
                    case ssEllipsis:
                        {
                         L +=2;
                         B -= rowheight/2;
                         int Rad = 1;
                         sf::Shape dot= sf::Shape::Circle(L, B, Rad, windowTextColor);
                         for (int i=0; i<3; i++)
                         {
                             dot  = sf::Shape::Circle(L, B, Rad, windowTextColor);
                             GUIapp->Draw(dot);
                             L += rowheight/4;
                         }
                        }
                        break;
                    case ssRightTriangle:
                        {
                            sf::Shape tri;
                            tri.AddPoint(L+2, T+2, windowTextColor);
                            tri.AddPoint(L+2, B-2, windowTextColor);
                            tri.AddPoint(R-2, (T+B)/2, windowTextColor);
                            GUIapp->Draw(tri);
                        }
                        break;
                    default: ;
                }
            }

            if (i == selected)
            {
                myText.SetColor(windowTextColor);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

void StringGridVertScrollChange(GUIwidget *pWidget)
{
  if (pWidget->parent != NULL);
  GUIstringGrid* pGrid;
  pGrid = static_cast<GUIstringGrid*>(pWidget->parent);
  pGrid->topRow = pGrid->VertScroll.position;
}

void StringGridHorzScrollChange(GUIwidget *pWidget)
{
  if (pWidget->parent != NULL);
  GUIstringGrid* pGrid;
  pGrid = static_cast<GUIstringGrid*>(pWidget->parent);
  pGrid->leftCol = pGrid->HorzScroll.position;
}

void GUIstringGrid::StringGridInitialize(GUIwidget*  pWidget)
{
    SelectionState = esNoSelection;
    topRow = 1;
    leftCol = 1;
    colCount = 0;
    rowCount = 0;
    selectedRow = 0;
    selectedCol = 0;
    showSelected = true;
    allowEdit = false;
    rowHeight = GUIdefaultTextSize;
    defaultColWidth = GUIdefaultButtonWidth;
    fixedCol = true;
    fixedRow = true;
    doFixedVertLine = true;
    doFixedHorzLine = true;
    doVertLine = true;
    doHorzLine = true;
    bevelWidth = 2;
    bevelOuter = bsDown;
    textItem = true;
    thisType = tguiStringGrid;
    if (pWidget != NULL) pWidget->AddChild(this);
    VertScroll.orientation = orVertical;
    HorzScroll.orientation = orHorizontal;
    VertScroll.width = 20;
    HorzScroll.height = VertScroll.width;
    VertScroll.top = 0;
    HorzScroll.left = 0;
    VertScroll.isVisible = false;
    HorzScroll.isVisible = false;

    CopyParentColors(this);
    myText.SetColor(windowTextColor);
    myText.SetSize(GUIdefaultTextSize);

    AddChild(&VertScroll);
    AddChild(&HorzScroll);
    box.bevelOuter = bsNone;
    box.isVisible = false;
    AddChild(&box);
    Edit.bevelOuter = bsNone;
    Edit.bevelWidth = 0;
    Edit.isVisible = false;
    AddChild(&Edit);

    Resize(width, height);

    VertScroll.OnChange = &StringGridVertScrollChange;
    HorzScroll.OnChange = &StringGridHorzScrollChange;
}

void GUIstringGrid::Resize(int Width, int Height)
{
    width = Width;
    height = Height;
    rowHeight = myText.GetSize()*1.3;
    VertScroll.left = width-VertScroll.width;
    HorzScroll.top = height-HorzScroll.height;
    HorzScroll.Resize(width, HorzScroll.height);
    VertScroll.Resize(VertScroll.width, height);
    // determine if vertical scroll needed based purely on number of rows
    // without regard to possible horizontal scroll bar
    VertScroll.isVisible = (rowCount*rowHeight > (height-bevelWidth*2));
    // now determine width available for columns to appear in
    int widthAvailable = width-bevelWidth*2;
    if (VertScroll.isVisible) widthAvailable = widthAvailable + bevelWidth - VertScroll.width;
    // determine total width of columns
    int neededWidth = 0;
    for (int i=0; i<colCount; i++)
    {
        if (i < int(colWidths.size())) neededWidth += colWidths[i]; else neededWidth += defaultColWidth;
    }
    HorzScroll.isVisible = (neededWidth > widthAvailable);
    // if H scroll necessary, and V scroll not previously needed, does H scroll now take up enough space to require V scroll?
    if (HorzScroll.isVisible && !VertScroll.isVisible)
    {
        if (rowCount*rowHeight > (height-bevelWidth-HorzScroll.height))
        {
            VertScroll.isVisible = true;
            widthAvailable = widthAvailable - VertScroll.width + bevelWidth;
        }
    }
    if (HorzScroll.isVisible && VertScroll.isVisible)
    {       // both visible, shorten each to prevent overlap
        HorzScroll.width -= VertScroll.width;
        VertScroll.height -= HorzScroll.height;
        HorzScroll.Resize(HorzScroll.width, HorzScroll.height);
        VertScroll.Resize(VertScroll.width, VertScroll.height);
        box.left = VertScroll.left;
        box.top = HorzScroll.top;
        box.width = VertScroll.width;
        box.height = HorzScroll.height;
        box.isVisible = true;
    }
    else box.isVisible = false;
    HorzScroll.sliderPercent = float(widthAvailable)/float(neededWidth);
    float heightAvailable = height - bevelWidth*2;
    if (HorzScroll.isVisible) heightAvailable = heightAvailable + bevelWidth - HorzScroll.height;
    VertScroll.sliderPercent = heightAvailable/float(rowCount*rowHeight);

    // adjust vert scroll range settings
    int rowsVisible = heightAvailable/rowHeight;
    VertScroll.max = rowCount - rowsVisible;
    if (fixedRow)
    {
        VertScroll.min = 1;
//        VertScroll.max++;
    }
    else VertScroll.min = 0;
    if (VertScroll.position > VertScroll.max) VertScroll.position = VertScroll.max;
    if (VertScroll.position < VertScroll.min) VertScroll.position = VertScroll.min;
    topRow = VertScroll.position;

    // adjust horizontal scroll range settings
    // step in from right to determine index of right-most column to display at left edge

    int col = colCount-1;
    int widthSoFar = 0;
    widthAvailable = width-bevelWidth*2;
    if (VertScroll.isVisible) widthAvailable = widthAvailable+bevelWidth-VertScroll.width;
    if (fixedCol)
    {
        if (colWidths.size()>0) widthAvailable -= colWidths[0]; else widthAvailable -= defaultColWidth;

    }
    // widthavailable is now the available range for view scrolling

    while ((widthSoFar < widthAvailable)&&(col >= 0))
    {
        widthSoFar += defaultColWidth;
        col--;
    }
    // col is now the index of the left edge column at furthest point to right
    HorzScroll.max = col+1;
    if (fixedCol)
    {
        HorzScroll.min = 1;
//        HorzScroll.max++;
    }
    else HorzScroll.min = 0;
    if (HorzScroll.position > HorzScroll.max) HorzScroll.position = HorzScroll.max;
    if (HorzScroll.position < HorzScroll.min) HorzScroll.position = HorzScroll.min;
    leftCol = HorzScroll.position;


}
/*
GUIstringGrid::GUIstringGrid() : GUIpanel()
{
    StringGridInitialize(NULL);
}
*/
GUIstringGrid::GUIstringGrid(GUIwidget*  pWidget) : GUIpanel()
{
    StringGridInitialize(pWidget);
}

GUIstringGrid::GUIstringGrid(int Left, int Top) : GUIpanel(Left, Top)
{
    StringGridInitialize(NULL);
}

GUIstringGrid::GUIstringGrid(GUIwidget*  pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    StringGridInitialize(pWidget);
}

GUIstringGrid::GUIstringGrid(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    StringGridInitialize(NULL);
}

GUIstringGrid::GUIstringGrid(GUIwidget*  pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    StringGridInitialize(pWidget);
}

void GUIstringGrid::SetLeftCol(int newLeft)
{
  leftCol = newLeft;
  HorzScroll.position = leftCol;
  Resize(width, height);
}

void GUIstringGrid::SetTopRow(int newTop)
{
  topRow = newTop;
  VertScroll.position = topRow;
  Resize(width, height);
}

void GUIstringGrid::SetFixedRow(bool doit)
{
    if (fixedRow && !doit)
    {   // change from fixed to no fixed
        topRow--;
        VertScroll.position = topRow;
    }
    if (!fixedRow && doit)
    {   // change from no fixed to fixed
        topRow++;
        VertScroll.position = topRow;
    }
    fixedRow = doit;
    Resize(width, height);
}

void GUIstringGrid::SetFixedCol(bool doit)
{
    if (fixedCol && !doit)
    {   // change from fixed to no fixed
        leftCol--;
        HorzScroll.position = leftCol;
    }
    if (!fixedCol && doit)
    {   // change from no fixed to fixed
        leftCol++;
        HorzScroll.position = leftCol;
    }
    fixedCol = doit;
    Resize(width, height);
}

void GUIstringGrid::SetColCount(int newCount)
{
    if (newCount >= 0)
    {
        colCount = newCount;
        for(unsigned int i=0; i<items.size(); i++)
        {
            items[i].resize(colCount);
        }
        if (newCount > int(colWidths.size()))
        {
            for (int i=colWidths.size(); i < newCount; i++)
            {
                colWidths.push_back(defaultColWidth);
            }
        }
        Resize(width, height);
    }
}

void GUIstringGrid::SetRowCount(int newCount)
{
    if (newCount >= 0)
    {
        rowCount = newCount;
        items.resize(rowCount);
        for(unsigned int i=0; i<items.size(); i++)
        {
            items[i].resize(colCount);
        }
        Resize(width, height);
    }
}

void GUIstringGrid::SetText(int row, int col, sf::Unicode::Text newText)
{
    if ((row >= 0)&&(col >= 0)&&(row < rowCount)&&(col < colCount))
    {
        if (items[row][col] == NULL)
        {
            sf::String* newstr = new sf::String;
            *newstr = myText; // copy basic settings
            newstr->SetText(newText);
            items[row][col] = newstr;
        }
        else
        {
            (items[row][col])->SetText(newText);
        }
    }
}

void GUIstringGrid::MouseSelect(int X, int Y)
{
    int oldSelectedRow = selectedRow;
    int oldSelectedCol = selectedCol;
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);
    // determine row corresponding to this mouse Y position
    int T = screenTop + bevelWidth;
    if (fixedRow) T += rowHeight; // fixed row cannot be "selected"
    selectedRow = topRow + (Y-T)/rowHeight;
    int H = height-bevelWidth;
    if (HorzScroll.isVisible) H = H+bevelWidth-HorzScroll.height;
    int rowsDown = selectedRow-topRow+1;
    if (fixedRow) rowsDown++;
    if ((rowsDown+1)*rowHeight > H) selectedRow--;

    // determine col corresponding to this mouse X position
    int L = screenLeft+bevelWidth;
    if (fixedCol)
    {
        if (colWidths.size() > 0) L += colWidths[0]; else L += defaultColWidth;
    }
    int index = leftCol;
    while ((L < X)&&(index < colCount))
    {
        if (int(colWidths.size()) > index) L += colWidths[index]; else L += defaultColWidth;
        index++;
    }
    index--;
    if (index < leftCol) index  = leftCol;
    selectedCol = index;
    if ((selectedRow != oldSelectedRow)||(selectedCol != oldSelectedCol))
    {   // changed which cell is selected
        switch (SelectionState)
        {
            case esNoSelection: SelectionState = esSelected; break;
            case esSelected:    break;
            case esEditing: SelectionState = esSelected;
                            Edit.isVisible = false;
                            SetText(oldSelectedRow, oldSelectedCol, Edit.GetText());
                            // emit onchange event
                            if (OnChange != NULL) OnChange(this);
                            break;
        }

    }
    else
    {   // clicked again on currently selected cell
        switch (SelectionState)
        {
            case esNoSelection: SelectionState = esSelected; break;
            case esSelected:    if (allowEdit)
                                {
                                    SelectionState = esEditing;
                                    Edit.isVisible = true;
                                    Edit.SetText(GetText(selectedRow, selectedCol));
                                    pWidgetInFocus = &Edit;
                                    GUIwidget * pWidget = this;
                                    while (pWidget->parent != NULL) pWidget = pWidget->parent;
                                    if (pWidget->thisType == tguiForm) static_cast<GUIform*>(pWidget)->ActiveWidget = &Edit;
                                    Edit.MouseCursor(X, Y);
                                } break;
            case esEditing: break;
        }


    }
}

sf::Unicode::Text GUIstringGrid::GetText(int row, int col)
{
    if ((row >= 0)&&(row < rowCount)&&(col >= 0)&&(col < colCount))
    {
        sf::String tempS = *items[row][col];
        return tempS.GetText();
    }
    else return "";
}


void GUIstringGrid::draw()
{
    GUIpanel::draw();

    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);
    sf::Color lineColor = sf::Color((faceColor.r+windowColor.r)/2, (faceColor.g+windowColor.g)/2, (faceColor.b+windowColor.b)/2);
    sf::Color shadowColor = sf::Color((faceColor.r)/2, (faceColor.g)/2, (faceColor.b)/2);
    sf::Shape back;

    int L, T, R, B;
    int row, col;
    if (fixedRow)
    {   // draw fixed row as face color
        L = screenLeft+bevelWidth; R = screenLeft+width-bevelWidth;
        T = screenTop+bevelWidth;  B = screenTop+bevelWidth+rowHeight;
        if (VertScroll.isVisible) R = R-VertScroll.width+bevelWidth;
        back = sf::Shape::Rectangle(L, T, R, B, faceColor);
        GUIapp->Draw(back);
        if (selectedRow == 0) selectedRow = topRow;
    }

    if (fixedCol)
    {   // draw fixed column as face color
        L = screenLeft+bevelWidth;
        if (colWidths.size() > 0) R = screenLeft+bevelWidth+colWidths[0];
        else R = screenLeft+bevelWidth+defaultColWidth;
        T = screenTop+bevelWidth;  B = screenTop-bevelWidth+height;
        if (HorzScroll.isVisible) B = B-HorzScroll.height+bevelWidth;
        back = sf::Shape::Rectangle(L, T, R, B, faceColor);
        GUIapp->Draw(back);
        if (selectedCol == 0) selectedCol = leftCol;
    }

    if (doFixedHorzLine || doHorzLine)
    {   // draw horizontal lines
        L = screenLeft+bevelWidth; // left edge of view region
        R = screenLeft+width-bevelWidth; // right edge of view region if no scroll bar
        if (VertScroll.isVisible) R = R - VertScroll.width+bevelWidth; // adjust if scroll bar present

        if (!doFixedHorzLine && fixedCol)
        {   // move to the right of the fixed column
            if (colWidths.size() > 0) L += colWidths[0]; else L += defaultColWidth;
        }

        if (!doHorzLine && fixedCol)
        {   // move to right edge of fixed column
            R = screenLeft+bevelWidth;
            if (colWidths.size() > 0) R += colWidths[0]; else R += defaultColWidth;
        }
        // now, L is the left edge and R is the right edge of the horizontal lines to be drawn
        T = screenTop+bevelWidth+rowHeight;
        B = screenTop+height - bevelWidth;
        if (HorzScroll.isVisible) B -= HorzScroll.height;
        // T is the top row, B is the bottom of the drawing area
        sf::Shape myLine = sf::Shape::Line(L,T,R,T,1,shadowColor);
        for (int i=T; i < B; i+=rowHeight)
        {
            GUIapp->Draw(myLine);
            myLine.Move(0, rowHeight);
        }
    }

    if (doFixedVertLine || doVertLine)
    {   // draw vertical lines
        T = screenTop+bevelWidth; // top edge of view region
        B = screenTop+height-bevelWidth; // bottom edge of view region if no scroll bar
        if (HorzScroll.isVisible) B = B - HorzScroll.height+bevelWidth; // adjust if scroll bar present

        if (!doFixedVertLine && fixedRow)
        {   // move to the bottom of the fixed row
            T += rowHeight;
        }

        if (!doVertLine && fixedRow)
        {   // move to bottom edge of fixed row
            B = screenTop+bevelWidth+rowHeight;
        }
        // now, T is the top edge and B is the bottom edge of the vertical lines to be drawn

        L = screenLeft+bevelWidth;
        int columnIndex;
        if (fixedCol)
        {
            if (colWidths.size() > 0) L += colWidths[0]; else L += defaultColWidth;
            columnIndex = leftCol;
        }
        else
        {
            if (int(colWidths.size()) > leftCol) L += colWidths[leftCol]; else L += defaultColWidth;
            columnIndex = leftCol+1;
        }
        // L is now the horizontal position of the first vertical line to be drawn
        R = screenLeft+width-bevelWidth; // right edge of view region if no scroll bar
        if (VertScroll.isVisible) R = R - VertScroll.width + bevelWidth;
        // R is now the limit for drawing lines

        int delta;
        sf::Shape myLine = sf::Shape::Line(L,T,L,B,1,shadowColor);
        while (L < R)
        {
            GUIapp->Draw(myLine);
            if (int(colWidths.size()) > columnIndex) delta = colWidths[columnIndex]; else delta = defaultColWidth;
            L += delta;
            myLine.Move(delta, 0);
            columnIndex++;
        }

    }

    // now draw strings
    bool selectedCellViewable = false;
    L = screenLeft+bevelWidth;
    R = screenLeft+width-bevelWidth; // R is the right edge limit for drawing
    if (VertScroll.isVisible) R = R-VertScroll.width+bevelWidth; // resize if scroll bar present
    int nextCol;

    if (fixedCol)
    {
        col = 0; nextCol = leftCol;
    }
    else
    {
        col = leftCol; nextCol = col+1;
    }

    while ((L < R)&&(col < colCount))
    {   // step through columns
        T = screenTop+bevelWidth; B = screenTop+height-bevelWidth;
        if (HorzScroll.isVisible) B = B - HorzScroll.height + bevelWidth;
        B = T - ((T-B)/rowHeight)*rowHeight; // dont draw to a partial row at the bottom
        int nextRow;
        if (fixedRow)
        {
            row = 0; nextRow = topRow;
        }
        else
        {
            row = topRow; nextRow = row+1;
        }
        while ((T < B)&&(row < rowCount))
        {
            if (items[row][col] == NULL) myText.SetText("");
            else  myText.SetText((items[row][col])->GetText());
            std::string astr = myText.GetText();
            int thisWidth = defaultColWidth;
            if (int(colWidths.size())>col) thisWidth = colWidths[col];
            int index = 0;
            while((index < int(astr.length()))&&(myText.GetCharacterPos(index).x < thisWidth))
            {
                index++;
            }
            if ((myText.GetCharacterPos(index).x > thisWidth)&&(index>0)) index--;
            astr = astr.substr(0, index);
            myText.SetText(astr);
            myText.SetPosition(L, T);
            if ((row == selectedRow)&&(col == selectedCol))
            {   // this cell is selected
                selectedCellViewable = true;
                int R = L;
                if (int(colWidths.size()) > col) R += colWidths[col]; else R += defaultColWidth;
                int RR = screenLeft+width-bevelWidth;
                if (VertScroll.isVisible) RR = RR +bevelWidth-VertScroll.width;
                if (R > RR) R = RR;
                Edit.left = L-screenLeft;
                Edit.width = R-L;
                Edit.top = T-screenTop;
                Edit.height = rowHeight;
                if (SelectionState == esEditing)
                {
                    /*
                    Edit.left = L-screenLeft;
                    Edit.width = R-L;
                    Edit.top = T-screenTop;
                    Edit.height = rowHeight;
                    */
                }
                else
                {
                    back = sf::Shape::Rectangle(L, T, R, T+rowHeight, highlightColor);
                    GUIapp->Draw(back);
                    myText.SetColor(highlightTextColor);
                    GUIapp->Draw(myText);
                    myText.SetColor(windowTextColor);
                }
            }
            else GUIapp->Draw(myText);
            T += rowHeight;
            row = nextRow;
            nextRow++;
        }
        if (nextCol < int(colWidths.size())) L += colWidths[nextCol]; else L += defaultColWidth;
        col = nextCol;
        nextCol++;
    }
    if ((!selectedCellViewable)&&(SelectionState == esEditing))
    {
        SelectionState = esSelected;
        Edit.isVisible = false;
        SetText(selectedRow, selectedCol, Edit.GetText());
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ComboDropClick(GUIwidget *pWidget)
{
    GUIwidget* pKeep = pWidget;
    while ((pWidget->parent != NULL)&&(pWidget->thisType != tguiComboBox)) pWidget = pWidget->parent;
    if (pWidget->thisType == tguiComboBox)
    {
        GUIcomboBox* pCombo = static_cast<GUIcomboBox*>(pWidget);

        pCombo->ListBox.isVisible = !pCombo->ListBox.isVisible;

        while (pWidget->parent != NULL) pWidget = pWidget->parent; // find uber parent
        if (pWidget->thisType == tguiForm)
        {
            GUIform* pForm = static_cast<GUIform*>(pWidget);
            pWidget = &(pCombo->ListBox);
            if (pCombo->ListBox.isVisible)
            {
                pForm->ClearPopUps();
                pForm->AddPopUp(static_cast<GUIwidget*>(pWidget));
                pForm->AddPopUp(static_cast<GUIwidget*>(pKeep));
            }
            else
            {
                pForm->ClearPopUps();
            }
        }
    }
}

void ComboListClick(GUIwidget *pWidget)
{

//    while ((pWidget->parent != NULL)&&(pWidget->thisType != tguiComboBox)) pWidget = pWidget->parent;
    if (pWidget->parent != NULL) pWidget = pWidget->parent;
    if (pWidget->thisType == tguiComboBox)
    {
        GUIcomboBox* pCombo = static_cast<GUIcomboBox*>(pWidget);
        sf::String tmpStr = pCombo->ListBox.Items[pCombo->ListBox.selected].text;
        pCombo->Edit.SetText(tmpStr.GetText());
        while (pWidget->parent != NULL) pWidget = pWidget->parent; // find uber parent
        if (pWidget->thisType == tguiForm)
        {
            GUIform* pForm = static_cast<GUIform*>(pWidget);
            pForm->ClearPopUps();
        }
        if (pCombo->OnChange != NULL) pCombo->OnChange(static_cast<GUIwidget*>(pCombo));

    }
}

void ComboModified(GUIwidget *pWidget)
{

}

void GUIcomboBox::ComboBoxInitialize(GUIwidget* pWidget)
{
    canAlign = false;
    thisType = tguiComboBox;
    if (pWidget != NULL) pWidget->AddChild(this);
    dropDownCount = 8;
    DropBtn.drawSpecialSymbol = true;
    DropBtn.symbol = ssDownTriangle;
    DropBtn.top = bevelWidth;
    DropBtn.bevelWidth = 2;
    DropBtn.toggle = false;
    Box.bevelWidth = 2;
    Box.bevelOuter = bsDown;
    AddChild(&ListBox);
    AddChild(&Edit);
    AddChild(&Box);
    Box.AddChild(&DropBtn);
    ListBox.isVisible = false;
    ListBox.isPopUp = true;
    DropBtn.OnClick = &ComboDropClick;
    ListBox.OnClick = &ComboListClick;
    Resize(width, height);
}

void GUIcomboBox::Resize(int Width, int Height)
{
    width = Width; height = Height;
    Box.width = height;
    Box.height = height;
    Box.left = width - Box.width;
    DropBtn.height = height-bevelWidth*2;
    Edit.height = height;
    DropBtn.width = DropBtn.height;
    Edit.width = width - DropBtn.width;
    DropBtn.left = bevelWidth;
    ListBox.width = width;
    ListBox.top = height;
    if (int(ListBox.Items.size()) > dropDownCount) ListBox.height = dropDownCount*ListBox.myText.GetSize()+ListBox.bevelWidth*2;
    else ListBox.height = ListBox.Items.size()*ListBox.myText.GetSize()+ListBox.bevelWidth*2;
    if (ListBox.Items.size() == 0) ListBox.height = ListBox.myText.GetSize()+ListBox.bevelWidth*2;
    ListBox.Resize(ListBox.width, ListBox.height);
}

GUIcomboBox::GUIcomboBox(GUIwidget* pWidget) : GUIpanel()
{
    ComboBoxInitialize(pWidget);
}

GUIcomboBox::GUIcomboBox(int Left, int Top) : GUIpanel(Left, Top)
{
    left = Left; top = Top;
    ComboBoxInitialize(NULL);
}

GUIcomboBox::GUIcomboBox(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    left = Left; top = Top;
    ComboBoxInitialize(pWidget);
}

GUIcomboBox::GUIcomboBox(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    left = Left; top = Top; width = Width; height = Height;
    ComboBoxInitialize(NULL);
}
GUIcomboBox::GUIcomboBox(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    left = Left; top = Top; width = Width; height = Height;
    ComboBoxInitialize(pWidget);
}
void GUIcomboBox::AppendItem(sf::Unicode::Text newText)
{
    ListBox.AppendItem(newText);
    Resize(width, height);
}

void GUIcomboBox::draw()
{
    GUIpanel::draw();
}

void GUIpopup::PopUpInitialize()
{
    canAlign = false;
    thisType = tguiPopUp;
    isPopUp = true;
    bevelInner = bsDown;
    bevelOuter = bsUp;
    borderWidth=0;
    bevelWidth = 1;
    enableMore = true;
}

GUIpopup::GUIpopup() : GUIlistBox()
{
    PopUpInitialize();
}

GUIpopup::GUIpopup(GUIwidget* pWidget, unsigned int index) : GUIlistBox()
{
    PopUpInitialize();
    if (pWidget != NULL)
    {
        if (pWidget->thisType == tguiMenu)
        {
            static_cast<GUImenuBar*>(pWidget)->LinkPopUp(index, this);
        }
    }
}

GUIpopup::GUIpopup(sf::Unicode::Text newItems)
{
    PopUpInitialize();
    AppendItems(newItems);
}

GUIpopup::GUIpopup(GUIwidget* pWidget, unsigned int index, sf::Unicode::Text newItems)
{
    PopUpInitialize();
    AppendItems(newItems);

    if (pWidget != NULL)
    {
        switch (pWidget->thisType)
        {
            case tguiMenu:  static_cast<GUImenuBar*>(pWidget)->LinkPopUp(index, this); break;
            case tguiPopUp: static_cast<GUIpopup*>(pWidget)->LinkPopUp(index, this); break;
            default: break;
        }
    }
}

void GUIpopup::LinkPopUp(unsigned int index, GUIpopup* pPopUP)
{
    if (index < Items.size())
    {
        Items[index].NextItem = pPopUP;
        Items[index].more = ssRightTriangle;
        pPopUP->parent = this;
    }
}


void GUIpopup::draw()
{
    if ((parent != NULL))
    {
        if (parent->thisType == tguiButton)
        {
            left = 0;
            top = parent->top + parent->height;
        }
        if (parent->thisType == tguiPopUp)
        {
            left = parent->width-2;
            top = parent->top + 20;
        }
        height = Items.size() * myText.GetSize() + 4;
    }
    GUIlistBox::draw();
}

void GUItree::TreeInitialize(GUIwidget* pWidget)
{
    textItem = true;
    bevelOuter = bsDown;
    thisType = tguiTreeView;
    if (pWidget != NULL) pWidget->AddChild(this);
    Root.expand = true;
    Root.parent = NULL;
    Root.myText.SetText("");
    Root.myText.SetSize(GUIdefaultTextSize);
    Root.myText.SetColor(GUIdefaultWindowTextColor);
    myText.SetSize(GUIdefaultTextSize);
    myText.SetColor(GUIdefaultWindowTextColor);
}

GUItree::GUItree(GUIwidget* pWidget) : GUIpanel()
{
    TreeInitialize(pWidget);
}

GUItree::GUItree(int Left, int Top) : GUIpanel(Left, Top)
{
    TreeInitialize(NULL);
}

GUItree::GUItree(GUIwidget* pWidget, int Left, int Top) : GUIpanel(Left, Top)
{
    TreeInitialize(pWidget);
}

GUItree::GUItree(int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    TreeInitialize(NULL);
}

GUItree::GUItree(GUIwidget* pWidget, int Left, int Top, int Width, int Height) : GUIpanel(Left, Top, Width, Height)
{
    TreeInitialize(pWidget);
}

void GUItree::AddNode(Node* parentNode, Node* newNode)
{
    if ((parentNode != NULL)&&(newNode != NULL))
    {
        parentNode->children.push_back(newNode);
    }
}

Node* GUItree::CreateNode()
{
    Node* newNode = new Node;
    newNode->childIndex = 0;
    return newNode;
}

Node* GUItree::CreateNode(sf::Unicode::Text newText, Node* parentNode)
{
    Node* newNode = new Node;
    newNode->myText.SetText(newText);
    newNode->childIndex = 0;
    if (parentNode != NULL)
    {
        parentNode->children.push_back(newNode);
        newNode->parent = parentNode;
    }
    return newNode;
}

Node* GUItree::CreateNode(sf::Sprite Glyph, sf::Unicode::Text newText, Node* parentNode)
{
    Node* newNode = new Node;
    newNode->myText.SetText(newText);
    newNode->glyph = Glyph;
    newNode->childIndex = 0;
    if (parentNode != NULL)
    {
        parentNode->children.push_back(newNode);
        newNode->parent = parentNode;
    }
    return newNode;
}

void GUItree::draw()
{
    GUIpanel::draw();
    int NodeDepth = 0; // how far down the tree - and therefor how far to right to begin drawing
    int spacing = 26;
    int rowHeight = myText.GetSize()*1.3;
    int screenLeft, screenTop;
    FindSceenOfs(this, screenLeft, screenTop);
    bool bDone = false;
    Node* thisNode = &Root;
    int thisLeft;
    int thisTop = screenTop+bevelWidth*2;
    int bottom = screenTop+height-bevelWidth - rowHeight;
    Root.childIndex = 0;
    while (!bDone)
    {
        thisLeft = screenLeft+bevelWidth*2+NodeDepth*spacing;
        if (thisNode->children.size()>0)
        {
            int sp = 6;
            int L = thisLeft+sp;
            int R = thisLeft+spacing-sp-2;
            int T = thisTop+sp;
            int B = thisTop+spacing-sp-1;
            sf::Shape square = sf::Shape::Rectangle(L, T, R, B, windowTextColor, 1, windowTextColor);
            square.EnableFill(false);
            GUIapp->Draw(square);
            sf::Shape line = sf::Shape::Line(L+2, (T+B)/2, R-2, (T+B)/2, 1, windowTextColor);
            GUIapp->Draw(line);
            if (thisNode->expand)
            {
                // draw "expand" symbol
                line = sf::Shape::Line((L+R)/2, T+2, (L+R)/2, B-2, 1, windowTextColor);
                GUIapp->Draw(line);
            }
            else
            {
                // draw "contract" symbol
            }
        }
        thisLeft += spacing;
        // draw glyph
        thisNode->glyph.SetPosition(thisLeft, thisTop);
        GUIapp->Draw(thisNode->glyph);

        thisLeft += spacing;
        myText.SetText(thisNode->myText.GetText());
        myText.SetPosition(thisLeft, thisTop);
        GUIapp->Draw(myText);
        thisTop += rowHeight;
        if (thisTop > bottom) bDone = true;

        // change to next node to display
        if ((thisNode->children.size()>0)&&(thisNode->expand))
        {   // has children to display
            thisNode->childIndex = 0;
            thisNode = thisNode->children[0];
            NodeDepth++;
//            bScan = false;
        }
        else    // no children to display
        {
            bool bScan = true;
            while (bScan && !bDone)
            {
                if (thisNode->parent == NULL) bDone = true;
                else
                {
                    // is there a sibling?
                    thisNode = thisNode->parent;
                    thisNode->childIndex++; // look at next child (if any)
                    if (thisNode->childIndex < thisNode->children.size())
                    {
                        // has a sibling to display
                        thisNode = thisNode->children[thisNode->childIndex];
                        thisNode->childIndex = 0;
                        bScan = false;
                    }
                    else
                    {
                        // no sibling
                        NodeDepth--;
                        if (NodeDepth < 0) bDone = true;
                    }
                }
            }
        }
    }
}
