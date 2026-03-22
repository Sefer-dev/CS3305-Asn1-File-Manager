/*
 * Author: Melvin Roger
 * Description: Implementation file for FileManagerFrame class, main window functionality app with all required features
 * Date: 2026-01-29
 */


#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "FileManagerFrame.h"

using namespace std;

// This macro maps event IDs to handler methods
wxBEGIN_EVENT_TABLE(FileManagerFrame, wxFrame)

// Menu events
EVT_MENU(ID_New, FileManagerFrame::OnNew)
EVT_MENU(ID_Open, FileManagerFrame::OnOpen)
EVT_MENU(ID_Rename, FileManagerFrame::OnRename)
EVT_MENU(ID_Delete, FileManagerFrame::OnDelete)
EVT_MENU(ID_Copy, FileManagerFrame::OnCopy)
EVT_MENU(ID_Cut, FileManagerFrame::OnCut)
EVT_MENU(ID_Paste, FileManagerFrame::OnPaste)
EVT_MENU(ID_Refresh, FileManagerFrame::OnRefresh)
EVT_MENU(wxID_EXIT, FileManagerFrame::OnExit)

// List control events
EVT_LIST_ITEM_ACTIVATED(wxID_ANY, FileManagerFrame::OnItemActivated)

// Text control events
EVT_TEXT_ENTER(ID_PathText, FileManagerFrame::OnPathEnter)
wxEND_EVENT_TABLE()

/*
 * Function: FileManagerFrame (Constructor)
 * Description: Initializes the main window with menu bar, status bar, and file list
 * Parameters:
 *   title: Window title to display
 *   pos: Initial window position
 *   size: Initial window size
 * Return: N/A
 */
    FileManagerFrame::FileManagerFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size),
    pathTextCtrl(nullptr),
    fileListCtrl(nullptr),
    statusBar(nullptr),
    currentPath(filesystem::current_path()),
    clipboardPath(),
    isClipboardCut(false),
    hasClipboardContent(false)
{
    //creates the frame and loads the current directory
    CreateMenuBar();
    statusBar = CreateStatusBar();
    statusBar->SetStatusText("Welcome to File Manager!");
    SetupLayout();
    LoadDirectory(currentPath);
}


/*
 * Function: ~FileManagerFrame (Destructor)
 * Description: Cleans up resources when the frame is destroyed
 * Parameters: None
 * Return: N/A
 */
FileManagerFrame::~FileManagerFrame()
{
}

/*
 * Function: CreateMenuBar
 * Description: Creates and configures the application menu bar with File and Edit menus
 * Parameters: None
 * Return: void
 */
void FileManagerFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();

    fileMenu->Append(ID_New, "&New...\tCtrl-N", "Create a new directory");
    fileMenu->Append(ID_Open, "&Open...\tCtrl-O", "Open the selected file or directory");
    fileMenu->Append(ID_Rename, "&Rename...\tCtrl-E", "Rename the selected file or directory");
    fileMenu->Append(ID_Delete, "&Delete...\tDel", "Delete the selected file or directory");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-F4", "Exit the application");

    wxMenu* editMenu = new wxMenu();
    editMenu->Append(ID_Copy, "&Copy\tCtrl-C", "Mark file for copying");
    editMenu->Append(ID_Cut, "Cu&t\tCtrl-X", "Mark file for moving");
    editMenu->Append(ID_Paste, "&Paste\tCtrl-V", "Paste the marked file");
    editMenu->AppendSeparator();
    editMenu->Append(ID_Refresh, "&Refresh\tF5", "Refresh the directory listing");

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    SetMenuBar(menuBar);
}

/*
 * Function: SetupLayout
 * Description: Arranges UI components using sizers for automatic layout management
 * Parameters: None
 * Return: void
 */
void FileManagerFrame::SetupLayout()
{
    wxPanel* panel = new wxPanel(this);
    pathTextCtrl = new wxTextCtrl(panel, ID_PathText, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    CreateFileList();
    fileListCtrl->Reparent(panel);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(pathTextCtrl, 0, wxEXPAND | wxALL, 5);
    vbox->Add(fileListCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    panel->SetSizer(vbox);
}

/*
 * Function: CreateFileList
 * Description: Creates the file list control with columns for Name, Type, Size, and Date
 * Parameters: None
 * Return: void
 */
void FileManagerFrame::CreateFileList()
{
    fileListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    fileListCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 250);
    fileListCtrl->AppendColumn("Type", wxLIST_FORMAT_LEFT, 100);
    fileListCtrl->AppendColumn("Size", wxLIST_FORMAT_RIGHT, 100);
    fileListCtrl->AppendColumn("Date Modified", wxLIST_FORMAT_LEFT, 200);
}

/*
 * Function: LoadDirectory
 * Description: Reads and displays contents of the specified directory
 * Parameters:
 *   path: Directory path to load and display
 * Return: void
 */
void FileManagerFrame::LoadDirectory(const filesystem::path& path)
{
    try
    {
        if (!filesystem::exists(path))//checks if the given directory exists
        {
            wxMessageBox("Directory does not exist: " + path.string(), "Error", wxOK | wxICON_ERROR);
            return;
        }

        if (!filesystem::is_directory(path))//checks if the given path is a directory 
        {
            wxMessageBox("Path is not a directory: " + path.string(), "Error", wxOK | wxICON_ERROR);
            return;
        }

        currentPath = path;
        fileListCtrl->DeleteAllItems();
        UpdatePathDisplay();

        // Add parent directory if not at root
        if (currentPath.has_parent_path() && currentPath != currentPath.root_path())
        {
            long index = fileListCtrl->InsertItem(0, "..");
            fileListCtrl->SetItem(index, 1, "Dir");
            fileListCtrl->SetItem(index, 2, "");
            fileListCtrl->SetItem(index, 3, "");
        }

        // Collect directory entries
        vector<filesystem::directory_entry> entries;
        for (const auto& entry : filesystem::directory_iterator(currentPath))
        {
            entries.push_back(entry);
        }

        // sorts directories first, then files alphabetically
        sort(entries.begin(), entries.end(), [](const filesystem::directory_entry& a, const filesystem::directory_entry& b)
            {
                if (a.is_directory() != b.is_directory())
                    return a.is_directory();

                string nameA = a.path().filename().string();
                string nameB = b.path().filename().string();
                transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
                transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);
                return nameA < nameB;
            });

        // Add entries to list
        for (const auto& entry : entries)
        {
            wxString name = entry.path().filename().string();
            wxString type = entry.is_directory() ? wxString("Dir") : wxString("File");
            wxString size = entry.is_directory() ? wxString("") : FormatFileSize(entry.file_size());
            wxString date = FormatDateTime(entry.last_write_time());

            long index = fileListCtrl->InsertItem(fileListCtrl->GetItemCount(), name);
            fileListCtrl->SetItem(index, 1, type);
            fileListCtrl->SetItem(index, 2, size);
            fileListCtrl->SetItem(index, 3, date);
        }

        statusBar->SetStatusText("Loaded " + to_string(entries.size()) + " items");
    }
    catch (const filesystem::filesystem_error& e)
    {
        wxMessageBox("Error loading directory: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/*
 * Function: UpdatePathDisplay
 * Description: Updates the path text control with the current directory path
 * Parameters: None
 * Return: void
 */
void FileManagerFrame::UpdatePathDisplay()
{
    pathTextCtrl->SetValue(currentPath.string());
}

/*
 * Function: FormatFileSize
 * Description: Converts file size in bytes to human-readable format
 * Parameters:
 *   bytes: File size in bytes
 * Return: wxString - Formatted size string (e.g., "1.5 KB", "2.3 MB")
 */
wxString FileManagerFrame::FormatFileSize(uintmax_t bytes)
{
    const uintmax_t KB = 1024;
    const uintmax_t MB = KB * 1024;
    const uintmax_t GB = MB * 1024;

    ostringstream space;
    space << fixed << setprecision(1);

    if (bytes >= GB)
    {
        space << (static_cast<double>(bytes) / GB) << " GB";
    }
    else if (bytes >= MB)
    {
        space << (static_cast<double>(bytes) / MB) << " MB";
    }
    else if (bytes >= KB)
    {
        space << (static_cast<double>(bytes) / KB) << " KB";
    }
    else
    {
        space << bytes << " bytes";
    }

    return wxString(space.str());
}

/*
 * Function: FormatDateTime
 * Description: Converts filesystem time to readable date/time string
 * Parameters:
 *   ftime: Filesystem time point
 * Return: wxString - Formatted date/time string
 */
wxString FileManagerFrame::FormatDateTime(filesystem::file_time_type ftime)
{
    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(ftime - filesystem::file_time_type::clock::now() + chrono::system_clock::now());
    auto time_t_format = chrono::system_clock::to_time_t(sctp);
    tm* tm = localtime(&time_t_format);
    ostringstream time;
    time << put_time(tm, "%A, %B %d, %Y %H:%M:%S");
    return wxString(time.str());
}

/*
 * Function: GetSelectedFilePath
 * Description: Gets the full path of the currently selected list item
 * Parameters: None
 * Return: filesystem::path - Full path of selected file/folder, or empty path if nothing selected
 */
filesystem::path FileManagerFrame::GetSelectedFilePath()
{
    long selectedIndex = fileListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (selectedIndex == -1)
    {
        return filesystem::path();
    }

    wxString name = fileListCtrl->GetItemText(selectedIndex, 0);

    if (name == "..")
    {
        return currentPath.parent_path();
    }

    return currentPath / name.ToStdString();
}

/*
 * Function: OnNew
 * Description: creates a new directory
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnNew(wxCommandEvent& event)
{
    wxTextEntryDialog dialog(this, "Enter name for new directory:", "New Directory", "New Folder");

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString dirName = dialog.GetValue();//gets user iuput for new directory

        if (dirName.IsEmpty())
        {
            wxMessageBox("Directory name cannot be empty.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        try
        {
            filesystem::path newPath = currentPath / dirName.ToStdString();

            if (filesystem::exists(newPath))
            {
                wxMessageBox("A file or directory with that name already exists.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            filesystem::create_directory(newPath);
            statusBar->SetStatusText("Created directory: " + dirName);
            LoadDirectory(currentPath);
        }
        catch (const filesystem::filesystem_error& e)
        {
            wxMessageBox("Error creating directory: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }
}

/*
 * Function: OnOpen
 * Description: opens files or navigates to directories
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnOpen(wxCommandEvent& event)
{
    filesystem::path selectedPath = GetSelectedFilePath();

    if (selectedPath.empty())
    {
        wxMessageBox("Please select a file or directory to open.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    try
    {
        if (filesystem::is_directory(selectedPath))
        {
            LoadDirectory(selectedPath);
        }
        else
        {
            wxString pathStr(selectedPath.string());

            if (!wxLaunchDefaultApplication(pathStr))
            {
                wxMessageBox("Could not open file with default application.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
    catch (const filesystem::filesystem_error& e)
    {
        wxMessageBox("Error opening: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/*
 * Function: OnRename
 * Description: renames selected file or directory
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnRename(wxCommandEvent& event)
{
    filesystem::path selectedPath = GetSelectedFilePath();

    if (selectedPath.empty())
    {
        wxMessageBox("Please select a file or directory to rename.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    if (selectedPath == currentPath.parent_path())
    {
        wxMessageBox("Cannot rename parent directory (..).", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString currentName = selectedPath.filename().string();
    wxTextEntryDialog dialog(this, "Enter new name:", "Rename", currentName);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString newName = dialog.GetValue();

        if (newName.IsEmpty())
        {
            wxMessageBox("Name cannot be empty.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        try
        {
            filesystem::path newPath = currentPath / newName.ToStdString();

            if (filesystem::exists(newPath))
            {
                wxMessageBox("A file or directory with that name already exists.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            filesystem::rename(selectedPath, newPath);
            statusBar->SetStatusText("Renamed to: " + newName);
            LoadDirectory(currentPath);
        }
        catch (const filesystem::filesystem_error& e)
        {
            wxMessageBox("Error renaming: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }
}

/*
 * Function: OnDelete
 * Description: deletes selected file or directory
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnDelete(wxCommandEvent& event)
{
    filesystem::path selectedPath = GetSelectedFilePath();

    if (selectedPath.empty())
    {
        wxMessageBox("Please select a file or directory to delete.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    if (selectedPath == currentPath.parent_path())
    {
        wxMessageBox("Cannot delete parent directory (..).", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString fileName = selectedPath.filename().string();
    wxString message = "Are you sure you want to delete '" + fileName + "'?";

    if (filesystem::is_directory(selectedPath))
    {
        message += "\n\nThis directory and all its contents will be deleted!";
    }

    int result = wxMessageBox(message, "Confirm Delete", wxYES_NO | wxICON_QUESTION);

    if (result == wxYES)
    {
        try
        {
            if (filesystem::is_directory(selectedPath))
            {
                filesystem::remove_all(selectedPath);
            }
            else
            {
                filesystem::remove(selectedPath);
            }

            statusBar->SetStatusText("Deleted: " + fileName);
            LoadDirectory(currentPath);
        }
        catch (const filesystem::filesystem_error& e)
        {
            wxMessageBox("Error deleting: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
        }
    }
}

/*
 * Function: OnCopy
 * Description: marks file for copying
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnCopy(wxCommandEvent& event)
{
    filesystem::path selectedPath = GetSelectedFilePath();

    if (selectedPath.empty())
    {
        wxMessageBox("Please select a file or directory to copy.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    if (selectedPath == currentPath.parent_path())
    {
        wxMessageBox("Cannot copy parent directory (..).", "Error", wxOK | wxICON_ERROR);
        return;
    }

    clipboardPath = selectedPath;
    isClipboardCut = false;
    hasClipboardContent = true;

    wxString fileName = selectedPath.filename().string();
    statusBar->SetStatusText("Marked for copying: " + fileName);
}

/*
 * Function: OnCut
 * Description: marks file for moving
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnCut(wxCommandEvent& event)
{
    filesystem::path selectedPath = GetSelectedFilePath();

    if (selectedPath.empty())
    {
        wxMessageBox("Please select a file or directory to cut.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    if (selectedPath == currentPath.parent_path())
    {
        wxMessageBox("Cannot cut parent directory (..).", "Error", wxOK | wxICON_ERROR);
        return;
    }

    clipboardPath = selectedPath;
    isClipboardCut = true;
    hasClipboardContent = true;

    wxString fileName = selectedPath.filename().string();
    statusBar->SetStatusText("Marked for cutting: " + fileName);
}

/*
 * Function: OnPaste
 * Description: completes copy or cut operation
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnPaste(wxCommandEvent& event)
{
    if (!hasClipboardContent)
    {
        wxMessageBox("Clipboard is empty. Use Copy or Cut first.", "Clipboard Empty", wxOK | wxICON_INFORMATION);
        return;
    }

    try
    {
        wxString fileName = clipboardPath.filename().string();
        filesystem::path destinationPath = currentPath / fileName.ToStdString();

        // Check if destination exists
        if (filesystem::exists(destinationPath))
        {
            wxString message = "A file or directory named '" + fileName + "' already exists in this location.\n\nDo you want to overwrite it?";
            int result = wxMessageBox(message, "File Exists", wxYES_NO | wxICON_QUESTION);

            if (result != wxYES)
            {
                return;
            }

            // Remove existing file/directory
            if (filesystem::is_directory(destinationPath))
            {
                filesystem::remove_all(destinationPath);
            }
            else
            {
                filesystem::remove(destinationPath);
            }
        }

        wxString operation;

        // Perform copy or move
        if (isClipboardCut)
        {
            filesystem::rename(clipboardPath, destinationPath);
            operation = "Moved";
        }
        else
        {
            if (filesystem::is_directory(clipboardPath))
            {
                filesystem::copy(clipboardPath, destinationPath, filesystem::copy_options::recursive);
            }
            else
            {
                filesystem::copy_file(clipboardPath, destinationPath);
            }
            operation = "Copied";
        }

        hasClipboardContent = false;
        clipboardPath.clear();
        LoadDirectory(currentPath);
        statusBar->SetStatusText(operation + ": " + fileName + " - Clipboard cleared");
    }
    catch (const filesystem::filesystem_error& e)
    {
        wxMessageBox("Error pasting: " + string(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

/*
 * Function: OnRefresh
 * Description: reloads current directory listing
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnRefresh(wxCommandEvent& event)
{
    LoadDirectory(currentPath);
    statusBar->SetStatusText("Directory refreshed");
}

/*
 * Function: OnExit
 * Description: closes the application
 * Parameters:
 *   event: Command event from menu selection
 * Return: void
 */
void FileManagerFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

/*
 * Function: OnItemActivated
 * Description: opens files or navigates
 * Parameters:
 *   event: List event from double-click
 * Return: void
 */
void FileManagerFrame::OnItemActivated(wxListEvent& event)
{
    wxCommandEvent dummyEvent;
    OnOpen(dummyEvent);
}

/*
 * Function: OnPathEnter
 * Description: navigates to typed path
 * Parameters:
 *   event: Command event from text control
 * Return: void
 */
void FileManagerFrame::OnPathEnter(wxCommandEvent& event)
{
    wxString newPathStr = pathTextCtrl->GetValue();
    filesystem::path newPath(newPathStr.ToStdString());
    LoadDirectory(newPath);
}
