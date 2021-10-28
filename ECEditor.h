#include "ECObserver.h"
#include "ECTextViewImp.h"
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>


using namespace std;


class ECEditor : public ECObserver
{
public:
    ECEditor(ECTextViewImp *wnd, string fileName);
    ECEditor(ECTextViewImp *wnd);
    void Update();
    void HandleEvent();
    void ArrowHandler();
    void EnterHandler();
    void BackspaceHandler();
    void UndoRedoHandler();
    void InsertionHandler();
    void UpdateUndoStack(int key=9999, int cx=9999, int cy=9999, int keyDeleted=9999);
    void UpdateRedoStack(vector<int> vals);
    void AddRows(vector<string> rowVec);
    void WriteToFile();
    void ChangeCursorX(int x);
    void ChangeCursorY(int y);
    int GetEditorCursorX(){return cursorX;}
    int GetEditorCursorY(){return cursorY;}
    int GetPageNum(){ return pageNum;}
    void DecrementPageNum() {pageNum --;}
    void IncrementPageNum() {pageNum ++;}

private:
    ECTextViewImp *window;
    vector<string> rows;
    //ViewHandler viewHandler;
    stack<vector<int> > undoStack;
    stack<vector<int> > redoStack;
    fstream textFile;
    string fileName;
    int cursorX;
    int cursorY;
    int key;
    int pageNum=0;
};


class ViewHandler
{
public:
    ViewHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings);
    ViewHandler();
    ~ViewHandler() {};
    void ArrowHandler(int key);
    void LeftHandler();
    void RightHandler();
    void UpHandler();
    void DownHandler();
    vector<string> FitToView(bool UpdateCursor=false);
    void UpdateCursorView();
    int GetRowsCX(){return this->rowsCX;}
    int GetRowsCY(){return this->rowsCY;}

private:
    ECEditor *Editor;
    vector<string> paragraphs;
    vector<string> rowsFitToView;
    ECTextViewImp *window;
    int numCols;
    int numRows;
    int EditorCX;
    int EditorCY;
    int WndCX;
    int WndCY;
    int rowsCX;
    int rowsCY;
};
/*
class LeftHandler
{
public:
    LeftHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings);

private:
    ECEditor *Editor;
    vector<string> paragraphs;
    vector<string> rowsFitToView;
    ECTextViewImp *window;
    int numCols;
    int numRows;
    int EditorCX;
    int EditorCY;
    int WndCX;
    int WndCY;
};

class RightHandler
{
public:
    RightHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings);

private:
    ECEditor *Editor;
    vector<string> paragraphs;
    vector<string> rowsFitToView;
    ECTextViewImp *window;
    int numCols;
    int numRows;
    int EditorCX;
    int EditorCY;
    int WndCX;
    int WndCY;
};

class UpHandler
{
public:
    UpHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings);

private:
    ECEditor *Editor;
    vector<string> paragraphs;
    vector<string> rowsFitToView;
    ECTextViewImp *window;
    int numCols;
    int numRows;
    int EditorCX;
    int EditorCY;
    int WndCX;
    int WndCY;
};

class DownHandler
{
public:
    DownHandler(ECEditor *Editor, ECTextViewImp *wnd, vector<string> paragraphStrings);

private:
    ECEditor *Editor;
    vector<string> paragraphs;
    vector<string> rowsFitToView;
    ECTextViewImp *window;
    int numCols;
    int numRows;
    int EditorCX;
    int EditorCY;
    int WndCX;
    int WndCY;
};
*/

/*
class KeyEventHandler : public ECEditor
{
public:
    KeyEventHandler(ECTextViewImp *wnd, int cursorX, int cursorY, int key);
    void HandleEvent();
    void ArrowHandler();
    void EnterHandler();
    void BackspaceHandler();
    void UndoRedoHandler();
    void InsertionHandler();
    
private:
    ECTextViewImp *window;
    int cursorX;
    int cursorY;
    int key;
};
*/