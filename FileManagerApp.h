/*
 * Author: Melvin Roger
 * Description: Implementation file for FileManagerApp class, main application initialization method declaration 
 * Date: 2026-01-29
 */
#ifndef FILEMANAGER_APP_H
#define FILEMANAGER_APP_H

#include <wx/wx.h>


class FileManagerApp : public wxApp
{
public:
    /**
     * OnInit - Application initialization method
     *
     * returns true if initialization succeeds, false to abort the application
     */
    virtual bool OnInit();
};

#endif 