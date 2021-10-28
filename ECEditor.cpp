#include "ECObserver.h"
#include "ECTextViewImp.h"
#include "ECEditor.h"
#include <string>
#include <vector>

using namespace std;

ECEditor :: ECEditor(ECTextViewImp *wnd, string fileName){
  string line;
  ifstream textFile;
  this->fileName = fileName;
  textFile.open(fileName);
  if (textFile.is_open()){
    while (getline(textFile,line))
    {
      rows.push_back(line);
    }
    textFile.close();
  }
  this->window = wnd;
  this->window->Attach(this);
  ViewHandler viewHandler(this, wnd, rows);
  //this->viewHandler = ViewHandler(this, wnd, rows);
  this->cursorX = 0;
  this->cursorY = 0;
  AddRows(viewHandler.FitToView());
  this->window->Show();
}

ECEditor :: ECEditor(ECTextViewImp *wnd){
  this->fileName = "default.txt";
  this->window = wnd;
  this->window->Attach(this);
  ViewHandler viewHandler(this, wnd, rows);
  //this->viewHandler = ViewHandler(this, wnd, rows);
  this->cursorX = 0;
  this->cursorY = 0;
  AddRows(viewHandler.FitToView());
  this->window->Show();
}

void ECEditor :: AddRows(vector<string> rowVec){
  this->window->ClearStatusRows();
  string cx = to_string(cursorX);
  string cy = to_string(cursorY);
  string wndcx = to_string(window->GetCursorX());
  string wndcy = to_string(window->GetCursorY());
  this->window->AddStatusRow("ECX,ECY,PageNum,PgSze=" + cx + " " + cy + " " + to_string(GetPageNum()) + " " + to_string(rows.size()), "WndCX,WndCY,numRows,numCols,rwsSz=" + wndcx + " " + wndcy + " " + to_string(this->window->GetRowNumInView()) + " " + to_string(this->window->GetColNumInView()) + " " + to_string(rowVec.size()), false);
  for(int i=pageNum*(window->GetRowNumInView()); i<rowVec.size() && i<(window->GetRowNumInView()*(GetPageNum()+1)); i++){
      this->window->AddRow(rowVec[i]);
    }
}

void ECEditor :: Update(){
    this->key = window->GetPressedKey();
    //this->cursorX = window->GetCursorX();
    //this->cursorY = window->GetCursorY();
    try{
      HandleEvent();
    }
    catch(const std::out_of_range& oor){

    }
    WriteToFile();
    this->window->Refresh();
    //KeyEventHandler handler = KeyEventHandler(window, cursorX, cursorY, key);
    //handler.HandleEvent();
}

void ECEditor :: HandleEvent(){
    if(key > 999 && key < 1004){
        ArrowHandler();
    }
    else if(key == 13){
      EnterHandler();
      ViewHandler viewHandler(this, window, rows);
      this->window->InitRows();
      AddRows(viewHandler.FitToView(true));
    }
    else if(key == 127){
      BackspaceHandler();
      ViewHandler viewHandler(this, window, rows);
      this->window->InitRows();
      AddRows(viewHandler.FitToView(true));
    }
    else if(key == 25 || key == 26){
        UndoRedoHandler();
        ViewHandler viewHandler(this, window, rows);
        this->window->InitRows();
        AddRows(viewHandler.FitToView(true));
    }
    else if(key > 31 && key < 127){
      InsertionHandler();
      ViewHandler viewHandler(this, window, rows);
      this->window->InitRows();
      AddRows(viewHandler.FitToView(true));
    }
    //ViewHandler viewHandler(this, window, rows);
    //rows = viewHandler.FitToView();
}
void ECEditor :: ArrowHandler(){
  if(rows.size()>0){
    ViewHandler viewHandler(this, window, rows);
    viewHandler.ArrowHandler(key);
    this->window->InitRows();
    AddRows(viewHandler.FitToView());
  }
}

void ECEditor :: EnterHandler(){
  if(rows.size() == 0){
    rows.push_back("");
    rows.push_back("");
    cursorY = 0;
  }
  else if(cursorX == rows[cursorY].size()){
    rows.insert(rows.begin()+cursorY+1, "");
    rows[cursorY+1].erase();
  }
  else{
    string strL = rows[cursorY].substr(0,cursorX);
    string strR = rows[cursorY].substr(cursorX);
    rows.insert(rows.begin()+cursorY+1, strR);
    rows[cursorY] = strL;
  }
  //window->SetCursorX(0);
  cursorX = 0;
  //window->SetCursorY(cursorY+1);
  cursorY ++;
  UpdateUndoStack(key, 0, cursorY);
}

void ECEditor :: BackspaceHandler(){
  if(cursorX == 0){
    //UpdateUndoStack();
    if(cursorY != 0){
      int tempX = rows[cursorY-1].size();
      for(int i=0; i<rows[cursorY].size(); i++){
        rows[cursorY-1].push_back(rows[cursorY][i]);
      }
      if(rows[cursorY].size() == 0){
        rows.erase(rows.begin()+cursorY);
        UpdateUndoStack(key, rows[cursorY-1].size()-rows[cursorY].size(), cursorY-1);
        cursorY -= 1;
        cursorX = rows[cursorY].size();
        return;
      }
      UpdateUndoStack(key, rows[cursorY-1].size()-rows[cursorY].size(), cursorY-1);
      //window->SetCursorY(cursorY-1);
      cursorY -= 1;
      //window->SetCursorX(rows[cursorY-1].size()-rows[cursorY].size());
      cursorX = tempX;
      rows.erase(rows.begin()+cursorY+1);
    }
    //else if(key == 26 || key == 127){
     // UpdateUndoStack(key, 0, cursorY);
    //}
  }
  else{
    UpdateUndoStack(key, cursorX-1, cursorY, int(rows[cursorY][cursorX-1]));
    string strL = rows[cursorY].substr(0,cursorX-1);
    string strR = rows[cursorY].substr(cursorX);
    for(int i=0; i<strR.size(); i++){
        strL.push_back(strR[i]);
      }
    rows[cursorY] = strL;
    //window->SetCursorX(cursorX-1);
    cursorX -= 1;
    //UpdateUndoStack(key, cursorX-1, cursorY, int(rows[cursorY][cursorX]));
  }
}

void ECEditor :: UndoRedoHandler(){
  //Undo
  if(key == 26){
    if(!undoStack.empty()){
      vector<int> vec = undoStack.top();
      undoStack.pop();
      int oldKey = vec[0];
      cursorX = vec[1];
      cursorY = vec[2];
      //Enter
      if(oldKey == 13){
        BackspaceHandler();
      }
      //Backspace
      else if(oldKey == 127){
        if(cursorX==0){
          EnterHandler();
        }
        else{
          key = vec[3];
          InsertionHandler();
        }
      }
      //Insertion
      else{
        BackspaceHandler();
      }
      //undoStack.pop();
      redoStack.push(vec);
    }
  }
  //Redo
  else if(key == 25){
    if(!redoStack.empty()){
      vector<int> vec = redoStack.top();
      redoStack.pop();
      int oldKey = vec[0];
      cursorX = vec[1];
      cursorY = vec[2];
      //Enter
      if(oldKey == 13){
        BackspaceHandler();
      }
      //Backspace
      else if(oldKey == 127){
        if(cursorX==0){
          EnterHandler();
        }
        else{
          key = vec[3];
          InsertionHandler();
        }
      }
      //Insertion
      else{
        BackspaceHandler();
      }
      //undoStack.pop();
      undoStack.push(vec);
    }
  }
}

void ECEditor :: InsertionHandler(){
  if(rows.size() == 0){
    string str;
    str.push_back(char(key));
    rows.push_back(str);
  }
  else{
    if(cursorX == rows[cursorY].size()){
      string str = rows[cursorY];
      str.push_back(char(key));
      rows[cursorY] = str;
    }
    else{
      string strL = rows[cursorY].substr(0,cursorX);
      string strR = rows[cursorY].substr(cursorX);
      strL.push_back(char(key));
      for(int i=0; i<strR.size(); i++){
        strL.push_back(strR[i]);
      }
      rows[cursorY] = strL;
    }
  }
  UpdateUndoStack(key, cursorX+1);
  //window->SetCursorX(cursorX + 1);
  cursorX += 1;
}

void ECEditor :: UpdateUndoStack(int key, int cx, int cy, int keyDeleted){
    int Key, CX, CY;
    if(key==9999){
      Key = key;}
    else{
      Key = this->key;}
    if(cx!=9999){
      CX = cx;}
    else{
      CX = this->cursorX;}
    if(cy!=9999){
      CY = cy;}
    else{
      CY = this->cursorY;}
    if(Key != 26 && Key != 27){
      vector<int> vals;
      vals.push_back(Key);
      vals.push_back(CX);
      vals.push_back(CY);
      if(keyDeleted!=9999){
        vals.push_back(keyDeleted);
      }
      undoStack.push(vals);
    }
}

void ECEditor :: UpdateRedoStack(vector<int> vals){
  redoStack.push(vals);
}

void ECEditor :: WriteToFile(){
  ofstream myfile;
  myfile.open(this->fileName);
  for(int i=0; i<rows.size(); i++){
    myfile << rows[i] << endl;
  }
  myfile.close();
}

void ECEditor :: ChangeCursorX(int x){
  cursorX = x;
}

void ECEditor :: ChangeCursorY(int y){
  cursorY = y;
}


ViewHandler :: ViewHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings){
  this->Editor = Editor;
  this->window = wnd;
  this->paragraphs = paragraphStrings;
  this->numCols = window->GetColNumInView();
  this->numRows = window->GetRowNumInView();
  this->EditorCX = Editor->GetEditorCursorX();
  this->EditorCY = Editor->GetEditorCursorY();
  this->rowsFitToView = FitToView(true);
  //this->WndCX = window->GetCursorX();
  //this->WndCY = window->GetCursorY();
}

void ViewHandler :: ArrowHandler(int key){
if(paragraphs.size() > 0){
    //Left Arrow
    if(key == 1000){
      LeftHandler();
    }
    //Right Arrow
    else if(key == 1001){
      RightHandler();
    }

    //Up Arrow
    else if(key == 1002){
      UpHandler();
    }
  
    //Down Arrow
    else if(key == 1003){
      DownHandler();
    }
  }
}

void ViewHandler :: LeftHandler(){
  if(EditorCX > 0){ //don't change EditorCY, staying in same paragraph
    Editor->ChangeCursorX(EditorCX-1);
    if(WndCX > 0){  //Just moving cursor left in Wnd line
      UpdateCursorView();
    }
    else if (WndCY > 0){ //Moving cursor to end of previos Window Row
      UpdateCursorView();
    }
    else{  //move to previous page if applicable
      if(Editor->GetPageNum() > 0){
        Editor->DecrementPageNum();
        window->SetCursorX(0);
        window->SetCursorY(0);
        window->ClearStatusRows();
        Editor->AddRows(rowsFitToView);
        UpdateCursorView();
      }
    }
  }
  else{
    if(EditorCY > 0){
      Editor->ChangeCursorY(EditorCY-1);
      Editor->ChangeCursorX(paragraphs[EditorCY-1].size());
      if (WndCY == 0){  //move to previous page if applicable
        if(Editor->GetPageNum() > 0){
          Editor->DecrementPageNum();
          window->SetCursorX(0);
          window->SetCursorY(0);
          Editor->AddRows(rowsFitToView);
        }
      }
      UpdateCursorView();
    }
  }
}

void ViewHandler :: RightHandler(){
  if(EditorCX < paragraphs[EditorCY].size()){ //Stay in current paragraph
    Editor->ChangeCursorX(EditorCX+1);
    if(WndCX < rowsFitToView[rowsCY].size()){  //Stay in current view line
      UpdateCursorView();
    }
    else if(WndCY < numRows-1){ //Stay on this page, move to the beginning of next view line
      UpdateCursorView();
    }
    else{ //Move to first position of next page 
      Editor->IncrementPageNum();
      window->SetCursorX(0);
      window->SetCursorY(0);
      window->ClearStatusRows();
      Editor->AddRows(rowsFitToView);
      UpdateCursorView();
    }
  }
  else{ //Move to next paragraph if applicable
    if(EditorCY < paragraphs.size()-1){ //not at the last paragraph, move to next paragraph
      Editor->ChangeCursorX(0);
      Editor->ChangeCursorY(EditorCY + 1);
      if(WndCY < numRows-1){ //Stay on this page, move to beginning of next view line
        UpdateCursorView();
      }
      else{ //At the last line in view, must move to next page since we're not in the last paragraph (there must be more on next page)
        Editor->IncrementPageNum();
        window->SetCursorX(0);
        window->SetCursorY(0);
        window->ClearStatusRows();
        Editor->AddRows(rowsFitToView);
        UpdateCursorView();
      }
    }
  }
}

void ViewHandler :: UpHandler(){
  if(paragraphs.size() > 0){
    if(Editor->GetPageNum() > 0 || WndCY > 0){
      if(EditorCX == WndCX){
        if(rowsFitToView[rowsCY-1].size() <= WndCX){
          Editor->ChangeCursorX(paragraphs[EditorCY-1].size());
        }
        else if(rowsFitToView[rowsCY-1].size() > WndCX){
          Editor->ChangeCursorX(paragraphs[EditorCY-1].size() - (rowsFitToView[rowsCY-1].size() - WndCX));
        }
        Editor->ChangeCursorY(EditorCY-1);
      }
      else if (EditorCX > WndCX){
        if(rowsFitToView[rowsCY-1].size() <= WndCX){
          Editor->ChangeCursorX(EditorCX - WndCX);
        }
        else if(rowsFitToView[rowsCY-1].size() > WndCX){
          Editor->ChangeCursorX(EditorCX - (WndCX + (rowsFitToView[rowsCY-1].size() - WndCX)));
        }
      }
      //UpdateCursorView();
    }
    if(Editor->GetPageNum() > 0 && WndCY == 0){
      Editor->DecrementPageNum();
      window->SetCursorX(0);
      window->SetCursorY(0);
      Editor->AddRows(rowsFitToView);
      //UpdateCursorView();
    }
    UpdateCursorView();
  }
}

void ViewHandler :: DownHandler(){
  int x=0;
  if(Editor->GetPageNum() > 0){
    x += 1;
  }
  if((WndCY + (((numRows-1) * Editor->GetPageNum())+x) < rowsFitToView.size()-1) && paragraphs.size()>0){ //if not at last view row
    if(EditorCX + (rowsFitToView[rowsCY].size() - WndCX) >= paragraphs[EditorCY].size()){ //Move to next paragraph
      if(rowsFitToView[rowsCY+1].size() <= WndCX){
        Editor->ChangeCursorX(paragraphs[EditorCY+1].size());
      }
      else if(rowsFitToView[rowsCY+1].size() > WndCX){
        Editor->ChangeCursorX(WndCX);
      }
      Editor->ChangeCursorY(EditorCY+1);
    }
    else if ((EditorCX + (rowsFitToView[rowsCY].size() - WndCX)) < paragraphs[EditorCY].size()){ //stay in this paragraph
      if(rowsFitToView[rowsCY+1].size() <= WndCX){
        Editor->ChangeCursorX(EditorCX + rowsFitToView[rowsCY+1].size() + (rowsFitToView[rowsCY].size()-WndCX-1));
      }
      else if(rowsFitToView[rowsCY+1].size() > WndCX){
        Editor->ChangeCursorX(EditorCX + (WndCX + (rowsFitToView[rowsCY].size() - WndCX)));
      }
    }
    UpdateCursorView();
    if(window->GetCursorY() > numRows-1){
      Editor->IncrementPageNum();
      window->SetCursorX(0);
      window->SetCursorY(0);
      window->ClearStatusRows();
      Editor->AddRows(rowsFitToView);
      UpdateCursorView();
    }
  }
}

vector<string> ViewHandler :: FitToView(bool UpdateCursor){
  rowsFitToView.clear(); 
  for(int i = 0; i < paragraphs.size(); i++){
    if(paragraphs[i].size() < numCols){
      rowsFitToView.push_back(paragraphs[i]);
    }
    else{
      vector<string> words;
      int wordStart = 0;
      for(int j=0; j < paragraphs[i].size(); j++){
        if(paragraphs[i][j] == ' '){
          words.push_back(paragraphs[i].substr(wordStart, j-wordStart + 1));
          wordStart = j+1;
        }
        else if(j == paragraphs[i].size()-1){
          words.push_back(paragraphs[i].substr(wordStart, j-wordStart + 1));
        }
      }
      int i = 0;
      string str;
      while(i<words.size()){
        if(str.size() + words[i].size() < numCols){
          str.append(words[i]);
          if(i == words.size()-1){
            rowsFitToView.push_back(str);
            str.clear();
          }
          i += 1;
        }
        else{
          rowsFitToView.push_back(str);
          str.clear();
          str.append(words[i]);
          if(i == words.size()-1){
            rowsFitToView.push_back(str);
          }
          i += 1;
        }
      }
      //words.clear();
      //str.clear();
    }
  }
  if(UpdateCursor){
    UpdateCursorView();
  }
  return rowsFitToView;
}

void ViewHandler :: UpdateCursorView(){
  int totX = 0;
  if(Editor->GetEditorCursorY() > 0){
    for(int i=0; i<Editor->GetEditorCursorY(); i++){
      if(paragraphs[i].size() == 0){
        totX += 1;
      }
      else{
        totX += paragraphs[i].size()+1;
      }
    }
    totX += Editor->GetEditorCursorX();
  }
  else{
    totX += Editor->GetEditorCursorX();
  }

  int counter = 0;
  int paragraphNum = 0;
  for(int i=0; i<rowsFitToView.size(); i++){
    if(totX == rowsFitToView[i].size() && Editor->GetEditorCursorX() < paragraphs[Editor->GetEditorCursorY()].size()){
      totX = 0;
      i += 1;
      rowsCX = totX;
      rowsCY = i;
      WndCY = i-(Editor->GetPageNum() * (numRows-1));
      WndCX = totX;
      break;
    }
    if(rowsFitToView[i].size() < totX){
      totX -= rowsFitToView[i].size();
      counter += rowsFitToView[i].size();
      if(counter >= paragraphs[paragraphNum].size()){
      totX -= 1;
      counter = 0;
        if (paragraphNum < paragraphs.size()-1){
          paragraphNum += 1;
        }
      }
    }
    else{
      WndCX = totX;
      rowsCX = totX;

      WndCX = totX;
      rowsCX = totX;

      rowsCY = i;
      WndCY = i-(Editor->GetPageNum() * numRows);
      break;
    }
  }
  if(WndCY > numRows-1){
    Editor->IncrementPageNum();
    window->SetCursorX(0);
    window->SetCursorY(0);
    window->ClearStatusRows();
    Editor->AddRows(rowsFitToView);
    UpdateCursorView();
  }
  else if(WndCY < 0){
    if(Editor->GetPageNum() > 0){
      Editor->DecrementPageNum();
      window->SetCursorX(0);
      window->SetCursorY(0);
      Editor->AddRows(rowsFitToView);
      UpdateCursorView();
    }
  }
  else{
    window->SetCursorX(WndCX);
    window->SetCursorY(WndCY);
  }
}

