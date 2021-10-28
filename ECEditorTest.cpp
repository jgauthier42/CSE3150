// Test code for editor
#include "ECTextViewImp.h"
#include "ECEditor.h"
#include "ECObserver.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace  std;

int myCounter = 0;

int main(int argc, char *argv[])
{
    ECTextViewImp wndTest;
    //wndTest.AddStatusRow("left", "right", false);

    if(argc < 2){
        ECEditor editor(&wndTest);
    }
    //wndTest.AddRow("CSE 3150");
    //wndTest.AddRow("This is a very simple demo of the ECTextViewImp functionalities.");
    //wndTest.AddRow("Press ctrl-q to quit");    
    else{
        ECEditor editor(&wndTest, argv[1]);
    }
    //wndTest.Show();
    
    
    return 0;
}
