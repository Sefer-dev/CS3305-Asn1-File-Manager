/*
 * Author: Melvin Roger
 * Description: Implementation file for FileManagerFrame header, delcares the constructor and all functions used in FileManagerFrame class
 * Date: 2026-01-29
 */
#ifndef FILEMANAGER_FRAME_H
#define FILEMANAGER_FRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <filesystem>
#include <string>


class FileManagerFrame : public wxFrame
{
public:
    //Constructor
    FileManagerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // Destructor
    ~FileManagerFrame();

private:
    wxTextCtrl* pathTextCtrl;      // Text box showing current directory path
    wxListCtrl* fileListCtrl;      // List control displaying files and folders
    wxStatusBar* statusBar;        // Status bar at bottom of window

    std::filesystem::path currentPath;     // Current directory being displayed
    std::filesystem::path clipboardPath;   // Path of file marked for copy/cut
    bool isClipboardCut;                   // true if clipboard operation is cut, false if copy
    bool hasClipboardContent;              // true if clipboard has a file marked

    //Event IDs
    enum
    {
        ID_New = wxID_HIGHEST + 1,  // Start after wxWidgets' reserved IDs
        ID_Open,
        ID_Rename,
        ID_Delete,
        ID_Copy,
        ID_Cut,
        ID_Paste,
        ID_Refresh,
        ID_PathText
    };


    /**
     * Creates and configures the application menu bar
     * Sets up File and Edit menus with keyboard shortcuts
     */
    void CreateMenuBar();

    /**
    * Arranges all UI components in the window
    * Uses sizers for automatic layout management
    */
    void SetupLayout();

    /**
     * Creates and configures the file listing control
     * Sets up columns for Name, Type, Size, and Date
     */
    void CreateFileList();


    /**
     * Reads and displays contents of current directory
     * Populates the file list with files and folders
     */
    void LoadDirectory(const std::filesystem::path& path);

    /**
     * Updates the path text box
     * Shows the current directory path
     */
    void UpdatePathDisplay();

   
   //Converts file size in bytes to readable string
    wxString FormatFileSize(uintmax_t bytes);


    //Converts filesystem time to readable string
    wxString FormatDateTime(std::filesystem::file_time_type ftime);


    // Gets the full path of currently selected item
    std::filesystem::path GetSelectedFilePath();


    //Creates a new directory
    void OnNew(wxCommandEvent& event);

    
    //Opens selected file with default application, or navigates to directory
    void OnOpen(wxCommandEvent& event);

 
    //Renames the selected file or directory
    void OnRename(wxCommandEvent& event);


    //Deletes the selected file or directory after confirmation
    void OnDelete(wxCommandEvent& event);


    //Marks selected file for copying
    void OnCopy(wxCommandEvent& event);


    //Marks selected file for moving
    void OnCut(wxCommandEvent& event);


     //Completes copy or cut operation
    void OnPaste(wxCommandEvent& event);


    //Reloads the current directory listing
    void OnRefresh(wxCommandEvent& event);


    //Closes the application
    void OnExit(wxCommandEvent& event);


    //Opens files or navigates to directories
    void OnItemActivated(wxListEvent& event);


    //Navigates to the typed directory path
    void OnPathEnter(wxCommandEvent& event);

    //Event Table Declaration
    // This macro tells wxWidgets that this class handles events
    wxDECLARE_EVENT_TABLE();
};

#endif 
