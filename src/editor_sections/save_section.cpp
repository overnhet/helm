/* Copyright 2013-2015 Matt Tytel
 *
 * helm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * helm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with helm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "save_section.h"
#include "helm_common.h"
#include "load_save.h"
#include "synth_gui_interface.h"
#include "text_look_and_feel.h"

#define SAVE_WIDTH 400
#define SAVE_HEIGHT 300
#define TEXT_EDITOR_HEIGHT 24
#define BUTTON_HEIGHT 30
#define DIVISION 100
#define PADDING_X 25
#define PADDING_Y 15

SaveSection::SaveSection(String name) : Component(name) {
  static Font patch_font(Typeface::createSystemTypefaceFor(BinaryData::DroidSansMono_ttf,
                                                           BinaryData::DroidSansMono_ttfSize));

  listener_ = nullptr;
  folders_model_ = new FileListBoxModel();
  Array<File> folder_locations;
  File bank_dir = LoadSave::getUserBankDirectory();
  folder_locations.add(bank_dir);
  folders_model_->rescanFiles(folder_locations);

  folders_view_ = new ListBox("folders", folders_model_);
  folders_view_->updateContent();
  folders_view_->setColour(ListBox::backgroundColourId, Colour(0xff323232));
  addAndMakeVisible(folders_view_);

  patch_name_ = new TextEditor("Patch Name");
  patch_name_->addListener(this);
  patch_name_->setTextToShowWhenEmpty(TRANS("Patch Name"), Colour(0xff777777));
  patch_name_->setFont(patch_font.withPointHeight(16.0f));
  patch_name_->setColour(CaretComponent::caretColourId, Colour(0xff888888));
  patch_name_->setColour(TextEditor::textColourId, Colour(0xff03a9f4));
  patch_name_->setColour(TextEditor::highlightedTextColourId, Colour(0xff03a9f4));
  patch_name_->setColour(TextEditor::highlightColourId, Colour(0xff888888));
  patch_name_->setColour(TextEditor::backgroundColourId, Colour(0xff323232));
  patch_name_->setColour(TextEditor::outlineColourId, Colour(0xff888888));
  patch_name_->setColour(TextEditor::focusedOutlineColourId, Colour(0xff888888));
  addAndMakeVisible(patch_name_);

  author_ = new TextEditor("Author");
  author_->addListener(this);
  author_->setTextToShowWhenEmpty(TRANS("Author"), Colour(0xff777777));
  author_->setFont(patch_font.withPointHeight(16.0f));
  author_->setColour(CaretComponent::caretColourId, Colour(0xff888888));
  author_->setColour(TextEditor::textColourId, Colour(0xffcccccc));
  author_->setColour(TextEditor::highlightedTextColourId, Colour(0xffcccccc));
  author_->setColour(TextEditor::highlightColourId, Colour(0xff888888));
  author_->setColour(TextEditor::backgroundColourId, Colour(0xff323232));
  author_->setColour(TextEditor::outlineColourId, Colour(0xff888888));
  author_->setColour(TextEditor::focusedOutlineColourId, Colour(0xff888888));
  addAndMakeVisible(author_);

  save_button_ = new TextButton(TRANS("Save"));
  save_button_->addListener(this);
  addAndMakeVisible(save_button_);

  cancel_button_ = new TextButton(TRANS("Cancel"));
  cancel_button_->addListener(this);
  addAndMakeVisible(cancel_button_);
}

void SaveSection::paint(Graphics& g) {
  static Font roboto_reg(Typeface::createSystemTypefaceFor(BinaryData::RobotoRegular_ttf,
                                                           BinaryData::RobotoRegular_ttfSize));
  static Font roboto_light(Typeface::createSystemTypefaceFor(BinaryData::RobotoLight_ttf,
                                                             BinaryData::RobotoLight_ttfSize));
  static const DropShadow shadow(Colour(0xff000000), 5, Point<int>(0, 0));

  g.setColour(Colour(0xbb111111));
  g.fillAll();

  Rectangle<int> save_rect = getSaveRect();
  shadow.drawForRectangle(g, save_rect);
  g.setColour(Colour(0xff212121));
  g.fillRect(save_rect);

  g.saveState();
  g.setOrigin(save_rect.getX() + PADDING_X, save_rect.getY() + PADDING_Y);

  g.setFont(roboto_light.withPointHeight(14.0f));
  g.setColour(Colour(0xff888888));

  g.drawText(TRANS("PATCH NAME"),
             0, PADDING_Y, DIVISION - 10, TEXT_EDITOR_HEIGHT,
             Justification::centredRight, false);
  g.drawText(TRANS("AUTHOR"),
             0, 2 * PADDING_Y + TEXT_EDITOR_HEIGHT, DIVISION - 10, TEXT_EDITOR_HEIGHT,
             Justification::centredRight, false);
  g.drawText(TRANS("FOLDER"),
             0, 3 * PADDING_Y + 2 * TEXT_EDITOR_HEIGHT, DIVISION - 10, TEXT_EDITOR_HEIGHT,
             Justification::centredRight, false);
  
  g.restoreState();
}

void SaveSection::resized() {
  Rectangle<int> save_rect = getSaveRect();
  patch_name_->setBounds(save_rect.getX() + PADDING_X + DIVISION,
                         save_rect.getY() + 2 * PADDING_Y,
                         save_rect.getWidth() - 2 * PADDING_X - DIVISION,
                         TEXT_EDITOR_HEIGHT);
  author_->setBounds(save_rect.getX() + PADDING_X + DIVISION,
                     save_rect.getY() + 3 * PADDING_Y + TEXT_EDITOR_HEIGHT,
                     save_rect.getWidth() - 2 * PADDING_X - DIVISION,
                     TEXT_EDITOR_HEIGHT);

  float button_width = (save_rect.getWidth() - 3 * PADDING_X) / 2.0f;
  save_button_->setBounds(save_rect.getX() + PADDING_X,
                          save_rect.getBottom() - PADDING_Y - BUTTON_HEIGHT,
                          button_width, BUTTON_HEIGHT);
  cancel_button_->setBounds(save_rect.getX() + button_width + 2 * PADDING_X,
                            save_rect.getBottom() - PADDING_Y - BUTTON_HEIGHT,
                            button_width, BUTTON_HEIGHT);

  float folder_y = save_rect.getY() + 4 * PADDING_Y + 2 * TEXT_EDITOR_HEIGHT;
  folders_view_->setBounds(save_rect.getX() + PADDING_X + DIVISION, folder_y,
                           save_rect.getWidth() - 2 * PADDING_X - DIVISION,
                           save_button_->getY() - folder_y - PADDING_Y);
}

void SaveSection::visibilityChanged() {
  if (isVisible()) {
    SparseSet<int> selected_rows = folders_view_->getSelectedRows();
    if (selected_rows.size() == 0)
      folders_view_->selectRow(0);
  }
}

void SaveSection::mouseUp(const MouseEvent &e) {
  if (!getSaveRect().contains(e.getPosition()))
    setVisible(false);
}

void SaveSection::textEditorReturnKeyPressed(TextEditor& editor) {
  save();
}

void SaveSection::buttonClicked(Button* clicked_button) {
  if (clicked_button == save_button_)
    save();
  else if (clicked_button == cancel_button_)
    setVisible(false);
}

Rectangle<int> SaveSection::getSaveRect() {
  if (active_rect_.getWidth() > 0.0f)
    return active_rect_;
  
  int x = (getWidth() - SAVE_WIDTH) / 2;
  int y = (getHeight() - SAVE_HEIGHT) / 2;
  return Rectangle<int>(x, y, SAVE_WIDTH, SAVE_HEIGHT);
}

void SaveSection::save() {
  SynthGuiInterface* parent = findParentComponentOfClass<SynthGuiInterface>();
  SparseSet<int> selected_rows = folders_view_->getSelectedRows();
  if (selected_rows.size() == 0)
    return;

  File folder = folders_model_->getFileAtRow(selected_rows[0]);
  String patch_name = patch_name_->getText();
  if (patch_name.length() == 0)
    return;

  File save_file = folder.getChildFile(patch_name);
  if (save_file.getFileExtension() != mopo::PATCH_EXTENSION)
    save_file = save_file.withFileExtension(String(mopo::PATCH_EXTENSION));
  save_file.replaceWithText(JSON::toString(parent->saveToVar()));

  patch_name_->clear();
  setVisible(false);
  if (listener_)
    listener_->fileSaved(save_file);
}
