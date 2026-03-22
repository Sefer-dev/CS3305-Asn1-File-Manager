/*
 * Author: Melvin Roger
 * Description: Implementation file for FileManagerApp class, main application window creation 
 * Date: 2026-01-29
 */
#include "FileManagerApp.h"
#include "FileManagerFrame.h"

/*
 * This macro tells wxWidgets to use FileManagerApp as the main application class.
 * It creates the main() function 
 */
wxIMPLEMENT_APP(FileManagerApp);

/**
 * OnInit initializes the application
 * Creates and displays the main application window (the FileManagerFrame).
 */
bool FileManagerApp::OnInit()
{
    // Create the main application window, letting OS choose position at size of 800x600
    FileManagerFrame* frame = new FileManagerFrame("File Manager", wxDefaultPosition, wxSize(800, 600));

    // Show the window
    frame->Show(true);

    // Return true to indicate successful initialization
    return true;
}
