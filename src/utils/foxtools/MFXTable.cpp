/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MFXTable.h
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-21
///
// Button similar to FXButton but with the possibility of showing tooltips
/****************************************************************************/
#include <config.h>

#include "MFXTable.h"

#define EXTRAMARGING 1

FXDEFMAP(MFXTable) MFXTableMap[] = {
    FXMAPFUNC(SEL_ENTER,    0,  MFXTable::onEnter),
    FXMAPFUNC(SEL_LEAVE,    0,  MFXTable::onLeave),
};


// Object implementation
FXIMPLEMENT(MFXTable, FXHorizontalFrame, MFXTableMap, ARRAYNUMBER(MFXTableMap))


MFXTable::MFXTable(FXComposite *p , FXObject* tgt, FXSelector sel) :
    FXHorizontalFrame(p, GUIDesignAuxiliarMFXTable),
    myTarget(tgt),
    mySelector(sel) {
}


MFXTable::~MFXTable() {}


long
MFXTable::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onEnter(sender, sel, ptr);
}


long
MFXTable::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    return FXHorizontalFrame::onLeave(sender, sel, ptr);
}


void
MFXTable::setItemText(FXint row, FXint column, const FXString& text, FXbool notify) {
    if ((row < myRows.size()) && (column < myColumns.size())) {
        myRows.at(row)->setText(column, text, notify);
    } else {
        throw ProcessError("Invalid row or column");
    }
}


FXString
MFXTable::getItemText(FXint row, FXint column) const {
    if ((row < myRows.size()) && (column < myColumns.size())) {
        return myRows.at(row)->getText(column);
    } else {
        throw ProcessError("Invalid row or column");
    }
}


FXint
MFXTable::getNumRows() const {
    return myRows.size();
}


FXint 
MFXTable::getCurrentRow() const {
    return currentRow;
}


FXbool
MFXTable::selectRow(FXint row, FXbool notify) {
    if (row < myRows.size()) {
        myRows.at(row)->select();
    } else {
        throw ProcessError("Invalid row");
    }
}


void
MFXTable::setCurrentItem(FXint row, FXint column, FXbool notify) {
    // CHECK
}


void 
MFXTable::setColumnText(FXint column, const FXString& text) {
    if (column < myColumns.size()) {
        myColumns.at(column)->setColumnLabel(text);
    } else {
        throw ProcessError("Invalid column");
    }
}


void
MFXTable::setTableSize(FXint numberRow, FXint numberColumn, FXbool notify) {
    // first clear table
    clearTable();
    // create columns
    for (int i = 0; i < numberColumn; i++) {
        myColumns.push_back(new Column(this, i));
    }
    // create rows
    for (int i = 0; i < numberRow; i++) {
        myRows.push_back(new Row(this));
    }
}


FXTextField* 
MFXTable::getItem(FXint row, FXint col) const {
    if ((row < myRows.size()) && (col < myColumns.size())) {
        return myRows.at(row)->getTextField(col);
    }
    else {
        throw ProcessError("Invalid row or column");
    }
}


int
MFXTable::getSelStartRow() {
    return -1;
    // CHECK
}

void 
MFXTable::clearTable() {
    // clear rows (always before columns)
    for (const auto& row : myRows) {
        delete row;
    }
    // clear columns
    for (const auto& column : myColumns) {
        delete column;
    }
    // drop rows and columns
    myRows.clear();
    myColumns.clear();
}


MFXTable::Column::Column(MFXTable* table, const int index) :
    myTable(table),
    myIndex(index) {
    // create vertical frame
    myVerticalFrame = new FXVerticalFrame(table, GUIDesignAuxiliarMFXTable);
    myVerticalFrame->create();
    // create label for column
    myLabel = new FXLabel(myVerticalFrame, "", nullptr, GUIDesignLabelMFXTable);
    myLabel->create();
}


MFXTable::Column::~Column() {
    // destroy frame and label
    myVerticalFrame->destroy();
    myLabel->destroy();
    // delete vertical frame (this also delete Label and Row textFields)
    delete myVerticalFrame;
}


FXVerticalFrame*
MFXTable::Column::getVerticalFrame() const {
    return myVerticalFrame;
}


void
MFXTable::Column::setColumnLabel(const FXString& text) {
    myLabel->setText(text);
    // adjust column width
    adjustColumnWidth();
}


void 
MFXTable::Column::adjustColumnWidth() {
    // declare width using label
    int width = myLabel->getFont()->getTextWidth(myLabel->getText().text(), myLabel->getText().length() + EXTRAMARGING);
    // iterate over all textFields and check widths
    for (const auto & row : myTable->myRows) {
        // get text field
        const auto textField = row->getTextField(myIndex);
        // get textField width
        const auto textFieldWidth = textField->getFont()->getTextWidth(textField->getText().text(), textField->getText().length() + EXTRAMARGING);
        // compare widths
        if (textFieldWidth > width) {
            width = textFieldWidth;
        }
    }
    // set width in all elements
    myLabel->setWidth(width);
    for (const auto & row : myTable->myRows) {
        row->getTextField(myIndex)->setWidth(width);
    }
}


MFXTable::Column::Column() :
    myIndex(0) {}


MFXTable::Row::Row(MFXTable* table) :
    myTable(table) {
    // build textFields
    for (int i = 0; i < table->myColumns.size(); i++) {
        auto textField = new FXTextField(table->myColumns.at(i)->getVerticalFrame(), GUIDesignTextFieldNCol, table->myTarget, table->mySelector, GUIDesignTextFieldMFXTable);
        textField->create();
        myTextFields.push_back(textField);
    }
}


MFXTable::Row::~Row() {
    // destroy all textFields
    for (const auto& textField : myTextFields) {
        textField->destroy();
    }
}


FXString 
MFXTable::Row::getText(int index) const {
    return myTextFields.at(index)->getText();
}


void 
MFXTable::Row::setText(int index, const FXString& text, FXbool notify) const {
    myTextFields.at(index)->setText(text, notify);
    // adjust column width
    myTable->myColumns.at(index)->adjustColumnWidth();
}


FXTextField* 
MFXTable::Row::getTextField(int index) const {
    return myTextFields.at(index);
}


void MFXTable::Row::select() {
    // finish
}


MFXTable::Row::Row() {}

/****************************************************************************/