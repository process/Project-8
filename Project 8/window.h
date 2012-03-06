class Window
{
private:
    HWND hwnd;
    HINSTANCE hInst;

public:
    //Default constructor
    Window();

    //Constructor that makes a window
    Window(int width, int height);

    //Create a window with the given width and height
    //Return its handle
    HWND makeWindow(int width, int height);

    //Create the "Open file" dialog box and open a ROM
    void openDialog();

    //Returns the handle to the window
    HWND getWindowHwnd();

    //Opens a file that has been dragged and dropped into the window
    void fileDropped(HDROP file);

    //Toggles the status of the sound (un/muted)
    void changeSound();

    //Resizes the window to the given width and height. Also updates openGL
    void resizeWindow(int width, int height);

    //Window procedure for main window
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, 
        WPARAM wParam, LPARAM lParam);
};
